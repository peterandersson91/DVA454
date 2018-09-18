#include "board.h"
#include "compiler.h"
#include <unistd.h>

// The Maximum number of pins
#define GPIO_MAX_PIN_NUMBER 32
// The number for which port the led is located on
#define LED0_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// The pin number for the led
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
// Bit value for the led
#define LED0_BIT_VALUE (1 << LED0_PIN)
// Port for the buttons
#define BUTTON_PORT (GPIO_PUSH_BUTTON_0 / GPIO_MAX_PIN_NUMBER)
// Pin for the buttons
#define BUTTON0_PIN (1 << (GPIO_PUSH_BUTTON_0 & (GPIO_MAX_PIN_NUMBER - 1)))
#define BUTTON1_PIN (1 << (GPIO_PUSH_BUTTON_1 & (GPIO_MAX_PIN_NUMBER - 1)))

// Initializes the LEDs
void initLED (void)
{
	// LED port pointer
	volatile avr32_gpio_port_t * led0_port;
	// pointing to the correct port
	led0_port = &AVR32_GPIO.port[LED0_PORT];
	// Enables the registers for the LED
	led0_port->gpers = LED0_BIT_VALUE;
	// Turning the LED off
	led0_port->ovrs = LED0_BIT_VALUE;
	// Setting the ledpin to output
	led0_port->oders = LED0_BIT_VALUE;
}// Initializes the buttonvoid initButton (void)
{
	// Button port pointer
	volatile avr32_gpio_port_t * button_port;
	button_port = &AVR32_GPIO.port[BUTTON_PORT];

	// Enables the register
	button_port->gpers = BUTTON0_PIN;
	// Sets the pin to an input
	button_port->oderc = BUTTON0_PIN;
	
	button_port->gpers = BUTTON1_PIN;
	button_port->oderc = BUTTON1_PIN;
	
}// Function to create a delay (not in milliseconds)void mdelay(int milliseconds) // delay by executing this function x times{	while (milliseconds != 0)	{		milliseconds--;	}	}int main (void)
{
	initLED();
	initButton();
	int button0_state, button1_state; // variables to see if the button has been pushed
	int button_changed = 0; // Variable to make sure the button has been released before pushed again
	while(1) // Infinite loop
	{
		button0_state = AVR32_GPIO.port[BUTTON_PORT].pvr & (BUTTON0_PIN); // Read input from button 0
		button1_state = AVR32_GPIO.port[BUTTON_PORT].pvr & (BUTTON1_PIN); // Read input from button 1
	
		/* Main loop that will toggle a single bit on the GPIO port
		*/
		if (button0_state == 0) // True when pushed
		{
			while(button0_state == 0) // True as long as the button is pushed
			{
				AVR32_GPIO.port[LED0_PORT].ovrc = LED0_BIT_VALUE; // Clear output value to make the led light
				button0_state = AVR32_GPIO.port[BUTTON_PORT].pvr & (BUTTON0_PIN); // Read input from button 0				
				
			}
			AVR32_GPIO.port[LED0_PORT].ovrs = LED0_BIT_VALUE; // Set output value to make the led turn off
		}
		
		if(button1_state == 0) // True when pushed
		{
			button_changed = 1;
			AVR32_GPIO.port[LED0_PORT].ovrt = LED0_BIT_VALUE; // Toggle output value to make the led toggle
			while(button_changed == 1) // True as long as the button is pushed
			{
				button1_state = AVR32_GPIO.port[BUTTON_PORT].pvr & (BUTTON1_PIN); // Read input from button 1
				if(button1_state != 0) // When button is released it will change the variable to tell the program to exit this loop
					button_changed = 0;
			}
		}
	}
	while (1);
}