#include "task.h"
#include "kernel/task.h"
#include "iostream"


extern pt_regs * current_thread;


// -----------------------------------------------------------------------------
CGBANDSTask::CGBANDSTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CTask()
{
  pTaskState_ = new pt_regs;

  if(stack != 0)
    pStack_ = new uint32_t[stack];
  if(svcstack != 0)
    pSvcStack_ = new uint32_t[svcstack];

  pTaskState_->pc     = reinterpret_cast<uint32_t>(entry);
  pTaskState_->sp     = reinterpret_cast<uint32_t>(pStack_) + stack;
  pTaskState_->sp_svc = reinterpret_cast<uint32_t>(pSvcStack_) + svcstack;
  pTaskState_->r0     = argc;
  pTaskState_->r1     = reinterpret_cast<uint32_t>(argv);
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
  if(pStack_ != 0)
    delete pStack_;
  if(pSvcStack_ != 0)
    delete pSvcStack_;
}

// -----------------------------------------------------------------------------
void
CGBANDSTask::run()
{
  // Switch return stack to the stack of this task.
  current_thread = pTaskState_;
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
