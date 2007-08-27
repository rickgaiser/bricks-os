#include "ace/Thread.h"
#include "ace/OS.h"


//---------------------------------------------------------------------------
int
ACE_Thread::spawn(ACE_THR_FUNC func, void * arg)
{
  return(ACE_OS::thr_create(func, arg));
}
