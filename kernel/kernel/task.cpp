#include "kernel/task.h"
#include "asm/cpu.h"
#include "iostream"


uint32_t  CTaskManager::iTaskCount_(0);
CTask   * CTaskManager::pCurrentTask_ = 0;
CTask   * CTaskManager::taskTable_[MAX_TASK_COUNT];

IFileIO * pSTDIN = 0;
IFileIO * pSTDOUT = 0;


// -----------------------------------------------------------------------------
CTask::CTask()
 : eState_(TS_UNINITIALIZED)
{
  for(int i(0); i < MAX_CHANNEL_COUNT; i++)
    pChannel_[i] = 0;

  for(int i(0); i < MAX_CONNECTION_COUNT; i++)
    pConnection_[i] = 0;

  prev = this;
  next = this;
}

// -----------------------------------------------------------------------------
CTask::~CTask()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
CTaskManager::addTask(CTask * pTask)
{
  //std::cout<<"CTaskManager::addTask"<<std::endl;

  if(iTaskCount_ == 0)
  {
    for(int iTask(0); iTask < MAX_TASK_COUNT; iTask++)
      taskTable_[iTask] = 0;
    pCurrentTask_ = pTask;
    taskTable_[0] = pTask;
    iTaskCount_++;
  }
  else if(iTaskCount_ < MAX_TASK_COUNT)
  {
    pTask->next = pCurrentTask_->next;
    pTask->prev = pCurrentTask_;

    pCurrentTask_->next->prev = pTask;
    pCurrentTask_->next = pTask;

    for(int iTask(0); iTask < MAX_TASK_COUNT; iTask++)
    {
      if(taskTable_[iTask] == 0)
      {
        taskTable_[iTask] = pTask;
        break;
      }
    }

    iTaskCount_++;
  }
  else
  {
    std::cout<<"CTaskManager::addTask: ERROR: Task list full"<<std::endl;
  }
}

// -----------------------------------------------------------------------------
void
CTaskManager::removeTask(CTask * pTask)
{
  //std::cout<<"CTaskManager::removeTask"<<std::endl;

  if(pTask != pTask->next)
  {
    // Remove current task from list
    pTask->prev->next = pTask->next;
    pTask->next->prev = pTask->prev;

    // Prevent removing the current task
    if(pTask == CTaskManager::pCurrentTask_)
    {
      // Set new current task to prev task
      CTaskManager::pCurrentTask_ = pTask->prev;
      // Reschedule so the next task will run
      schedule();
    }

    // Delete task
    //delete pTask;
  }
  else
  {
    std::cout<<"CTaskManager::removeTask: ERROR: Can not kill last task"<<std::endl;
    CCPU::halt();
  }
}

// -----------------------------------------------------------------------------
bool
CTaskManager::schedule()
{
  //std::cout<<"CTaskManager::schedule"<<std::endl;

  CTask * pPrevTask = CTaskManager::pCurrentTask_;

  // Locate the next running task
  CTaskManager::pCurrentTask_->eState_ = TS_READY;
  CTaskManager::pCurrentTask_ = CTaskManager::pCurrentTask_->next;
  while(CTaskManager::pCurrentTask_->eState_ != TS_READY)
    CTaskManager::pCurrentTask_ = CTaskManager::pCurrentTask_->next;
  CTaskManager::pCurrentTask_->eState_ = TS_RUNNING;

  return pPrevTask != CTaskManager::pCurrentTask_;
}

// -----------------------------------------------------------------------------
void
CTaskManager::setStandardInput(IFileIO * stdin)
{
  pSTDIN = stdin;
}

// -----------------------------------------------------------------------------
void
CTaskManager::setStandardOutput(IFileIO * stdout)
{
  pSTDOUT = stdout;
}
