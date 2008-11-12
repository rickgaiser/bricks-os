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
#include "asm/aspace.h"


// -----------------------------------------------------------------------------
void task_init();

// -----------------------------------------------------------------------------
class CThreadImpl
{
public:
#ifndef CONFIG_MMU
  CThreadImpl();
#else
  CThreadImpl(CAddressSpace * pASpace);
#endif
  ~CThreadImpl();

  void init(void * entry, int argc = 0, char * argv[] = 0);

  // Task switch #1: Jump to task immediately.
  //  - Used from caller context
  void runJump();
  void runCall();
  // Task switch #2: Setup stack so interrupt return will couse this task to run.
  //  - Used from interrupt context
  //virtual void runReturn();

public:
  // Task state
  STaskStateSegment * pTSS_;
  uint32_t iTSSSize_;
  selector_t selTSS_;

#ifdef CONFIG_MMU
  CAddressSpace * pASpace_;
#endif

  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};

// -----------------------------------------------------------------------------
class CV86Thread
{
public:
#ifndef CONFIG_MMU
  CV86Thread();
#else
  CV86Thread(CAddressSpace * pASpace);
#endif
  ~CV86Thread();

  void init();

  // Task switch #1: Jump to task immediately.
  //  - Used from caller context
  void runJump();
  void runCall();

  // Setup for v86 interrupt and runJump to the v86 task
  void interrupt(uint8_t nr);

public:
  // Task state
  STaskStateSegment * pTSS_;
  uint32_t iTSSSize_;
  selector_t selTSS_;

#ifdef CONFIG_MMU
  CAddressSpace * pASpace_;
#endif

  uint32_t * pStack_;
  uint32_t * pSvcStack_;
};


class CTask;
class CThread;
extern CTask       * pMainTask;
extern CThread     * pMainThread;
extern CThreadImpl * pMainThreadImpl;


#endif
