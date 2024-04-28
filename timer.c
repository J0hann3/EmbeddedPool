#include "tool.h"
#include "uart.h"

void init_timer()
{
	sei();
	// set waveform generation to mode 4 CTC
	RESET(TCCR1A, WGM10);
	RESET(TCCR1A, WGM11);
	SET(TCCR1B, WGM12);
	RESET(TCCR1B, WGM13);

	// set prescaler to 64
	RESET(TCCR1B, CS10);
	SET(TCCR1B, CS11);
	RESET(TCCR1B, CS12);

	// set interrupt for OCR1A
	SET(TIMSK1, OCIE0A);

	OCR1A = 10000;//every 5ms => 200Hz
	// 16 000 000 / 50 / 8 = 10 000
}

ISR(TIMER1_COMPA_vect)
{
	counter_start++;
	if (reset_sensor_counter == 1)
	{
		reset_sensor_counter = 0;
		sensor_counter = 0;
	}
	else
		sensor_counter++;
	if (reset_led_rgb_counter == 1)
	{
		reset_led_rgb_counter = 0;
		led_rgb_counter = 0;
	}
	else
		led_rgb_counter++;
}