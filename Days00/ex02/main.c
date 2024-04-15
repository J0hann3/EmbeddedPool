#include <avr/io.h>
#include <util/delay.h>

int main()
{
	DDRD &= ~(1 << PD2); //set switch PortD2 for input data
	DDRB |= (1 << PB0); //set light PortB0 for output data
	PORTB &= ~(1 << PB0); //init PortB0 with the light off

	while(1)
	{
		if ((PIND & (1 << PD2)) == 0) // check if switch PortD2 input data is actived
			PORTB |= (1 << PB0); //light on PortB0
		else
			PORTB &= ~(1 << PB0); //light off PortB0
		_delay_ms(10); //wait before checking again
	}
}



