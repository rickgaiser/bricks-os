#include "task.h"
#include "kernel/task.h"
#include "iostream"


extern pt_regs * current_thread;


// -----------------------------------------------------------------------------
CGBANDSTask::CGBANDSTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CTask(entry, stack, svcstack, argc, argv)
{
  pTaskState_ = new pt_regs;

  pTaskState_->pc     = reinterpret_cast<uint32_t>(entry);
  pTaskState_->sp     = reinterpret_cast<uint32_t>(pStack_ + stack);
  pTaskState_->sp_svc = reinterpret_cast<uint32_t>(pSvcStack_ + svcstack);
  pTaskState_->r0     = argc;
  pTaskState_->r1     = (uint32_t)argv;
  pTaskState_->lr     = reinterpret_cast<uint32_t>(kill);
  pTaskState_->cpsr   = CPU_MODE_SYSTEM | CPU_MODE_THUMB;

  if(current_thread == 0)
    current_thread = pTaskState_;
}

// -----------------------------------------------------------------------------
CGBANDSTask::~CGBANDSTask()
{
  if(pTaskState_ != 0)
    delete pTaskState_;
}

// -----------------------------------------------------------------------------
void
CGBANDSTask::kill()
{
  std::cout<<"suicide!"<<std::endl;
  CTaskManager::removeTask(CTaskManager::pCurrentTask_);
}

// -----------------------------------------------------------------------------
CTask *
getNewTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CGBANDSTask(entry, stack, svcstack, argc, argv);
}
