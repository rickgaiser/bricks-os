#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#include "inttypes.h"


#define MAX_INTERRUPTS 0x30 // ???


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
