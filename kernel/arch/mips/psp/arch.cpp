#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include "asm/arch/config.h"
#include "kernel/bricks.h"
#include "kernel/debug.h"
//#include "kernel/memoryManager.h"
#include "video.h"


CPSPVideo      cVideo;

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

  // Initialize the memory manager so we can use new/delete/malloc/free
  //init_heap(&__heap_start, (uint32_t)(&__heap_end - &__heap_start));

  SetupCallbacks();

  if(cVideo.init() == -1)
    iRetVal = -1;
  pDebug = &cVideo;

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CPSPVideoDevice;
#endif // CONFIG_FRAMEBUFFER

//  sceKernelSleepThread();

  return bricks_main();
}
