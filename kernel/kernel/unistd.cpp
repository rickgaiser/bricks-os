#include "kernel/debug.h"
#include "kernel/unistd_k.h"
#include "kernel/genwait.h"
#include "kernel/task.h"


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
