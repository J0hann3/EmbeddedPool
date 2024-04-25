#include "tool.h"
#include "uart.h"
#include <stdbool.h>

#define MOSI PB3
#define SCK PB5
#define SS PB2

#define LED_D6 0
#define LED_D7 1
#define LED_D8 2
#define RESET_LED 3

void spi_init()
{
	SET(DDRB, SS); //set ss to output
	SET(DDRB, MOSI); //set mosi to output
	SET(DDRB, SCK); //set sck tooutput
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	/* 
	SPE => enable SPI
	DORD reset => MSB
	MSTR => choose mode master
	SPR0 => prescal de 16
	CPHA + CPOL => chose clock
	 */
}

void spi_write(uint8_t data)
{
	SPDR = data;

	while(!(SPSR & (1<<SPIF)))
	{}
}

void send_start()
{
	for(uint8_t i = 0; i < 4 ; i++)
		spi_write(0x00); //start data 32 bit
}

void send_end()
{
	for(uint8_t i = 0; i < 4 ; i++)
		spi_write(0xff); //end data 32 bit
}

void set_led_rgb(uint8_t red, uint8_t green, uint8_t blue, uint8_t lum)
{
	spi_write(lum | 0b11100000);
	spi_write(blue); //blue
	spi_write(green); //green
	spi_write(red); //red
}

void set_led(uint8_t red, uint8_t green, uint8_t blue, uint8_t lum, uint8_t led)
{
	send_start();
	if (led == LED_D6)
		set_led_rgb(red, green, blue, lum);
	else
		set_led_rgb(0, 0, 0, 0);
	if (led == LED_D7)
		set_led_rgb(red, green, blue, lum);
	else
		set_led_rgb(0, 0, 0, 0);
	if (led == LED_D8)
		set_led_rgb(red, green, blue, lum);
	else
		set_led_rgb(0, 0, 0, 0);
	send_end();
}

void init_timer()
{
	sei();
	// set waveform generation to mode 14 Fast PWM
	RESET(TCCR1A, WGM10);
	SET(TCCR1A, WGM11);
	SET(TCCR1B, WGM12);
	SET(TCCR1B, WGM13);

	// set prescaler to 64
	RESET(TCCR1B, CS10);
	RESET(TCCR1B, CS11);
	SET(TCCR1B, CS12);

	// set interrupt for OCR1A
	SET(TIMSK1, OCIE0A);

	// set interrupt for overflow
	SET(TIMSK1, TOIE1);

	//set TOP
	ICR1 = 20833;//every 333ms => 3Hz
	// 16 000 000 / 3 / 256 = 20 833

	OCR1A = 15625;//every 250ms => 4Hz
	// 16 000 000 / 4 / 256 = 15 625
}

ISR(TIMER1_COMPA_vect)
{
	static uint8_t nb_led = 0;

	set_led(0xff, 0, 0, 0x01, nb_led);
	nb_led++;
	if (nb_led == 3)
		nb_led = 0;
}

ISR(TIMER1_OVF_vect)
{
	set_led(0, 0, 0, 0, RESET_LED);
}

int main()
{
	spi_init();
	init_timer();
	set_led(0, 0, 0, 0, RESET_LED);
	
	while(1)
	{}
}
