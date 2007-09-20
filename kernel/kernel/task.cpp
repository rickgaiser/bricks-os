#include "kernel/debug.h"
#include "kernel/task.h"
#include "asm/cpu.h"
#include "string.h"


CTask        * CTaskManager::pCurrentTask_ = 0;
CThread      * CTaskManager::pCurrentThread_ = 0;
STaskQueue     CTaskManager::task_queue    = TAILQ_HEAD_INITIALIZER(CTaskManager::task_queue);
SThreadQueue   CTaskManager::thread_queue  = TAILQ_HEAD_INITIALIZER(CTaskManager::thread_queue);
SThreadQueue   CTaskManager::ready_queue   = TAILQ_HEAD_INITIALIZER(CTaskManager::ready_queue);
SThreadQueue   CTaskManager::timer_queue   = TAILQ_HEAD_INITIALIZER(CTaskManager::timer_queue);
uint32_t       CTaskManager::iPIDCount_    = 1;
useconds_t     CTaskManager::iCurrentTime_ = 0;


// -----------------------------------------------------------------------------
// To be implemeted in arch
extern CThread * getNewThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);


// -----------------------------------------------------------------------------
CThread::CThread(CTask * task)
 : iTimeout_(0)
 , pTask_(task)
 , pParent_(0)
 , eState_(TS_UNINITIALIZED)
{
  TAILQ_INIT(&children_queue);

  // Insert in the "all threads" queue
  TAILQ_INSERT_TAIL(&CTaskManager::thread_queue, this, thread_qe);
}

// -----------------------------------------------------------------------------
CThread::~CThread()
{
}

// -----------------------------------------------------------------------------
void
CThread::state(EThreadState state)
{
  // State change?
  if(eState_ != state)
  {
    // Remove from current state queue
    switch(eState_)
    {
    case TS_READY:
      TAILQ_REMOVE(&CTaskManager::ready_queue, this, state_qe);
      break;
    case TS_RUNNING:
      CTaskManager::pCurrentTask_   = NULL;
      CTaskManager::pCurrentThread_ = NULL;
      break;
    case TS_WAITING:
      TAILQ_REMOVE(&CTaskManager::timer_queue, this, state_qe);
      break;
    default:
      ;
    };

    eState_ = state;

    // Add to new state queue
    switch(eState_)
    {
    case TS_READY:
      TAILQ_INSERT_TAIL(&CTaskManager::ready_queue, this, state_qe);
      break;
    case TS_RUNNING:
      if(CTaskManager::pCurrentThread_ != NULL)
        CTaskManager::pCurrentThread_->state(TS_READY);
      CTaskManager::pCurrentTask_   = this->pTask_;
      CTaskManager::pCurrentThread_ = this;
      break;
    case TS_WAITING:
      {
        CThread * pThread;

        // Place task in timer queue
        TAILQ_FOREACH(pThread, &CTaskManager::timer_queue, state_qe)
        {
          if(this->iTimeout_ < pThread->iTimeout_)
          {
            TAILQ_INSERT_BEFORE(pThread, this, state_qe);
            break;
          }
        }
        // Place at end if no later one found
        if(pThread == NULL)
          TAILQ_INSERT_TAIL(&CTaskManager::timer_queue, this, state_qe);
        break;
      }
    default:
      printk("CThread::state: Warning: unhandled state\n");
    };
  }
}

// -----------------------------------------------------------------------------
CThread *
CThread::createChild(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  CThread * pThread;

  pThread = getNewThread(pTask_, entry, stack, svcstack, argc, argv);
  if(pThread != NULL)
    TAILQ_INSERT_TAIL(&children_queue, pThread, children_qe);

  return pThread;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CTask::CTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : iPID_(CTaskManager::iPIDCount_++)
{
  thr_ = getNewThread(this, entry, stack, svcstack, argc, argv);

  memset(pChannel_,    0, sizeof(pChannel_));
  memset(pConnection_, 0, sizeof(pConnection_));

  // Insert in the "all tasks" queue
  TAILQ_INSERT_TAIL(&CTaskManager::task_queue, this, task_qe);
}

// -----------------------------------------------------------------------------
CTask::~CTask()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool
CTaskManager::schedule()
{
  //printk("CTaskManager::schedule\n");

  CThread * pPrevThread = pCurrentThread_;
  CThread * pThread;

  // FIXME!
  iCurrentTime_++;
  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH(pThread, &timer_queue, state_qe)
  {
    // We can leave the loop if a not timed out task is found since the list
    // is sorted.
    if(iCurrentTime_ < pThread->iTimeout_)
      break;

    // Time out! Task is ready to run!
    pThread->iTimeout_ = 0;
    pThread->state(TS_READY);
  }

  // Locate the first task in the ready queue
  TAILQ_FOREACH(pThread, &ready_queue, state_qe)
  {
    pThread->state(TS_RUNNING);
    break;
  }

  // Make sure we have a task
  if(pCurrentThread_ == NULL)
    panic("CTaskManager::schedule: ERROR: No task to run\n");

  return pPrevThread != pCurrentThread_;
}

// -----------------------------------------------------------------------------
CTask *
CTaskManager::getTaskFromPID(pid_t pid)
{
  CTask * pTask;

  TAILQ_FOREACH(pTask, &task_queue, task_qe)
    if(pTask->iPID_ == pid)
      return pTask;

  return NULL;
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
    CTaskManager::pCurrentThread_->iTimeout_ = CTaskManager::iCurrentTime_ + useconds;
    CTaskManager::pCurrentThread_->state(TS_WAITING);

    // Schedule next task
    // FIXME: We can schedule the task, but since our context is unknown we
    //        can't run it.
    //CTaskManager::schedule();

    return 0;
  }

  return -1;
}
