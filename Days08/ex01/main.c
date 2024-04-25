#include "tool.h"
#include "uart.h"
#include <stdbool.h>

#define MOSI PB3
#define SCK PB5
#define SS PB2

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

void set_led_r6(uint8_t red, uint8_t green, uint8_t blue, uint8_t lum)
{
	send_start();
	set_led_rgb(red, green, blue, lum);
	set_led_rgb(0, 0, 0, 0);
	set_led_rgb(0, 0, 0, 0);
	send_end();
}

int main()
{
	spi_init();
	
	while(1)
	{
		set_led_r6(0xff, 0, 0, 0x01);
		_delay_ms(1000);
		set_led_r6(0, 0xff, 0, 0x01);
		_delay_ms(1000);
		set_led_r6(0, 0, 0xff, 0x01);
		_delay_ms(1000);
		set_led_r6(0xff, 0xff, 0, 0x01);
		_delay_ms(1000);
		set_led_r6(0, 0xff, 0xff, 0x01);
		_delay_ms(1000);
		set_led_r6(0xff, 0, 0xff, 0x01);
		_delay_ms(1000);
		set_led_r6(0xff, 0xff, 0xff, 0x01);
		_delay_ms(1000);
	}
}
