#include "ace/OS.h"


//---------------------------------------------------------------------------
int
ACE_OS::thr_create(ACE_THR_FUNC func, void * args)
{
  return -1;
}

//---------------------------------------------------------------------------
int
ACE_OS::sleep(const ACE_Time_Value & tv)
{
  return -1;
}
