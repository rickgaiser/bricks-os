#include "task.h"
#include "kernel/debug.h"


// -----------------------------------------------------------------------------
CDCThread::CDCThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CThread(task)
{
}

// -----------------------------------------------------------------------------
CDCThread::~CDCThread()
{
}

// -----------------------------------------------------------------------------
CThread *
getNewThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CDCThread(task, entry, stack, svcstack, argc, argv);
}
