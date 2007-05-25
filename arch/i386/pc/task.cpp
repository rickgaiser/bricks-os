#include "task.h"
#include "kernel/task.h"
#include "iostream"


// -----------------------------------------------------------------------------
CPCTask::CPCTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CTask(entry, stack, svcstack, argc, argv)
{
}

// -----------------------------------------------------------------------------
CPCTask::~CPCTask()
{
}

// -----------------------------------------------------------------------------
void
CPCTask::kill()
{
  std::cout<<"suicide!"<<std::endl;
  CTaskManager::removeTask(CTaskManager::pCurrentTask_);
}

// -----------------------------------------------------------------------------
CTask *
getNewTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CPCTask(entry, stack, svcstack, argc, argv);
}
