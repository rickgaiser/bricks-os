#include "kernel/bricks.h"
#include "ps2Video.h"

#ifdef CONFIG_MULTITASKING
#include "kernel/task.h"
#endif // CONFIG_MULTITASKING


IFileIO          cDummy;
CPS2Video        cVideo;


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

#ifdef CONFIG_MULTITASKING
  // Create task structure
  CTask * pTask = new CTask(0, 0, 0);
  pTask->eState_ = TS_RUNNING;
  CTask::addTask(pTask);
#endif // CONFIG_MULTITASKING

  return bricks_main();
}
