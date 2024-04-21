#include "tool.h"
#include "i2c.h"
#include "uart.h"

#define LED_PORT PORTB
#define LED_DDR DDRB
#define ADDRESS 0x18
#define BUTTON_PRESS 0x10
#define LED_D1 0x12
#define LED_D2 0x13
#define LED_D3 0x14
#define LED_D4 0x15
#define GREEN_LED 0x16

void init_slave();
void slave_mode();
void master_mode();
void init_LED_SW1();

int main()
{
	init_LED_SW1();
	uart_init();
	i2c_init();
	init_slave();
	while(1)
	{
		if ((PIND & (1 << PD2)) == 0) //switch press == MASTER
			master_mode();
		else if ((TWCR & (1 << TWINT)) && TW_STATUS == TW_SR_SLA_ACK) //SLAVE mode
			slave_mode();
	}
}

void init_LED_SW1()
{
	RESET(DDRD, PD2);

	SET(DDRB, PB0);
	SET(DDRB, PB1);
	SET(DDRB, PB2);
	SET(DDRB, PB4);
}

void init_slave()
{
	TWAR = ADDRESS << 1;
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
}

void light_countdown_slave()
{
	uint8_t stop = 0;

	while(1)
	{
		if ((PIND & (1 << PD2)) == 0)
		{
			TWCR = (1 << TWEN) | (1 << TWINT);
			stop = 1;
		}
		else
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
		while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
		{}
		unsigned char c = TWDR;
		switch (c)
		{
			case LED_D1:
				uart_printhex(c);
				TOGGLE(PORTB, PB0);
			case LED_D2:
				uart_printhex(c);
				TOGGLE(PORTB, PB1);
			case LED_D3:
				uart_printhex(c);
				TOGGLE(PORTB, PB2);
			case LED_D4:
				uart_printhex(c);
				TOGGLE(PORTB, PB4);
			case GREEN_LED:
				stop = 1;
				uart_printhex(c);
				TOGGLE(PORTD, PD6);
		}
		if (stop == 1)
			break;
	}
}

void slave_mode()
{
	uart_printstr("Start slave ...\r\n");
	uint8_t is_press = 0;
	while(1)
	{
		if ((PIND & (1 << PD2)) == 0)
		{
			TWCR = (1 << TWEN) | (1 << TWINT);
			is_press = 1;
		}
		else
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
		while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
		{}
		unsigned char c = TWDR;
		if (is_press == 1)
			break;
	}
	SET(PORTB, PB0);
	init_slave();
	while (!(TWCR & (1 << TWINT)) && TW_STATUS != TW_SR_SLA_ACK)
	{}
	uart_printstr("restart");
	light_countdown_slave();
	_delay_ms(5000);
}

uint8_t stair_leds() {
	while (1) {
		for (int i = 0; i < 4; i++) {
			if(i2c_write(LED_D1 + i) == TW_MT_DATA_NACK)
				return 1;
			_delay_ms(500);
		}
		for (int i = 2; i >= 0; i--) {
			if(i2c_write(LED_D1 + i) == TW_MT_DATA_NACK)
				return 1;
			_delay_ms(500);
		}
	}
	if(i2c_write(GREEN_LED) == TW_MT_DATA_NACK)
		return 1;
	return 0;
}

void master_mode()
{
	uart_printstr("Being matser ... \r\n");
	i2c_start(TW_WRITE, ADDRESS);
	while(1)	//check press button of slave
	{
		uint8_t status = i2c_write(BUTTON_PRESS);
		if (status == TW_MT_DATA_NACK)
			break ;
	}
	SET(PORTB, PB0);
	i2c_stop();
	_delay_ms(100);
	i2c_start(TW_WRITE, ADDRESS);
	_delay_ms(1000);
	stair_leds();
	_delay_ms(5000);
}