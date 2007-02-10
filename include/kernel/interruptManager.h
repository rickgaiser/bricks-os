#ifndef INTERRUPTMANAGER_H
#define INTERRUPTMANAGER_H


#include "interrupt.h"
#include "asm/irq.h"


// Early declaration
struct pt_regs;

//---------------------------------------------------------------------------
class CInterruptManager
{
public:
  static void attach(unsigned int irq, IInterruptServiceRoutine * isr);
  static void detach(unsigned int irq, IInterruptServiceRoutine * isr);

  static void attach(unsigned int irq, CIRQ * irqhardware);
  static void detach(unsigned int irq, CIRQ * irqhardware);

  static unsigned int isr(unsigned int irq, pt_regs * regs);

private:
  CInterruptManager(){}

  static CInterrupt   interrupt_[MAX_INTERRUPTS];  // Interrupt objects
};


#endif
