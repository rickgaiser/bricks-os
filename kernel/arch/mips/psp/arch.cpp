#include <pspkernel.h>
#include <pspdebug.h> 
#include <pspctrl.h>
//#include "kernel/bricks.h"
//#include "kernel/debug.h"
//#include "kernel/memoryManager.h"


PSP_MODULE_INFO("Bricks-OS", 0, 1, 1);


// -----------------------------------------------------------------------------
int
exit_callback(int arg1, int arg2, void *common)
{
  sceKernelExitGame();
  return 0;
}

// -----------------------------------------------------------------------------
int
CallbackThread(SceSize args, void *argp)
{
  int cbid;

  cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
  sceKernelRegisterExitCallback(cbid);

  sceKernelSleepThreadCB();

  return 0;
}

// -----------------------------------------------------------------------------
int
SetupCallbacks(void)
{
  int thid = 0;

  thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
  if(thid >= 0)
  {
    sceKernelStartThread(thid, 0, 0);
  }

  return thid;
} 

// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  // Initialize the memory manager so we can use new/delete/malloc/free
  //init_heap(&__heap_start, (uint32_t)(&__heap_end - &__heap_start));

  pspDebugScreenInit();
  SetupCallbacks();

  pspDebugScreenPrintf("Bricks-OS\n");
  pspDebugScreenPrintf("=========\n");

  sceKernelSleepThread(); 
  
  return 0;//bricks_main();
}
