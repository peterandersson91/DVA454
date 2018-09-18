#include "board.h"
#include "compiler.h"
#include <unistd.h>

// The Maximum number of pins
#define GPIO_MAX_PIN_NUMBER 32
// The number for which port the led is located on
#define LED_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// The pin number for each led
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED1_PIN (LED1_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED2_PIN (LED2_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED3_PIN (LED3_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED4_PIN (LED5_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED5_PIN (LED7_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
// Bit value for each led
#define LED0_BIT_VALUE (1 << LED0_PIN)
#define LED1_BIT_VALUE (1 << LED1_PIN)
#define LED2_BIT_VALUE (1 << LED2_PIN)
#define LED3_BIT_VALUE (1 << LED3_PIN)
#define LED4_BIT_VALUE (1 << LED4_PIN)
#define LED5_BIT_VALUE (1 << LED5_PIN)

// Port for the button
#define BUTTON_PORT (GPIO_PUSH_BUTTON_2 / GPIO_MAX_PIN_NUMBER)
// Pin for the button
#define BUTTON2_PIN (1 << (GPIO_PUSH_BUTTON_2 & (GPIO_MAX_PIN_NUMBER - 1)))

// Initializes the LEDs
void initLED (void)
{
	// LED port pointer
	volatile avr32_gpio_port_t * led_port;
	// pointing to the correct port
	led_port = &AVR32_GPIO.port[LED_PORT];
	
	// Enables the registers for the LEDs
	led_port->gpers = LED0_BIT_VALUE;
	led_port->gpers = LED1_BIT_VALUE;
	led_port->gpers = LED2_BIT_VALUE;
	led_port->gpers = LED3_BIT_VALUE;
	led_port->gpers = LED4_BIT_VALUE;
	led_port->gpers = LED5_BIT_VALUE;
	
	// Turning all LEDs off
	led_port->ovrs = LED0_BIT_VALUE;
	led_port->ovrs = LED1_BIT_VALUE;
	led_port->ovrs = LED2_BIT_VALUE;
	led_port->ovrs = LED3_BIT_VALUE;
	led_port->ovrs = LED4_BIT_VALUE;
	led_port->ovrs = LED5_BIT_VALUE;
	
	// Setting the ledpins to outputs
	led_port->oders = LED0_BIT_VALUE;
	led_port->oders = LED1_BIT_VALUE;
	led_port->oders = LED2_BIT_VALUE;
	led_port->oders = LED3_BIT_VALUE;
	led_port->oders = LED4_BIT_VALUE;
	led_port->oders = LED5_BIT_VALUE;
}// Function to create a delay (not in milliseconds)void mdelay(int milliseconds){	while (milliseconds != 0)	{		milliseconds--;	}	}
int main (void)
{
	// Initialization
	initLED();
	
	int led_pos = 0;		// Contains the current light led
	int led_dir = 1;		// Contains the direction of the led movement
	while(1)
	{
		// The switch controls which led to light up
		// When one led lights up, the ones adjacent to it turns off
		// led_pos controls which led to currently light up
		switch(led_pos)
		{
			case 0:
			{
				AVR32_GPIO.port[LED_PORT].ovrs = LED1_BIT_VALUE; // turns off LED1
				AVR32_GPIO.port[LED_PORT].ovrc = LED0_BIT_VALUE; // turns on LED0
				led_dir = 1;	// Changes direction
				break;				
			}
			case 1:
			{
				AVR32_GPIO.port[LED_PORT].ovrs = LED2_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrc = LED1_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrs = LED0_BIT_VALUE;		
				break;
			}			
			case 2:
			{
				AVR32_GPIO.port[LED_PORT].ovrs = LED3_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrc = LED2_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrs = LED1_BIT_VALUE;
				break;				
			}
			case 3:
			{
				AVR32_GPIO.port[LED_PORT].ovrs = LED4_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrc = LED3_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrs = LED2_BIT_VALUE;
				break;				
			}
			case 4:
			{
				AVR32_GPIO.port[LED_PORT].ovrs = LED5_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrc = LED4_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrs = LED3_BIT_VALUE;
				break;				
			}
			case 5:
			{
				AVR32_GPIO.port[LED_PORT].ovrc = LED5_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrs = LED4_BIT_VALUE;
				led_dir = 0;
				break;				
			}	
		}
		
		mdelay(1000); // Delay effect
		if(led_dir == 1) // if the direction is right led_dir is 1 and the position is increasing
			led_pos++;
		else
			led_pos--;
	}
	while(1);
}
