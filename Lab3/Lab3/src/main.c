#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "USART_driver.h"

// The number for which port the led is located on
#define LED_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// The pin number for each led
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED1_PIN (LED1_GPIO & ( GPIO_MAX_PIN_NUMBER -1))

// Bit value for each led
#define LED0_BIT_VALUE (1 << LED0_PIN)
#define LED1_BIT_VALUE (1 << LED1_PIN)


// Port for the button
#define BUTTON_PORT (GPIO_PUSH_BUTTON_2 / GPIO_MAX_PIN_NUMBER)
// Pin for the button
#define BUTTON2_PIN (1 << (GPIO_PUSH_BUTTON_2 & (GPIO_MAX_PIN_NUMBER - 1)))

void initLED (void)
{
	// LED port pointer
	volatile avr32_gpio_port_t * led_port;
	// pointing to the correct port
	led_port = &AVR32_GPIO.port[LED_PORT];
	
	// Enables the registers for the LEDs
	led_port->gpers = LED0_BIT_VALUE;

	// Turning all LEDs off
	led_port->ovrs = LED0_BIT_VALUE;
	
	// setting the led pins to outputs
	led_port->oders = LED0_BIT_VALUE;
}
// Initializes the button
void initButton (void)
{
	// Button port pointer
	volatile avr32_gpio_port_t * button_port;
	button_port = &AVR32_GPIO.port[BUTTON_PORT];
	
	// Enables the register
	button_port->gpers = BUTTON2_PIN;
	// Sets the pin to an input
	button_port->oderc = BUTTON2_PIN;
	
}

// Function to create a delay (not in milliseconds)
void mdelay(int milliseconds)
{
	while (milliseconds != 0)
	{
		milliseconds--;
	}
}

int main(void)
{
	volatile avr32_usart_t * usart = USART;
	// Initialize USART
	USART_init(usart);
	
	// Initialization
	initLED();
	initButton();
	char input_string[64];
	
	while(1)
	{
		USART_getString(&input_string);
		USART_putString(input_string);
	}
	while(1);
}