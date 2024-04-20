#ifndef I2C_H
# define I2C_H
# include <util/twi.h>
# include <stdlib.h>
# include "uart.h"

void i2c_init(void);

uint8_t i2c_start(uint8_t read, uint8_t address);

uint8_t i2c_restart(uint8_t read, uint8_t address);

void i2c_stop(void);

void i2c_write(unsigned char data);

unsigned char i2c_read(uint8_t ack);

#endif