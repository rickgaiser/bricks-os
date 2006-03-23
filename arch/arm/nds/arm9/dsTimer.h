#ifndef DSTIMER_H
#define DSTIMER_H


#include "kernel/interrupt.h"
#include "kernel/archTimer.h"
#include "kernel/timeValue.h"


#define DS_TIMER_COUNT 1


// -----------------------------------------------------------------------------
class CDSTimer
 : public ITimer
 , public IInterruptServiceRoutine
{
public:
  CDSTimer(unsigned short * base = 0);
  virtual ~CDSTimer();

  void setRegisterBase(unsigned short * base);

  // Derived from IArchTimer
  virtual long schedule(IEventHandler & handler, void * arg, const CTimeValue & start, const CTimeValue & interval);
  virtual int cancel(long timer_id);
  virtual int resetInterval(long timer_id, const CTimeValue &interval);
 
  // Derived from IInterruptServiceRoutine
  virtual int isr(int irq);

private:
  void setTimeout(const CTimeValue & time);

  unsigned short * pBase_;
  IEventHandler  * pHandler_;
  void           * pArg_;
  CTimeValue       tmInterval_;
};

// -----------------------------------------------------------------------------
class CDSTimerManager
{
public:
  CDSTimerManager();
  virtual ~CDSTimerManager();

  int init();

private:
  CDSTimer timer_[DS_TIMER_COUNT];
};


#endif
