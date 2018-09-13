#include "board.h"
#include "compiler.h"
#include <unistd.h>

// The Maximum number of pins
#define GPIO_MAX_PIN_NUMBER 32
// The number for which port the led is located on
#define LED_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// The pin number for each led
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))

// Bit value for each led
#define LED0_BIT_VALUE (1 << LED0_PIN)


// Port for the button
#define BUTTON_PORT (GPIO_PUSH_BUTTON_2 / GPIO_MAX_PIN_NUMBER)
// Pin for the button
#define BUTTON2_PIN (1 << (GPIO_PUSH_BUTTON_2 & (GPIO_MAX_PIN_NUMBER - 1)))

#define MODE_SETTINGS 0x08C0

#define CONTROL_SETTINGS 0x80 //Tx Enable, Rx Enable

#define BAUD_SETTINGS 0x01AE // CD = CLKfq / (16*9600) //CLKfq = 66MHz

// Initializes the LEDs
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

void USART_init(volatile avr32_usart_t * usart)
{
	//usart->mr = MODE_SETTINGS;
	// Mode register init
	usart->MR.onebit		= 0;
    usart->MR.modsync		= 0;
    usart->MR.man			= 0;
    usart->MR.filter		= 0;
    usart->MR.max_iteration = 0;
    usart->MR.var_sync		= 0;
    usart->MR.dsnack		= 0;
    usart->MR.inack			= 1;
    usart->MR.over          = 0;
    usart->MR.clko          = 1;
    usart->MR.mode9         = 0;
    usart->MR.msbf          = 0;
    usart->MR.chmode        = 0; //2bits
    usart->MR.nbstop        = 0;
    usart->MR.par           = 4;
    usart->MR.sync          = 1;
    usart->MR.chrl          = 3;
    usart->MR.usclks        = 0;
    usart->MR.mode          = 0;
	
	// Control register init
	//usart->cr = CONTROL_SETTINGS;
	usart->CR.rtsdis		= 0;
	usart->CR.rtsen         = 0;
	usart->CR.retto         = 0;
	usart->CR.rstnack       = 0;
	usart->CR.rstit         = 0;
	usart->CR.senda         = 0;
	usart->CR.sttto         = 0;
	usart->CR.stpbrk        = 0;
	usart->CR.sttbrk        = 0;
	usart->CR.rststa        = 0;
	usart->CR.txdis         = 0;
	usart->CR.txen          = 1;
	usart->CR.rxdis         = 0;
	usart->CR.rxen          = 1;
	usart->CR.rsttx         = 0;
	usart->CR.rstrx         = 0;
	
	// BaudRateGeneratorRegister
	//usart->brgr = BAUD_SETTINGS;
	usart->BRGR.fp			= 0;
	usart->BRGR.cd          = 430;
}

char USART_getChar()
{
	
	
}
void USART_putChar(char c)
{
	
}

void USART_reset()
{
	
}

// Function to create a delay (not in milliseconds)
void mdelay(int milliseconds)
{
	while (milliseconds != 0)
	{
		milliseconds--;
	}
}
int main (void)
{
	// Initialization
	initLED();
	initButton();
	volatile avr32_usart_t * usart;
	usart = USART0;
	
	USART_init(usart);
	
	usart->thr  = 0x00;
	
	int csr = usart->csr;
	int cr = usart->cr;
	int mr = usart->mr;
	int brgr = usart->brgr;
	
	while(1)
	{
		csr = usart->csr;
		
	}
	while(1);
}