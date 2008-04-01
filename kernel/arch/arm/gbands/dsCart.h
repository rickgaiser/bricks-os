#ifndef DSCART_H
#define DSCART_H


#include "kernel/interrupt.h"
#include "kernel/interruptManager.h"


class CDSCart
 : public IInterruptServiceRoutine
{
public:
  CDSCart();
  virtual ~CDSCart();

  int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;
};


#endif
