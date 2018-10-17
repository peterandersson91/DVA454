#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "board.h"
#include "compiler.h"

#define GPIO_MAX_PIN_NUMBER 32

// The port for the buttons
#define BUTTON_PORT (GPIO_PUSH_BUTTON_0 / GPIO_MAX_PIN_NUMBER)
// The bit position of the GPIO pin to button 0.
#define BUTTON0_PIN (1 << (GPIO_PUSH_BUTTON_0 & (GPIO_MAX_PIN_NUMBER - 1)))
// The bit position of the GPIO pin to button 1.
#define BUTTON1_PIN (1 << (GPIO_PUSH_BUTTON_1 & (GPIO_MAX_PIN_NUMBER - 1)))

#define BUTTON2_PIN (1 << (GPIO_PUSH_BUTTON_2 & (GPIO_MAX_PIN_NUMBER - 1)))

void initButton (void);
int buttonIsPressed(int buttonNr);

#endif /* BUTTONS_H_ */