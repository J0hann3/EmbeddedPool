#include "i2c.h"

void i2c_init(void)
{
	// frequence 100kHz
	// SCLf = F_CPU / (16 + 2 *(TWBR) * prescaler)
	// (TWBR) = (F_CPU / SCLf - 16) /2 / prescaler = 72 and prescaler = 1
	TWBR = 72;
	RESET(TWSR, TWPS0);
	RESET(TWSR, TWPS1);
}

uint8_t i2c_start(uint8_t read, uint8_t address)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while (!(TWCR & (1 << TWINT))) //wait for transmittion of START
	{}
	// uart_printstr("Start...\r\n");
	if (TW_STATUS != TW_START) //check status code == start
	{
		uart_printstr("ERROR START");
		return TW_STATUS;
	}
	// read address of slave into register
	TWDR = (address << 1) + read;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
	{}

	// uart_printstr("Bind address ...\r\n");
	if ((TW_STATUS != TW_MT_SLA_ACK && !read)
		|| (TW_STATUS != TW_MR_SLA_ACK && read))
	{
		uart_printstr("ERROR SLAVE ACK");
		return TW_STATUS;
	}
	return 0;
}

uint8_t i2c_restart(uint8_t read, uint8_t address)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	while (!(TWCR & (1 << TWINT))) //wait for transmittion of RESTART
	{}
	// uart_printstr("Restart...\r\n");
	if (TW_STATUS != TW_REP_START) //check status code == start
	{
		uart_printstr("ERROR RESTART");
		return 1;
	}
	// read address of slave into register
	TWDR = (address << 1) + read;
	TWCR = (1<<TWINT) | (1<<TWEN);

	while (!(TWCR & (1 << TWINT)))
	{}

	// uart_printstr("Bind address ...\r\n");
	if ((TW_STATUS != TW_MT_SLA_ACK && !read)
		|| (TW_STATUS != TW_MR_SLA_ACK && read))
	{
		uart_printstr("ERROR SLAVE ACK");
		return 1;
	}
	return 0;
}

void i2c_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	// uart_printstr("Stop connection ...\r\n");
}

void i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);

	while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
	{}

	// uart_printstr("Write data ...\r\n");
	if (TW_STATUS != TW_MT_DATA_ACK)
	{
		uart_printstr("T ERROR DATA ACK");
		return ;
	}
}

unsigned char i2c_read(uint8_t ack)
{
	volatile unsigned char c;
	if (ack == 1)
		TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	else
		TWCR = (1<<TWINT) | (1<<TWEN);

	while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
	{}
	c = TWDR;

	if ((TW_STATUS != TW_MR_DATA_ACK && ack)
		|| (TW_STATUS != TW_MR_DATA_NACK && !ack))
	{
		uart_printstr("T ERROR DATA ACK");
		return 0xff;
	}

	return c;
}