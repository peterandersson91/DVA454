#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
#include "gpio.h"#include "pm.h"#include "semphr.h"#include "queue.h"#include "display_init.h"#include <string.h>#include <stdio.h>#include "board.h"
#include "adc.h"#define BUFFER_SIZE 10xSemaphoreHandle xFillCountTemperature;		// Semaphore handle - Queue TempxSemaphoreHandle xEmptyCountTemperature;xSemaphoreHandle xFillCountPotentiometer;	// Semaphore handle - Queue PotentiometerxSemaphoreHandle xEmptyCountPotentiometer;xSemaphoreHandle xFillCountLight;			// Semaphore handle - Queue LightxSemaphoreHandle xEmptyCountLight;xSemaphoreHandle xLCDSemaphore;		// Semaphore handle - DisplayxQueueHandle xQHandleTemperature;	// Queue handle - Temperature, ConsumerxQueueHandle xQHandlePotentiometer;	// Queue handle - Potentiometer, ConsumerxQueueHandle xQHandleLight;			// Queue handle - Light, Consumer
xTaskHandle xHandleConsumer;		// Task handle - ConsumerxTaskHandle xHandleTemperature;		// Task handle - TemperaturexTaskHandle xHandlePotentiometer;	// Task handle - PotentiometerxTaskHandle xHandleLight;			// Task handle - Light

void init_usart ( void )
{
	static const gpio_map_t USART_SERIAL_GPIO_MAP =
	{
		{ serialPORT_USART_RX_PIN , serialPORT_USART_RX_FUNCTION },
		{ serialPORT_USART_TX_PIN , serialPORT_USART_TX_FUNCTION }
	};
	static const gpio_map_t USART_DEBUG_GPIO_MAP =
	{
		{ configDBG_USART_RX_PIN , configDBG_USART_RX_FUNCTION } ,
		{ configDBG_USART_TX_PIN , configDBG_USART_TX_FUNCTION }
	};
	static const usart_options_t USART_OPTIONS =
	{
		. baudrate = 115200,
		. charlength = 8,
		. paritytype = USART_NO_PARITY ,
		. stopbits = USART_1_STOPBIT ,
		. channelmode = USART_NORMAL_CHMODE
	};
	pm_switch_to_osc0 (& AVR32_PM , FOSC0 , OSC0_STARTUP );				// Switch to 12 MHz clock
	gpio_enable_module ( USART_SERIAL_GPIO_MAP , 2);					// Enable usart
	gpio_enable_module ( USART_DEBUG_GPIO_MAP , 2);
	usart_init_rs232 ( serialPORT_USART , & USART_OPTIONS , FOSC0 );	// Usart options
	usart_init_rs232 ( configDBG_USART , & USART_OPTIONS , FOSC0 );
}

