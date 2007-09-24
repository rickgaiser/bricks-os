#ifndef CPU_H
#define CPU_H


#include "asm/arch/registers.h"


extern "C"
{
unsigned long local_save_flags(void);
void local_irq_restore(unsigned long flags);
void local_irq_disable(void);
void local_irq_enable(void);
};


static inline void halt(){while(true){}}


#endif
