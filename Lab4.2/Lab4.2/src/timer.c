#include "timer.h"

// Init the timer
void TC_init()
{
	TC0_REG_CMR |= (0x01 << 1); // CLK selection, clock 3
	TC0_REG_CMR |= (0x01 << 14); // RC compare trigger enable
	TC0_REG_CMR |= (0x01 << 15); // WAVEFORM MODE
}

// Starts the timer
void TC_start()
{
	TC0_REG_CCR |= (0x01 << 0); // Enable clock 
	TC0_REG_CCR |= (0x01 << 2); // Reset counter and the clock starts
}// Returns the counter valueint get_tc_cv(){	return TC0_REG_CV;}