void vTemperature(void *pvParameters)
{
	volatile int value_to_send;		// Character to send to Consumer
	
	onLED(LED0_BIT_VALUE);	// On when the temp-producer is active
	
	while(1)
	{
		adc_start(&AVR32_ADC);
		offLED(LED0_BIT_VALUE);
		if(xSemaphoreTake(xEmptyCountTemperature, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		onLED(LED0_BIT_VALUE);
		
		value_to_send = adc_get_value(&AVR32_ADC, ADC_TEMPERATURE_CHANNEL);
		xQueueSendToBack(xQHandleTemperature, &value_to_send, (portTickType)portMAX_DELAY);
		
		if(xSemaphoreGive(xFillCountTemperature) == pdTRUE)
		{
			// Semaphore given
		}
		//We have CLK_ADC/2 - 10-bit requires 10 clock cycles - Sample & Hold Time = (SHTIM+1) / ADCClock where SHTIM = 15 - Startup Time = (STARTUP+1) * 8 / ADCClock where STARTUP = 31  
		// ADCClock = CLK_ADC / ( (PRESCAL+1) * 2 ) where PRESCAL = 0 => ADCClock = CLK_ADC / 2
		// IF CLK_ADC = 12 MHz => ADDClock = 6 MHz => Sample hold time = 16/6MHz, Startup time = 32 * 8 / 6 MHz
		vTaskDelay(TASK_DELAY_MS(100));
	}
}

void vPotentiometer(void *pvParameters)
{
	//volatile portTickType xLastWakeTime; // Holds tick count last led toggle
	//volatile const portTickType xFreq = TASK_DELAY_MS(1000); // Holds the period		
	//xLastWakeTime = xTaskGetTickCount(); // Sets current tick count
	volatile int value_to_send;		// Character to send to Consumer
	
	onLED(LED1_BIT_VALUE);	// On when the temp-producer is active
	
	while(1)
	{
		adc_start(&AVR32_ADC);
		
		offLED(LED1_BIT_VALUE);
		if(xSemaphoreTake(xEmptyCountPotentiometer, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		onLED(LED0_BIT_VALUE);
		
		value_to_send = adc_get_value(&AVR32_ADC, ADC_POTENTIOMETER_CHANNEL);
		xQueueSendToBack(xQHandlePotentiometer, &value_to_send, (portTickType)portMAX_DELAY);
		
		if(xSemaphoreGive(xFillCountPotentiometer) == pdTRUE)
		{
			// Semaphore given
		}
		vTaskDelay(TASK_DELAY_MS(100));
	}
}

void vLight(void *pvParameters)
{
	volatile int value_to_send;		// Character to send to Consumer
	
	onLED(LED2_BIT_VALUE);	// On when the temp-producer is active
	
	while(1)
	{
		adc_start(&AVR32_ADC);
		
		offLED(LED2_BIT_VALUE);
		if(xSemaphoreTake(xEmptyCountLight, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		onLED(LED2_BIT_VALUE);
		
		value_to_send = adc_get_value(&AVR32_ADC, ADC_LIGHT_CHANNEL);
		xQueueSendToBack(xQHandleLight, &value_to_send, (portTickType)portMAX_DELAY);
		
		if(xSemaphoreGive(xFillCountLight) == pdTRUE)
		{
			// Semaphore given
		}
		vTaskDelay(TASK_DELAY_MS(100));
	}
}

void vConsumer( void *pvParameters )
{
	volatile int temperature_received;		// Value received
	volatile int potentiometer_received;	// Value received
	volatile int light_received;			// Value received
	volatile int temp_usart_write;		// Value received
	volatile int pot_usart_write;	// Value received
	volatile int light_usart_write;			// Value received
	volatile int row_temp = 1;			// Where on display to write
	volatile int row_potentiometer = 2;	// Where on display to write
	volatile int row_light = 3;			// Where on display to write
	const char * string_temp = "Temperature:";
	const char * string_potentiometer = "Potentiometer:";
	const char * string_light = "Light:";
	char temperature_usart[10];
	char potentiometer_usart[10];
	char light_usart[10];
	
	dip204_clear_display();
	dip204_set_cursor_position(1, row_temp);
	dip204_write_string(string_temp);
	dip204_set_cursor_position(1, row_potentiometer);
	dip204_write_string(string_potentiometer);
	dip204_set_cursor_position(1, row_light);
	dip204_write_string(string_light);
	
	onLED(LED3_BIT_VALUE);	// On when Consumer is active
	
	while(1)
	{
		offLED(LED3_BIT_VALUE);
		if(xSemaphoreTake(xFillCountTemperature, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		onLED(LED3_BIT_VALUE);
		
		xQueueReceive(xQHandleTemperature, &temperature_received, (portTickType)10);
		temp_usart_write = temperature_received;
		
		if(xSemaphoreGive(xEmptyCountTemperature) == pdTRUE)
		{
			// Semaphore given
		}
		
		offLED(LED3_BIT_VALUE);
		if(xSemaphoreTake(xFillCountPotentiometer, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		onLED(LED3_BIT_VALUE);
		
		xQueueReceive(xQHandlePotentiometer, &potentiometer_received, (portTickType)10);
		pot_usart_write = potentiometer_received;
		
		if(xSemaphoreGive(xEmptyCountPotentiometer) == pdTRUE)
		{
			// Semaphore given
		}
		
		offLED(LED3_BIT_VALUE);
		if(xSemaphoreTake(xFillCountLight, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		onLED(LED3_BIT_VALUE);
		
		xQueueReceive(xQHandleLight, &light_received, (portTickType)10);
		light_usart_write = light_received;
		
		if(xSemaphoreGive(xEmptyCountLight) == pdTRUE)
		{
			// Semaphore given
		}
		
		if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)	// Semaphore - Display
		{
			sprintf(temperature_usart, "Temperature %d\n", temp_usart_write);
			usart_write_line(serialPORT_USART, temperature_usart);
			sprintf(potentiometer_usart, "Potentiometer %d\n", pot_usart_write);
			usart_write_line(serialPORT_USART, potentiometer_usart);
			sprintf(light_usart, "Light %d\n", light_usart_write);
			usart_write_line(serialPORT_USART, light_usart);
			dip204_set_cursor_position(15, row_temp);
			dip204_printf_string("%04d", temperature_received); //print value
			dip204_set_cursor_position(15, row_potentiometer);
			dip204_printf_string("%04d", potentiometer_received); //print value
			dip204_set_cursor_position(15, row_light);
			dip204_printf_string("%04d", light_received); //print value
			if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
			{
				//Successfully given back
			}
		}
	}
}

int main(void)
{
	initLED();
	init_usart();
	display_init();
	dip204_clear_display();
	
	// Configure the ADC module and enable the potentiometer, temperature and light channel
	adc_configure(&AVR32_ADC);
	adc_enable(&AVR32_ADC, ADC_POTENTIOMETER_CHANNEL);
	adc_enable(&AVR32_ADC, ADC_TEMPERATURE_CHANNEL);
	adc_enable(&AVR32_ADC, ADC_LIGHT_CHANNEL);
	
	vSemaphoreCreateBinary(xLCDSemaphore);	// Semaphore - Display
	xFillCountTemperature = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
	xEmptyCountTemperature = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
	xFillCountPotentiometer = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
	xEmptyCountPotentiometer = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
	xFillCountLight = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
	xEmptyCountLight = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
	
	xQHandleTemperature = xQueueCreate(BUFFER_SIZE, sizeof(int*));		// Queue - 10 spots of char-pointer size
	if(xQHandleTemperature == NULL)
	{
		//ERROR!
	}
	xQHandlePotentiometer = xQueueCreate(BUFFER_SIZE, sizeof(int*));		// Queue - 10 spots of char-pointer size
	if(xQHandlePotentiometer == NULL)
	{
		//ERROR!
	}
	xQHandleLight = xQueueCreate(BUFFER_SIZE, sizeof(int*));		// Queue - 10 spots of char-pointer size
	if(xQHandleLight == NULL)
	{
		//ERROR!
	}
	
	
	// Create the task , store the handle .
	xTaskCreate(	vConsumer, "vConsumer", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleConsumer);	
	xTaskCreate(	vTemperature, "vTemperature", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleTemperature);
	xTaskCreate(	vPotentiometer, "vPotentiometer", configMINIMAL_STACK_SIZE, NULL, 1, &xHandlePotentiometer);
	xTaskCreate(	vLight, "vLight", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleLight);
	
	vTaskStartScheduler();		// Starts the schedueling

	
	for( ;; )
	{
		
	}
}
/*
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
		temperature = temp_value;// * 600 / 1024;
		dip204_set_cursor_position(column+13, line+1);	//First column second line
		dip204_printf_string("%04d", temperature); //print value
		//Convert the light value to a value btwn 0-255
		light = light_value;// * 255 / 1024;
		dip204_set_cursor_position(column+7, line+2);	//First column third line
		dip204_printf_string("%04d", light); //print value
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
}*/
