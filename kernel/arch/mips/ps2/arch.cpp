#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "kernel/task.h"
#include "asm/arch/config.h"
#include "ps2Video.h"

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER


extern char _end;
extern char _heap_size;

CPS2Video        cVideo;

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

  if(cVideo.init() == -1)
    iRetVal = -1;

  pDebug = &cVideo;

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CPS2VideoDevice;
#endif // CONFIG_FRAMEBUFFER

  // Create task structure
  CTask * pTask = getNewTask(0, 0, 0);
  pTask->state(TS_RUNNING);

  return bricks_main();
}
