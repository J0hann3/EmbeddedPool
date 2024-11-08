#ifndef ADC_H
# define ADC_H
# include "tool.h"

void adc_10_AVCC_init();
void adc_10_1V_init();
uint16_t adc_read(uint8_t pin);

#endif