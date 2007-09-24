#include "task.h"
#include "kernel/debug.h"
#include "asm/cpu.h"
#include "asm/irq.h"


extern pt_regs * current_thread;   // Return state for the current thread, only valid in interrupt
extern "C" void kill_thread();     // Return function for thread, kills the current thread
extern "C" void runJump(pt_regs * old_thread, pt_regs * new_thread);


// -----------------------------------------------------------------------------
CGBANDSThread::CGBANDSThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CThread(task)
{
  pThreadState_ = new pt_regs;

  if(stack != 0)
    pStack_ = new uint32_t[stack];
  if(svcstack != 0)
    pSvcStack_ = new uint32_t[svcstack];

  pThreadState_->pc     = reinterpret_cast<uint32_t>(entry) + 8;
  pThreadState_->sp     = reinterpret_cast<uint32_t>(pStack_) + stack;
  pThreadState_->sp_svc = reinterpret_cast<uint32_t>(pSvcStack_) + svcstack;
  pThreadState_->r0     = argc;
  pThreadState_->r1     = reinterpret_cast<uint32_t>(argv);
  pThreadState_->lr     = reinterpret_cast<uint32_t>(kill);
  pThreadState_->cpsr   = CPU_MODE_SYSTEM | CPU_MODE_THUMB;

  if(current_thread == 0)
    current_thread = pThreadState_;
}

// -----------------------------------------------------------------------------
CGBANDSThread::~CGBANDSThread()
{
  if(pThreadState_ != 0)
    delete pThreadState_;
  if(pStack_ != 0)
    delete pStack_;
  if(pSvcStack_ != 0)
    delete pSvcStack_;
}

// -----------------------------------------------------------------------------
void
CGBANDSThread::runJump()
{
  // Store state in "current_thread" and jump to "pThreadState_"
  ::runJump(current_thread, pThreadState_);
}

// -----------------------------------------------------------------------------
void
CGBANDSThread::runReturn()
{
  // Switch return stack to the stack of this task.
  current_thread = pThreadState_;
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

// -----------------------------------------------------------------------------
CThread *
getNewThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CGBANDSThread(task, entry, stack, svcstack, argc, argv);
}
