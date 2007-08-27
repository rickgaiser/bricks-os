#include "ace/Thread_Manager.h"


ACE_Thread_Manager * ACE_Thread_Manager::thr_mgr_ = 0;


//---------------------------------------------------------------------------
ACE_Thread_Manager::ACE_Thread_Manager()
{
}

//---------------------------------------------------------------------------
ACE_Thread_Manager::~ACE_Thread_Manager()
{
}

//---------------------------------------------------------------------------
ACE_Thread_Manager *
ACE_Thread_Manager::instance()
{
  if(ACE_Thread_Manager::thr_mgr_ == 0)
    thr_mgr_ = new ACE_Thread_Manager;
  return(thr_mgr_);
}

//---------------------------------------------------------------------------
int
ACE_Thread_Manager::spawn(ACE_THR_FUNC func, void * args)
{
  return(ACE_Thread::spawn(func, args));
}
