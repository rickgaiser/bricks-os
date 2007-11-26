#include "kernel/debug.h"
#include "unistd.h"
#include "pthread.h"


//---------------------------------------------------------------------------
// Test sleeping
void
testSleep()
{
  int i;

  printk(" - Sleep 5x 1s:");
  for(i = 0; i < 5; i++)
  {
    sleep(1);
    printk(".");
  }
  printk("done\n");
}

//---------------------------------------------------------------------------
// Main test thread
void *
testThread(void * arg)
{
  printk(" - Test Thread running\n");

  testSleep();

  // Thread exit
  pthread_exit(NULL);

  return 0;
}

//---------------------------------------------------------------------------
int
appMain(int argc, char * argv[])
{
  printk("Bricks-OS system test:\n");

  // Start test thread since we're called from the kernel thread, and
  // sleeping here will couse the system to die.
  printk(" - Starting test thread: ");
  pthread_t thr;
  if(pthread_create(&thr, 0, testThread, 0) == 0)
    printk("done\n");
  else
    printk("error\n");

  while(1);

  return 0;
}
