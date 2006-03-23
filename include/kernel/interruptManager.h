#ifndef INTERRUPTMANAGER_H
#define INTERRUPTMANAGER_H


#include "interrupt.h"
#include "archIRQ.h"


// Max number of supported interrupts
#define MAX_INT_COUNT 0x30


// Early declaration
struct pt_regs;

//---------------------------------------------------------------------------
class CInterruptManager
{
public:
  static CInterruptManager * instance();  

  void attach(unsigned int irq, IInterruptServiceRoutine * isr);
  void detach(unsigned int irq, IInterruptServiceRoutine * isr);

  void attach(unsigned int irq, IArchIRQ * irqhardware);
  void detach(unsigned int irq, IArchIRQ * irqhardware);

  unsigned int isr(unsigned int irq, pt_regs * regs);

private:
  CInterruptManager();
  virtual ~CInterruptManager();

  static CInterruptManager   singleton_;   // Interrupt Manager Singleton

  CInterrupt   interrupt_[MAX_INT_COUNT];  // Interrupt objects
};


#endif
