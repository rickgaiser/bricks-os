#include "kernel/debug.h"
#include "kernel/genwait.h"
#include "kernel/task.h"


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
  CTaskManager::pCurrentThread_->impl().runJump();

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
