#include "kernel/debug.h"
#include "kernel/task.h"
#include "asm/cpu.h"


CTask      * CTaskManager::pCurrentTask_ = 0;
STaskQueue   CTaskManager::task_queue  = TAILQ_HEAD_INITIALIZER(CTaskManager::task_queue);
STaskQueue   CTaskManager::run_queue   = TAILQ_HEAD_INITIALIZER(CTaskManager::run_queue);


// -----------------------------------------------------------------------------
CTask::CTask()
 : eState_(TS_UNINITIALIZED)
 , iTimeout_(0)
{
  for(int i(0); i < MAX_CHANNEL_COUNT; i++)
    pChannel_[i] = 0;

  for(int i(0); i < MAX_CONNECTION_COUNT; i++)
    pConnection_[i] = 0;
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

  // Insert in the run queue (only if not already running)
  if(pCurrentTask_ == 0)
    pCurrentTask_ = pTask;
  else
    TAILQ_INSERT_TAIL(&run_queue, pTask, state_queue);

  // Insert in the "all tasks" queue
  TAILQ_INSERT_TAIL(&task_queue, pTask, task_queue);
}

// -----------------------------------------------------------------------------
void
CTaskManager::removeTask(CTask * pTask)
{
  //printk("CTaskManager::removeTask\n");

  // Remove from "all tasks" queue
  TAILQ_REMOVE(&task_queue, pTask, task_queue);
  // Remove from run queue
  if(pTask != pCurrentTask_)
    TAILQ_REMOVE(&run_queue, pTask, state_queue);
}

// -----------------------------------------------------------------------------
void
k_usleep(unsigned long usec)
{
  if(usec > 0)
  {
    CTaskManager::pCurrentTask_->eState_   = TS_WAITING;
    CTaskManager::pCurrentTask_->iTimeout_ = usec;
    // Place back into the run queue
    TAILQ_INSERT_TAIL(&CTaskManager::run_queue, CTaskManager::pCurrentTask_, state_queue);
    // Schedule next task
    CTaskManager::schedule();
  }
}

// -----------------------------------------------------------------------------
bool
CTaskManager::schedule()
{
  //printk("CTaskManager::schedule\n");

  CTask * pPrevTask = pCurrentTask_;
  CTask * pTask;

  // If the current task is running, put it on ready at the end of the queue
  if(pCurrentTask_->eState_ == TS_RUNNING)
  {
    pCurrentTask_->eState_ = TS_READY;
    TAILQ_INSERT_TAIL(&run_queue, pCurrentTask_, state_queue);
  }

  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH(pTask, &run_queue, state_queue)
  {
    if(pTask->iTimeout_ > 0)
    {
      pTask->iTimeout_--;

      if(pTask->iTimeout_ == 0)
      {
        // Wake up task
        pTask->eState_ = TS_READY;
      }
    }
  }

  // Locate the first task in the run queue
  TAILQ_FOREACH(pTask, &run_queue, state_queue)
  {
    if(pCurrentTask_->eState_ == TS_READY)
    {
      // Remove from the run queue
      pCurrentTask_ = pTask;
      pCurrentTask_->eState_ = TS_RUNNING;
      TAILQ_REMOVE(&run_queue, pCurrentTask_, state_queue);
      break;
    }
  }

  // Make sure we have a task
  if(pCurrentTask_ == 0)
    panic("CTaskManager::schedule: ERROR: No task to run\n");

  return pPrevTask != pCurrentTask_;
}
