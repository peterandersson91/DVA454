#ifndef TIMER_H_
#define TIMER_H_

// TIMER/COUNTER DEFINES#define TC_BASE_ADRESS	0xFFFF3800#define TC_CHANNEL0		0x00#define TC_CHANNEL0_ADRESS TC_BASE_ADRESS+TC_CHANNEL0#define TC_CCR			0x00#define TC_CMR			0x04#define TC_CV			0x10#define TC_RA			0x14#define TC_RB			0x18#define TC_RC			0x1C#define TC_SR			0x20#define TC_IER			0x28#define TC_IDR			0x2C#define TC0_REG_CMR (*((volatile int*)(TC_CHANNEL0_ADRESS + TC_CMR)))
#define TC0_REG_CCR (*((volatile int*)(TC_CHANNEL0_ADRESS + TC_CCR)))
#define TC0_REG_CV (*((volatile int*)(TC_CHANNEL0_ADRESS + TC_CV)))

#define TC0_REG_RC (*((volatile int*)(TC_CHANNEL0_ADRESS + TC_RC)))

#define TC0_REG_SR (*((volatile int*)(TC_CHANNEL0_ADRESS + TC_SR)))

#define TC0_REG_IER (*((volatile int*)(TC_CHANNEL0_ADRESS + TC_IER)))
#define TC0_REG_IDR (*((volatile int*)(TC_CHANNEL0_ADRESS + TC_IDR)))

void TC_init();
void TC_start();
int get_tc_cv();

#endif /* TIMER_H_ */