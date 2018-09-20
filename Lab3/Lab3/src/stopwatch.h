#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include "TC_driver.h"

// The number for which port the led is located on
#define LED_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// The pin number for each led
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED1_PIN (LED1_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED4_PIN (LED4_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED5_PIN (LED5_GPIO & ( GPIO_MAX_PIN_NUMBER -1))

// Bit value for each led
#define LED0_BIT_VALUE (1 << LED0_PIN)
#define LED1_BIT_VALUE (1 << LED1_PIN)
#define LED4_BIT_VALUE (1 << LED4_PIN)
#define LED5_BIT_VALUE (1 << LED5_PIN)


// Port for the button
#define BUTTON_PORT (GPIO_PUSH_BUTTON_2 / GPIO_MAX_PIN_NUMBER)
// Pin for the button
#define BUTTON0_PIN (1 << (GPIO_PUSH_BUTTON_0 & (GPIO_MAX_PIN_NUMBER - 1)))
#define BUTTON1_PIN (1 << (GPIO_PUSH_BUTTON_1 & (GPIO_MAX_PIN_NUMBER - 1)))
#define BUTTON2_PIN (1 << (GPIO_PUSH_BUTTON_2 & (GPIO_MAX_PIN_NUMBER - 1)))

extern bool timer_running;

void stopwatch_init(void);
void startTimer(void);
void stopTimer(void);
void resetTimer(void);
void checkInput(void);
void displayTime(struct time_struct ts);
void initLED (void);


#endif /* STOPWATCH_H_ */