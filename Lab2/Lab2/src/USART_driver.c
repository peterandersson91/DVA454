#include "USART_driver.h"

void USART_init(volatile avr32_usart_t * usart)
{
	USART_reset(); //Resets the mode register and the Control register.
	// Mode register init
	usart->MR.onebit		= 0;
	usart->MR.modsync		= 0;
	usart->MR.man			= 0;
	usart->MR.filter		= 0;
	usart->MR.max_iteration = 0;
	usart->MR.var_sync		= 0;
	usart->MR.dsnack		= 0;
	usart->MR.inack			= 0;
	usart->MR.over          = 0;
	usart->MR.clko          = 0;
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
	usart->CR.txen          = 1; // enable transmit
	usart->CR.rxdis         = 0;
	usart->CR.rxen          = 1; // enable receive
	usart->CR.rsttx         = 0;
	usart->CR.rstrx         = 0;
	
	// BaudRateGeneratorRegister
	//Clock frequency = 115200 Hz
	usart->BRGR.fp			= 0;
	usart->BRGR.cd          = 12;
	
	volatile avr32_gpio_port_t * usart_gpio;
	usart_gpio = &AVR32_GPIO.port[USART_RXD_PIN/GPIO_MAX_PIN_NUMBER];
	usart_gpio->pmr0c = 1 << (USART_RXD_PIN & 0x1F); //Peripheral MUX register
	usart_gpio->pmr1c = 1 << (USART_RXD_PIN & 0x1F); //1 << (5 & 31)
	usart_gpio->gperc = 1 << (USART_RXD_PIN & 0x1F); //Disables GPIO on receiver pin.
	
	usart_gpio->pmr0c = 1 << (USART_TXD_PIN & 0x1F); //1 << (6 & 31)
	usart_gpio->pmr1c = 1 << (USART_TXD_PIN & 0x1F);
	usart_gpio->gperc = 1 << (USART_TXD_PIN & 0x1F); //Disables GPIO on transmitter pin.
}

char USART_getChar()
{
	volatile avr32_usart_t * usart = USART;
	return usart->RHR.rxchr;
	
}
void USART_putChar(char c)
{
	volatile avr32_usart_t * usart = USART; 

	if((USART->csr & AVR32_USART_CSR_TXRDY_MASK) != 0)
	{
		USART->THR.txchr = c; //( c << AVR32_USART_THR_TXCHR_OFFSET) & AVR32_USART_THR_TXCHR_MASK;
	}
	
}

void USART_reset()
{
	volatile avr32_usart_t * usart = USART;
	usart->mr = 0;	 //Reset Mode register	
	
	//Reset Control register
	usart->cr = AVR32_USART_CR_RSTRX_MASK   |
	AVR32_USART_CR_RSTTX_MASK   |
	AVR32_USART_CR_RSTSTA_MASK  |
	AVR32_USART_CR_RSTIT_MASK   |
	AVR32_USART_CR_RSTNACK_MASK |
	AVR32_USART_CR_RTSDIS_MASK;
}