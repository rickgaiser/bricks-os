#include "kernel/bricks.h"
#include "kernel/task.h"
#include "ps2Video.h"

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER


IFileIO          cDummy;
CPS2Video        cVideo;

#ifdef CONFIG_FRAMEBUFFER
CPS2VideoDevice * pVideoDevice;
#endif // CONFIG_FRAMEBUFFER


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

  if(cVideo.init() == -1)
    iRetVal = -1;

  // Set standard in/out for tasks
  CTask::setStandardOutput(&cVideo);
  CTask::setStandardInput(&cDummy);

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CPS2VideoDevice;
#endif // CONFIG_FRAMEBUFFER

#ifdef CONFIG_MULTITASKING
  // Create task structure
  CTask * pTask = new CTask(0, 0, 0);
  pTask->eState_ = TS_RUNNING;
  CTask::addTask(pTask);
#endif // CONFIG_MULTITASKING

  return bricks_main();
}
