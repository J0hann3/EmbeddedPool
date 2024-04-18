#include "tool.h"
#define SPEED_INTERRUPT 31250

void uart_init()
{
	//UBRRn = F_CPU/(16 * BAUD) -1
	// 7.6805 = 16 000 000 / / (16 * 115 200) -1
	uint16_t ubrr = F_CPU / (16. * UART_BAUDRATE) - 1 + 0.5; //add 0.5 to round the value to the nearest int
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)(ubrr & 0xFF);

	// Asynchronous USART
	RESET(UCSR0C, UMSEL00);
	RESET(UCSR0C, UMSEL01);

	// 8 data bits, no parity bits, and 1 stop 
	// => UCSZn2:0 (data bit),  UPMn1:0(parity bit), USBSn(stop bit) in UCSRnB and UCSRnC
	// Parity mode disabled
	RESET(UCSR0C, UPM00);
	RESET(UCSR0C, UPM01);

	// Stop bit => 1-bit
	RESET(UCSR0C, USBS0);

	//set number data bit to 8 bit
	SET(UCSR0C, UCSZ00);
	SET(UCSR0C, UCSZ01);
	RESET(UCSR0B, UCSZ02);

	// Activated transmitter
	SET(UCSR0B, TXEN0);
}

void uart_tx(char c)
{
	// exit the loop if ready to write data
	while((UCSR0A & (1 << UDRE0)) == 0)
	{}
	// transmit data c
	UDR0 = c;
}

void uart_printstr(const char* str)
{
	if (str == 0)
		return;
	for (int i = 0; str[i] != '\0'; i++)
		uart_tx(str[i]);
}

void init_timer()
{
	// enabled global interrupt
	SET(SREG, 7);

	//pin OC0A disconnected
	RESET(TCCR1A, COM1A0);
	RESET(TCCR1A, COM1A1);

	// set waveform generation to mode 2 CTC
	RESET(TCCR1A, WGM10);
	RESET(TCCR1A, WGM11);
	SET(TCCR1B, WGM12);
	RESET(TCCR1B, WGM13);

	// set prescaler to 1024
	SET(TCCR1B, CS10);
	RESET(TCCR1B, CS11);
	SET(TCCR1B, CS12);

	//set output Compare A Match Interrupt Enable
	SET(TIMSK1, OCIE1A);

	OCR1A = SPEED_INTERRUPT;
}

ISR(TIMER1_COMPA_vect)
{
	uart_printstr("Hello World!\n\r");
}

int main()
{
	init_timer();
	uart_init();
	while (1)
	{
	}
}