#ifndef LED_H_
#define LED_H_

#include "board.h"
#include "compiler.h"

/* Define the ports and pins to be used */
// The maximum number of pins in a port
#define GPIO_MAX_PIN_NUMBER 32
// This is the port which LED0_GPIO is located on
#define LED0_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// This is the bit position of the GPIO pin
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
// This is a 1 - bit written to the bit position of the GPIO pin
#define LED0_BIT_VALUE (1 << LED0_PIN)

#define LED1_PIN (LED1_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
// This is a 1 - bit written to the bit position of the GPIO pin
#define LED1_BIT_VALUE (1 << LED1_PIN)

#define LED2_PIN (LED2_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
// This is a 1 - bit written to the bit position of the GPIO pin
#define LED2_BIT_VALUE (1 << LED2_PIN)

void initLED(void);
void toggleLED(int ledNr);
void onLED(int ledNr);
void offLED(int ledNr);

#endif /* LED_H_ */