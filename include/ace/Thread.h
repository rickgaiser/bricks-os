#ifndef ACE_THREAD_H
#define ACE_THREAD_H


#include "ace/config.h"


class ACE_Thread
{
public:
  static int spawn(ACE_THR_FUNC func, void * arg = 0);

private:
  ACE_Thread();
};


#endif
