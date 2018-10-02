#include "timer.h"

// Init the timer
void TC_init()
{
	TC0_REG_CMR |= (0x01 << 1); // CLK selection, clock 3
	TC0_REG_CMR |= (0x01 << 14); // WAVESEL 10
	TC0_REG_CMR |= (0x01 << 15); // WAVEFORM MODE
	
	// configure enabled interrupts
	TC0_REG_IER = 0xFF;
	TC0_REG_IDR = 0x00;
	
	TC0_REG_SR; // Clears all flags
}

// Starts the timer
void TC_start()
{
	TC0_REG_CCR |= (0x01 << 0); // Enable clock 
	TC0_REG_CCR |= (0x01 << 2); // Reset counter and the clock starts
}