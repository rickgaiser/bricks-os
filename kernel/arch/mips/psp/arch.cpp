#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include "asm/arch/config.h"
#include "kernel/bricks.h"
#include "kernel/debug.h"
//#include "kernel/memoryManager.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER


#ifdef CONFIG_DEBUGGING
CPSPDebugScreen cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
CPSPVideoDevice * pVideoDevice;
#endif // CONFIG_FRAMEBUFFER


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
  int iRetVal(0);

  //init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

  SetupCallbacks();

#ifdef CONFIG_DEBUGGING
  if(cDebug.init() == -1)
    iRetVal = -1;
  pDebug = &cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CPSPVideoDevice;
#endif // CONFIG_FRAMEBUFFER

//  sceKernelSleepThread();

  //printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);

  printk("PSP arch ready\n");

  return bricks_main();
}
