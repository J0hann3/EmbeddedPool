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

#define RED 0
#define GREEN 1
#define BLUE 2

typedef struct
{
	uint8_t lum;
	uint8_t blue;
	uint8_t green;
	uint8_t red;
} color_t;

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


int main()
{
	cli();
	spi_init();
	RESET(DDRC, PC0);
	init_analog();
	set_led(0, 0, 0, 0, RESET_LED);

	RESET(DDRD, PD2);
	RESET(DDRD, PD4);
	uint8_t color = RED;
	uint8_t led = LED_D6;
	color_t tab_color[3] = {0};
	
	while(1)
	{
		if ((PIND & (1 << PD2)) == 0)
		{
			color++;
			if (color > BLUE)
				color = RED;
			_delay_ms(300);
		}
		if ((PIND & (1 << PD4)) == 0)
		{
			led++;
			if (led > LED_D8)
				led = LED_D6;
			_delay_ms(300);
		}
		else
		{
			uint8_t tmp = read_analog(PC0);
			if (color == RED)
				tab_color[led].red = tmp;
			else if (color == GREEN)
				tab_color[led].green = tmp;
			else if (color == BLUE)
				tab_color[led].blue = tmp;
			_delay_ms(25);
			send_start();
			set_led_rgb(tab_color[LED_D6].red, tab_color[LED_D6].green, tab_color[LED_D6].blue, 0x01);
			set_led_rgb(tab_color[LED_D7].red, tab_color[LED_D7].green, tab_color[LED_D7].blue, 0x01);
			set_led_rgb(tab_color[LED_D8].red, tab_color[LED_D8].green, tab_color[LED_D8].blue, 0x01);
			send_end();
		}

	}
}
