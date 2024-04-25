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

void init_analog()
{
	//reference voltage AVcc
	SET(ADMUX, REFS0);
	RESET(ADMUX, REFS1);

	// write right to left, use for 8 bits resolution
	SET(ADMUX, ADLAR);

	// set prescale to 128 to have frequence < 200kHz
	SET(ADCSRA, ADPS0);
	SET(ADCSRA, ADPS1);
	SET(ADCSRA, ADPS2);

	SET(ADCSRA, ADEN); //enable ADC
}

uint8_t read_analog(uint8_t pin)
{
	// set pin to read
	ADMUX &= 0xF0;
	ADMUX |= pin & 0x0F;
	
	SET(ADCSRA, ADSC); //start a convertion
	while((ADCSRA & (1 << ADSC)) != 0) //wait for the end of the converion
	{}
	return ADCH;
}

void print_gauge(uint8_t value)
{
	send_start();
	if (value >= 255 * 0.33)
		set_led_rgb(0xff, 0, 0, 0x01);
	else
		set_led_rgb(0, 0, 0, 0);
	
	if (value >= 255 * 0.66)
		set_led_rgb(0xff, 0, 0, 0x01);
	else
		set_led_rgb(0, 0, 0, 0);
	
	if (value >= 255 * 1)
		set_led_rgb(0xff, 0, 0, 0x01);
	else
		set_led_rgb(0, 0, 0, 0);
	send_end();
}

int main()
{
	spi_init();
	RESET(DDRC, PC0);
	init_analog();
	set_led(0, 0, 0, 0, RESET_LED);
	
	while(1)
	{
		uint8_t tmp = read_analog(PC0);
		print_gauge(tmp);
		_delay_ms(250);
	}
}
