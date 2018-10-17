#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
#include "gpio.h"#include "pm.h"#include "semphr.h"#include "queue.h"#include <string.h>#define DATALEN 10#define BUFFER_SIZE 10xQueueHandle xQHandle;			// Queue handle - Producer, ConsumerxTaskHandle xHandleProducer;	// Task handle - Producer
xTaskHandle xHandleConsumer;	// Task handle - Consumerint byteCount = 0;				// How long is the queuestruct msg{	int id;	char data[DATALEN];	portTickType timestamp;};

// Initialize USART and 12 MHz clock, 115,2 kHz baud rate
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
	pm_switch_to_osc0 (& AVR32_PM , FOSC0 , OSC0_STARTUP );
	gpio_enable_module ( USART_SERIAL_GPIO_MAP , 2);
	gpio_enable_module ( USART_DEBUG_GPIO_MAP , 2);
	usart_init_rs232 ( serialPORT_USART , & USART_OPTIONS , FOSC0 );
	usart_init_rs232 ( configDBG_USART , & USART_OPTIONS , FOSC0 );
}

void vProducer(void *pvParameters)
{	
	struct msg message;
	message.id = 0;
	message.timestamp = 0;
	
	struct msg *pMesg = &message;
	
	char data[DATALEN] = "hej";
	
	onLED(LED0_BIT_VALUE);				// On when the producer is active
	
	while(1)
	{
		if(byteCount == BUFFER_SIZE)	// If the message queue is full	
		{
			offLED(LED0_BIT_VALUE);		// Off when the producer is sleeping
			vTaskSuspend(NULL);			// Go to sleep
		}
		
		strcpy(pMesg->data, data);		// copy data to pMesg->data
		
		xQueueSendToBack(xQHandle, &pMesg, (portTickType)10);
		
		byteCount++;					// One more message in queue
		
		if(byteCount == 1)				// There are messages to read
		{
			vTaskResume(xHandleConsumer);	// Wake up consumer
			onLED(LED1_BIT_VALUE);			// On when the consumer is active
		}
		
	}
}

void vConsumer( void *pvParameters )
{	
	struct msg *pMesg;
	
	onLED(LED1_BIT_VALUE);	// On when the consumer is active
	
	while(1)
	{
		if(byteCount == 0)			// If the queue is empty
		{
			offLED(LED1_BIT_VALUE);	// Off when the consumer is sleeping
			vTaskSuspend(NULL);		// Go to sleep
		}
			
		xQueueReceive(xQHandle, &pMesg, (portTickType)10);
		usart_write_line(serialPORT_USART, pMesg->data);
		
		byteCount--;				// One message removed from queue
		
		if(byteCount == BUFFER_SIZE - 1)	// If there is place in the queue
		{
			vTaskResume(xHandleProducer);	// Wake up the producer
			onLED(LED0_BIT_VALUE);			// On when the producer is active
		}
	}
}

int main(void)
{
	initLED();
	init_usart();
	
	xQHandle = xQueueCreate(10, sizeof(struct msg*));
	if(xQHandle == NULL)
	{
		//ERROR!
	}
	
	
	// Create the task , store the handle .
	xTaskCreate(	vProducer,
					"vProducer",
					configMINIMAL_STACK_SIZE,
					NULL,
					1,
					&xHandleProducer);
	xTaskCreate(	vConsumer, "vConsumer", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleConsumer);
	
	
	
	vTaskStartScheduler();

	
	for( ;; )
	{
		
	}
}

