#include "kernel/debug.h"
#include "kernel/task.h"
#include "asm/cpu.h"


uint32_t  CTaskManager::iTaskCount_(0);
CTask   * CTaskManager::pCurrentTask_ = 0;
CTask   * CTaskManager::taskTable_[MAX_TASK_COUNT];


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
  //printk("CTaskManager::addTask\n");

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
    printk("CTaskManager::addTask: ERROR: Task list full\n");
  }
}

// -----------------------------------------------------------------------------
void
CTaskManager::removeTask(CTask * pTask)
{
  //printk("CTaskManager::removeTask\n");

  if(pTask != pTask->next)
  {
    // Remove current task from list
    pTask->prev->next = pTask->next;
    pTask->next->prev = pTask->prev;

    // Flag task to be destroyed
    pTask->eState_ = TS_DESTROY;
  }
  else
  {
    panic("CTaskManager::removeTask: ERROR: Can not kill last task\n");
  }
}

// -----------------------------------------------------------------------------
bool
CTaskManager::schedule()
{
  //printk("CTaskManager::schedule\n");

  CTask * pPrevTask = CTaskManager::pCurrentTask_;

  // If the current task is running, put it on ready
  if(CTaskManager::pCurrentTask_->eState_ == TS_RUNNING)
    CTaskManager::pCurrentTask_->eState_ = TS_READY;

  // Locate the next ready task
  do
  {
    CTaskManager::pCurrentTask_ = CTaskManager::pCurrentTask_->next;
  }
  while((CTaskManager::pCurrentTask_ != pPrevTask) && (CTaskManager::pCurrentTask_->eState_ != TS_READY));

  // Make it run if ready
  if(CTaskManager::pCurrentTask_->eState_ == TS_READY)
    CTaskManager::pCurrentTask_->eState_ = TS_RUNNING;
  else
  {
    panic("CTaskManager::schedule: ERROR: No tasks to schedule\n");
  }

  return pPrevTask != CTaskManager::pCurrentTask_;
}
