#ifndef INTERRUPT_H
#define INTERRUPT_H


#define MAX_OBSERVERS 2


// -----------------------------------------------------------------------------
class IInterruptProvider
{
public:
  virtual ~IInterruptProvider(){}

  virtual void enable (unsigned int irq) = 0;
  virtual void disable(unsigned int irq) = 0;
};

//---------------------------------------------------------------------------
class IInterruptServiceRoutine
{
public:
  virtual ~IInterruptServiceRoutine(){}

  virtual int isr(int irq) = 0;
};

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

  IInterruptProvider * hardware_;

private:
  IInterruptServiceRoutine * observers_[MAX_OBSERVERS];
};


#endif
