#include "tool.h"

#define SPEED_BLINK 6000
#define SPEED_INTERRUPT 77
/* 
200 = 16 000 000 / (2 * 64 * (1 + OCR1A))
 */

static void _init_timer1()
{
	//clear the light on match with OCR1A and set the light on BOTTOM
	RESET(TCCR1A, COM1A0);
	SET(TCCR1A, COM1A1);

	// set waveform generation to mode 14 Fast PWM
	RESET(TCCR1A, WGM10);
	SET(TCCR1A, WGM11);
	SET(TCCR1B, WGM12);
	SET(TCCR1B, WGM13);

	// set prescaler to 64
	SET(TCCR1B, CS10);
	SET(TCCR1B, CS11);
	RESET(TCCR1B, CS12);

	//set TOP to SPEED_BLINK
	ICR1 = SPEED_BLINK;
	OCR1A = 0;
}

static void _init_timer0()
{
	// enabled global interrupt
	SET(SREG, 7);

	//pin OC0A disconnected
	RESET(TCCR0A, COM0A0);
	RESET(TCCR0A, COM0A1);

	// set waveform generation to mode 2 CTC
	RESET(TCCR0A, WGM00);
	SET(TCCR0A, WGM01);
	RESET(TCCR0B, WGM02);

	// set prescaler to 1024
	SET(TCCR0B, CS00);
	RESET(TCCR0B, CS01);
	SET(TCCR0B, CS02);

	//set output Compare A Match Interrupt Enable
	SET(TIMSK0, OCIE0A);

	OCR0A = SPEED_INTERRUPT;
}

ISR(TIMER0_COMPA_vect)
{
	static int pourcentage = 0; //pourcentage of the light
	static int is_increment = 1;
	if (is_increment == 1)
	{
		if (pourcentage == 100)
		{
			is_increment = 0;
			pourcentage--;
		}
		else
			pourcentage++;
	}
	else
	{
		if (pourcentage == 0)
		{
			is_increment = 1;
			pourcentage++;
		}
		else
			pourcentage--;
	}
	OCR1A = SPEED_BLINK / 100. * pourcentage;
}

int main()
{
	SET(DDRB, PB1); //set light to output
	_init_timer1();
	_init_timer0();
	while (1)
	{}
}