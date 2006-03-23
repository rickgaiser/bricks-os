#include "kernel/taskManager.h"
#include "kernel/timerManager.h"
#include "kernel/timeValue.h"
#include "iostream"


CTaskManager   CTaskManager::singleton_;


// -----------------------------------------------------------------------------
CTaskManager::CTaskManager()
 : iCurrentTaskIndex_(0)
 , pCurrentTask_(0)
 , pTimer_(0)
{
}

// -----------------------------------------------------------------------------
CTaskManager::~CTaskManager()
{
  if(pTimer_ != 0)
  {
    CTimerManager::instance()->releaseTimer(pTimer_);
  }
}

// -----------------------------------------------------------------------------
CTaskManager *
CTaskManager::instance()
{
  return(&singleton_);
}

// -----------------------------------------------------------------------------
int
CTaskManager::init()
{
  int iRetVal(0);

  // Get hardware timer
  pTimer_ = CTimerManager::instance()->getTimer();
  if(pTimer_ != 0)
  {
    if(pTimer_->schedule(*this, 0, CTimeValue(0, 0), CTimeValue(0, 10000)) == -1)
      iRetVal = -1;
  }
  else
  {
    iRetVal = -1;
  }

  // Create task object for current task
  task_[0].eState_ = TS_RUNNING;
  pCurrentTask_ = &task_[0];

  return(iRetVal);
}

// -----------------------------------------------------------------------------
CTask *
CTaskManager::getCurrentTask()
{
  return(pCurrentTask_);
}

// -----------------------------------------------------------------------------
int
CTaskManager::handle_timeout(const CTimeValue & tv, const void * arg)
{
//  std::cout<<"CTaskManager::handle_timeout"<<std::endl;
/*
  for(int i(iCurrentTaskIndex_ + 1); i != iCurrentTaskIndex_; i++)
  {
    if(i >= MAX_TASK_COUNT)
      i = 0;

    bool bFound(false);
    switch(task_[i].eState_)
    {
    case TS_RUNNING: // fall
    case TS_READY:
      bFound = true;
      break;
    }

    if(bFound == true)
    {
      // Set state of current task
      task_[iCurrentTaskIndex_].eState_ = TS_READY;

      // Set state of next task
      task_[i].eState_ = TS_RUNNING;

      // Switch tasks
      iCurrentTaskIndex_ = i;
      pCurrentTask_ = &task_[i];
      break;
    }
  }
*/
  return(0);
}
