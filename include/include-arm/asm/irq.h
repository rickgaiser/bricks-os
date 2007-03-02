#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#include "asm/arch/irq.h"
#include "asm/arch/registers.h"


//---------------------------------------------------------------------------
struct pt_regs
{
  // r4-r11
  unsigned long r4, r5, r6, r7, r8, r9, r10, r11;

  // r0-r3, r12, r14
  unsigned long r0, r1, r2, r3, r12, pc;

  unsigned long cpsr; // value from spsr_irq
  unsigned long spsr_svc, lr_svc, sp_svc; // svc mode banked registers
  unsigned long lr, sp; // system mode banked registers
};

// -----------------------------------------------------------------------------
class CIRQ
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  inline void enable (unsigned int irq){REG_IE |= (1 << irq);}
  inline void disable(unsigned int irq){REG_IE &= ~(1 << irq);}
  inline void ack    (unsigned int irq){REG_IF |= (1 << irq);}
  inline void end    (unsigned int irq){}
};


#endif
