#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
#include "gpio.h"#include "pm.h"#include "semphr.h"#include "queue.h"#include "display_init.h"#include <string.h>#include <stdio.h>
#include "adc.h"#include "board.h"#define BUFFER_SIZE 1volatile xSemaphoreHandle xFillCountTemperature;		// Semaphore handle - Queue Tempvolatile xSemaphoreHandle xEmptyCountTemperature;volatile xSemaphoreHandle xFillCountPotentiometer;	// Semaphore handle - Queue Potentiometervolatile xSemaphoreHandle xEmptyCountPotentiometer;volatile xSemaphoreHandle xFillCountLight;			// Semaphore handle - Queue Lightvolatile xSemaphoreHandle xEmptyCountLight;xQueueHandle xQHandleTemperature;	// Queue handle - Temperature, ConsumerxQueueHandle xQHandlePotentiometer;	// Queue handle - Potentiometer, ConsumerxQueueHandle xQHandleLight;			// Queue handle - Light, Consumer
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
	volatile uint32_t value_to_send;		// Character to send to Consumer
	
	while(1)
	{
		if(xSemaphoreTake(xEmptyCountTemperature, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
			adc_start(&AVR32_ADC);
			value_to_send = adc_get_value(&AVR32_ADC, ADC_TEMPERATURE_CHANNEL);
			xQueueSendToBack(xQHandleTemperature, &value_to_send, (portTickType)10);
		
			if(xSemaphoreGive(xFillCountTemperature) == pdTRUE)
			{
				// Semaphore given
			}
		}
	}
}

void vPotentiometer(void *pvParameters)
{
	volatile uint32_t value_to_send;		// Character to send to Consumer
	
	while(1)
	{		
		if(xSemaphoreTake(xEmptyCountPotentiometer, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
			adc_start(&AVR32_ADC);
			value_to_send = adc_get_value(&AVR32_ADC, ADC_POTENTIOMETER_CHANNEL);
			xQueueSendToBack(xQHandlePotentiometer, &value_to_send, (portTickType)10);	
				
			if(xSemaphoreGive(xFillCountPotentiometer) == pdTRUE)
			{
				// Semaphore given
			}
		}
	}
}

void vLight(void *pvParameters)
{
	volatile uint32_t value_to_send;		// Character to send to Consumer
	
	while(1)
	{		
		if(xSemaphoreTake(xEmptyCountLight, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
			adc_start(&AVR32_ADC);
			value_to_send = adc_get_value(&AVR32_ADC, ADC_LIGHT_CHANNEL);
			xQueueSendToBack(xQHandleLight, &value_to_send, (portTickType)10);
		
			if(xSemaphoreGive(xFillCountLight) == pdTRUE)
			{
				// Semaphore given
			}
		}
	}
}

void vConsumer( void *pvParameters )
{
	volatile uint32_t temperature_received = 0;		// Value received
	volatile uint32_t potentiometer_received = 0;	// Value received
	volatile uint32_t light_received = 0;			// Value received
	volatile uint32_t temp_usart_write = 0;			// Value received
	volatile uint32_t pot_usart_write = 0;			// Value received
	volatile uint32_t light_usart_write = 0;			// Value received
	volatile const char * string_temp = "Temperature:";
	volatile const char * string_potentiometer = "Potentiometer:";
	volatile const char * string_light = "Light:";
	volatile char temperature_usart[10];
	volatile char potentiometer_usart[10];
	volatile char light_usart[10];
	
	volatile portTickType xLastWakeTime; // Holds tick count
	xLastWakeTime = xTaskGetTickCount(); // Sets current tick count
	volatile const portTickType xFreq = TASK_DELAY_MS(100); // Holds the period	
	
	dip204_clear_display();
	dip204_set_cursor_position(1, 1);
	dip204_write_string(string_temp);
	dip204_set_cursor_position(1, 2);
	dip204_write_string(string_potentiometer);
	dip204_set_cursor_position(1, 3);
	dip204_write_string(string_light);
	
	while(1)
	{
		if(xSemaphoreTake(xFillCountTemperature, (portTickType)10) == pdTRUE)
		{
			// Semaphore taken
			xQueueReceive(xQHandleTemperature, &temperature_received, (portTickType)10);
			temp_usart_write = temperature_received;
		
			if(xSemaphoreGive(xEmptyCountTemperature) == pdTRUE)
			{
				// Semaphore given
			}
		}
		
		if(xSemaphoreTake(xFillCountPotentiometer, (portTickType)10) == pdTRUE)
		{
			// Semaphore taken
			xQueueReceive(xQHandlePotentiometer, &potentiometer_received, (portTickType)10);
			pot_usart_write = potentiometer_received;
		
			if(xSemaphoreGive(xEmptyCountPotentiometer) == pdTRUE)
			{
				// Semaphore given
			}
		}		

		if(xSemaphoreTake(xFillCountLight, (portTickType)10) == pdTRUE)
		{
			// Semaphore taken
			xQueueReceive(xQHandleLight, &light_received, (portTickType)10);
			light_usart_write = light_received;
		
			if(xSemaphoreGive(xEmptyCountLight) == pdTRUE)
			{
				// Semaphore given
			}
		}

		sprintf(temperature_usart, "Temperature %d\n", temp_usart_write);
		usart_write_line(serialPORT_USART, temperature_usart);
		sprintf(potentiometer_usart, "Potentiometer %d\n", pot_usart_write);
		usart_write_line(serialPORT_USART, potentiometer_usart);
		sprintf(light_usart, "Light %d\n", light_received);
		usart_write_line(serialPORT_USART, light_usart);
		dip204_set_cursor_position(15, 1);
		dip204_printf_string("%04d", temperature_received); //print value
		dip204_set_cursor_position(15, 2);
		dip204_printf_string("%04d", potentiometer_received); //print value
		dip204_set_cursor_position(15, 3);
		dip204_printf_string("%04d", light_received); //print value
		
		vTaskDelayUntil(&xLastWakeTime,xFreq);
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
	
	xFillCountPotentiometer = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
	xEmptyCountPotentiometer = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
	
	xFillCountTemperature = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
	xEmptyCountTemperature = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
	
	xFillCountLight = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
	xEmptyCountLight = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
	
	xQHandlePotentiometer = xQueueCreate(BUFFER_SIZE, sizeof(uint32_t*));
	xQHandleTemperature = xQueueCreate(BUFFER_SIZE, sizeof(uint32_t*));
	xQHandleLight = xQueueCreate(BUFFER_SIZE, sizeof(uint32_t*));
	
	xTaskCreate(	vConsumer, "vConsumer", (( unsigned portSHORT ) 2000), NULL, 1, NULL);	
	xTaskCreate(	vTemperature, "vTemperature", (( unsigned portSHORT ) 2000), NULL, 1,NULL);
	xTaskCreate(	vPotentiometer, "vPotentiometer", (( unsigned portSHORT ) 2000), NULL, 1, NULL);
	xTaskCreate(	vLight, "vLight", (( unsigned portSHORT ) 2000), NULL, 1, NULL);
	
	vTaskStartScheduler();		// Starts the schedueling

	
	for( ;; )
	{
		
	}
}