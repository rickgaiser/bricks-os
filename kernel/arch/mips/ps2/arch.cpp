#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "kernel/task.h"
#include "asm/arch/config.h"
#include "videoDevice.h"
#include "task.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING


extern char _end;
extern char _heap_size;
#define HEAP_START ((uint32_t)(&_end))
#define HEAP_END   ((uint32_t)(&_end + &_heap_size))


#ifdef CONFIG_DEBUGGING
CPS2DebugScreen cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
CPS2VideoDevice * pVideoDevice;
#endif // CONFIG_FRAMEBUFFER


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

  init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

#ifdef CONFIG_DEBUGGING
  if(cDebug.init() == -1)
    iRetVal = -1;
  pDebug = &cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CPS2VideoDevice;
#endif // CONFIG_FRAMEBUFFER

  CPS2Thread::init();

  printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);

  printk("PS2 arch ready\n");

  return bricks_main();
}
