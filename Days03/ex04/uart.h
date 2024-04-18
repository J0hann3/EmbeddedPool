#ifndef UART_H
# define UART_H

void uart_init();

unsigned char uart_rx(void);

void uart_tx(char c);

void uart_printstr(const char* str);

// ISR(USART_RX_vect) => get_interrupt

#endif