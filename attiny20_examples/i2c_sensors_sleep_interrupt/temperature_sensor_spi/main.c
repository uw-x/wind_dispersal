/****************************************************************************
Title:    Access serial EEPROM 24C02 using I2C interace
Author:   Peter Fleury <pfleury@gmx.ch>
File:     $Id: test_i2cmaster.c,v 1.3 2015/09/16 09:29:24 peter Exp $
Software: AVR-GCC 4.x
Hardware: any AVR device can be used when using i2cmaster.S or any
          AVR device with hardware TWI interface when using twimaster.c

Description:
    This example shows how the I2C/TWI library i2cmaster.S or twimaster.c
	can be used to access a serial eeprom.

*****************************************************************************/
#define F_CPU 4000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include "i2cmaster.h"
#include "sensors.h"

//Initialize clock to 8MHz
void Clock_Init(void)
{
	CCP = 0xD8; //enter the secret password
	CLKPSR = 0b00000001; //Divide 8MHz clock by 2 for 4MHz IO
}

ISR(PCINT0_vect)
{
	sleep_disable();					// Disable Sleep on Wakeup
    cli();                              // Disable interrupts

	i2c_init();							// init I2C interface
	uint8_t data[15];
	read_hdc2010(data);
	read_lv0104cs(data+4);
	debug_output(0b00010000, data, 6);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set Sleep Mode: Power Down
	sleep_enable();                      // Enable Sleep Mode
	CCP = 0xD8; //enter the secret password
	MCUCR |= _BV(BODS); //Disable Brown out detector
	sei();                               // Enable Interrupts

	// Turn things off
	ADCSRA &= ~(1<<ADEN); //Disable ADC
	TCCR1B &= ~(1<< CS12);
	TCCR1B &= ~(1<< CS11);
	TCCR1B &= ~(1<< CS11);
	TCCR0B &= ~(1<< CS02);
	TCCR0B &= ~(1<< CS01);
	TCCR0B &= ~(1<< CS00);

	DDRA=0;                       // All inputs.
	DDRB=0;                       // All inputs.
	PORTA=0xff;                   // All pull-ups to prevent floating.
	PORTB=0xff;                   // All pull-ups to prevent floating
	power_all_disable();

	PRR = 0xFF;
	CCP = 0xD8; //enter the secret password
	MCUCR |= _BV(BODS); //Disable Brown out detector

	GIMSK |= _BV(PCIE0);                // Enable interrupt on PA2
	PCMSK0 |= _BV(PCINT2);              // Turn on PA2 as interrupt pin
	sei();                              // Enable Interrupts

	sleep_cpu();        // Go to Sleep
}


int main(void)
{
	Clock_Init();

    cli();                                  // Disable interrupts
    i2c_init();							    // init I2C interface
	init_hdc2010();


	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set Sleep Mode: Power Down
	sleep_enable();                      // Enable Sleep Mode
	CCP = 0xD8; //enter the secret password
	MCUCR |= _BV(BODS); //Disable Brown out detector

	// Turn things off
	ADCSRA &= ~(1<<ADEN); //Disable ADC
	TCCR1B &= ~(1<< CS12);
	TCCR1B &= ~(1<< CS11);
	TCCR1B &= ~(1<< CS11);
	TCCR0B &= ~(1<< CS02);
	TCCR0B &= ~(1<< CS01);
	TCCR0B &= ~(1<< CS00);

	DDRA=0;                       // All inputs.
	DDRB=0;                       // All inputs.
	PORTA=0xff;                   // All pull-ups to prevent floating.
	PORTB=0xff;                   // All pull-ups to prevent floating
	power_all_disable();

	PRR = 0xFF;
	CCP = 0xD8; //enter the secret password
	MCUCR |= _BV(BODS); //Disable Brown out detector

	GIMSK |= _BV(PCIE0);                // Enable interrupt on PA2
	PCMSK0 |= _BV(PCINT2);              // Turn on PA2 as interrupt pin
	sei();                              // Enable Interrupts

	sleep_cpu();        // Go to Sleep

    for(;;)
	{
	}
}
