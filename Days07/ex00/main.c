#include "tool.h"
#include "uart.h"
#include <stdbool.h>

void init_analog()
{
	//reference voltage AVCC
	SET(ADMUX, REFS0);
	RESET(ADMUX, REFS1);

	// write left to right, good for 8 bits resolution
	SET(ADMUX, ADLAR);

	// connection ADC0 pin
	RESET(ADMUX, MUX0);
	RESET(ADMUX, MUX1);
	RESET(ADMUX, MUX2);
	RESET(ADMUX, MUX3);

	// set prescale to 128 to have frequence < 200kHz
	SET(ADCSRA, ADPS0);
	SET(ADCSRA, ADPS1);
	SET(ADCSRA, ADPS2);

	SET(ADCSRA, ADEN); //enable ADC
}

int main()
{
	uart_init();
	RESET(DDRC, PC0);
	init_analog();

	while(1)
	{
		SET(ADCSRA, ADSC); //start a convertion
		while((ADCSRA & (1 << ADSC)) != 0) //wait for the end of the convertion
		{}
		uart_printhex(ADCH);
		uart_printstr("\r\n");
		_delay_ms(20);
	}
}
