#include "BUTTONS.h"

void initButton (void)
{
	/*
	* First make access to the GPIO port registers
	* shorter to type , just for our convenience
	*/
	volatile avr32_gpio_port_t * button_port;
	button_port = &AVR32_GPIO.port[BUTTON_PORT];

	// Enable button 0 in the GPIO register set
	button_port->gpers = BUTTON0_PIN;
	// Clear output drive enable to make it an input
	button_port->oderc = BUTTON0_PIN;
	
	button_port->gpers = BUTTON1_PIN;
	button_port->oderc = BUTTON1_PIN;
	
}

int buttonIsPressed(int buttonNr)
{
	int buttonState = AVR32_GPIO.port[BUTTON_PORT].pvr & (buttonNr);
	
	if(buttonState == 0)
		return 1;
	else
		return 0;
}