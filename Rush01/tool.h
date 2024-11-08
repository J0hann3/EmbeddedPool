#ifndef TOOL_H
# define TOOL_H

# define __AVR_ATmega328P__ 1

# include <avr/io.h>
# include <util/delay.h>
# include <avr/interrupt.h>

#define SW1 PB2
#define SW2 PB4
#define SW3 0
#define SW1_STATE 0
#define SW2_STATE 1
#define SW3_STATE 2

#define RED_D5 PB5
#define GREEN_D5 PB6
#define BLUE_D5 PB3

#define D9 3
#define D10 2
#define D11 1

#define MAX_MODE 11
#define MIN_MODE 0

#define BIT_1 0b00000001
#define BIT_2 0b00000010
#define BIT_3 0b00000100
#define BIT_4 0b00001000
#define LIGHT_BITS 0b00010111

# define SET(PORT, N) ((PORT) |= (1 << (N)))
# define RESET(PORT, N) ((PORT) &= ~(1 << (N)))
# define TOGGLE(PORT, N) ((PORT) ^= (1 << (N)))

extern volatile uint16_t sensor_counter;
extern volatile uint16_t reset_sensor_counter;
extern volatile uint16_t led_rgb_counter;
extern volatile uint16_t reset_led_rgb_counter;
extern volatile uint8_t get_date;
extern volatile uint16_t counter_start;

void mode_0();
void mode_1();
void mode_2();
void mode_3();
void mode_4();
void mode_5();
void mode_6();
void mode_7();
void mode_8();
void mode_9();
void mode_10();
void mode_11();

typedef struct
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t day;
	uint8_t month;
	uint16_t year;
} date_t;

void update_date();

void init_timer();

#endif