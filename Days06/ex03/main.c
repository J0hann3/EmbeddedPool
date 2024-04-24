#include "tool.h"
#include "uart.h"
#include <stdbool.h>

#define GREEN PD6
#define RED PD5
#define BLUE PD3
#define SIZE_INPUT 7

static void _init_timer0()
{
	//clear the light on match with OCR0A and set the light on BOTTOM
	RESET(TCCR0A, COM0A0);
	SET(TCCR0A, COM0A1);

	//clear the light on match with OCR0B and set the light on BOTTOM
	RESET(TCCR0A, COM0B0);
	SET(TCCR0A, COM0B1);

	// set waveform generation to mode 3 Fast PWM top=0xff
	SET(TCCR0A, WGM00);
	SET(TCCR0A, WGM01);
	RESET(TCCR0B, WGM02);

	// set prescaler to 1024
	SET(TCCR0B, CS00);
	RESET(TCCR0B, CS01);
	SET(TCCR0B, CS02);

	OCR0A = 0;
	OCR0B = 0;
}

static void _init_timer2()
{
	//disable OCR2A
	RESET(TCCR2A, COM2A0);
	RESET(TCCR2A, COM2A1);

	//clear the light on match with OCR2B and set the light on BOTTOM
	RESET(TCCR2A, COM2B0);
	SET(TCCR2A, COM2B1);

	// set waveform generation to mode 3 Fast PWM top=0xff
	SET(TCCR2A, WGM20);
	SET(TCCR2A, WGM21);
	RESET(TCCR2B, WGM22);

	// set prescaler to 1024
	SET(TCCR2B, CS20);
	SET(TCCR2B, CS21);
	SET(TCCR2B, CS22);

	OCR2B = 0;
}


void init_rgb()
{
	_init_timer0();
	_init_timer2();
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
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

void convert_input(char *buffer)
{
	if (buffer[0] != '#')
	{
		uart_printstr("Error: Invalid input, need #RRGGBB\r\n");
		return;
	}
	for (uint8_t i; i < SIZE_INPUT; i++)
	{
		if (!is_alpha_num(buffer[i]))
		{
			uart_printstr("Error: Invalid input, need hex value\r\n");
			return;
		}
	}
	uint16_t red = convert_hex(buffer[1], buffer[2]);
	uint16_t green = convert_hex(buffer[3], buffer[4]);
	uint16_t blue = convert_hex(buffer[5], buffer[6]);
	set_rgb(red, green, blue);
}

ISR(USART_RX_vect)
{
	static char line[SIZE_INPUT] = {0};
	static uint8_t index = 0;
	unsigned char c = uart_rx();
	uart_tx(c);
	line[index] = c;
	index++;
	if (index == SIZE_INPUT)
	{
		uart_printstr("\r\n");
		convert_input(line);
		index = 0;
	}
}

int main()
{
	uart_init();
	SET(DDRD, PD3);
	SET(DDRD, PD5);
	SET(DDRD, PD6);

	RESET(PORTD, PD3);
	RESET(PORTD, PD5);
	RESET(PORTD, PD6);

	init_rgb();

	uint8_t i = 0;

	while(1)
	{}
}
