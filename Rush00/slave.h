#ifndef SLAVE_H
# define SLAVE_H
# include "tool.h"
# include "tool_main.h"
# include "i2c.h"

void init_slave();

uint8_t light_countdown_slave();

void slave_mode();

#endif