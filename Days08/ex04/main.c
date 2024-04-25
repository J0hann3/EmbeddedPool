#include "tool.h"
#include "uart.h"
#include <stdbool.h>

#define SIZE_INPUT 12
#define INTENSITY 0x01

#define MOSI PB3
#define SCK PB5
#define SS PB2

#define LED_D6 0
#define LED_D7 1
#define LED_D8 2
#define RESET_LED 3

void spi_init()
{
	SET(DDRB, SS); //set ss to output
	SET(DDRB, MOSI); //set mosi to output
	SET(DDRB, SCK); //set sck tooutput
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	/* 
	SPE => enable SPI
	DORD reset => MSB
	MSTR => choose mode master
	SPR0 => prescal de 16
	CPHA + CPOL => chose clock
	 */
}

void spi_write(uint8_t data)
{
	SPDR = data;

	while(!(SPSR & (1<<SPIF)))
	{}
}

void send_start()
{
	for(uint8_t i = 0; i < 4 ; i++)
		spi_write(0x00); //start data 32 bit
}

void send_end()
{
	for(uint8_t i = 0; i < 4 ; i++)
		spi_write(0xff); //end data 32 bit
}

void set_led_rgb(uint8_t red, uint8_t green, uint8_t blue, uint8_t lum)
{
	spi_write(lum | 0b11100000);
	spi_write(blue); //blue
	spi_write(green); //green
	spi_write(red); //red
}

void set_led(uint8_t red, uint8_t green, uint8_t blue, uint8_t lum, uint8_t led)
{
	send_start();
	if (led == LED_D6)
		set_led_rgb(red, green, blue, lum);
	else
		set_led_rgb(0, 0, 0, 0);
	if (led == LED_D7)
		set_led_rgb(red, green, blue, lum);
	else
		set_led_rgb(0, 0, 0, 0);
	if (led == LED_D8)
		set_led_rgb(red, green, blue, lum);
	else
		set_led_rgb(0, 0, 0, 0);
	send_end();
}

bool is_alpha_num(char c)
{
	return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

uint8_t convert_hex(char c1, char c2)
{
	uint8_t res;
	if (c1 >= '0' && c1 <= '9')
		res = c1 - '0';
	else if (c1 >= 'a' && c1 <= 'f')
		res = c1 - 'a' + 10;
	else
		res = c1 - 'A' + 10;
	res = res << 4;

	if (c2 >= '0' && c2 <= '9')
		res += (c2 - '0');
	else if (c2 >= 'a' && c2 <= 'f')
		res += c2 - 'a' + 10;
	else
		res += c2 - 'A' + 10;
	return res;
}

uint8_t is_rainbow(char *buffer)
{
	char cmp[] = "#FULLRAINBOW";

	for (uint8_t i = 1; i < SIZE_INPUT; i++)
	{
		if (cmp[i] != buffer[i])
			return false;
	}
	return true;
}

volatile uint8_t rainbow = 0;

void convert_input(char *buffer)
{
	if (buffer[0] != '#')
	{
		uart_printstr("Error: Invalid input, need #RRGGBBDX\r\n");
		return;
	}
	if (is_rainbow(buffer))
	{
		rainbow = 1;
		return ;
	}
	for (uint8_t i = 1; i < 7; i++)
	{
		if (!is_alpha_num(buffer[i]))
		{
			uart_printstr("Error: Invalid input, need hex value\r\n");
			return;
		}
	}
	if (buffer[7] != 'D')
	{
		uart_printstr("Error: Invalid input, need #RRGGBBDX\r\n");
		return;
	}
	if (!(buffer[8] >= '6' && buffer[8] <= '8'))
	{
		uart_printstr("Error: Invalid input, need #RRGGBBDX\r\n");
		return;
	}
	if (buffer[9] != '\r')
	{
		uart_printstr("Error: Invalid input, need #RRGGBBDX\r\n");
		return;
	}
	rainbow = 0;
	uint16_t red = convert_hex(buffer[1], buffer[2]);
	uint16_t green = convert_hex(buffer[3], buffer[4]);
	uint16_t blue = convert_hex(buffer[5], buffer[6]);
	send_start();
	set_led(red, green, blue, INTENSITY, buffer[8] - '6');
	send_end();
}

ISR(USART_RX_vect)
{
	static char line[SIZE_INPUT] = {0};
	static uint8_t index = 0;
	unsigned char c = uart_rx();
	uart_tx(c);
	line[index] = c;
	index++;
	if (c == '\r' && index < SIZE_INPUT)
		line[index] = '\0';
	if (c == '\r' || index == SIZE_INPUT)
	{
		uart_printstr("\r\n");
		convert_input(line);
		index = 0;
	}
}

void wheel(uint8_t pos) {
	pos = 255 - pos;
	send_start();
	if (pos < 85) {
		set_led_rgb(255 - pos * 3,  0,  pos * 3, INTENSITY);
		set_led_rgb(255 - pos * 3,  0,  pos * 3, INTENSITY);
		set_led_rgb(255 - pos * 3,  0,  pos * 3, INTENSITY);
	} else if (pos < 170) {
		pos = pos - 85;
		set_led_rgb(0, pos * 3, 255 - pos * 3, INTENSITY);
		set_led_rgb(0, pos * 3, 255 - pos * 3, INTENSITY);
		set_led_rgb(0, pos * 3, 255 - pos * 3, INTENSITY);
	} else {
		pos = pos - 170;
		set_led_rgb(pos * 3, 255 - pos * 3, 0, INTENSITY);
		set_led_rgb(pos * 3, 255 - pos * 3, 0, INTENSITY);
		set_led_rgb(pos * 3, 255 - pos * 3, 0, INTENSITY);
	}
	send_end();
}

int main()
{
	spi_init();
	uart_init();
	set_led(0, 0, 0, 0, RESET_LED);


	uint8_t i = 0;
	while(1)
	{
		if (rainbow == 1)
		{
			wheel(i);
			i++;
		}
		_delay_ms(10);
	}
}
