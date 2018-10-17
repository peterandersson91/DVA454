/********************************************************
 Name          : main.c
 Author        : Marcus Jansson
 Copyright     : GPL
 Description   : EVK1100 template
 **********************************************************/

// Include Files
#include "display_init.h"

void display_init(void)
{
	volatile avr32_pm_t* pm = &AVR32_PM;

	// Display pin map
	static const gpio_map_t DIP204_SPI_GPIO_MAP =
	{
	   { DIP204_SPI_SCK_PIN, DIP204_SPI_SCK_FUNCTION },
	   { DIP204_SPI_MISO_PIN, DIP204_SPI_MISO_FUNCTION },
	   { DIP204_SPI_MOSI_PIN, DIP204_SPI_MOSI_FUNCTION },
	   { DIP204_SPI_NPCS_PIN, DIP204_SPI_NPCS_FUNCTION }
	};

	/* SPI options for LCD */
	spi_options_t spiOptions =
	{
	   .reg                  = DIP204_SPI_NPCS,
	   .baudrate             = 1000000, // min 1 us max 20 us
	   .bits                 = 8,
	   .spck_delay           = 0,
	   .trans_delay  		 = 8,
	   .stay_act             = 1,
	   .spi_mode             = 0,
	   .modfdis              = 1
	};

	// Switch clock to external 12 MHz crystal
	// 1) Configure OSC0 in crystal mode, external crystal with a FOSC0 Hz frequency.
	pm_enable_osc0_crystal(pm, FOSC0);

	// 2) Enable the OSC0
	pm_enable_clk0(pm, OSC0_STARTUP);

	// 3) Set the main clock source as being OSC0.
	pm_switch_to_clock(pm, AVR32_PM_MCSEL_OSC0);

	// Initialize the delay function
	delay_init(12000000);
	// Delay for a ms, to allow the display to power up properly
	delay_ms(1);

	// Enable the SPI peripheral on GPIO pins
	gpio_enable_module(DIP204_SPI_GPIO_MAP,
						sizeof(DIP204_SPI_GPIO_MAP) / 	sizeof(DIP204_SPI_GPIO_MAP[0]));

	// Initialize as SPI master
	spi_initMaster(DIP204_SPI, &spiOptions);
	spi_selectionMode(DIP204_SPI, 0, 0, 0);
	spi_enable(DIP204_SPI);
	spi_setupChipReg(DIP204_SPI, &spiOptions, FOSC0);

	dip204_init(backlight_IO, ON);
	dip204_clear_display();
	dip204_hide_cursor();
	dip204_set_cursor_position(1,1);
}
