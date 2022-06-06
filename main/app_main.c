/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "hdc1080.h"                                          /*Biblioteca do sensor*/

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)
#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ CONFIG_I2C_MASTER_FREQUENCY        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

static const char *TAG = "MAIN";

/**
* @brief i2c master initialization
*/
static void i2c_master_init()
{
   i2c_port_t i2c_master_port = I2C_MASTER_NUM;
   i2c_config_t conf;
   conf.clk_flags = 0;

   conf.mode = I2C_MODE_MASTER;
   conf.sda_io_num = (gpio_num_t) I2C_MASTER_SDA_IO;
   conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
   conf.scl_io_num = (gpio_num_t) I2C_MASTER_SCL_IO;
   conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
   conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
   i2c_param_config( i2c_master_port, &conf);
   if ( i2c_driver_install(i2c_master_port, conf.mode,
                             I2C_MASTER_RX_BUF_DISABLE,
                             I2C_MASTER_TX_BUF_DISABLE, 0) != ESP_OK)
   {
       ESP_LOGE(TAG, "Failed to initialize I2C driver!\r\n");
   }
}

#if 0
static void i2c_master_deinit(void)
{
    if (i2c_driver_delete(I2C_MASTER_NUM) != ESP_OK)
        ESP_LOGE(TAG, "Failed to uninstall I2C driver!\r\n");
}
#endif

void app_main()
{
    float temperature, humidity;

    i2c_master_init();
    if ( hdc1080_init(I2C_MASTER_NUM) != ESP_OK)
    {
        printf("Erro print: %i\n", hdc1080_init(I2C_MASTER_NUM));
        ESP_LOGE(TAG, "Failed to initialize HDC1080 sensor!\r\n");
    }

    while(1) {
	// Get the temperature and humidity
    //  printf("Erro read: %i\n", hdc1080_read_temperature(I2C_MASTER_NUM, &temperature, &humidity));
        if ( hdc1080_read_temperature(I2C_MASTER_NUM, &temperature, &humidity) == ESP_OK)
        {
            printf("Current temperature = %.2f C, Relative Humidity = %.2f %%\n", 
                    temperature, humidity);
        }
    }
}
