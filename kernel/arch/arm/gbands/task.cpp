#include "task.h"
#include "kernel/debug.h"
#include "asm/cpu.h"


extern pt_regs * current_thread;   // Return state for the current thread, only valid in interrupt
extern "C" void kill_thread();     // Return function for thread, kills the current thread
extern "C" void runJump(pt_regs * old_thread, pt_regs * new_thread);
CTask         * pMainTask;
CGBANDSThread * pMainThread;


// -----------------------------------------------------------------------------
void
CGBANDSThread::init()
{
  pMainTask   = new CTask(0, 0, 0);
  pMainThread = (CGBANDSThread *)pMainTask->thr_;

  current_thread = &(pMainThread->threadState_);

  // Add task to taskmanagers list
  pMainThread->state(TS_RUNNING);
}

// -----------------------------------------------------------------------------
CGBANDSThread::CGBANDSThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : CThread(task)
 , pStack_(NULL)
 , pSvcStack_(NULL)
{
  // Main thread?
  if(entry != NULL)
  {
    // Set default stack values
    if(stack == 0)    stack    = 512;
    if(svcstack == 0) svcstack = 512;

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
}

// -----------------------------------------------------------------------------
CGBANDSThread::~CGBANDSThread()
{
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
  ::runJump(current_thread, &threadState_);
}

// -----------------------------------------------------------------------------
void
CGBANDSThread::runReturn()
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

// -----------------------------------------------------------------------------
CThread *
getNewThread(CTask * task, void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
{
  return new CGBANDSThread(task, entry, stack, svcstack, argc, argv);
}
