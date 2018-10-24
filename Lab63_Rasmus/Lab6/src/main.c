#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
#include "gpio.h"#include "pm.h"#include "semphr.h"#include "queue.h"#include "display_init.h"#include <string.h>#include "board.h"
#include "adc.h"
#include "display_init.h"#include <stdio.h>#define BUFFER_SIZE 10xSemaphoreHandle xFillCount;		// Semaphore handle - QueuexSemaphoreHandle xEmptyCount;xSemaphoreHandle xLCDSemaphore;		// Semaphore handle - DisplayxSemaphoreHandle xMutex;			// Mutex semaphorexQueueHandle xQHandle;				// Queue handle - Producer, ConsumerxTaskHandle xHandlePotentiometer;		// Task handle - Potentiometer
xTaskHandle xHandleTemperature;			// Task handle - TemperaturexTaskHandle xHandleLight;				// Task handle - LightxTaskHandle xHandleLCDWrite;			// Task handle - Write on LCDstruct sensor_struct{
		volatile int sensor_value;
		volatile int ID;
		};

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

// Potentiometer task which acts as a producer
void vPotentiometer( void *pvParameters )
{
	volatile struct sensor_struct pot_struct;
	while(1)
	{
		// Start a ADC sampling of all active channels
		adc_start(&AVR32_ADC);		
		
		// Polling for semaphore EmptyCount before getting access to queue (removes value from semaphore) 
		if(xSemaphoreTake(xEmptyCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		if(xSemaphoreTake(xLCDSemaphore,(portTickType)portMAX_DELAY) == pdTRUE)
		{
			usart_write_line(serialPORT_USART, "Pot tar mutex");
		}
		// Get the potentiometer value
		pot_struct.sensor_value = adc_get_value(&AVR32_ADC, ADC_POTENTIOMETER_CHANNEL);
		// Convert the potentiometer value to a value btwn 0-255
		pot_struct.sensor_value = pot_struct.sensor_value * 255 / 1024;
		pot_struct.ID = 0;
		
		LED_Set_Intensity(LED5, pot_struct.sensor_value); //Green light
		
		// Put the value on the queue
		xQueueSendToBack(xQHandle, &pot_struct, (portTickType)10);
		
		if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
		{
			usart_write_line(serialPORT_USART, "pot lämnar mutex");
		}
		// Give the semaphore FillCount (adds value to semaphore) 
		if(xSemaphoreGive(xFillCount) == pdTRUE)
		{
			// Semaphore given
		}
		// Delay
		vTaskDelay(TASK_DELAY_MS(50));
	}
}

// Temperature task which acts as a producer
void vTemperature( void *pvParameters )
{
	volatile struct sensor_struct temp_struct;
	while (1)
	{
		// Start a ADC sampling of all active channels
		adc_start(&AVR32_ADC);
		
		// Polling for semaphore EmptyCount before getting access to queue (removes value from semaphore)
		if(xSemaphoreTake(xEmptyCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		if(xSemaphoreTake(xLCDSemaphore,(portTickType)portMAX_DELAY) == pdTRUE)
		{
			usart_write_line(serialPORT_USART, "temp tar mutex");
		}

		//Get the temperature value
		temp_struct.sensor_value = adc_get_value(&AVR32_ADC, ADC_TEMPERATURE_CHANNEL);
		temp_struct.ID = 1;

		// Put the value on the queue
		xQueueSendToBack(xQHandle, &temp_struct, (portTickType)10);
		
		if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
		{
			usart_write_line(serialPORT_USART, "temp lämnar mutex");
		}
		// Give the semaphore FillCount (adds value to semaphore)
		if(xSemaphoreGive(xFillCount) == pdTRUE)
		{
			// Semaphore given
		}
		// Delay
		vTaskDelay(TASK_DELAY_MS(50));
	}
	
}

// Light sensor task which acts as a producer
void vLight( void *pvParameters )
{
	volatile struct sensor_struct light_struct;
	while (1)
	{
		// Start a ADC sampling of all active channels
		adc_start(&AVR32_ADC);
		
		// Polling for semaphore EmptyCount before getting access to queue (removes value from semaphore)
		if(xSemaphoreTake(xEmptyCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		if(xSemaphoreTake(xLCDSemaphore,(portTickType)portMAX_DELAY) == pdTRUE)
		{
			usart_write_line(serialPORT_USART, "Light tar mutex");
		}
		
		//Get the light sensor value
		light_struct.sensor_value = adc_get_value(&AVR32_ADC, ADC_LIGHT_CHANNEL);
		light_struct.ID = 2;
		
		// Put the value on the queue
		xQueueSendToBack(xQHandle, &light_struct, (portTickType)10);
		
		if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
		{
			usart_write_line(serialPORT_USART, "Light lämnar mutex");
		}
		// Give the semaphore FillCount (adds value to semaphore)
		if(xSemaphoreGive(xFillCount) == pdTRUE)
		{
			// Semaphore given
		}
		// Delay
		vTaskDelay(TASK_DELAY_MS(50));
	}
}

// Task which reads from queue and writes to LCD display. It acts as a consumer.
void vLCDWrite( void *pvParameters )
{
	volatile struct sensor_struct sensor_struct1;
	volatile char str_pot[20];
	volatile char str_temp[20];
	volatile char str_light[20];
	while (1)
	{
		// Polling for semaphore FillCount before getting access to queue (removes value from semaphore)
		if(xSemaphoreTake(xFillCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		
		if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			usart_write_line(serialPORT_USART, "LCD tar mutex");
		}
		// Read value from queue
		xQueueReceive(xQHandle, &sensor_struct1, (portTickType)10);
		if (sensor_struct1.ID == 0)	// Potentiometer sensor
		{
			dip204_set_cursor_position(1, 1);
			dip204_printf_string("Potentiometer: %03d", sensor_struct1.sensor_value); //print value
			sprintf(str_pot, "Potentiometer: %d, ", sensor_struct1.sensor_value);
			
		}
		else if (sensor_struct1.ID == 1)	//Temperature sensor	
		{
			dip204_set_cursor_position(1, 2);
			dip204_printf_string("Temperature: %03d", sensor_struct1.sensor_value); //print value
			sprintf(str_temp, "Temperature: %d, ", sensor_struct1.sensor_value);
		}
		else if (sensor_struct1.ID == 2)	// Light sensor
		{
			dip204_set_cursor_position(1, 3);
			dip204_printf_string("Light: %04d", sensor_struct1.sensor_value); //print value
			sprintf(str_light, "Light: %d", sensor_struct1.sensor_value);
		}
		usart_write_line(serialPORT_USART, "\r");
		usart_write_line(serialPORT_USART, "\033[2J");
		usart_write_line(serialPORT_USART, str_pot);
		usart_write_line(serialPORT_USART, str_temp);
		usart_write_line(serialPORT_USART, str_light);
		
		if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
		{
			usart_write_line(serialPORT_USART, "LCD lämnar mutex");
		}
		// Give the semaphore EmptyCount (adds value to semaphore)
		if(xSemaphoreGive(xEmptyCount) == pdTRUE)
		{
			// Semaphore given
		}
		vTaskDelay(TASK_DELAY_MS(20));
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
	xFillCount = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
	xEmptyCount = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
	xMutex = xSemaphoreCreateMutex();
	
	xQHandle = xQueueCreate(1, sizeof(struct sensor_struct));		// Queue - 10 spots of char-pointer size
	if(xQHandle == NULL)
	{
		//ERROR!
	}
	
	
	// Create the task , store the handle .
	xTaskCreate(	vPotentiometer,
					"vPotentiometer",
					configMINIMAL_STACK_SIZE,
					NULL,
					1,
					&xHandlePotentiometer);
	xTaskCreate(	vTemperature, "vTemperature", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleTemperature);
	
	xTaskCreate(	vLight, "vLight", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleLight);
	
	xTaskCreate(	vLCDWrite, "vLCDWrite", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleLCDWrite);
	
	vTaskStartScheduler();		// Starts the scheduling

	
	for( ;; )
	{
		
	}
}

