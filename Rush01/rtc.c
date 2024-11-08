#include "i2c.h"
#include "uart.h"

// 28/02/2023 18:03:17
static volatile char buffer[20] = {0};
static volatile uint8_t index = 0;

ISR(USART_RX_vect)
{

	char c = uart_rx();
	uart_tx(c);
	if (c == '\r')
	{
		if (index < 20)
			buffer[index] = '\0';
		get_date = 1;
		uart_printstr("\r\n");
		RESET(UCSR0B, RXCIE0);
		return ;
	}
	if (index < 20)
		buffer[index] = c;
	index++;
}

uint8_t is_number(char c)
{
	return (c >= '0' && c <= '9');
}

void error()
{
	uart_printstr("\r\nInvalid input, need DD/MM/YYYY HH:mm:ss\r\n");
}

date_t parse_date(uint8_t *err)
{
	date_t date = {0};
	if (index != 19)
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	if (!(is_number(buffer[0]) && is_number(buffer[1])))
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	date.day = (buffer[0] - '0') * 10 + buffer[1] - '0';
	if (buffer[2] != '/')
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	if (!(is_number(buffer[3]) && is_number(buffer[4])))
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	date.month = (buffer[3] - '0') * 10 + buffer[4] - '0';
	if (buffer[5] != '/')
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	if (!(is_number(buffer[6]) && is_number(buffer[7])
		&& is_number(buffer[8]) && is_number(buffer[9])))
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	date.year = (buffer[6] - '0') * 1000 + (buffer[7] - '0') * 100
		+ (buffer[8] - '0') * 10 + buffer[9] - '0';
	if (buffer[10] != ' ')
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	if (!(is_number(buffer[11]) && is_number(buffer[12])))
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	date.hour = (buffer[11] - '0') * 10 + buffer[12] - '0';
	if (buffer[13] != ':')
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	if (!(is_number(buffer[14]) && is_number(buffer[15])))
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	date.min = (buffer[14] - '0') * 10 + buffer[15] - '0';
	if (buffer[16] != ':')
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	if (!(is_number(buffer[17]) && is_number(buffer[18])))
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	date.sec = (buffer[17] - '0') * 10 + buffer[18] - '0';
	if (buffer[19] != '\0')
	{
		*err = 1;
		error();
		return (date_t){0};
	}
	return date;
}

uint8_t check_date(date_t date)
{
	int months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (date.sec > 59 || date.sec < 0)
	{
		error();
		return 1;
	}
	if (date.min > 59 || date.min < 0)
	{
		error();
		return 1;
	}
	if (date.hour > 23 || date.hour < 0)
	{
		error();
		return 1;
	}
	if (date.year > 2099 || date.year < 1900)
	{
		error();
		return 1;
	}
	if (date.day > 31 || date.day < 1)
	{
		error();
		return 1;
	}
	if (date.year % 4 == 0 && (date.year % 100 != 0 || date.year % 400 == 0) && date.month == 2)
	{
		if (date.day > 29)
		{
			error();
			return 1;
		}
	}
	else if (date.month > 12 || date.month < 1 || date.day > months[date.month - 1])
	{
		error();
		return 1;
	}
	return 0;
}

uint8_t bcd_encoding(uint8_t nb)
{
	uint8_t res = (nb / 10) << 4;
	res |= nb % 10;
	return res;
}

void change_date(date_t date)
{
	uint8_t century = 0;
	date.year -= 1900;
	if (date.year > 1999)
	{
		century = 1;
		date.year -= 100;
	}
	i2c_start(TW_WRITE, 0b01010001);
	i2c_write(0x02); //address second
	i2c_write(bcd_encoding(date.sec) & 0b01111111 | 0x80); //sec
	i2c_write(bcd_encoding(date.min) & 0b01111111); //min
	i2c_write(bcd_encoding(date.hour) & 0b00111111); //hour
	i2c_write(bcd_encoding(date.day) & 0b00111111); //days
	i2c_write(0); //w
	i2c_write(bcd_encoding(date.month) & 0b0011111 | century << 7); //mounth
	i2c_write(bcd_encoding(date.year)); //year
	i2c_stop();
}

void update_date()
{
	get_date = 0;
	uint8_t error = 0;
	date_t date = parse_date(&error);
	index = 0;
	SET(UCSR0B, RXCIE0);
	if (error == 1)
		return;
	if (check_date(date) == 1)
		return;
	change_date(date);
	uart_printstr("Date has been update\r\n");
}
