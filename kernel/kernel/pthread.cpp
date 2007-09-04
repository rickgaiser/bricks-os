#include "pthread.h"
#include "kernel/task.h"
#include "asm/cpu.h"


// -----------------------------------------------------------------------------
extern "C" int
pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void *), void * arg)
{
  int iRetVal(-1);
  CTask * pTask = getNewTask((void *)start_routine, 512, 512, (int)arg, 0);
  if(pTask != 0)
  {
    pTask->eState_ = TS_READY;
    CTaskManager::addTask(pTask);
    thread->pThread = pTask;
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
  CCPU::cli();
  mutex->iLock = 0;
  CCPU::sti();

  return 0;
}

// -----------------------------------------------------------------------------
extern "C" int
pthread_mutex_lock(pthread_mutex_t * mutex)
{
  while(true)
  {
    CCPU::cli();
    if(mutex->iLock == 0)
      break;
    CCPU::sti();
  }
  mutex->iLock = 1;
  CCPU::sti();

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

  CCPU::cli();
  if(mutex->iLock == 0)
  {
    mutex->iLock = 1;
    iRetVal = 0;
  }
  CCPU::sti();

  return iRetVal;
}

// -----------------------------------------------------------------------------
extern "C" int
pthread_mutex_unlock(pthread_mutex_t * mutex)
{
  int iRetVal(-1);

  CCPU::cli();
  if(mutex->iLock == 1)
  {
    mutex->iLock = 0;
    iRetVal = 0;
  }
  CCPU::sti();

  return iRetVal;
}
