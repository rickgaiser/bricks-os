#include "task.h"
#include "kernel/debug.h"


CTask      * pMainTask;
CPS2Thread * pMainThread;


// -----------------------------------------------------------------------------
void
CPS2Thread::init()
{
  pMainTask   = new CTask(0, 0, 0);
  pMainThread = (CPS2Thread *)pMainTask->thr_;

  // Add task to taskmanagers list
  pMainThread->state(TS_RUNNING);
}

// -----------------------------------------------------------------------------
CPS2Thread::CPS2Thread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CThread(task)
{
}

// -----------------------------------------------------------------------------
CPS2Thread::~CPS2Thread()
{
}

// -----------------------------------------------------------------------------
CThread *
getNewThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CPS2Thread(task, entry, stack, svcstack, argc, argv);
}
