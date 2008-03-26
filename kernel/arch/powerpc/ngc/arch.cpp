#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "asm/arch/config.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING

#include "serial.h"

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER


extern char __ArenaLo, __ArenaHi;
#define HEAP_START ((uint32_t)(&__ArenaLo))
#define HEAP_END   ((uint32_t)(&__ArenaHi))


#ifdef CONFIG_DEBUGGING
CNGCDebugScreen cDebug;
#endif // CONFIG_DEBUGGING

CNGCSerial cSerial;

#ifdef CONFIG_FRAMEBUFFER
CNGCVideoDevice * pVideoDevice;
#endif // CONFIG_FRAMEBUFFER


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

  //init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

#ifdef CONFIG_DEBUGGING
  if(cDebug.init() == -1)
    iRetVal = -1;
  pDebug = &cDebug;
#endif // CONFIG_DEBUGGING

  cSerial.init();

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CNGCVideoDevice;
#endif // CONFIG_FRAMEBUFFER

  //printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);

  printk("NGC arch ready\n");

  return bricks_main();
}
