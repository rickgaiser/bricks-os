#include "dsTimer.h"
#include "kernel/interruptManager.h"
#include "kernel/timerManager.h"
#include "asm/arch/registers.h"


// -----------------------------------------------------------------------------
CDSTimerManager::CDSTimerManager()
{
  timer_[0].setRegisterBase((unsigned short *)(&REG_TM0D));
  timer_[1].setRegisterBase((unsigned short *)(&REG_TM1D));
  timer_[2].setRegisterBase((unsigned short *)(&REG_TM2D));
  timer_[3].setRegisterBase((unsigned short *)(&REG_TM3D));
}

// -----------------------------------------------------------------------------
CDSTimerManager::~CDSTimerManager()
{
  // Detach interrupt handlers
  CInterruptManager::instance()->detach(3, &(timer_[0]));
  CInterruptManager::instance()->detach(4, &(timer_[1]));
  CInterruptManager::instance()->detach(5, &(timer_[2]));
  CInterruptManager::instance()->detach(6, &(timer_[3]));

  // Detach timers
  for(int i(0); i < DS_TIMER_COUNT; i++)
    CTimerManager::instance()->detach(&(timer_[i]));
}

// -----------------------------------------------------------------------------
int
CDSTimerManager::init()
{
  int iRetVal(0);

  // Register interrupt handlers
  CInterruptManager::instance()->attach(3, &(timer_[0]));
  CInterruptManager::instance()->attach(4, &(timer_[1]));
  CInterruptManager::instance()->attach(5, &(timer_[2]));
  CInterruptManager::instance()->attach(6, &(timer_[3]));

  // Register timers
  for(int i(0); i < DS_TIMER_COUNT; i++)
  {
    if(CTimerManager::instance()->attach(&(timer_[i])) == -1)
      iRetVal = -1;
  }

  return(iRetVal);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CDSTimer::CDSTimer(unsigned short * base)
 : pBase_(base)
{
}

// -----------------------------------------------------------------------------
CDSTimer::~CDSTimer()
{
}

// -----------------------------------------------------------------------------
void
CDSTimer::setRegisterBase(unsigned short * base)
{
  pBase_ = base;
}

// -----------------------------------------------------------------------------
long
CDSTimer::schedule(IEventHandler & handler, void * arg, const CTimeValue & start, const CTimeValue & interval)
{
  long iRetVal(0);

  if(pBase_ != 0)
  {
    pHandler_   = &handler;
    pArg_       = arg;
    tmInterval_ = interval;

    // Set timeout
    if(start.msec() == 0)
      setTimeout(tmInterval_);
    else
      setTimeout(start);
    
    // Enable timer
    pBase_[1] = (1<<6) | (1<<7);// | (1<<9);

    iRetVal = 0;
  }
  else
  {
    iRetVal = -1;
  }

  return(iRetVal);
}

// -----------------------------------------------------------------------------
int
CDSTimer::cancel(long timer_id)
{
  if(timer_id == 0)
  {
    // Disable timer
    pBase_[1] = 0;

    return(0);
  }
  else
  {
    return(-1);
  }
}

// -----------------------------------------------------------------------------
int
CDSTimer::resetInterval(long timer_id, const CTimeValue &interval)
{
  if(timer_id == 0)
  {
    // Set new interval time
    tmInterval_ = interval;

    return(0);
  }
  else
  {
    return(-1);
  }
}

// -----------------------------------------------------------------------------
int
CDSTimer::isr(int irq)
{
  if((pBase_ != 0) && (pHandler_ != 0))
  {
    if(tmInterval_.msec() != 0)
    {
      // Set new interval
      setTimeout(tmInterval_);
    }
    else
    {
      // Disable timer
      pBase_[1] = 0;
    }

    pHandler_->handle_timeout(CTimeValue(0, 0), pArg_);
  }
  else
  {
    // ERROR
  }

  return(0);
}

// -----------------------------------------------------------------------------
void
CDSTimer::setTimeout(const CTimeValue & time)
{
  // Clock == 33.514MHz == 2^25MHz)
  // 0 =    1 (0s..0.002s)
  // 1 =   64 (0s..0.125s)
  // 2 =  256 (0s..0.500s)
  // 3 = 1024 (0s..2.000s)

  if(time.msec() <= 1)
  {
    // 0..1ms
    // FIXME!!!
    pBase_[0] = 0;
    pBase_[1] = (pBase_[1] & 0xfffc) | 0;
  }
  else if(time.msec() <= 125)
  {
    // 1..125ms
    pBase_[0] = (64*1024) - (time.msec() * (64*1024)) / 125;
    pBase_[1] = (pBase_[1] & 0xfffc) | 1;
  }
  else if(time.msec() <= 500)
  {
    // 125..500ms
    pBase_[0] = (64*1024) - (time.msec() * (64*1024)) / 500;
    pBase_[1] = (pBase_[1] & 0xfffc) | 2;
  }
  else if(time.msec() <= 2000)
  {
    // 500..2000ms
    pBase_[0] = (64*1024) - (time.msec() * (64*1024)) / 2000;
    pBase_[1] = (pBase_[1] & 0xfffc) | 3;
  }
  else
  {
    // > 2000ms
    // FIXME!!!
    pBase_[0] = 0;
    pBase_[1] = (pBase_[1] & 0xfffc) | 3;
  }
}
