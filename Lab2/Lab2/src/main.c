#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
//#include "power_clocks_lib.h"
#include "gpio.h"
//#include "usart.h"

#define CLK_FREQUENCY_FOUND 115200
// The Maximum number of pins
#define GPIO_MAX_PIN_NUMBER 32
// The number for which port the led is located on
#define LED_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// The pin number for each led
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED1_PIN (LED1_GPIO & ( GPIO_MAX_PIN_NUMBER -1))

// Bit value for each led
#define LED0_BIT_VALUE (1 << LED0_PIN)
#define LED1_BIT_VALUE (1 << LED1_PIN)


// Port for the button
#define BUTTON_PORT (GPIO_PUSH_BUTTON_2 / GPIO_MAX_PIN_NUMBER)
// Pin for the button
#define BUTTON2_PIN (1 << (GPIO_PUSH_BUTTON_2 & (GPIO_MAX_PIN_NUMBER - 1)))

#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000  // PBA clock target frequency, in Hz
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA

#define USART_1_STOPBIT               AVR32_USART_MR_NBSTOP_1   //!< Use 1 stop bit.
#define USART_NO_PARITY               AVR32_USART_MR_PAR_NONE   //!< Don't use a parity bit.
#define USART_NORMAL_CHMODE           AVR32_USART_MR_CHMODE_NORMAL      //!< Normal communication.

#define AVR32_USART_THR_TXCHR_MASK                           0x000001ff
#define AVR32_USART_THR_TXCHR_OFFSET                                  0

typedef struct
{
	//! Set baud rate of the USART (unused in slave modes).
	unsigned long baudrate;

	//! Number of bits to transmit as a character (5 to 9).
	unsigned char charlength;

	//! How to calculate the parity bit: \ref USART_EVEN_PARITY, \ref USART_ODD_PARITY,
	//! \ref USART_SPACE_PARITY, \ref USART_MARK_PARITY, \ref USART_NO_PARITY or
	//! \ref USART_MULTIDROP_PARITY.
	unsigned char paritytype;

	//! Number of stop bits between two characters: \ref USART_1_STOPBIT,
	//! \ref USART_1_5_STOPBITS, \ref USART_2_STOPBITS or any number from 3 to 257
	//! which will result in a time guard period of that length between characters.
	//! \note \ref USART_1_5_STOPBITS is supported in asynchronous modes only.
	unsigned short stopbits;

	//! Run the channel in testmode: \ref USART_NORMAL_CHMODE, \ref USART_AUTO_ECHO,
	//! \ref USART_LOCAL_LOOPBACK or \ref USART_REMOTE_LOOPBACK.
	unsigned char channelmode;
} usart_options_t;

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

char USART_getChar(volatile avr32_usart_t * usart)
{
	return usart->RHR.rxchr;
	
}
void USART_putChar(volatile avr32_usart_t * usart, char c)
{
	usart->THR.txsynh = 0;
	usart->thr = c;
}

void USART_reset(volatile avr32_usart_t * usart)
{
		usart->mr = 0;
		usart->rtor = 0;
		usart->ttgr = 0;
		
		usart->cr = AVR32_USART_CR_RSTRX_MASK   |
		AVR32_USART_CR_RSTTX_MASK   |
		AVR32_USART_CR_RSTSTA_MASK  |
		AVR32_USART_CR_RSTIT_MASK   |
		AVR32_USART_CR_RSTNACK_MASK |
		AVR32_USART_CR_RTSDIS_MASK;
}

// Function to create a delay (not in milliseconds)
void mdelay(int milliseconds)
{
	while (milliseconds != 0)
	{
		milliseconds--;
	}
}

/*! \brief This is an example demonstrating the USART RS232 TX and RX
 *         functionalities using the USART driver.
 */

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
	usart->CR.txen          = 1; // enable transmit
	usart->CR.rxdis         = 0;
	usart->CR.rxen          = 1; // enable receive
	usart->CR.rsttx         = 0;
	usart->CR.rstrx         = 0;
	
	// BaudRateGeneratorRegister
	//usart->brgr = BAUD_SETTINGS;
	usart->BRGR.fp			= 2;
	usart->BRGR.cd          = 1;
	
	volatile avr32_gpio_port_t * usart_gpio;
	usart_gpio = &AVR32_GPIO.port[USART_RXD_PIN/GPIO_MAX_PIN_NUMBER];
	usart_gpio->pmr0c = 1 << (USART_RXD_PIN & 0x1F);
	usart_gpio->pmr1c = 1 << (USART_RXD_PIN & 0x1F);
	usart_gpio->gperc = 1 << (USART_RXD_PIN & 0x1F);
	
	usart_gpio->pmr0c = 1 << (USART_TXD_PIN & 0x1F);
	usart_gpio->pmr1c = 1 << (USART_TXD_PIN & 0x1F);
	usart_gpio->gperc = 1 << (USART_TXD_PIN & 0x1F);
}

