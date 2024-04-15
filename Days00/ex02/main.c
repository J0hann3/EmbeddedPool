#include <avr/io.h>
#include <util/delay.h>

int main()
{
	DDRD = (0 << PD2); //set PortD2 for input data
	DDRB = (1 << PB0); //set PortB0 for output data
	PORTB = (0 << PB0); //init PortB0 with the light off

	while(1)
	{
		if ((PIND & (1 << PD2)) == 0) // check if PortD2 input data is 1
			PORTB = (1 << PB0); //light on PortB0
		else
			PORTB = (0 << PB0); //light off PortB0
		_delay_ms(10); //wait before checking again
	}
}



