//Testing the backscatter transmit capabilities of the Attiny20
#define F_CPU 8000000UL
#define PACKET_SIZE 12

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//external transmit function from ASM
extern void TRANSMIT(uint8_t*, uint8_t);

void Clock_Init(void)
{
	CCP = 0xD8; //enter the secret password
	CLKPSR = 0b00000000; //Divide 8MHz clock by 2 for 4MHz IO
}

void Interrupt_Init(void)
{
	DDRB &= ~(1 << PORTB2);  //PORTB2 as input (INT0)
	PUEB |= (1 << PUEB2);  //pull-up on INT0
	
	MCUCR &= ~((1 << ISC01) | (1 << ISC00));  //INT0 level triggered interrupt
	sei();                //enable interrupts
}

void Backscatter_Transmit(uint8_t preamble, uint8_t data)
{
	uint8_t packet[PACKET_SIZE]; //data for the packet
	uint8_t *start_ptr; //the start of the TX data in memory
	start_ptr = packet;	//point the pointer at the packet
	
	//assemble the packet
	packet[0] = preamble;
	packet[1] = preamble;
	packet[2] = preamble;
	packet[3] = preamble;
	packet[4] = 0x69;
	packet[5] = 0x81;
	packet[6] = 0x7E;
	packet[7] = 0x96;
	packet[8] = data;
	packet[9] = preamble;
	packet[10] = preamble;
	packet[11] = preamble;
	
	//Backscatter transmit
	TRANSMIT(start_ptr, PACKET_SIZE);
	_delay_ms(400); //necessary if we are transmitting constantly
}

int main(void)
{
	//initialize the hardware
	Clock_Init();
	//Interrupt_Init();
	
	while(1){
		Backscatter_Transmit(0x55, 0xA1);        //backscatter transmit the data
		//This sleep code for transmitting on External Interrupt (INT0)
		/*
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();
		GIMSK |= (1 << INT0);                    //turn on the interrupt
		sleep_cpu();
		sleep_disable();
		GIMSK &= ~(1 << INT0);                   //turn off the INT0 interrupt
		*/
	}
	
}

ISR(INT0_vect)  //This needs to be here if we're using the External Interrupt
{

}

