#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/pthread_k.h"
#include "kernel/task.h"
#include "asm/arch/config.h"
#include "asm/cpu.h"


extern "C" int appMain(int argc, char * argv[]);


// -----------------------------------------------------------------------------
// Main thread
void *
mainThread(void *)
{
  int iRetVal;

  // Execute main application
  iRetVal = appMain(0, 0);

  // Return
  printk("Main thread done\n");
  k_pthread_exit((void *)iRetVal);

  return (void *)iRetVal;
}

// -----------------------------------------------------------------------------
// Idle thread (never returns)
void *
idleThread(void *)
{
  while(true)
  {
    ::halt();
  }

  return NULL;
}

// -----------------------------------------------------------------------------
int
bricks_main()
{
  // Welcome user :-)
  printk("Bricks-OS\n");
  printk("=========\n");

  // Create main thread
  pthread_t thr;
  if(k_pthread_create(&thr, 0, mainThread, (void *)NULL) != 0)
    panic("Unable to create main thread\n");

  // We become the idle thread
  CTaskManager::pIdleThread_ = CTaskManager::pCurrentThread_;
  CTaskManager::pIdleThread_->state(TS_READY);
  idleThread(NULL);

  return 0;
}
