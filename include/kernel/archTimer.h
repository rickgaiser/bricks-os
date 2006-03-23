#ifndef ARCHTIMER_H
#define ARCHTIMER_H


#include "eventHandler.h"
#include "timeValue.h"


/**
 * \brief Interface for Timer hardware
 */
class ITimer
{
public:
  virtual long schedule(IEventHandler & handler, void * arg, const CTimeValue & start, const CTimeValue & interval) = 0;
  virtual int cancel(long timer_id) = 0;
  virtual int resetInterval(long timer_id, const CTimeValue &interval) = 0;
};


#endif
