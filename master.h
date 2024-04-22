#ifndef MASTER_H
# define MASTER_H
# include "tool.h"
# include "tool_main.h"
# include "i2c.h"

unsigned int get_random_number();
uint8_t stair_leds();
uint8_t check_button();
void master_mode();

#endif