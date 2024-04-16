#include <avr/io.h>
#include "tool.h"

int main()
{
	// light register
	SET(DDRB, PB1);

	// time register
	RESET(TCCR1A, WGM10);	 // mode 14 Fast PWM
	SET(TCCR1A, WGM11);		 // mode 14 Fast PWM
	SET(TCCR1B, WGM12);		 // mode 14 Fast PWM
	SET(TCCR1B, WGM13);		 // mode 14 Fast PWM

	RESET(TCCR1A, COM1A0);	// set pin OC1A=0 on Compare Match with OCR1A and 1 on BOTTOM
	SET(TCCR1A, COM1A1);	// set pin OC1A=0 on Compare Match with OCR1A and 1 on BOTTOM

	SET(TCCR1B, CS12);		// divise clock by 256
	RESET(TCCR1B, CS11);	// divise clock by 256
	RESET(TCCR1B, CS10);	// divise clock by 256

	OCR1A = 62500 * 0.1; //set value for compare match

	ICR1 = 62500; //set TOP value
	while(1)
	{
	}
}

