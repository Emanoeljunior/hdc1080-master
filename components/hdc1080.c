#include "hdc1080.h"

#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */

/**
* @brief Test code to write esp-i2c-slave
*        Master device write data to slave(both esp32),
*        the data will be stored in slave buffer.
*        We can read them out from slave buffer.
*
* ___________________________________________________________________
* | start | slave_addr + wr_bit + ack | write n bytes + ack  | stop |
* --------|---------------------------|----------------------|------|
*
*/
static esp_err_t i2c_master_write_slave(i2c_port_t i2c_num, uint8_t slave_addr, uint8_t *data_wr, size_t size)
{
   i2c_cmd_handle_t cmd = i2c_cmd_link_create();
   i2c_master_start(cmd);
   i2c_master_write_byte(cmd, ( slave_addr << 1) | WRITE_BIT, ACK_CHECK_EN);
   i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
   i2c_master_stop(cmd);
   esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
   i2c_cmd_link_delete(cmd);
   return ret;
}


esp_err_t hdc1080_init(i2c_port_t i2cnum)
{
   /* Write to reg addr 0x20 (config register), with value of 0x10 (14 bit adc resolution)
    */
    uint8_t reg = 0x02;
    uint8_t config_data = 0x10; // Enable both temp and humid measurement at high resolution

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (HDC1080_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, config_data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2cnum, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret; 
}

esp_err_t hdc1080_read_temperature(i2c_port_t i2cnum, float * temperature, float * humidity)
{
    esp_err_t err;
    uint8_t data[4];
    size_t size = 4;
    
    data[0] = 0;

    i2c_master_write_slave(i2cnum, HDC1080_ADDR, (uint8_t *) &data[0], 1);

    vTaskDelay( 50 / portTICK_PERIOD_MS );
    
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
   
    i2c_master_write_byte(cmd, (HDC1080_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
	    i2c_master_read(cmd, ((uint8_t *)&data[0]), size - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, ((uint8_t *)&data[0]) + size - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    err = i2c_master_cmd_begin(i2cnum, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    // Process the tempereature and humidity data
    uint16_t rawTemp = (data[0] << 8) | data[1];
    uint16_t rawHumid = (data[2] << 8) | data[3];

    *temperature = ( (float) rawTemp )*165/65536 - 40;
    *humidity = ( (float) rawHumid )*100/65536;

    return err;
}


