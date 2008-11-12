/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "asm/task.h"
#include "kernel/debug.h"
#include "kernel/task.h"
#include "asm/cpu.h"


extern pt_regs * current_thread;   // Return state for the current thread, only valid in interrupt
extern "C" void kill();
extern "C" void kill_thread();     // Return function for thread, kills the current thread
extern "C" void runJump(pt_regs * current_thread, pt_regs * new_thread);
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

  current_thread  = &(pMainThreadImpl->threadState_);

  // Add task to taskmanagers list
  pMainThread->state(TS_RUNNING);
}

// -----------------------------------------------------------------------------
CThreadImpl::CThreadImpl()
 : pStack_(NULL)
{
}

// -----------------------------------------------------------------------------
CThreadImpl::~CThreadImpl()
{
  if(pStack_ != 0)
    delete pStack_;
}

// -----------------------------------------------------------------------------
void
CThreadImpl::init(void * entry, int argc, char * argv[])
{
  size_t stackSize(4 * 1024);

  // Allocate stacks
  pStack_ = new uint32_t[stackSize/4];

  // Setup initial task state
  threadState_.r0        = argc;
  threadState_.r1        = reinterpret_cast<uint32_t>(argv);
  threadState_.r2        = 0;
  threadState_.r3        = 0;
  threadState_.r4        = 0;
  threadState_.r5        = 0;
  threadState_.r6        = 0;
  threadState_.r7        = 0;
  threadState_.r8        = 0;
  threadState_.r9        = 0;
  threadState_.r10       = 0;
  threadState_.r11       = 0;
  threadState_.r12       = 0;
  threadState_.sp        = reinterpret_cast<uint32_t>(pStack_) + stackSize;
  threadState_.lr        = reinterpret_cast<uint32_t>(kill);
  threadState_.pc        = reinterpret_cast<uint32_t>(entry) + 8;
  threadState_.cpsr      = CPU_MODE_SYSTEM | CPU_MODE_THUMB; // Mode on irq return
}

// -----------------------------------------------------------------------------
void
CThreadImpl::runJump()
{
  // Jump to "threadState_" (current state saved in "current_thread")
  ::runJump(current_thread, &threadState_);

  // "current_thread" will resume here, after it is has been scheduled again
}

// -----------------------------------------------------------------------------
void
CThreadImpl::runReturn()
{
  // Set this threads state as the new "current_thread"
  // An interrupt return will activate this task
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
