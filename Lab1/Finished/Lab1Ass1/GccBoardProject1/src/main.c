#include "board.h"
#include "compiler.h"
#include <unistd.h>

// The Maximum number of pins
#define GPIO_MAX_PIN_NUMBER 32
// The number for which port the led is located on
#define LED0_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// The pin number for each led
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
// Bit value for each led
#define LED0_BIT_VALUE (1 << LED0_PIN)

// Port for the button
#define BUTTON_PORT (GPIO_PUSH_BUTTON_0 / GPIO_MAX_PIN_NUMBER)
// Pin for the button
#define BUTTON0_PIN (1 << (GPIO_PUSH_BUTTON_0 & (GPIO_MAX_PIN_NUMBER - 1)))

// Initializes the LEDs
void initLED (void)
{
	// LED port pointer
	volatile avr32_gpio_port_t * led0_port;
	// pointing to the correct port
	led0_port = &AVR32_GPIO.port[LED0_PORT];
	
	// Enables the registers for the LEDs
	led0_port->gpers = LED0_BIT_VALUE;
	
	// Turning all LEDs off
	led0_port->ovrs = LED0_BIT_VALUE;
	
	// Setting the ledpins to outputs
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
}// Function to create a delay (not in milliseconds)void mdelay(int milliseconds){	while (milliseconds != 0){		milliseconds--;	}	}int main (void)
{
	// Initialization
	initLED();
	initButton();
	int button0_state;		// Contains the state of the button
	int button_changed = 0;	// 1 if the button has been pressed, 0 if the button has been released
	while(1){
		button0_state = AVR32_GPIO.port[BUTTON_PORT].pvr & (BUTTON0_PIN); //Reads status of button
		
		if (button0_state == 0 && button_changed == 0){
			button_changed = 1;
			while(1) {
				AVR32_GPIO.port[LED0_PORT].ovrt = LED0_BIT_VALUE; //Toggle LED0 
				mdelay(10000);
				button0_state = AVR32_GPIO.port[BUTTON_PORT].pvr & (BUTTON0_PIN); //Reads status of button		
						
				if (button0_state != 0){ //If button is released then enter
					button_changed = 0;
				}
				if (button0_state == 0 && button_changed == 0){ //If button is pushed once more
					button_changed = 1;
					AVR32_GPIO.port[LED0_PORT].ovrs = LED0_BIT_VALUE; //Turns the LED off
					break; //Exit the loop
				}
			}
		}
		if(button0_state != 0)
			button_changed = 0;
	}
	while (1);
}