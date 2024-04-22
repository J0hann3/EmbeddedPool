#ifndef TOOL_MAIN_H
# define TOOL_MAIN_H

#define LED_PORT PORTB
#define LED_DDR DDRB
#define ADDRESS 0x18
#define BUTTON_PRESS 0x10
#define LED_D1 0x12
#define LED_D2 0x13
#define LED_D3 0x14
#define LED_D4 0x16
#define GREEN_LED 0x17
#define BLUE_LED 0x18
#define RED_LED 0x19
#define WINNER 1
#define LOSER 0
#define CONTINUE 2

void set_red_light();
void set_green_light();
void set_blue_light();
void set_light();
void end_game(uint8_t res);

#endif