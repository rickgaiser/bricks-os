#ifndef ACE_OS_H
#define ACE_OS_H


#include "ace/config.h"
#include "ace/Time_Value.h"


class ACE_OS
{
public:
  static int thr_create(ACE_THR_FUNC func, void * args);
  static int sleep(const ACE_Time_Value & tv);
};


#endif
