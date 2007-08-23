#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H


#include "timeValue.h"


class IEventHandler
{
public:
  virtual ~IEventHandler(){}

  virtual int handle_timeout(const CTimeValue & tv, const void * arg = 0){return(-1);}
};


#endif
