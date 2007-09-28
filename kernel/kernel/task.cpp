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
SThreadQueue   CTaskManager::wait_queue    = TAILQ_HEAD_INITIALIZER(CTaskManager::wait_queue);
SThreadQueue   CTaskManager::dead_queue    = TAILQ_HEAD_INITIALIZER(CTaskManager::dead_queue);
uint32_t       CTaskManager::iPIDCount_    = 1;
useconds_t     CTaskManager::iCurrentTime_ = 0;


// -----------------------------------------------------------------------------
// To be implemeted in arch
extern CThread * getNewThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);


// -----------------------------------------------------------------------------
CThread::CThread(CTask * task)
 : iTimeout_(0)
 , pWaitObj_(NULL)
 , iWaitReturn_(0)
 , pTask_(task)
 , pParent_(NULL)
 , eState_(TS_UNINITIALIZED)
{
  TAILQ_INIT(&children_queue);

  // Insert in the "all threads" queue
  TAILQ_INSERT_TAIL(&CTaskManager::thread_queue, this, thread_qe);
}

// -----------------------------------------------------------------------------
CThread::~CThread()
{
  // Remove from the "all threads" queue
  TAILQ_REMOVE(&CTaskManager::thread_queue, this, thread_qe);
}

// -----------------------------------------------------------------------------
void
CThread::runJump()
{
  panic("CThread::runJump: Can't jump to task!\n");
}

// -----------------------------------------------------------------------------
void
CThread::runReturn()
{
  panic("CThread::runReturn: Can't return to task!\n");
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
      // Remove from timer queue
      if(this->iTimeout_ > 0)
      {
        this->iTimeout_ = 0;
        TAILQ_REMOVE(&CTaskManager::timer_queue, this, state_qe);
      }
      // Remove from wait queue
      if(this->pWaitObj_ != NULL)
      {
        this->pWaitObj_ = NULL;
        TAILQ_REMOVE(&CTaskManager::wait_queue, this, wait_qe);
      }
      break;
    case TS_DEAD:
      TAILQ_REMOVE(&CTaskManager::dead_queue, this, state_qe);
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

        if((this->iTimeout_ <= 0) && (this->pWaitObj_ == NULL))
          panic("CThread::state: What are we waiting for???\n");

        // Place task in sorted timer queue, if timeout present
        if(this->iTimeout_ > 0)
        {
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
        }

        // Place task in object wait queue, if object present
        if(this->pWaitObj_ != NULL)
          TAILQ_INSERT_TAIL(&CTaskManager::wait_queue, this, wait_qe);

        break;
      }
    case TS_DEAD:
      TAILQ_INSERT_TAIL(&CTaskManager::dead_queue, this, state_qe);
      break;
    case TS_DESTROY:
      // FIXME:
      //  - Free used resources?
      //  - notify connected threads/tasks?
      //  - free memory?
      //  - Do it here, in destructor or in removeDestroyed?
      break;
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
void
CTaskManager::updateSleepers()
{
  CThread * pThread;

  //printk("CTaskManager::updateSleepers\n");

  // FIXME! This only works with 100Hz scheduler (10000us/10ms interval)
  iCurrentTime_ += 10000;
  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH(pThread, &timer_queue, state_qe)
  {
    // We can leave the loop if a not timed out task is found since the list
    // is sorted.
    if(iCurrentTime_ < pThread->iTimeout_)
      break;

    // Time out! Task is ready to run!
    if(pThread->pWaitObj_ != NULL)
      pThread->iWaitReturn_ = -1;
    else
      pThread->iWaitReturn_ = 0;
    pThread->state(TS_READY);
  }
}

// -----------------------------------------------------------------------------
void
CTaskManager::removeDestroyed()
{
  CThread * pThread;
  CThread * pSafe;

  //printk("CTaskManager::removeDestroyed\n");

  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH_SAFE(pThread, &dead_queue, state_qe, pSafe)
  {
    pThread->state(TS_DESTROY);
  }
}

// -----------------------------------------------------------------------------
bool
CTaskManager::schedule()
{
  CThread * pPrevThread = pCurrentThread_;

  //printk("CTaskManager::schedule\n");

  // Run the first task in the ready queue
  if(TAILQ_FIRST(&ready_queue) != NULL)
    TAILQ_FIRST(&ready_queue)->state(TS_RUNNING);

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
int
genwait_wait(void * obj, useconds_t useconds)
{
  //printk("genwait_wait %dus\n", useconds);

  // Set timeout if present
  if(useconds > 0)
    CTaskManager::pCurrentThread_->iTimeout_ = CTaskManager::iCurrentTime_ + useconds;
  else
    CTaskManager::pCurrentThread_->iTimeout_ = 0;

  // Set wait object
  CTaskManager::pCurrentThread_->pWaitObj_ = obj;

  // change thread state
  CTaskManager::pCurrentThread_->state(TS_WAITING);

  // Schedule next thread
  CTaskManager::schedule();

  // Run next thread
  CTaskManager::pCurrentThread_->runJump();

  return CTaskManager::pCurrentThread_->iWaitReturn_;
}

// -----------------------------------------------------------------------------
int
genwait_wake(void * obj, int maxcount)
{
  int count(0);
  CThread * pThread;
  CThread * pSafe;

  //printk("genwait_wake %d\n", count);

  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH_SAFE(pThread, &CTaskManager::wait_queue, wait_qe, pSafe)
  {
    if(pThread->pWaitObj_ == obj)
    {
      pThread->iWaitReturn_ = 0;
      pThread->state(TS_READY);
      if(maxcount > 0)
      {
        count++;
        if(count >= maxcount)
          break;
      }
    }
  }
  return count;
}

// -----------------------------------------------------------------------------
pid_t
k_getpid(void)
{
  return CTaskManager::pCurrentTask_->iPID_;
}

// -----------------------------------------------------------------------------
unsigned int
k_sleep(unsigned int iSeconds)
{
  return genwait_wait(NULL, iSeconds * 1000000);
}

// -----------------------------------------------------------------------------
int
k_usleep(useconds_t useconds)
{
  return genwait_wait(NULL, useconds);
}
