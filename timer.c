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
	SET(TCCR1B, CS10);
	SET(TCCR1B, CS11);
	RESET(TCCR1B, CS12);

	// set interrupt for OCR1A
	SET(TIMSK1, OCIE0A);

	OCR1A = 5000;//every 20ms => 50Hz
	// 16 000 000 / 50 / 64 = 5000
}

ISR(TIMER1_COMPA_vect)
{
	counter++;
}