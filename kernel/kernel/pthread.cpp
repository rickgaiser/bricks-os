#include "pthread.h"
#include "kernel/task.h"
#include "asm/cpu.h"


// -----------------------------------------------------------------------------
extern "C" int
pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void *), void * arg)
{
  int iRetVal(-1);

  CThread * pThread = CTaskManager::pCurrentThread_->createChild((void *)start_routine, 512, 512, (int)arg, 0);
  if(pThread != 0)
  {
    pThread->state(TS_READY);
    thread->pThread = pThread;
    iRetVal = 0;
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
extern "C" void
pthread_exit(void * status)
{
}

// -----------------------------------------------------------------------------
extern "C" int
pthread_mutex_destroy(pthread_mutex_t * mutex)
{
  return 0;
}
/*
// -----------------------------------------------------------------------------
extern "C" int
pthread_mutex_getprioceiling(const pthread_mutex_t * mutex, int *)
{
}
*/
// -----------------------------------------------------------------------------
extern "C" int
pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutexattr_t * attr)
{
  unsigned long flags = local_save_flags();
  local_irq_disable();
  mutex->iLock = 0;
  local_irq_restore(flags);

  return 0;
}

// -----------------------------------------------------------------------------
extern "C" int
pthread_mutex_lock(pthread_mutex_t * mutex)
{
  unsigned long flags = local_save_flags();

  while(true)
  {
    local_irq_disable();
    if(mutex->iLock == 0)
      break;
    local_irq_restore(flags);
  }
  mutex->iLock = 1;
  local_irq_restore(flags);

  return 0;
}
/*
// -----------------------------------------------------------------------------
extern "C" int
pthread_mutex_setprioceiling(pthread_mutex_t * mutex, int, int *)
{
}
*/
// -----------------------------------------------------------------------------
extern "C" int
pthread_mutex_trylock(pthread_mutex_t * mutex)
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
extern "C" int
pthread_mutex_unlock(pthread_mutex_t * mutex)
{
  int iRetVal(-1);
  unsigned long flags = local_save_flags();

  local_irq_disable();
  if(mutex->iLock == 1)
  {
    mutex->iLock = 0;
    iRetVal = 0;
  }
  local_irq_restore(flags);

  return iRetVal;
}
