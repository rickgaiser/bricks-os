#include "asm/task.h"
#include "kernel/debug.h"


// -----------------------------------------------------------------------------
CPSPThread::CPSPThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CThread(task)
{
}

// -----------------------------------------------------------------------------
CPSPThread::~CPSPThread()
{
}

// -----------------------------------------------------------------------------
CThread *
getNewThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CPSPThread(task, entry, stack, svcstack, argc, argv);
}
