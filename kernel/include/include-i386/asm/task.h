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


#ifndef PC_TASK_H
#define PC_TASK_H


#include "asm/arch/config.h"
#include "asm/irq.h"
#include "asm/hal.h"
#include "asm/vmm.h"


// -----------------------------------------------------------------------------
void task_init();

// -----------------------------------------------------------------------------
class CThreadImpl
{
public:
  CThreadImpl(CAddressSpace * pASpace);
  ~CThreadImpl();

  void init(void * entry, int argc = 0, char * argv[] = 0);

  // Selects this thread for execution on interrupt return
  void run();

  // Jump to task
  void jump();
  // Call task (use iret to return to caller)
  void call();

public:
  // Task state
  STaskStateSegment * pTSS_;
  uint32_t iTSSSize_;
  selector_t selTSS_;

  CAddressSpace * pASpace_;

  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};

// -----------------------------------------------------------------------------
class CV86Thread
{
public:
  CV86Thread(CAddressSpace * pASpace);
  ~CV86Thread();

  void init();

  // Selects this thread for execution on interrupt return
  void run();

  // Jump to task
  void jump();
  // Call task (use iret to return to caller)
  void call();

  // Setup for v86 interrupt and jump to the v86 task
  void interrupt(uint8_t nr);

public:
  // Task state
  STaskStateSegment * pTSS_;
  uint32_t iTSSSize_;
  selector_t selTSS_;

  CAddressSpace * pASpace_;

  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};


class CTask;
class CThread;
extern CTask       * pMainTask;
extern CThread     * pMainThread;
extern CThreadImpl * pMainThreadImpl;


#endif
