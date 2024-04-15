#include <avr/io.h>

int main()
{
	DDRB |= (1 << PB0);//0b00000001; //set PortB0 to listen
	PORTB |= (1 << PB0);//0b00000001; //init PortB0 output data to 1, to enabled the LED
	while (1) // infinite loop to run the programm
	{}
}



