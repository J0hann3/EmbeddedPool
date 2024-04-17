#include "tool.h"
volatile uint8_t enable = 0;

ISR(INT0_vect)
{
	enable = 1;
	TOGGLE(PORTB, PB0); //change the state of the light LED D1
}

int main()
{
	RESET(DDRD, PD2); //set switch to input
	SET(DDRB, PB0); // set light to output

	//Activate global interrupt
	SET(SREG, 7);

	//Active INT0
	SET(EIMSK, INT0);

	// The falling edge of INT0 generates an interrupt request.
	SET(EICRA, ISC01);
	RESET(EICRA, ISC00);

	while(1)
	{
		if (enable == 1)
		{
			cli();
			_delay_ms(500);
			enable = 0;
			sei();
		}
	}
}