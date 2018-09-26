#ifndef LED_H_
#define LED_H_

#define GPIO_BASE_ADDRESS 0xFFFF1000#define GPIO_PORT1_OFFSET 0x100

#define GPIO_PORT1_ADDRESS GPIO_BASE_ADDRESS+GPIO_PORT1_OFFSET

#define GPIO_GPER_OFFSET 0x00
#define GPIO_ODER_OFFSET 0x40
#define GPIO_OVR_OFFSET 0x50

#define GPIO_PORT1_GPER (*((volatile int*)(GPIO_PORT1_ADDRESS + GPIO_GPER_OFFSET)))
#define GPIO_PORT1_ODER (*((volatile int*)(GPIO_PORT1_ADDRESS + GPIO_ODER_OFFSET)))
#define GPIO_PORT1_OVR (*((volatile int*)(GPIO_PORT1_ADDRESS + GPIO_OVR_OFFSET)))

void LED0_init(void);
void LED0_output(int value);


#endif /* LED_H_ */