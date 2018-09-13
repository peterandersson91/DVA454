#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "usart.h"

#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ FOSC0  // PBA clock target frequency, in Hz

#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA

/*! \brief This is an example demonstrating the USART RS232 TX and RX
 *         functionalities using the USART driver.
 */
int main(void)
{
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	static const gpio_map_t USART_GPIO_MAP =
	{
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	// USART options.
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = 9600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP,
			sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode.
	usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, EXAMPLE_TARGET_PBACLK_FREQ_HZ);

	// Hello world!
	usart_write_line(EXAMPLE_USART, "Hello, this is the AVR UC3 MCU saying hello! (press enter)\r\n");

	// Press enter to continue.
	while (usart_get_echo_line(EXAMPLE_USART) == USART_FAILURE);  // Get and echo characters until end of line.

	usart_write_line(EXAMPLE_USART, "Goodbye.\r\n");

	//*** Sleep mode
	// This program won't be doing anything else from now on, so it might as well
	// sleep.
	// Modules communicating with external circuits should normally be disabled
	// before entering a sleep mode that will stop the module operation.
	// Make sure the USART dumps the last message completely before turning it off.
	while(!usart_tx_empty(EXAMPLE_USART));
	pcl_disable_module(EXAMPLE_USART_CLOCK_MASK);

	// Since we're going into a sleep mode deeper than IDLE, all HSB masters must
	// be stopped before entering the sleep mode.
	pcl_disable_module(EXAMPLE_PDCA_CLOCK_HSB);
	pcl_disable_module(EXAMPLE_PDCA_CLOCK_PB);

	// If there is a chance that any PB write operations are incomplete, the CPU
	// should perform a read operation from any register on the PB bus before
	// executing the sleep instruction.
	AVR32_INTC.ipr[0];  // Dummy read

	// Go to STATIC sleep mode.
	SLEEP(AVR32_PM_SMODE_STATIC);

	while (true);
}
