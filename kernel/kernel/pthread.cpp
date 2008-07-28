#include "pthread.h"
#include "kernel/debug.h"
#include "kernel/task.h"
#include "kernel/genwait.h"
#include "asm/cpu.h"


// -----------------------------------------------------------------------------
// Threads
// -----------------------------------------------------------------------------
int
k_pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void *), void * arg)
{
  int iRetVal(-1);

  CThread * pThread = CTaskManager::pCurrentThread_->createChild((void *)start_routine, (int)arg, 0);
  if(pThread != 0)
  {
    pThread->state(TS_READY);
    thread->pThread = pThread;
    iRetVal = 0;
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
void
k_pthread_exit(void * status)
{
  // Change tasks state
  CTaskManager::pCurrentThread_->state(TS_DEAD);
  // Schedule next thread
  CTaskManager::schedule();
  // Jump to next task
  CTaskManager::pCurrentThread_->impl().runJump();
}

// -----------------------------------------------------------------------------
int
k_pthread_join(pthread_t thread, void ** value_ptr)
{
  // Check if thread exists
  // ...

  // Wait untill thread becomes TS_DEAD
  // ...

  // Set return value
  //*value_ptr = thread.exit_status;

  // Return
  //return 0;
  return -1;
}

// -----------------------------------------------------------------------------
// Condition
// -----------------------------------------------------------------------------
int
k_pthread_cond_init(pthread_cond_t * cond, const pthread_condattr_t * attr)
{
  cond->iLock = 1;

  return 0;
}

// -----------------------------------------------------------------------------
int
k_pthread_cond_destroy(pthread_cond_t * cond)
{
  return 0;
}

// -----------------------------------------------------------------------------
int
k_pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex)
{
  unsigned long flags = local_save_flags();

  // Unlock the thing we're waiting for
  k_pthread_mutex_unlock(mutex);

  // Wait for condition
  local_irq_disable();
  if(cond->iLock != 0)
    genwait_wait(cond, 0);
  cond->iLock = 1;
  local_irq_restore(flags);

  // Lock it again so we can use it
  k_pthread_mutex_lock(mutex);

  return 0;
}

// -----------------------------------------------------------------------------
int
k_pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec * ts)
{
  unsigned long flags = local_save_flags();

  // Unlock the thing we're waiting for
  k_pthread_mutex_unlock(mutex);

  // Wait for condition
  local_irq_disable();
  if(cond->iLock != 0)
    genwait_wait(cond, (ts->tv_sec * 1000000) + (ts->tv_nsec / 1000));
  cond->iLock = 1;
  local_irq_restore(flags);

  // Lock it again so we can use it
  k_pthread_mutex_lock(mutex);

  return 0;
}

// -----------------------------------------------------------------------------
int
k_pthread_cond_signal(pthread_cond_t * cond)
{
  int iRetVal(-1);
  unsigned long flags = local_save_flags();

  local_irq_disable();
  if(cond->iLock == 1)
  {
    cond->iLock = 0;
    genwait_wake(cond, 1); // Wake 1
    iRetVal = 0;
  }
  local_irq_restore(flags);

  return iRetVal;
}

// -----------------------------------------------------------------------------
int
k_pthread_cond_broadcast(pthread_cond_t * cond)
{
  int iRetVal(-1);
  unsigned long flags = local_save_flags();

  local_irq_disable();
  if(cond->iLock == 1)
  {
    cond->iLock = 0;
    genwait_wake(cond, 0); // Wake all
    iRetVal = 0;
  }
  local_irq_restore(flags);

  return iRetVal;
}

// -----------------------------------------------------------------------------
// Mutex
// -----------------------------------------------------------------------------
int
k_pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutexattr_t * attr)
{
  mutex->iLock = 0;

  return 0;
}

// -----------------------------------------------------------------------------
int
k_pthread_mutex_destroy(pthread_mutex_t * mutex)
{
  return 0;
}

// -----------------------------------------------------------------------------
int
k_pthread_mutex_lock(pthread_mutex_t * mutex)
{
  unsigned long flags = local_save_flags();

  local_irq_disable();
  if(mutex->iLock != 0)
    genwait_wait(mutex, 0);
  mutex->iLock = 1;
  local_irq_restore(flags);

  return 0;
}

// -----------------------------------------------------------------------------
int
k_pthread_mutex_trylock(pthread_mutex_t * mutex)
{
  int iRetVal(-1);
  unsigned long flags = local_save_flags();

  local_irq_disable();
  if(mutex->iLock == 0)
  {
    mutex->iLock = 1;
    iRetVal = 0;
  }
  local_irq_restore(flags);

  return iRetVal;
}

// -----------------------------------------------------------------------------
int
k_pthread_mutex_unlock(pthread_mutex_t * mutex)
{
  int iRetVal(-1);
  unsigned long flags = local_save_flags();

  local_irq_disable();
  if(mutex->iLock == 1)
  {
    mutex->iLock = 0;
    genwait_wake(mutex, 1);
    iRetVal = 0;
  }
  local_irq_restore(flags);

  return iRetVal;
}
