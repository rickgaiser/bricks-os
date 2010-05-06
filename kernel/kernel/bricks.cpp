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


#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/pthread_k.h"
#include "kernel/task.h"
#include "kernel/taskManager.h"
#include "asm/arch/config.h"
#include "asm/cpu.h"


extern "C" int appMain(int argc, char * argv[]);


#ifdef CONFIG_MULTI_THREADING
// -----------------------------------------------------------------------------
// Main thread
void *
mainThread(void *)
{
  int iRetVal;

  // Execute main application
  iRetVal = appMain(0, 0);

  // Return
  printk("Main thread done\n");
  k_pthread_exit((void *)iRetVal);

  return (void *)iRetVal;
}
#endif

// -----------------------------------------------------------------------------
// Idle thread (never returns)
void *
idleThread(void *)
{
  while(true)
  {
    ::halt();
  }

  return NULL;
}

// -----------------------------------------------------------------------------
int
bricks_main()
{
  // Welcome user :-)
  printk("Bricks-OS\n");
  printk("=========\n");

#ifdef CONFIG_MULTI_THREADING
  // Create main thread
  pthread_t thr;
  if(k_pthread_create(&thr, 0, mainThread, (void *)NULL) != 0)
    panic("Unable to create main thread\n");
#else
  // We are the main thread
  appMain(0, 0);
#endif

  // We become the idle thread
  CTaskManager::pIdleThread_ = CTaskManager::pCurrentThread_;
  CTaskManager::pIdleThread_->state(TS_READY);
  idleThread(NULL);

  return 0;
}
