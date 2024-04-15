#include <avr/io.h>
#include <util/delay.h>

int main()
{
	short is_pressing = 0;

	DDRD &= ~(1 << PD2); //set PortD2 for input data
	DDRB |= (1 << PB0); //set PortB0 for output data
	PORTB &= ~(1 << PB0); //init PortB0 with the light off

	while(1)
	{
		if (is_pressing == 0 && (PIND & (1 << PD2)) == 0) // check if PortD2 input data is actived and the button was release
		{
			PORTB ^= (1 << PB0); //switch light PortB0
			is_pressing ^= 1; //save the state button press
		}
		else if (is_pressing == 1 && (PIND & (1 << PD2)) != 0) // check if PortD2 input data is desactived and the button was press
			is_pressing ^= 1; //save the state button release
		_delay_ms(10); //wait before checking again
	}
}
