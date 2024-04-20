#include "tool.h"
#include "i2c.h"
#include "uart.h"

#define ADDRESS 0x08

void init_slave();
void i2c_start_slave(uint8_t read);
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
		{
			uart_printstr("Being matser ... \r\n");
			uart_printhex(i2c_start(TW_WRITE, ADDRESS));
			_delay_ms(5000);
		}
		else if (!(TWCR & (1 << TWINT))) //SLAVE mode
		{
			// if (TWDR == TWAR << 1)
				i2c_start_slave(TW_WRITE);
			// else if (TWDR == (TWAR << 1) + 1)
			// 	i2c_start_slave(TW_READ);
			// else
			// 	uart_printstr("request on bad address\r\n");
		}

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
	TWAR = ADDRESS;
	TWCR = (1 << TWEA) | (1 << TWEN);
}

void i2c_start_slave(uint8_t read)
{
	uart_printstr("Start slave ...\r\n");
	// SET(TWCR, TWINT);
	uart_printhex(TW_STATUS);
	_delay_ms(5000);

}