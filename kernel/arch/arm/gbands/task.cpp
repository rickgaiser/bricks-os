#include "asm/task.h"
#include "kernel/debug.h"
#include "kernel/task.h"
#include "asm/cpu.h"


extern pt_regs * current_thread;   // Return state for the current thread, only valid in interrupt
extern "C" void kill_thread();     // Return function for thread, kills the current thread
extern "C" void runJump(pt_regs * old_thread, pt_regs * new_thread);
CTask             * pMainTask;
CThread           * pMainThread;
CThreadImpl       * pMainThreadImpl;


// -----------------------------------------------------------------------------
void
task_init()
{
  pMainTask       = new CTask;
  pMainThread     = &pMainTask->thread();
  pMainThreadImpl = &pMainTask->thread().impl();

  current_thread = &(pMainThreadImpl->threadState_);

  // Add task to taskmanagers list
  pMainThread->state(TS_RUNNING);
}

// -----------------------------------------------------------------------------
CThreadImpl::CThreadImpl()
 : pStack_(NULL)
 , pSvcStack_(NULL)
{
}

// -----------------------------------------------------------------------------
CThreadImpl::~CThreadImpl()
{
  if(pStack_ != 0)
    delete pStack_;
  if(pSvcStack_ != 0)
    delete pSvcStack_;
}

// -----------------------------------------------------------------------------
void
CThreadImpl::init(void * entry, int argc, char * argv[])
{
  size_t stack(512);
  size_t svcstack(512);

  // Allocate stacks
  pStack_ = new uint32_t[stack];
  pSvcStack_ = new uint32_t[svcstack];

  threadState_.pc     = reinterpret_cast<uint32_t>(entry) + 8;
  threadState_.sp     = reinterpret_cast<uint32_t>(pStack_) + stack;
  threadState_.sp_svc = reinterpret_cast<uint32_t>(pSvcStack_) + svcstack;
  threadState_.r0     = argc;
  threadState_.r1     = reinterpret_cast<uint32_t>(argv);
  threadState_.lr     = reinterpret_cast<uint32_t>(kill);
  threadState_.cpsr   = CPU_MODE_SYSTEM | CPU_MODE_THUMB;

  if(current_thread == 0)
    current_thread = &threadState_;
}

// -----------------------------------------------------------------------------
void
CThreadImpl::runJump()
{
  // Store state in "current_thread" and jump to "pThreadState_"
  ::runJump(current_thread, &threadState_);
}

// -----------------------------------------------------------------------------
void
CThreadImpl::runReturn()
{
  // Switch return stack to the stack of this task.
  current_thread = &threadState_;
}

// -----------------------------------------------------------------------------
extern "C" void
kill_thread()
{
  printk("kill_thread\n");
  // Remove the current task from the list
  CTaskManager::pCurrentThread_->state(TS_DEAD);
  // FIXME: We should reschedule now so the next task can run, but we can't
  //        since we might not be called from an interrupt. So instead we
  //        just wait to rescheduled and destroyed.
  ::halt();
}
