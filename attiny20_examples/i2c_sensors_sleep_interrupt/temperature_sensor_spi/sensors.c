/*
 * sensors.c
 *
 * Created: 7/1/2021 1:55:01 PM
 *  Author: Vikram Iyer
 */

#include <util/delay.h>
#include "sensors.h"
#include "i2cmaster.h"

// Read SHTW2
	// NOTE: i2c start takes 7b address followed by read/write bit
	// Ex: For device address 0x70
	//                        = 1110000 0
	//                        = 0b1110 0000 = 0xE0

unsigned char read_shtw2(uint8_t* data){
	unsigned char ret;

	// Send measurement command for temperature + humidity (RH)
	ret = i2c_start(SHTW2_ADDR+I2C_WRITE);			// set device address and write mode
	i2c_write(0x7C);
	i2c_write(0xA2);
	i2c_stop();

	// Wait 1 ms
	_delay_ms(1);

    // Read 2B of temp data + CRC followed by 2B RH data + CRC
	ret = i2c_rep_start(SHTW2_ADDR+I2C_READ);		// set device address and read mode
	data[0] = i2c_readAck();						// Read temp MSB
	data[1] = i2c_readAck();						// Read temp LSB
	data[2] = i2c_readAck();						// Read temp CRC
	data[3] = i2c_readAck();						// Read RH MSB
	data[4] = i2c_readAck();						// Read RH LSB
	data[5] = i2c_readNak();						// Read RH CRC
	i2c_stop();										// set stop condition = release bus
    return ret;
}

unsigned char init_hdc2010(){
	unsigned char ret;

	// Trigger measurement
	i2c_start(HDC2010_ADDR+I2C_WRITE);
	i2c_write(0x0F);
	i2c_write(0x01);
	i2c_stop();

	i2c_start(HDC2010_ADDR+I2C_READ);		// set device address and read mode
	i2c_readAck();							// read Temp LSB from address 0
	i2c_readNak();							// read RH MSB from address 3
	i2c_stop();

	i2c_start(HDC2010_ADDR+I2C_WRITE);
	i2c_write(0x0E);
	i2c_write(0x56);
	i2c_stop();

	ret = i2c_start(HDC2010_ADDR+I2C_WRITE);
	i2c_write(0x07);
	i2c_write(0x80);
	i2c_stop();

	return ret;
}

// Read HDC2010 (Temperature + RH)
unsigned char read_hdc2010(uint8_t* data){
	unsigned char ret;

    // Set first temp output data register to read
	i2c_start(HDC2010_ADDR+I2C_WRITE);
	i2c_write(0x00);
	i2c_stop();

	ret = i2c_rep_start(HDC2010_ADDR+I2C_READ);		// set device address and read mode
	data[0] = i2c_readAck();					// read Temp LSB from address 0
	data[1] = i2c_readAck();					// read Temp MSB from address 1
	data[2] = i2c_readAck();					// read RH LSB from address 2
	data[3] = i2c_readNak();					// read RH MSB from address 3
	i2c_stop();

	return ret;
}
// Read LV0104CS (Lux sensor)
unsigned char read_lv0104cs(uint8_t* data){
	unsigned char ret;

    i2c_start(LV0104CS_ADDR+I2C_WRITE);
	i2c_write(0xE0);
	i2c_stop();

	// Wait for measurement, requires min 15 ms
	_delay_ms(15);

	// Read data
	i2c_rep_start(LV0104CS_ADDR+I2C_READ);			// set device address and read mode
	data[0] = i2c_readAck();                        // read Lux MSB
	data[1] = i2c_readAck();                        // read Lux LSB
	i2c_stop();

	// Back to sleep mode
	ret = i2c_start(LV0104CS_ADDR+I2C_WRITE);
	i2c_write(0x13);
	i2c_stop();

	return ret;
}

// Read MMC5633 (3-axis magnetometer)
unsigned char read_MMC5633(uint8_t* data){
    unsigned char ret;
	_delay_ms(11);
	// Set BW to max, min = 0x03
	i2c_start(0b01100000+I2C_WRITE);
	i2c_write(0x1C);
	i2c_write(0x00);
	i2c_stop();

	i2c_start(0b01100000+I2C_WRITE);
	i2c_write(0x1B);
	i2c_write(0x21);
	i2c_stop();

	i2c_start(0b01100000+I2C_WRITE);
	i2c_write(0x00);
	i2c_stop();
	data[0] = i2c_readAck();
	data[1] = i2c_readAck();
	data[2] = i2c_readAck();
	data[3] = i2c_readAck();
	data[4] = i2c_readAck();
	data[5] = i2c_readAck();
	data[6] = i2c_readAck();
	data[7] = i2c_readAck();
	data[8] = i2c_readNak();
	i2c_stop();

	return ret;
}

unsigned char debug_output(uint8_t addr, uint8_t* data, uint8_t data_len){
	unsigned char ret;
	i2c_start(addr+I2C_WRITE);
	for(int i = 0; i<data_len; i++){
		ret = i2c_write(data[i]);
	}
	i2c_stop();
	return ret;
}