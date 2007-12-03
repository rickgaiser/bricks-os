#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#include "inttypes.h"


#define INT_GS            0
#define INT_SBUS          1
#define INT_VBLANK_START  2
#define INT_VBLANK_END    3
#define INT_VIF0          4
#define INT_VIF1          5
#define INT_VU0           6
#define INT_VU1           7
#define INT_IPU           8
#define INT_TIMER0        9
#define INT_TIMER1       10

#define MAX_INTERRUPTS 0x0b


//---------------------------------------------------------------------------
struct pt_regs
{
};

// -----------------------------------------------------------------------------
inline void
taskInit(pt_regs * regs, void * pc, void * sp, void * svcsp, uint32_t arg1, uint32_t arg2)
{
}

// -----------------------------------------------------------------------------
class CIRQ
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  inline void enable (unsigned int irq){}
  inline void disable(unsigned int irq){}
  inline void ack    (unsigned int irq){}
  inline void end    (unsigned int irq){}
};


#endif
