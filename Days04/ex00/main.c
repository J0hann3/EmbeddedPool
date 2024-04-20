#include "tool.h"
#include "util/twi.h"
#include "uart.h"

void i2c_init(void)
{
	// frequence 100kHz
	// SCLf = F_CPU / (16 + 2 *(TWBR) * prescaler)
	// (TWBR) = (F_CPU / SCLf - 16) /2 / prescaler = 72 and prescaler = 1
	TWBR = 72;
	RESET(TWSR, TWPS0);
	RESET(TWSR, TWPS1);
	SET(TWCR, TWEN);
}

uint8_t i2c_start(void)
{
	SET(TWCR, TWINT); //clear flag
	SET(TWCR, TWSTA); // send start flag
	while (!(TWCR & (1 << TWINT))) //wait for transmittion of START
	{}
	uart_printhex(TW_STATUS);
	uart_tx(' ');
	if (TW_STATUS != TW_START) //check status code == start
	{
		uart_printstr("ERROR 1");
		return 1;
	}
	// read address of slave into register
	TWDR = 0x38 << 1;
	SET(TWCR, TWINT); //clear flag for new action

	while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
	{}

	uart_printhex(TW_STATUS);
	if (TW_STATUS != TW_MT_SLA_ACK) //check status code == start
	{
		uart_printstr("ERROR 2");
		return 1;
	}
	return 0;
}

void i2c_stop(void)
{
	SET(TWCR, TWINT);
	SET(TWCR, TWSTO);
}

int main()
{
	uart_init();
	i2c_init();
	i2c_start();
	i2c_stop();
	while(1)
	{
	}
}

/* 
PRTWI = 1 => enable I2C

Master initiates and terminates a data transmission

address + w/r

TWCR & (1 << TWEA) => receive ACK if != 0

SET(TWCR, TWSTA) => send start signal, should by clear after, wait for stop if busy then send start

SET(TWCR, TWSTO) => send stop signal in master mode, clear automatically after

TWWC ??

SET(TWCR, TWEN) => enable twi

TWS ???

TWPS => prescaler

TWDR can read or write data when TWINNT = 1

TWAR => slave address = TWA

SET(TWAR, TWEGCE)=> General Call Recognition Enable Bit

TWSR => identify the event when the TWINT is set




 */