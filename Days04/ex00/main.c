#include "tool.h"
#include "util/twi.h"
#include "uart.h"

void i2c_init(void)
{
	// frequence 100kHz
	// SCLf = F_CPU / (16 + 2 *(TWBR) * prescaler)
	// (TWBR) = (F_CPU / SCLf - 16) /2 / prescaler = 72 and prescaler = 1
	TWBR = 72;

	// Prescale de 1
	RESET(TWSR, TWPS0);
	RESET(TWSR, TWPS1);
}

uint8_t i2c_start(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // set start flag + enable TWI + clear TWINT
	while (!(TWCR & (1 << TWINT))) //wait for transmittion of START
	{}
	uart_printhex(TW_STATUS);
	uart_tx(' ');
	if (TW_STATUS != TW_START) //check status code == start
	{
		uart_printstr("ERROR START");
		return 1;
	}
	// read address of slave into register
	TWDR = 0x38 << 1;
	TWCR = (1<<TWINT) | (1<<TWEN); // clear Flag for new action

	while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
	{}

	uart_printhex(TW_STATUS);
	if (TW_STATUS != TW_MT_SLA_ACK) //check status
	{
		uart_printstr("ERROR SLAVE ACK");
		return 1;
	}
	return 0;
}

void i2c_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); //set flag to stop i2c
}

int main()
{
	uart_init();
	i2c_init();
	while(1)
	{
		i2c_start();
		_delay_ms(100);
		i2c_stop();
		_delay_ms(100);
		uart_printstr("\r\n");
	}
}
