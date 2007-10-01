#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "kernel/task.h"
#include "asm/arch/config.h"
#include "videoDevice.h"


extern char _end;
extern char _heap_size;


CPS2VideoDevice * pVideoDevice;


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  // FIXME
  init_heap(&_end, 4 * 1024 * 1024);

  pVideoDevice = new CPS2VideoDevice;
  pDebug = new CPS2Surface;

  // Create task structure
  CTask * pTask = new CTask(0, 0, 0);
  pTask->thr_->state(TS_RUNNING);

  return bricks_main();
}
