#ifndef SPI_H
# define SPI_H
# include "tool.h"

#define MOSI PB3
#define SCK PB5
#define SS PB2

#define LED_D6 0
#define LED_D7 1
#define LED_D8 2
#define RESET_LED 3
#define ALL_LED 4

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

void spi_init();
void spi_disable();
// void spi_write(uint8_t data);
void spi_send_start();
void spi_send_end();
void spi_set_led_rgb(uint8_t red, uint8_t green, uint8_t blue, uint8_t lum);
void spi_set_led(uint8_t red, uint8_t green, uint8_t blue, uint8_t lum, uint8_t led);

#endif