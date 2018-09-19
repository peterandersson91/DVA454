#ifndef USART_DRIVER_H_
#define USART_DRIVER_H_

#include "evk1100.h"

// The Maximum number of pins
#define GPIO_MAX_PIN_NUMBER 32

//Functions defines
void USART_init(volatile avr32_usart_t * usart); // Initializes the registers
char USART_getChar(); // Reads char from RHR
void USART_putChar(char c); // Writes char to THR
void USART_reset(); // Resets CR and MR
USART_getString(char input_string[]); // Reads a string from RHR
void USART_putString(char output_string[]); // Writes a string to THR
#endif /* USART_DRIVER_H_ */