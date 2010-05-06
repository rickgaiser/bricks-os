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
#include "kernel/taskManager.h"
#include "asm/cpu.h"
#include "string.h"


CTask        * CTaskManager::pCurrentTask_ = NULL;
CThread      * CTaskManager::pCurrentThread_ = NULL;
CThread      * CTaskManager::pIdleThread_  = NULL;
STaskQueue     CTaskManager::task_queue    = TAILQ_HEAD_INITIALIZER(CTaskManager::task_queue);
SThreadQueue   CTaskManager::thread_queue  = TAILQ_HEAD_INITIALIZER(CTaskManager::thread_queue);
SThreadQueue   CTaskManager::ready_queue   = TAILQ_HEAD_INITIALIZER(CTaskManager::ready_queue);
SThreadQueue   CTaskManager::timer_queue   = TAILQ_HEAD_INITIALIZER(CTaskManager::timer_queue);
SThreadQueue   CTaskManager::wait_queue    = TAILQ_HEAD_INITIALIZER(CTaskManager::wait_queue);
SThreadQueue   CTaskManager::dead_queue    = TAILQ_HEAD_INITIALIZER(CTaskManager::dead_queue);
uint32_t       CTaskManager::iPIDCount_    = 1;
useconds_t     CTaskManager::iCurrentTime_ = 0;


// -----------------------------------------------------------------------------
void
CTaskManager::updateSleepers(useconds_t us)
{
  CThread * pThread;

  //printk("CTaskManager::updateSleepers\n");

  // FIXME! This only works with 100Hz scheduler (10000us/10ms interval)
  iCurrentTime_ += us;
  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH(pThread, &timer_queue, state_qe)
  {
    // We can leave the loop if a not timed out task is found since the list
    // is sorted.
    if(iCurrentTime_ < pThread->iTimeout_)
      break;

    // Time out! Task is ready to run!
    if(pThread->pWaitObj_ != NULL)
      pThread->iWaitReturn_ = -1;
    else
      pThread->iWaitReturn_ = 0;
    pThread->state(TS_READY);
  }
}

// -----------------------------------------------------------------------------
void
CTaskManager::destroyDeadThreads()
{
  CThread * pThread;
  CThread * pSafe;

  //printk("CTaskManager::destroyDeadThreads\n");

  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH_SAFE(pThread, &dead_queue, state_qe, pSafe)
  {
    pThread->state(TS_DESTROY);
  }
}

// -----------------------------------------------------------------------------
bool
CTaskManager::schedule(bool timeout)
{
  bool bSchedule(false);
  CThread * pPrevThread = pCurrentThread_;

  //printk("CTaskManager::schedule\n");

  // FIXME: We can only use the timeout (100Hz) for timed waits
  if(timeout == true)
  {
    CTaskManager::updateSleepers(10000);
    // Remove destroyed thread
    CTaskManager::destroyDeadThreads();
  }

  // Run the first thread in the ready queue
  if(TAILQ_FIRST(&ready_queue) != NULL)
    TAILQ_FIRST(&ready_queue)->state(TS_RUNNING);

  // No thread to run? Run idle thread!
  if(pCurrentThread_ == NULL)
  {
    if(pIdleThread_ != NULL)
      pCurrentThread_ = pIdleThread_;
    else
      panic("CTaskManager::schedule: ERROR: No task to run\n");
  }

  // Did we get another thread? Update timeout timer
  if(pPrevThread != pCurrentThread_)
  {
    if(pCurrentThread_ == pIdleThread_)
    {
      // Disable the timeout timer
      //timer::disable();
    }
    else
    {
      // Enable and reset timout timer
      //timer::enable(100.0f); // 100Hz
    }

    bSchedule = true;

    // Select thread for execution
    pCurrentThread_->impl().run();
  }

  return bSchedule;
}

// -----------------------------------------------------------------------------
CTask *
CTaskManager::getTaskFromPID(pid_t pid)
{
  CTask * pTask;

  TAILQ_FOREACH(pTask, &task_queue, task_qe)
    if(pTask->iPID_ == pid)
      return pTask;

  return NULL;
}
