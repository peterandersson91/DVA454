#include <asf.h>
#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "TC_driver.h"

// The number for which port the led is located on
#define LED_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// The pin number for each led
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))

// Bit value for each led
#define LED0_BIT_VALUE (1 << LED0_PIN)

// Flag to update the timer value
volatile static bool update_timer = true;
// Variable to contain the time ticks occurred
volatile static uint32_t tc_tick = 0;

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

__attribute__((__interrupt__))static void tc_irq_handler(void)
{
	// Increment the ms seconds counter
	tc_tick++;

	// Clear the interrupt flag. This is a side effect of reading the TC SR.
	tc_read_sr(&AVR32_TC, TC_CHANNEL);

	// specify that an interrupt has been raised
	update_timer = true;
	// Toggle a GPIO pin (this pin is used as a regular GPIO pin).
	//LED_Toggle(LED0);
	AVR32_GPIO.port[LED_PORT].ovrt = LED0_BIT_VALUE;
}

int main (void)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
	volatile avr32_usart_t * usart = USART;
	uint32_t timer = 0;
	//char input_string[64] = {'n','i','\0'};
	char input_string[64];
	
	initLED();
	USART_init(usart);	
	
	//sysclk_init();
	// Enable the clock to the selected example Timer/counter peripheral module.
	//sysclk_enable_peripheral_clock(EXAMPLE_TC);
	// Initialize the USART module for trace messages
	//init_dbg_rs232(sysclk_get_pba_hz());
	// Disable the interrupts
	//cpu_irq_disable();
	
	//INTC_init_interrupts();
	/*Disable_global_interrupt();
	// Register the RTC interrupt handler to the interrupt controller.
	
	INTC_init_interrupts();
	INTC_register_interrupt(&tc_irq_handler, AVR32_TC_IRQ0, AVR32_INTC_INT0);
	Enable_global_interrupt();
	//cpu_irq_enable();
	
	tc_init(tc);*/
	
	while (1) {
		USART_getString(&input_string);
		USART_putString(input_string);
		// Update the display on USART every second.
		/*if ((update_timer) && (!(tc_tick%1000))) {
			timer++;
			//AVR32_GPIO.port[LED_PORT].ovrt = LED0_BIT_VALUE;
			// Set cursor to the position (1; 5)
			Disable_global_interrupt();
			USART_putString(input_string);
			Enable_global_interrupt();
			//USART_putString(timer);
			/*print_dbg("\x1B[5;1H");
			// Print the timer value
			print_dbg("ATMEL AVR UC3 - Timer/Counter Example 3\n\rTimer: ");
			print_dbg_ulong(timer);
			print_dbg(" s");*/
			// Reset the timer update flag to wait till next timer interrupt
			//update_timer = false;
		//}
	}
	
	while(1){}
}
