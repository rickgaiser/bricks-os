#include "kernel/timerManager.h"
#include "iostream"


// -----------------------------------------------------------------------------
// Static data members
CTimerManager   CTimerManager::singleton_;


// -----------------------------------------------------------------------------
CTimerManager::CTimerManager()
{
  for(int i(0); i < MAX_TIMER_COUNT; i++)
  {
    timer_[i] = 0;
  }
}

// -----------------------------------------------------------------------------
CTimerManager::~CTimerManager()
{
}

// -----------------------------------------------------------------------------
CTimerManager *
CTimerManager::instance()
{
  return(&singleton_);
}

// -----------------------------------------------------------------------------
int
CTimerManager::init()
{
  return(0);
}

// -----------------------------------------------------------------------------
int
CTimerManager::attach(ITimer * timerhardware)
{
  // Check parameters
  if(timerhardware == 0)
    return(-1);
  
  // Prevent double registrations
  for(int i(0); i < MAX_TIMER_COUNT; i++)
  {
    if(timer_[i] == timerhardware)
      return(-1);
  }

  // Register new timer
  for(int i(0); i < MAX_TIMER_COUNT; i++)
  {
    if(timer_[i] == 0)
    {
      timer_[i] = timerhardware;
      return(0);
    }
  }

  return(-1);
}

// -----------------------------------------------------------------------------
void
CTimerManager::detach(ITimer * timerhardware)
{
  // Remove registration
  for(int i(0); i < MAX_TIMER_COUNT; i++)
  {
    if(timer_[i] == timerhardware)
    {
      timer_[i] = 0;
      break;
    }
  }
}

// -----------------------------------------------------------------------------
ITimer *
CTimerManager::getTimer()
{
  ITimer * retVal(0);

  for(int i(0); i < MAX_TIMER_COUNT; i++)
  {
    if(timer_[i] != 0)
    {
      retVal = timer_[i];
      timer_[i] = 0;
      break;
    }
  }

  return(retVal);
}

// -----------------------------------------------------------------------------
void
CTimerManager::releaseTimer(ITimer * timer)
{
  attach(timer);
}
