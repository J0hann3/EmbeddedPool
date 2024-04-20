#include "tool.h"
#include <util/twi.h>
#include <stdlib.h>
#include "uart.h"


void print_hex_value();

void i2c_init(void)
{
	// frequence 100kHz
	// SCLf = F_CPU / (16 + 2 *(TWBR) * prescaler)
	// (TWBR) = (F_CPU / SCLf - 16) /2 / prescaler = 72 and prescaler = 1
	TWBR = 72;
	RESET(TWSR, TWPS0);
	RESET(TWSR, TWPS1);
}

uint8_t i2c_start(uint8_t read)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
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
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
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

uint8_t i2c_restart(uint8_t read)
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
	TWDR = (0x38 << 1) + read;
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

void print_value(float temp, float hum)
{
	static float tab_temp[3] = {0};
	static float tab_hum[3] = {0};
	static uint8_t index = 0;

	tab_temp[index] = temp;
	tab_hum[index] = hum;

	if (index == 2)
	{
		index = 0;

		temp = (tab_temp[0] + tab_temp[1] + tab_temp[2]) / 3;
		hum = (tab_hum[0] + tab_hum[1] + tab_hum[2]) / 3;
		char temp_str[5];
		char hum_str[7];

		dtostrf(temp, 4, 2, temp_str);
		dtostrf(hum, 6, 3, hum_str);
		uart_printstr("Temperature: ");
		uart_printstr(temp_str);
		uart_printstr("°C, Humidity: ");
		uart_printstr(hum_str);
		uart_printstr("%\r\n");
		return ;
	}
	index++;
}

void print_hex_value()
{
	uint8_t index = 0;
	uint32_t temperature = 0;
	uint32_t humidity = 0;

	while(index < 6)
	{
		uint8_t c = i2c_read(index != 5);
		if (index == 1 || index == 2)
			humidity = (humidity << 8) | c;
		else if (index == 3)
		{
			humidity = (humidity << 4) | (c >> 4);
			temperature = c & 0x0f;
		}
		else if (index == 4 || index == 5)
			temperature = (temperature << 8) | c;
		index++;
	}
	float temp_f = temperature / 1048576. * 200. - 50.;
	float hum_f = humidity / 1048576. * 100.;
	print_value(temp_f, hum_f);
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
		i2c_write(0xAC);
		i2c_write(0x33);
		i2c_write(0x00);
		i2c_stop();
		_delay_ms(200);
		i2c_start(TW_READ);
		_delay_ms(100);
		print_hex_value();
		_delay_ms(500); //wait
		i2c_stop();
	}
}
