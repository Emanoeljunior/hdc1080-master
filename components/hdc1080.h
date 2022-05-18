#ifndef _HDC1080_H_
#define _HDC1080_H_

#ifdef __cpluplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "driver/i2c.h"

#define HDC1080_ADDR 0x40

esp_err_t hdc1080_init(i2c_port_t i2cnum);
esp_err_t hdc1080_read_temperature(i2c_port_t i2cnum, float * temp, float * humid);

#ifdef __cpluplus
}
#endif

#endif
