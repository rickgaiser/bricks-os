#include "task.h"
#include "kernel/debug.h"


// -----------------------------------------------------------------------------
CNGCThread::CNGCThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CThread(task)
{
}

// -----------------------------------------------------------------------------
CNGCThread::~CNGCThread()
{
}

// -----------------------------------------------------------------------------
CThread *
getNewThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CNGCThread(task, entry, stack, svcstack, argc, argv);
}
