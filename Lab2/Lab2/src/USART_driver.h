#ifndef USART_DRIVER_H_
#define USART_DRIVER_H_

#include "evk1100.h"

// The Maximum number of pins
#define GPIO_MAX_PIN_NUMBER 32

#  define USART                 (&AVR32_USART1)
#  define USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION

#define AVR32_USART_THR_TXCHR_MASK                           0x000001ff
#define AVR32_USART_THR_TXCHR_OFFSET                                  0


//Functions defines
void USART_init(volatile avr32_usart_t * usart);
char USART_getChar();
void USART_putChar(char c);
void USART_reset();

#endif /* USART_DRIVER_H_ */