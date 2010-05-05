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


#ifndef TASKMANAGER_H
#define TASKMANAGER_H


#include "asm/arch/config.h"
#include "kernel/task.h"
#include "inttypes.h"


// -----------------------------------------------------------------------------
class CTaskManager
{
public:
  static void updateSleepers(useconds_t us);
  static void destroyDeadThreads();
  static bool schedule(bool timeout = false);
  static CTask * getTaskFromPID(pid_t pid);

  static CTask * pCurrentTask_;
  static CThread * pCurrentThread_;
  static CThread * pIdleThread_;
  static STaskQueue task_queue;        // All tasks
  static SThreadQueue thread_queue;    // All threads
  static SThreadQueue ready_queue;     // Ready to run threads
  static SThreadQueue timer_queue;     // Sleeping on timer threads
  static SThreadQueue wait_queue;      // Sleeping on objects
  static SThreadQueue dead_queue;      // Ready to be destroyed
  static uint32_t iPIDCount_;
  static useconds_t iCurrentTime_;

private:
  CTaskManager(){}
};


#endif
