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


#include "kernel/debug.h"
#include "kernel/genwait.h"
#include "kernel/task.h"


// -----------------------------------------------------------------------------
int
genwait_wait(void * obj, useconds_t useconds)
{
  //printk("genwait_wait %dus\n", useconds);

  // Set timeout if present
  if(useconds > 0)
    CTaskManager::pCurrentThread_->iTimeout_ = CTaskManager::iCurrentTime_ + useconds;
  else
    CTaskManager::pCurrentThread_->iTimeout_ = 0;

  // Set wait object
  CTaskManager::pCurrentThread_->pWaitObj_ = obj;

  // change thread state
  CTaskManager::pCurrentThread_->state(TS_WAITING);

  // Schedule next thread
  CTaskManager::schedule();

  // Run next thread
  CTaskManager::pCurrentThread_->impl().runJump();

  return CTaskManager::pCurrentThread_->iWaitReturn_;
}

// -----------------------------------------------------------------------------
int
genwait_wake(void * obj, int maxcount)
{
  int count(0);
  CThread * pThread;
  CThread * pSafe;

  //printk("genwait_wake %d\n", count);

  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH_SAFE(pThread, &CTaskManager::wait_queue, wait_qe, pSafe)
  {
    if(pThread->pWaitObj_ == obj)
    {
      pThread->iWaitReturn_ = 0;
      pThread->state(TS_READY);
      if(maxcount > 0)
      {
        count++;
        if(count >= maxcount)
          break;
      }
    }
  }
  return count;
}
