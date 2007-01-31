#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#include "asm/arch/irq.h"
#include "asm/gba.h"


//---------------------------------------------------------------------------
struct pt_regs
{
  unsigned long uregs[18];
};

// -----------------------------------------------------------------------------
class CIRQ
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  inline void enable (unsigned int irq){REG_IE |= (1 << irq);}
  inline void disable(unsigned int irq){REG_IE &= (1 << irq);}
  inline void ack    (unsigned int irq){REG_IF |= (1 << irq);}
  inline void end    (unsigned int irq){}
};


#endif
