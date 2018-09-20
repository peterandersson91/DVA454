#include "USART_driver.h"

void USART_init(volatile avr32_usart_t * usart)
{
	USART_reset(); //Resets the mode register and the Control register.
	// Mode register init
	usart->MR.onebit		= 0; //Start Frame Delimiter Selector - Start Frame delimiter is COMMAND or DATA SYNC.
	usart->MR.modsync		= 0; //Manchester Synchronization Mod - The Manchester Start bit is a 0 to 1 transition.
	usart->MR.man			= 0; //Manchester Encoder/Decoder Enable - Manchester Encoder/Decoder are disabled.
	usart->MR.filter		= 0; //Infrared Receive Line Filter - The USART does not filter the receive line.
	usart->MR.max_iteration = 0; //MAX_ITERATION - Defines the maximum number of iterations in mode ISO7816, protocol T= 0.
	usart->MR.var_sync		= 0; //Variable Synchronization of Command/Data Sync Start Frame Delimiter - User defined configuration of command or data sync field depending on SYNC value.
	usart->MR.dsnack		= 0; //Disable Successive NACK -  NACK is sent on the ISO line as soon as a parity error occurs in the received character (unless INACK is set).
	usart->MR.inack			= 0; //Inhibit Non Acknowledge - The NACK is generated.
	usart->MR.over          = 1; //Oversampling Mode - 8x Oversampling.
	usart->MR.clko          = 0; //Clock Output Select - The USART does not drive the CLK pin.
	usart->MR.mode9         = 0; //9-bit Character Length - CTRL defines the character length.
	usart->MR.msbf          = 0; //Bit Order or SPI Clock Polarity - Least Significant Bit is sent/received first.
	usart->MR.chmode        = 0; //Channel Mode - Normal Mode (0 0).
	usart->MR.nbstop        = 0; //Number of Stop Bits - 1 stop bit (0 0).
	usart->MR.par           = 4; //Parity Type - No parity (1 0).
	usart->MR.sync          = 0; //Synchronous Mode Select or SPI Clock Phase - USART operates in Asynchronous Mode.
	usart->MR.chrl          = 3; //Character Length - 8 bits (1 1).
	usart->MR.usclks        = 0; //Clock Selection - CLK_USART (0 0).
	usart->MR.mode          = 0; //Mode - Normal (0 0).
	
	// Control register init
	usart->CR.rtsdis		= 0; //Request to Send Disable/Release SPI Chip Select - No effect. Does not operate in Master/Slave mode.
	usart->CR.rtsen         = 0; //Request to Send Enable/Force SPI Chip Select - No effect. Does not operate in Master/Slave mode.
	usart->CR.retto         = 0; //Rearm Time-out - No effect.
	usart->CR.rstnack       = 0; //Reset Non Acknowledge - No effect.
	usart->CR.rstit         = 0; //Reset Iterations - No effect.
	usart->CR.senda         = 0; //Send Address - No effect.
	usart->CR.sttto         = 0; //Start Time-out - No effect.
	usart->CR.stpbrk        = 0; //Stop Break - No effect.
	usart->CR.sttbrk        = 0; //Start Break - No effect.
	usart->CR.rststa        = 0; //Reset Status Bits - No effect.
	usart->CR.txdis         = 0; //Transmitter Disable - No effect.
	usart->CR.txen          = 1; //Transmitter Enable. Enables the transmitter if TXDIS is 0.
	usart->CR.rxdis         = 0; //Receiver Disable - No effect.
	usart->CR.rxen          = 1; //Receiver Enable. Enables the receiver, if RXDIS is 0.
	usart->CR.rsttx         = 0; //Reset Transmitter - No effect.
	usart->CR.rstrx         = 0; //Reset Receiver - No effect.
	
	//BaudRateGeneratorRegister
	//Clock frequency = 115200 Hz
	//Baud Rate = 9600
	//CD + (FP/8) = Selected clock / (8 * Baudrate)
	usart->BRGR.fp			= 4; //Fractional Part.
	usart->BRGR.cd          = 1; //Clock Divider.
	
	volatile avr32_gpio_port_t * usart_gpio;
	//Set as peripheral ports instead of GPIO ports.
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
	while(usart->CSR.rxrdy == 0) //Checks the Control Status Register if there is a character received ready to be read.
	{}
	return usart->RHR.rxchr; // Returns the character in the Receive Holding Register.
}

void USART_putChar(char c)
{
	volatile avr32_usart_t * usart = USART;

	while(usart->CSR.txrdy == 0) //Checks the Control Status Register if ready to transmit.
	{		
	}
	USART->THR.txchr = c; //Sends a character to the transmit holding register ( c << AVR32_USART_THR_TXCHR_OFFSET) & AVR32_USART_THR_TXCHR_MASK;
}

USART_getString(char input_string[])
{
	char input_char = '\0'; // char to receive
	int i = 0;
	volatile avr32_usart_t * usart = USART;
	while (1) //Read if USART is ready
	{
		if(usart->CSR.rxrdy != 0) // If there is something to read
		{
			input_char = USART_getChar(); // Read the first char in line
			if(input_char == '\n') // When the user presses enter, return
			{
				input_string[i] = '\n';
				input_string[i+1] = '\0';
				break;
			}
			else
			{
				input_string[i] = input_char; // Put char to string
				i++;
			}
		}
	}
}

void USART_putString(char output_string[])
{
	volatile avr32_usart_t * usart = USART;
	int i = 0;
	while (output_string[i] != '\0') // Loop till end of string
	{
		if (usart->CSR.txrdy != 0) // If ready to write
		{
			USART_putChar(output_string[i]); // Write the next char in line
			i++;
		}
	}
}

void USART_putInt(int number)
{
	volatile avr32_usart_t * usart = USART;
	char temp[7];
	int i = sizeof(temp) - 1;
	temp [i] = '\0';
	i--;
	temp [i] = '\n';
	do 
	{
		i--;
		temp[i] = '0' + number % 10;
		number /= 10; 
	} while(i); // Loop till end of int
	
	USART_putString(temp); // Write the int
}

void USART_put2Int(int number)
{
	volatile avr32_usart_t * usart = USART;
	char temp[3];
	int i = sizeof(temp) - 1;
	temp [i] = '\0';
	do
	{
		i--;
		temp[i] = '0' + number % 10;
		number /= 10;
	} while(i); // Loop till end of int
	
	USART_putString(temp); // Write the int
}

void USART_reset()
{
	volatile avr32_usart_t * usart = USART;
	usart->mr = 0;	 //Reset Mode register
	
	//Reset Control register
	usart->cr = AVR32_USART_CR_RSTRX_MASK   |	// Resets receiver
	AVR32_USART_CR_RSTTX_MASK   |				// Resets transmitter
	AVR32_USART_CR_RSTSTA_MASK  |				// Resets status bit in CSR
	AVR32_USART_CR_RSTIT_MASK   |				// Reset iterations
	AVR32_USART_CR_RSTNACK_MASK |				// Reset NACK
	AVR32_USART_CR_RTSDIS_MASK;					// Disable Request to send
}