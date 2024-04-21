#include "tool.h"
#include "i2c.h"
#include "uart.h"

#define ADDRESS 0x18
#define BUTTON_PRESS 0x10

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
	TWCR = (1 << TWEN) | (1 << TWEA);
}

void slave_mode()
{
	SET(PORTB, PB4);
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
	SET(PORTB, PB2);
	_delay_ms(5000);
}

void master_mode()
{
	SET(PORTB, PB0);
	uart_printstr("Being matser ... \r\n");
	i2c_start(TW_WRITE, ADDRESS);
	while(1)	//check press button of slave
	{
		uint8_t status = i2c_write(BUTTON_PRESS);
		if (status == TW_MT_DATA_NACK)
			break ;
	}
	SET(PORTB, PB2);
	i2c_stop();
	i2c_start(TW_WRITE, ADDRESS);
	_delay_ms(5000);
}