#ifndef INTERRUPT_H
#define INTERRUPT_H


#include "archIRQ.h"


#define MAX_OBSERVERS 2


//---------------------------------------------------------------------------
// Observer pattern: observer
//---------------------------------------------------------------------------
class IInterruptServiceRoutine
{
public:
  virtual int isr(int irq) = 0;
};

//---------------------------------------------------------------------------
// Observer pattern: subject
//---------------------------------------------------------------------------
class CInterrupt
{
public:
  CInterrupt();
  virtual ~CInterrupt();

  virtual void attach(IInterruptServiceRoutine * isr);
  virtual void detach(IInterruptServiceRoutine * isr);

  virtual int  isrCount();

  virtual int  handle(int isr);

  IArchIRQ                 * hardware_;

private:
  IInterruptServiceRoutine * observers_[MAX_OBSERVERS];
};


#endif
