#include "task.h"
#include "kernel/debug.h"
#include "asm/cpu.h"
#include "asm/irq.h"


extern pt_regs * current_thread; // Return state for the current thread, only valid in interrupt
extern "C" void kill_task();     // Return function for thread, kills the current thread


// -----------------------------------------------------------------------------
CGBANDSTask::CGBANDSTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CTask()
{
  pTaskState_ = new pt_regs;

  if(stack != 0)
    pStack_ = new uint32_t[stack];
  if(svcstack != 0)
    pSvcStack_ = new uint32_t[svcstack];

  pTaskState_->lr_irq   = reinterpret_cast<uint32_t>(entry);  // BIOS return addr, task entry
  pTaskState_->sp_svc   = reinterpret_cast<uint32_t>(pStack_) + stack;  // Task stack
  pTaskState_->r0       = argc;
  pTaskState_->r1       = reinterpret_cast<uint32_t>(argv);
  pTaskState_->lr_svc   = reinterpret_cast<uint32_t>(kill_task);  // Task return addr
  pTaskState_->spsr_irq = CPU_MODE_SUPERVISOR | CPU_MODE_THUMB;  // Initial task state

  // System/User mode (used by bios syscalls)
  pTaskState_->sp_sys   = reinterpret_cast<uint32_t>(pSvcStack_) + svcstack;
  //pTaskState_->lr_sys   = 0;
  //pTaskState_->sp_sys   = 0;

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
extern "C" void
kill_task()
{
  printk("kill_task\n");
  // Remove the current task from the list
  CTaskManager::pCurrentTask_->state(TS_DESTROY);
  // FIXME: We should reschedule now so the next task can run, but we can't
  //        since we might not be called from an interrupt. So instead we
  //        just wait to rescheduled and destroyed.
  ::halt();
}

// -----------------------------------------------------------------------------
CTask *
getNewTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CGBANDSTask(entry, stack, svcstack, argc, argv);
}
