#ifndef TIMER_H_
#define TIMER_H_

// TIMER/COUNTER DEFINES
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