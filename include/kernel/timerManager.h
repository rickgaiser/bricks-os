#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H


#include "archTimer.h"


// Max number of supported timers
#define MAX_TIMER_COUNT 10


//---------------------------------------------------------------------------
class CTimerManager
{
public:
  static CTimerManager * instance();

  int init();

  int  attach(ITimer * timerhardware);
  void detach(ITimer * timerhardware);

  ITimer * getTimer();
  void releaseTimer(ITimer * timer);

private:
  CTimerManager();
  virtual ~CTimerManager();

  static CTimerManager   singleton_; // Timer Manager Singleton

  ITimer * timer_[MAX_TIMER_COUNT];  // Timer objects
};


#endif
