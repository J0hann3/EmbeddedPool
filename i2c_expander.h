#ifndef I2C_EXPANDER_H
# define I2C_EXPANDER_H

#include "i2c.h"

#define A0 0
#define A1 1
#define A2 2
#define ADDRESS (0b00100000 | (0 << A2) | (0 << A1) | (0 << A0))

//number 7-segments
#define ZERO	0b00111111
#define ONE		0b00000110
#define TWO		0b01011011
#define THREE	0b01001111
#define FOUR	0b01100110
#define FIVE	0b01101101
#define SIX		0b01111101
#define SEVEN	0b00000111
#define HEIGHT	0b01111111
#define NINE	0b01101111
#define TIRET	0b01000000

#define DIGIT_1 0b11101111
#define DIGIT_2 0b11011111
#define DIGIT_3 0b10111111
#define DIGIT_4 0b01111111

void	i2c_exp_send_command(uint8_t address, uint8_t command, uint8_t arg1, uint8_t arg2, uint8_t nb_arg);
uint8_t	i2c_exp_read_value(uint8_t address, uint8_t command);
void	i2c_exp_toggle_pin(uint8_t address, uint8_t command, uint8_t pin);
void	i2c_exp_print_one_digit(uint8_t nb, uint8_t unity, uint8_t point);
void	i2c_exp_print_number(uint16_t nb);
void	i2c_exp_print_float(float nb);

#endif