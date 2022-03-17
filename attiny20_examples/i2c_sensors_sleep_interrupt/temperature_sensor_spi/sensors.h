/*
 * sensors.h
 *
 * Created: 7/1/2021 1:53:15 PM
 *  Author: Vikram Iyer
 */


#ifndef SENSORS_H_
#define SENSORS_H_

#include <util/delay.h>
#include "i2cmaster.h"

#define SHTW2_ADDR 0b11100000
#define HDC2010_ADDR 0b10000000
#define LV0104CS_ADDR 0b00100110
#define MMC5633_ADDR 0b01100000

extern unsigned char read_shtw2(uint8_t* data);
extern unsigned char init_hdc2010();
extern unsigned char read_hdc2010(uint8_t* data);
extern unsigned char read_lv0104cs(uint8_t* data);
extern unsigned char read_MMC5633(uint8_t* data);
extern unsigned char debug_output(uint8_t addr, uint8_t* data, uint8_t data_len);

#endif /* SENSORS_H_ */