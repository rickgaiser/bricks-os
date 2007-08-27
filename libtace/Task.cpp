#include "ace/Task.h"


//---------------------------------------------------------------------------
ACE_Task_Base::ACE_Task_Base(ACE_Thread_Manager * thr_man)
 : thr_count_(0)
 , thr_mgr_  (thr_man)
{
}

//---------------------------------------------------------------------------
ACE_Task_Base::~ACE_Task_Base()
{
}

//---------------------------------------------------------------------------
int
ACE_Task_Base::svc()
{
  return(0);
}

//---------------------------------------------------------------------------
int
ACE_Task_Base::activate()
{
  int iRetVal(-1);

  if(this->thr_mgr_ == 0)
    this->thr_mgr_ = ACE_Thread_Manager::instance();

  iRetVal = this->thr_mgr_->spawn(this->svc_run, this);

  return(iRetVal);
}

//---------------------------------------------------------------------------
int
ACE_Task_Base::wait()
{
//  if(this->thr_mgr() != 0)
//    return(this->thr_mgr()->wait_task(this));
//  else
    return(0);
}

//---------------------------------------------------------------------------
ACE_Thread_Manager *
ACE_Task_Base::thr_mgr() const
{
  return(this->thr_mgr_);
}

//---------------------------------------------------------------------------
void
ACE_Task_Base::thr_mgr(ACE_Thread_Manager * thr_mgr)
{
  this->thr_mgr_ = thr_mgr;
}

//---------------------------------------------------------------------------
ACE_THR_FUNC_RETURN
ACE_Task_Base::svc_run(void * args)
{
  ACE_THR_FUNC_RETURN iRetVal;
  
  ACE_Task_Base * t = (ACE_Task_Base *)args;
  iRetVal = t->svc();

  return(iRetVal);
}
