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

  // FIXME
  init_heap(&_end, 4 * 1024 * 1024);

#ifdef CONFIG_DEBUGGING
  if(cDebug.init() == -1)
    iRetVal = -1;
  pDebug = &cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CPS2VideoDevice;
#endif // CONFIG_FRAMEBUFFER

  CPS2Thread::init();

  return bricks_main();
}
