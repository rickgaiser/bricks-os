#include "kernel/task.h"
#include "asm/arch.h"
#include "ps2Video.h"


IFileIO          cDummy;
CPS2Video        cVideo;


// -----------------------------------------------------------------------------
int
arch::init()
{
  int iRetVal(0);

  if(cVideo.init() == -1)
    iRetVal = -1;

  // Set standard in/out for tasks
  CTask::setStandardOutput(&cVideo);
  CTask::setStandardInput(&cDummy);
  // Create task structure
  CTask * pTask = new CTask(0, 0, 0);
  pTask->eState_ = TS_RUNNING;
  CTask::addTask(pTask);

  return iRetVal;
}
