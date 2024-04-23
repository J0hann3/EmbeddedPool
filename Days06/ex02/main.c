#include "tool.h"
#include "uart.h"

#define GREEN PD6
#define RED PD5
#define BLUE PD3

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

void wheel(uint8_t pos) {
	pos = 255 - pos;
	if (pos < 85) {
		set_rgb(255 - pos * 3, 0, pos * 3);
	} else if (pos < 170) {
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	} else {
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}

int main()
{
	SET(DDRD, PD3);
	SET(DDRD, PD5);
	SET(DDRD, PD6);

	RESET(PORTD, PD3);
	RESET(PORTD, PD5);
	RESET(PORTD, PD6);

	init_rgb();

	uint8_t i = 0;

	while(1)
	{
		wheel(i);
		i++;
		_delay_ms(25);
	}
}
