#include "task.h"
#include "kernel/debug.h"


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
