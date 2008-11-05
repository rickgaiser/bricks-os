#ifndef GBANDS_IRQ_H
#define GBANDS_IRQ_H


#include "kernel/interruptManager.h"
#include "asm/arch/registers.h"


// -----------------------------------------------------------------------------
class CIRQ
 : public IInterruptProvider
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  inline void enable (unsigned int irq){REG_IE |= (1 << irq);}
  inline void disable(unsigned int irq){REG_IE &= ~(1 << irq);}
};


#endif
