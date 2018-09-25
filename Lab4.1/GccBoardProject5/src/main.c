// Include Files
#include "board.h"
#include "adc.h"
#include "display_init.h"

int main(void)
{
	int pot_value, temp_value, light_value;
	int potentiometer, temperature, light;
	const char * temp = "Temperature:";
	const char * pot = "Potentiometer:";
	const char * lig = "Light:";
	// For the dip204 cursor position
	unsigned int column = 1;
	unsigned int line = 1;
	
	// Configure the ADC module and enable the potentiometer, temperature and light channel
	adc_configure(&AVR32_ADC);
	adc_enable(&AVR32_ADC, ADC_POTENTIOMETER_CHANNEL);
	adc_enable(&AVR32_ADC, ADC_TEMPERATURE_CHANNEL);
	adc_enable(&AVR32_ADC, ADC_LIGHT_CHANNEL);
	
	display_init();
	
	//Clear the screen
	dip204_clear_display();
	
	dip204_set_cursor_position(column, line);
	dip204_write_string(pot);
	dip204_set_cursor_position(column, line+1);
	dip204_write_string(temp);
	dip204_set_cursor_position(column, line+2);
	dip204_write_string(lig);
	
	
	while(true)
	{
		// Start a ADC sampling of all active channels
		adc_start(&AVR32_ADC);
		
		
		
		// Get the potentiometer value
		pot_value = adc_get_value(&AVR32_ADC, ADC_POTENTIOMETER_CHANNEL);
		//Get the temperature value
		temp_value = adc_get_value(&AVR32_ADC, ADC_TEMPERATURE_CHANNEL);
		//Get the light sensor value
		light_value = adc_get_value(&AVR32_ADC, ADC_LIGHT_CHANNEL);

		// Convert the potentiometer value to a value btwn 0-255
		potentiometer = pot_value * 255 / 1024;
		dip204_set_cursor_position(column+15, line); //First column and line
		dip204_printf_string("%03d", potentiometer); //print value
		// Convert the temperature value to a value btwn 0-255
		temperature = temp_value * 600 / 1024;
		dip204_set_cursor_position(column+13, line+1);	//First column second line
		dip204_printf_string("%d", temperature); //print value
		//Convert the light value to a value btwn 0-255
		light = light_value * 255 / 1024;
		dip204_set_cursor_position(column+7, line+2);	//First column third line
		dip204_printf_string("%d", light); //print value
		int i = 0;
		
		// Delay
		while (i < 1000*potentiometer)
		{
			i++;
		}
		
		// Set the intensity of the LED
		LED_Set_Intensity(LED4, potentiometer);	//Red light
		//LED_Set_Intensity(LED5, brightness_LED5); //Green light
		LED_Set_Intensity(LED6, potentiometer);	//Red light
		//LED_Set_Intensity(LED7, brightness_LED5);	//Green light
	}

	// Never return from main
	while(true);
}
