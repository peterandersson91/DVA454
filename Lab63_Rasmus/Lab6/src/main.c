#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
#include "gpio.h"#include "pm.h"#include "semphr.h"#include "queue.h"#include "display_init.h"#include <string.h>#include "board.h"
#include "adc.h"
#include "display_init.h"#define BUFFER_SIZE 10xSemaphoreHandle xFillCount;		// Semaphore handle - QueuexSemaphoreHandle xEmptyCount;xSemaphoreHandle xLCDSemaphore;		// Semaphore handle - DisplayxSemaphoreHandle xMutex;			// Mutex semaphorexQueueHandle xQHandle;				// Queue handle - Producer, ConsumerxTaskHandle xHandlePotentiometer;		// Task handle - Potentiometer
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

/*
void vProducer(void *pvParameters)
{	
	volatile char char_to_send;		// Character to send to Consumer
	
	onLED(LED0_BIT_VALUE);	// On when the producer is active
	
	while(1)
	{
		
		offLED(LED0_BIT_VALUE);
		if(xSemaphoreTake(xEmptyCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		onLED(LED0_BIT_VALUE);
		
		char_to_send = usart_getchar(serialPORT_USART);
		xQueueSendToBack(xQHandle, &char_to_send, (portTickType)10);
		
		if(xSemaphoreGive(xFillCount) == pdTRUE)
		{
			// Semaphore given
		}
	}
}
*/
/*
void vConsumer( void *pvParameters )
{
	volatile char char_received;		// Character received
	volatile int column = 0;			// Where on display to write
	
	onLED(LED1_BIT_VALUE);	// On when Consumer is active
	
	while(1)
	{
		offLED(LED1_BIT_VALUE);
		if(xSemaphoreTake(xFillCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		onLED(LED1_BIT_VALUE);
		
		xQueueReceive(xQHandle, &char_received, (portTickType)10);
		statusCount++;
		
		column++;
		if(column > 20)		// If display is full, restart
		{
			dip204_clear_display();
			column = 1;
		}
		if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)	// Semaphore - Display
		{
			dip204_set_cursor_position(column, 1);
			dip204_printf_string("%c", char_received); //print value
			
			if(statusTaskActive == pdTRUE)		// Show status on display?
			{
				dip204_set_cursor_position(1, 2);
				dip204_printf_string("%d", statusCount); //print value
			}
			if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
			{
				//Successfully given back
			}
		}
		
		if(xSemaphoreGive(xEmptyCount) == pdTRUE)
		{
			// Semaphore given
		}
	}
}
*/
/*
// Shows total amount of characters sent
void vStatus( void *pvParameters )
{
	while (1)
	{
		if(buttonIsPressed(BUTTON0_PIN))
		{
			statusTaskActive = pdTRUE;	// Show status on display
			onLED(LED2_BIT_VALUE);		// On when status is active
			if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)	// Semaphore - Display
			{
				dip204_set_cursor_position(1, 2);	
				dip204_printf_string("%d", statusCount); //print value
				if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
				{
					//Successfully given back
				}
			}
			vTaskDelay(TASK_DELAY_MS(10000));	// Sleep for 10 sec
			offLED(LED2_BIT_VALUE);				// Off when status is sleeping
			statusTaskActive = pdFALSE;			// Do not show status on display
			if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
			{
				dip204_set_cursor_position(1, 2);	
				dip204_write_string("                    ");	// Clear the status count row
				if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
				{
					//Successfully given back
				}
			}
		}
	}
}
*/
// Potentiometer task which acts as a producer
void vPotentiometer( void *pvParameters )
{
	volatile struct sensor_struct pot_struct;
	while(1)
	{
		// Start a ADC sampling of all active channels
		adc_start(&AVR32_ADC);
		// Get the potentiometer value
		pot_struct.sensor_value = adc_get_value(&AVR32_ADC, ADC_POTENTIOMETER_CHANNEL);
		// Convert the potentiometer value to a value btwn 0-255
		pot_struct.sensor_value = pot_struct.sensor_value * 255 / 1024;
		pot_struct.ID = 0;
		
		LED_Set_Intensity(LED5, pot_struct.sensor_value); //Green light
		
		
		// Polling for semaphore EmptyCount before getting access to queue (removes value from semaphore) 
		if(xSemaphoreTake(xEmptyCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		if(xSemaphoreTake(xMutex,(portTickType)portMAX_DELAY) == pdTRUE)
		{
			
		}
		// Put the value on the queue
		xQueueSendToBack(xQHandle, &pot_struct, (portTickType)10);
		
		if(xSemaphoreGive(xMutex) == pdTRUE)
		{
			
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
		//Get the temperature value
		temp_struct.sensor_value = adc_get_value(&AVR32_ADC, ADC_TEMPERATURE_CHANNEL);
		temp_struct.ID = 1;
		
		// Polling for semaphore EmptyCount before getting access to queue (removes value from semaphore)
		if(xSemaphoreTake(xEmptyCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		if(xSemaphoreTake(xMutex,(portTickType)portMAX_DELAY) == pdTRUE)
		{
			
		}
		
		// Put the value on the queue
		xQueueSendToBack(xQHandle, &temp_struct, (portTickType)10);
		
		if(xSemaphoreGive(xMutex) == pdTRUE)
		{
			
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
		//Get the light sensor value
		light_struct.sensor_value = adc_get_value(&AVR32_ADC, ADC_LIGHT_CHANNEL);
		light_struct.ID = 2;
		
		// Polling for semaphore EmptyCount before getting access to queue (removes value from semaphore)
		if(xSemaphoreTake(xEmptyCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		if(xSemaphoreTake(xMutex,(portTickType)portMAX_DELAY) == pdTRUE)
		{
			
		}
		
		// Put the value on the queue
		xQueueSendToBack(xQHandle, &light_struct, (portTickType)10);
		
		if(xSemaphoreGive(xMutex) == pdTRUE)
		{
			
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
	while (1)
	{
		// Polling for semaphore FillCount before getting access to queue (removes value from semaphore)
		if(xSemaphoreTake(xFillCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		
		if(xSemaphoreTake(xMutex, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			
		}
		// Read value from queue
		xQueueReceive(xQHandle, &sensor_struct1, (portTickType)10);
		if (sensor_struct1.ID == 0)	// Potentiometer sensor
		{
			dip204_set_cursor_position(1, 1);
			dip204_printf_string("Potentiometer: %03d", sensor_struct1.sensor_value); //print value
			sprintf()
			usart_write_line(serialPORT_USART, );
		}
		else if (sensor_struct1.ID == 1)	//Temperature sensor	
		{
			dip204_set_cursor_position(1, 2);
			dip204_printf_string("Temperature: %04d", sensor_struct1.sensor_value); //print value
		}
		else if (sensor_struct1.ID == 2)	// Light sensor
		{
			dip204_set_cursor_position(1, 3);
			dip204_printf_string("Light: %04d", sensor_struct1.sensor_value); //print value
		}
		
		if(xSemaphoreGive(xMutex) == pdTRUE)
		{
			
		}
		// Give the semaphore EmptyCount (adds value to semaphore)
		if(xSemaphoreGive(xEmptyCount) == pdTRUE)
		{
			// Semaphore given
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
	xFillCount = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
	xEmptyCount = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
	xMutex = xSemaphoreCreateMutex();
	
	xQHandle = xQueueCreate(10, sizeof(struct sensor_struct));		// Queue - 10 spots of char-pointer size
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
	
	xTaskCreate(	vLCDWrite, "vLCDWrit", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleLCDWrite);
	
	vTaskStartScheduler();		// Starts the scheduling

	
	for( ;; )
	{
		
	}
}