int main(void)
{
	//pm_switch_to_osc0(&AVR32_PM, fcrystal, startup);
	static const gpio_map_t USART_GPIO_MAP =
	{
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	// USART options.
	/*static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = 9600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};*/

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP,
			sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));
			
	// Initialize USART in RS232 mode.
	//usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, EXAMPLE_TARGET_PBACLK_FREQ_HZ);			
	USART_reset(EXAMPLE_USART);
	USART_init(EXAMPLE_USART);		
	
	// Initialization
	initLED();
	initButton();
	//volatile avr32_usart_t * usart;
	//usart = USART0;
	char *a = "bra";
	int c = a[1];
	//char char_read;
	
	//USART_init(usart);
	
	//usart->thr  = 0x00;
	
	/*int csr = usart->csr;
	int cr = usart->cr;
	int mr = usart->mr;
	int brgr = usart->brgr;
	if(usart->CSR.txrdy == 1)
	{
		USART_putChar(usart, c);
	}*/
	
	while(1)
	{
		if((EXAMPLE_USART->csr & AVR32_USART_CSR_TXRDY_MASK) != 0)
		{
			//EXAMPLE_USART->thr = (c << AVR32_USART_THR_TXCHR_OFFSET) & AVR32_USART_THR_TXCHR_MASK;
			EXAMPLE_USART->thr = (c << AVR32_USART_THR_TXCHR_OFFSET) & AVR32_USART_THR_TXCHR_MASK;
		}
		/*if(EXAMPLE_USART->csr & AVR32_USART_CSR_TXRDY_MASK == 1)
		{
			EXAMPLE_USART->thr = (c << AVR32_USART_THR_TXCHR_OFFSET) & AVR32_USART_THR_TXCHR_MASK;
		}
		if(EXAMPLE_USART->CSR.txrdy == 1)
		{
			EXAMPLE_USART->thr = (c++ << AVR32_USART_THR_TXCHR_OFFSET) & AVR32_USART_THR_TXCHR_MASK;
			EXAMPLE_USART->thr = c++;
		}*/
		/*if (usart->CSR.rxrdy == 1)
		{
			char_read = USART_getChar(usart);
			AVR32_GPIO.port[LED_PORT].ovrt = LED0_BIT_VALUE; // Toggle output value to make the led toggle
		}*/
		//char_read = AVR32_GPIO.port[0].pvr & (1 << (USART0_RXD_PIN & ( GPIO_MAX_PIN_NUMBER -1))); // Read input
		/*if(char_read != 0)
		{
			AVR32_GPIO.port[LED_PORT].ovrt = LED0_BIT_VALUE; // Toggle output value to make the led toggle
		}*/
		mdelay(1000);
	}
	while(1);
}

/*void USART_init(volatile avr32_usart_t * usart)
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
	usart->CR.rstrx         = 1;
	
	// BaudRateGeneratorRegister
	//usart->brgr = BAUD_SETTINGS;
	usart->BRGR.fp			= 0;
	usart->BRGR.cd          = 430;
	
	volatile avr32_gpio_port_t * usart_gpio;
	// pointing to the correct port
	usart_gpio = &AVR32_GPIO.port[0];
	usart_gpio->gpers = (1 << (USART0_RXD_PIN & ( GPIO_MAX_PIN_NUMBER -1)));
	usart_gpio->oderc = (1 << (USART0_RXD_PIN & ( GPIO_MAX_PIN_NUMBER -1)));
	
	usart_gpio->gpers = (1 << (USART0_TXD_PIN & ( GPIO_MAX_PIN_NUMBER -1)));
	usart_gpio->oders = (1 << (USART0_TXD_PIN & ( GPIO_MAX_PIN_NUMBER -1)));
	usart_gpio->ovrs  = (1 << (USART0_TXD_PIN & ( GPIO_MAX_PIN_NUMBER -1)));
}*/