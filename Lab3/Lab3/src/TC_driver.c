#include "TC_driver.h"

int centa_s = 0;

// Function to translate hundredths into hours, minutes, seconds and hundredths
struct time_struct update_time(int current_centa_s)
{
	struct time_struct ts;
	ts.hour = 0;
	ts.minute = 0;
	ts.second = 0;
	ts.centa = 0;
	ts.centa = current_centa_s % 100;
	current_centa_s /= 100;
	ts.second = current_centa_s % 60;
	current_centa_s /= 60;
	ts.minute = current_centa_s % 60;
	current_centa_s /= 60;
	ts.hour = current_centa_s;
	
	return ts;
}

// Interrupt function. Counts a global variable once in every interrupt (Every 0.01 second)
__attribute__((__interrupt__))static void tc_irq_handler(void)
{
	centa_s++;

	// Clear the interrupt flag
	tc_read_sr(&AVR32_TC, TC_CHANNEL);
}

void tc_init(volatile avr32_tc_t *tc)
{
	// Options for waveform generation.
	static const tc_waveform_opt_t waveform_opt = {
		// Channel selection.
		.channel  = TC_CHANNEL,
		 /* Waveform selection: Up mode with automatic trigger(reset)
		 * on RC compare.*/
		.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
		// External event trigger enable.
		.enetrg   = false,
		// External event selection.
		.eevt     = 0,
		// External event edge selection.
		.eevtedg  = TC_SEL_NO_EDGE,
		// Counter disable when RC compare.
		.cpcdis   = false,
		// Counter clock stopped with RC compare.
		.cpcstop  = false,
		// Burst signal selection.
		.burst    = false,
		// Clock inversion.
		.clki     = false,
		// Internal source clock 3, connected to fPBA / 8.
		.tcclks   = TC_CLOCK_SOURCE_TC3
	};

	// Options for enabling TC interrupts
	static const tc_interrupt_t tc_interrupt = {
		.cpcs  = 1, // Enable interrupt on RC compare alone
	};
	// Initialize the timer/counter.
	tc_init_waveform(tc, &waveform_opt);
	// Set trigger to every 10ms, Rc = (115200/8) / 100 = 144
	tc_write_rc(tc, TC_CHANNEL, (115200 / 8 / 100));
	// configure the timer interrupt
	tc_configure_interrupts(tc, TC_CHANNEL, &tc_interrupt);
	// Start the timer/counter.
	tc_start(tc, TC_CHANNEL);
	//Disable the interrupts
	Disable_global_interrupt();
	//Register the RTC interrupt handler to the interrupt controller.
	INTC_init_interrupts();
	INTC_register_interrupt(&tc_irq_handler, AVR32_TC_IRQ0, AVR32_INTC_INT0);
}