#include "kernel/bricks.h"
#include "kernel/memoryManager.h"
#include "kernel/task.h"
#include "ps2Video.h"


IFileIO          cDummy;
CPS2Video        cVideo;


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

  // Initialize the memory manager so we can use new/delete/malloc/free
  CMemoryManager::init();

  if(cVideo.init() == -1)
    iRetVal = -1;

  // Set standard in/out for tasks
  CTask::setStandardOutput(&cVideo);
  CTask::setStandardInput(&cDummy);
  // Create task structure
  CTask * pTask = new CTask(0, 0, 0);
  pTask->eState_ = TS_RUNNING;
  CTask::addTask(pTask);

  return bricks_main();
}
