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
  static CInterruptManager * instance();  

  void attach(unsigned int irq, IInterruptServiceRoutine * isr);
  void detach(unsigned int irq, IInterruptServiceRoutine * isr);

  void attach(unsigned int irq, CIRQ * irqhardware);
  void detach(unsigned int irq, CIRQ * irqhardware);

  unsigned int isr(unsigned int irq, pt_regs * regs);

private:
  CInterruptManager();
  virtual ~CInterruptManager();

  static CInterruptManager   singleton_;   // Interrupt Manager Singleton

  CInterrupt   interrupt_[MAX_INTERRUPTS];  // Interrupt objects
};


#endif
