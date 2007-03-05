#ifndef DSTIMER_H
#define DSTIMER_H


#include "kernel/timeValue.h"
#include "asm/arch/registers.h"
#include "inttypes.h"


#define TIMER_CONTROL(n)     (*(vuint16_t*)(0x04000102+(n*4)))
#define TIMER_COUNT(n)       (*(vuint16_t*)(0x04000100+(n*4)))


void setTimerFrequency(int timerId, float hz);
void setInterval(int timerId, unsigned int ms);


#endif
