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
  virtual int isr(int irq);
};


#endif
