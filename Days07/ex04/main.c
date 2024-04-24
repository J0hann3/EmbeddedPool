#include "tool.h"
#include "uart.h"
#include <stdbool.h>

uint8_t read_analog(uint8_t pin);

static void _init_timer0()
{
	//clear the light green on match with OCR0A and set the light on BOTTOM
	RESET(TCCR0A, COM0A0);
	SET(TCCR0A, COM0A1);

	//clear the light red on match with OCR0B and set the light on BOTTOM
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

	//clear the light blue on match with OCR2B and set the light on BOTTOM
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
	// init led to output
	SET(DDRD, PD3);
	SET(DDRD, PD5);
	SET(DDRD, PD6);

	RESET(PORTD, PD3);
	RESET(PORTD, PD5);
	RESET(PORTD, PD6);

	_init_timer0();
	_init_timer2();
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	// change value of the duty cycle
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

void init_analog()
{
	//reference voltage Internal 1.1V voltage reference
	SET(ADMUX, REFS0);
	RESET(ADMUX, REFS1);

	// write right to left, use for 10 bits resolution
	SET(ADMUX, ADLAR);

	// connection ADC0 pin
	// RESET(ADMUX, MUX0);
	// RESET(ADMUX, MUX1);
	// RESET(ADMUX, MUX2);
	// RESET(ADMUX, MUX3);

	// set prescale to 128 to have frequence < 200kHz
	SET(ADCSRA, ADPS0);
	SET(ADCSRA, ADPS1);
	SET(ADCSRA, ADPS2);

	SET(ADCSRA, ADEN); //enable ADC
}

uint8_t read_analog(uint8_t pin)
{
	// set pin to read
	ADMUX &= 0xF0;
	ADMUX |= pin & 0x0F;
	
	SET(ADCSRA, ADSC); //start a convertion
	while((ADCSRA & (1 << ADSC)) != 0) //wait for the end of the converion
	{}
	return ADCH;
}

void print_gauge(uint8_t value)
{
	if (value >= 255 * 0.25)
		SET(PORTB, PB0);
	else
		RESET(PORTB, PB0);
	if (value >= 255 * 0.5)
		SET(PORTB, PB1);
	else
		RESET(PORTB, PB1);
	if (value >= 255 * 0.75)
		SET(PORTB, PB2);
	else
		RESET(PORTB, PB2);
	if (value >= 255 * 1)
		SET(PORTB, PB4);
	else
		RESET(PORTB, PB4);
}

void init_led()
{
	SET(DDRB, PB0);
	SET(DDRB, PB1);
	SET(DDRB, PB2);
	SET(DDRB, PB4);

	RESET(PORTB, PB0);
	RESET(PORTB, PB1);
	RESET(PORTB, PB2);
	RESET(PORTB, PB4);
}

int main()
{
	uart_init();
	init_led();
	init_rgb();
	RESET(DDRC, PC0); //ADC_POT
	init_analog();

	while(1)
	{
		uint8_t tmp = read_analog(PC0);
		wheel(tmp);
		print_gauge(tmp);
		uart_printdec(tmp);
		uart_printstr("\r\n");
		_delay_ms(25);
	}
}
