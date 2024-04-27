#include "spi.h"

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

void spi_send_start()
{
	for(uint8_t i = 0; i < 4 ; i++)
		spi_write(0x00); //start data 32 bit
}

void spi_send_end()
{
	for(uint8_t i = 0; i < 4 ; i++)
		spi_write(0xff); //end data 32 bit
}

void spi_set_led_rgb(uint8_t red, uint8_t green, uint8_t blue, uint8_t lum)
{
	spi_write(lum | 0b11100000);
	spi_write(blue); //blue
	spi_write(green); //green
	spi_write(red); //red
}

void spi_set_led(uint8_t red, uint8_t green, uint8_t blue, uint8_t lum, uint8_t led)
{
	spi_send_start();
	if (led == LED_D6)
		spi_set_led_rgb(red, green, blue, lum);
	else
		spi_set_led_rgb(0, 0, 0, 0);
	
	if (led == LED_D7)
		spi_set_led_rgb(red, green, blue, lum);
	else
		spi_set_led_rgb(0, 0, 0, 0);
	
	if (led == LED_D8)
		spi_set_led_rgb(red, green, blue, lum);
	else
		spi_set_led_rgb(0, 0, 0, 0);
	spi_send_end();
}