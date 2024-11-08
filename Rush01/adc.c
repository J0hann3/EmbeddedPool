#include "adc.h"

void adc_10_AVCC_init()
{
	RESET(DDRC, PC0); //ADC_POT
	RESET(DDRC, PC1); //ADC_LDR
	RESET(DDRC, PC2); //ADC_NTC

	//reference voltage AVCC
	SET(ADMUX, REFS0);
	RESET(ADMUX, REFS1);

	// write right to left, use for 10 bits resolution
	RESET(ADMUX, ADLAR);

	// set prescale to 128 to have frequence < 200kHz
	SET(ADCSRA, ADPS0);
	SET(ADCSRA, ADPS1);
	SET(ADCSRA, ADPS2);

	SET(ADCSRA, ADEN); //enable ADC
}

void adc_10_1V_init()
{
	//reference voltage AVCC
	SET(ADMUX, REFS0);
	SET(ADMUX, REFS1);

	// write right to left, use for 10 bits resolution
	RESET(ADMUX, ADLAR);

	// set prescale to 128 to have frequence < 200kHz
	SET(ADCSRA, ADPS0);
	SET(ADCSRA, ADPS1);
	SET(ADCSRA, ADPS2);

	SET(ADCSRA, ADEN); //enable ADC
}

uint16_t adc_read(uint8_t pin)
{
	// set pin to read
	ADMUX &= 0xF0;
	ADMUX |= pin & 0x0F;
	
	SET(ADCSRA, ADSC); //start a convertion
	while((ADCSRA & (1 << ADSC)) != 0) //wait for the end of the converion
	{}
	return ADC;
}