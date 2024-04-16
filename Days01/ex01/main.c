#include <avr/io.h>
#include "tool.h"

int main()
{
	// light register set up to output
	SET(DDRB, PB1);

	// time register
	RESET(TCCR1A, WGM10);	// mode CTC(clear timer on Compare mach)
	RESET(TCCR1A, WGM11);	// mode CTC(clear timer on Compare mach)
	SET(TCCR1B, WGM12);		// mode CTC(clear timer on Compare mach)
	RESET(TCCR1B, WGM13);	// mode CTC(clear timer on Compare mach)

	RESET(TCCR1A, COM1A1);	// toggle pin OC1A on Compare Match with OCR1A
	SET(TCCR1A, COM1A0);	// toggle pin OC1A on Compare Match with OCR1A

	SET(TCCR1B, CS12);		// divise clock by 256
	RESET(TCCR1B, CS11);	// divise clock by 256
	RESET(TCCR1B, CS10);	// divise clock by 256

	OCR1A = 31250; // set time to switch the light

	while(1)
	{
	}
}

/* 
16 000 000 Hz / 256

62 500Hz  ???
160 000s  0.5s

31 250cycles => 0.5s

1Hz = 16 000 000Hz/(2 * 256 * (31250))
 */