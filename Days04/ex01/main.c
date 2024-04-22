#include "tool.h"
#include "util/twi.h"
#include "uart.h"

void print_hex_value();

void i2c_init(void)
{
	// frequence 100kHz
	// SCLf = F_CPU / (16 + 2 *(TWBR) * prescaler)
	// (TWBR) = (F_CPU / SCLf - 16) /2 / prescaler = 72 and prescaler = 1
	TWBR = 72;
	//Prescale of 1
	RESET(TWSR, TWPS0);
	RESET(TWSR, TWPS1);
}

uint8_t i2c_start(uint8_t read)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // set start flag + enable TWI + clear TWINT
	while (!(TWCR & (1 << TWINT))) //wait for transmittion of START
	{}
	// uart_printstr("Start...\r\n");
	if (TW_STATUS != TW_START) //check status code == start
	{
		uart_printstr("ERROR START");
		return 1;
	}
	// read address of slave into register
	TWDR = (0x38 << 1) + read;
	TWCR = (1<<TWINT) | (1<<TWEN); // clear Flag for new action
	while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
	{}

	// uart_printstr("Bind address ...\r\n");
	if ((TW_STATUS != TW_MT_SLA_ACK && !read)
		|| (TW_STATUS != TW_MR_SLA_ACK && read)) //check status
	{
		uart_printstr("ERROR SLAVE ACK");
		return 1;
	}
	return 0;
}

uint8_t i2c_restart(uint8_t read)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // set start flag + enable TWI + clear TWINT
	while (!(TWCR & (1 << TWINT))) //wait for transmittion of RESTART
	{}
	// uart_printstr("Restart...\r\n");
	if (TW_STATUS != TW_REP_START) //check status code == restart
	{
		uart_printstr("ERROR RESTART");
		return 1;
	}
	// read address of slave into register
	TWDR = (0x38 << 1) + read;
	TWCR = (1<<TWINT) | (1<<TWEN); // clear Flag for new action

	while (!(TWCR & (1 << TWINT)))
	{}

	// uart_printstr("Bind address ...\r\n");
	if ((TW_STATUS != TW_MT_SLA_ACK && !read)
		|| (TW_STATUS != TW_MR_SLA_ACK && read)) //check status
	{
		uart_printstr("ERROR SLAVE ACK");
		return 1;
	}
	return 0;
}

void i2c_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);  //set flag to stop i2c
	// uart_printstr("Stop connection ...\r\n");
}

void i2c_write(unsigned char data)
{
	TWDR = data; //write data
	TWCR = (1<<TWINT) | (1<<TWEN); //clear flag

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
		TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN); // set flag with ack
	else
		TWCR = (1<<TWINT) | (1<<TWEN); //set flag with nack

	while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
	{}
	c = TWDR;

	if ((TW_STATUS != TW_MR_DATA_ACK && ack)
		|| (TW_STATUS != TW_MR_DATA_NACK && !ack)) // check status
	{
		uart_printstr("T ERROR DATA ACK");
		return 0xff;
	}

	return c;
}

void print_hex_value()
{
	volatile uint8_t index = 0;

	while(index < 6)
	{
		uint8_t c = i2c_read(index != 5); // read the 5 valus of the sensor
		uart_printhex(c);
		uart_tx(' ');
		index++;
	}
	uart_printstr("\r\n");
}

void reset_sensor()
{
	i2c_start(TW_WRITE);
	_delay_ms(100);
	uart_printstr("Reset...\r\n");
	i2c_write(0b10111010);
	i2c_stop();
	_delay_ms(100);
}

void calibre_sensor()
{
	i2c_start(TW_READ);
	_delay_ms(100);
	volatile unsigned char c = i2c_read(0);
	i2c_stop();
	_delay_ms(100);

	if ((c & 0x18) != 0x18)
	{
		i2c_start(TW_WRITE);
		_delay_ms(100);
		uart_printstr("Calibrating...\r\n");
		i2c_write(0xbe);
		i2c_write(0x08);
		i2c_write(0x00);
		_delay_ms(100);
	}
}

int main()
{
	uart_init();
	i2c_init();
	_delay_ms(100);
	reset_sensor();
	calibre_sensor();
	while(1)
	{
		_delay_ms(100);
		i2c_start(TW_WRITE);
		_delay_ms(100);
		i2c_write(0xAC); //send command to the sensor
		i2c_write(0x33);
		i2c_write(0x00);
		i2c_stop();
		_delay_ms(200);
		i2c_start(TW_READ);
		_delay_ms(100);
		print_hex_value(); // read command receive
		_delay_ms(500); //wait
		i2c_stop();
	}
}
