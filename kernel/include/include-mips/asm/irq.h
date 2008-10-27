#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#include "inttypes.h"
#include "asm/arch/registers.h"


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

//---------------------------------------------------------------------------
#define MIPS_INT_0 0
#define MIPS_INT_1 1
#define MIPS_INT_2 2
class IInterruptHandler
{
public:
  virtual ~IInterruptHandler(){}

  virtual void isr(unsigned int irq, pt_regs * regs) = 0;
};
void initExceptions();
void setInterruptHandler(uint32_t nr, IInterruptHandler & handler);

// -----------------------------------------------------------------------------
class CIRQ
 : public IInterruptHandler
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  void isr(unsigned int irq, pt_regs * regs);

  inline void enable (unsigned int irq){REG_INT_MASK |=  (1 << irq);}
  inline void disable(unsigned int irq){REG_INT_MASK &= ~(1 << irq);}
  inline void ack    (unsigned int irq){REG_INT_STAT  =  (1 << irq);}
  inline void end    (unsigned int irq){}
};


#endif
