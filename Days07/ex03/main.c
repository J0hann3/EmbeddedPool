#include "tool.h"
#include "uart.h"
#include <stdbool.h>

#define ADC8 0b1000

uint16_t read_analog(uint8_t pin);

void init_analog()
{
	//reference voltage Internal 1.1V voltage reference
	SET(ADMUX, REFS0);
	SET(ADMUX, REFS1);

	// write right to left, use for 10 bits resolution
	RESET(ADMUX, ADLAR);

	// // connection ADC0 pin
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

uint16_t read_analog(uint8_t pin)
{
	// uart_printdec(pin);
	// uart_printstr(": ");

	// set pin to read
	ADMUX &= 0xF0;
	ADMUX |= pin & 0x0F;
	
	SET(ADCSRA, ADSC); //start a convertion
	while((ADCSRA & (1 << ADSC)) != 0) //wait for the end of the converion
	{}
	return ADC;
}

int main()
{
	uart_init();
	RESET(DDRC, PC0); //ADC_POT
	RESET(DDRC, PC1); //ADC_LDR
	RESET(DDRC, PC2); //ADC_NTC
	init_analog();
	read_analog(ADC8);

	while(1)
	{
		uart_printdec(read_analog(ADC8) * 25. / 314);
		uart_printstr("\r\n");
		_delay_ms(20);
	}
}
