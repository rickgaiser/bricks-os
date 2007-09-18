#include "kernel/debug.h"
#include "kernel/task.h"
#include "asm/cpu.h"
#include "string.h"


CTask      * CTaskManager::pCurrentTask_ = 0;
STaskQueue   CTaskManager::task_queue    = TAILQ_HEAD_INITIALIZER(CTaskManager::task_queue);
STaskQueue   CTaskManager::ready_queue   = TAILQ_HEAD_INITIALIZER(CTaskManager::ready_queue);
STaskQueue   CTaskManager::timer_queue   = TAILQ_HEAD_INITIALIZER(CTaskManager::timer_queue);
uint32_t     CTaskManager::iPIDCount_    = 1;
useconds_t   CTaskManager::iCurrentTime_ = 0;

// -----------------------------------------------------------------------------
CTask::CTask()
 : iTimeout_(0)
 , iPID_(CTaskManager::iPIDCount_++)
 , eState_(TS_UNINITIALIZED)
{
  memset(pChannel_,    0, sizeof(pChannel_));
  memset(pConnection_, 0, sizeof(pConnection_));

  // Insert in the "all tasks" queue
  TAILQ_INSERT_TAIL(&CTaskManager::task_queue, this, task_queue);
}

// -----------------------------------------------------------------------------
CTask::~CTask()
{
}

// -----------------------------------------------------------------------------
void
CTask::state(ETaskState state)
{
  // State change?
  if(eState_ != state)
  {
    // Remove from current state queue
    switch(eState_)
    {
    case TS_READY:
      TAILQ_REMOVE(&CTaskManager::ready_queue, this, state_queue);
      break;
    case TS_RUNNING:
      CTaskManager::pCurrentTask_ = NULL;
      break;
    case TS_WAITING:
      TAILQ_REMOVE(&CTaskManager::timer_queue, this, state_queue);
      break;
    default:
      ;
    };

    eState_ = state;

    // Add to new state queue
    switch(eState_)
    {
    case TS_READY:
      TAILQ_INSERT_TAIL(&CTaskManager::ready_queue, this, state_queue);
      break;
    case TS_RUNNING:
      if(CTaskManager::pCurrentTask_ != NULL)
        CTaskManager::pCurrentTask_->state(TS_READY);
      CTaskManager::pCurrentTask_ = this;
      break;
    case TS_WAITING:
      {
        CTask * pTask;

        // Place task in timer queue
        TAILQ_FOREACH(pTask, &CTaskManager::timer_queue, state_queue)
        {
          if(this->iTimeout_ < pTask->iTimeout_)
          {
            TAILQ_INSERT_BEFORE(pTask, this, state_queue);
            break;
          }
        }
        // Place at end if no later one found
        if(pTask == NULL)
          TAILQ_INSERT_TAIL(&CTaskManager::timer_queue, this, state_queue);
        break;
      }
    default:
      printk("CTask::state: Warning: unhandled state\n");
    };
  }
}

// -----------------------------------------------------------------------------
bool
CTaskManager::schedule()
{
  //printk("CTaskManager::schedule\n");

  CTask * pPrevTask = pCurrentTask_;
  CTask * pTask;

  // FIXME!
  iCurrentTime_++;
  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH(pTask, &timer_queue, state_queue)
  {
    // We can leave the loop if a not timed out task is found since the list
    // is sorted.
    if(iCurrentTime_ < pTask->iTimeout_)
      break;

    // Time out! Task is ready to run!
    pTask->iTimeout_ = 0;
    pTask->state(TS_READY);
  }

  // Locate the first task in the ready queue
  TAILQ_FOREACH(pTask, &ready_queue, state_queue)
  {
    pTask->state(TS_RUNNING);
    break;
  }

  // Make sure we have a task
  if(pCurrentTask_ == NULL)
    panic("CTaskManager::schedule: ERROR: No task to run\n");

  return pPrevTask != pCurrentTask_;
}

// -----------------------------------------------------------------------------
extern "C" pid_t
k_getpid(void)
{
  return CTaskManager::pCurrentTask_->iPID_;
}

// -----------------------------------------------------------------------------
int
k_usleep(useconds_t useconds)
{
  if(useconds > 0)
  {
    // Set timeout
    CTaskManager::pCurrentTask_->iTimeout_ = CTaskManager::iCurrentTime_ + useconds;
    CTaskManager::pCurrentTask_->state(TS_WAITING);

    // Schedule next task
    // FIXME: We can schedule the task, but since our context is unknown we
    //        can't run it.
    //CTaskManager::schedule();

    return 0;
  }

  return -1;
}
