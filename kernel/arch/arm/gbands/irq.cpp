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


#include "irq.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "kernel/task.h"
#include "kernel/taskManager.h"
#include "kernel/srr_k.h"
#include "asm/irq.h"
#include "asm/arch/memory.h"
#include "asm/arch/config.h"


// Assembler isr function calling our "C" isr function
//extern "C" void __isr();
extern "C" void __gba_isr();
// Function __gba_isr calls
extern "C" void isr(pt_regs * regs) INTERRUPT_CODE;


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern "C" void
isr(pt_regs * regs)
{
  bool bTimeout(false);
  // Find out who triggered the interrupt
  unsigned long iFlags(REG_IF & REG_IE);

  //printk("isr(0x%x)\n", iFlags);
  //printk(".");

  // Handle timer interrupt for scheduler separately (faster)
  if(iFlags & (1 << 5))
  {
    // Remove flag
    iFlags &= ~(1 << 5);
    // Acknowledge interrupt
    REG_IF |= (1 << 5);

    bTimeout = true;
  }

  // Handle other interrupts
  if(iFlags != 0)
  {
    for(int i(0); i < MAX_INTERRUPTS; i++)
    {
      if(iFlags & (1 << i))
      {
        // ACK
        REG_IF |= (1 << i);

        // Handle
        CInterruptManager::isr(i, regs);
      }
    }
  }

  // Run the scheduler
  if(CTaskManager::schedule(bTimeout) == true)
  {
    // Load return stack
    CTaskManager::pCurrentThread_->impl().runReturn();
    // Jump to task
    //CTaskManager::pCurrentThread_->impl().runJump();
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CIRQ::CIRQ()
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CIRQ::~CIRQ()
{
  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::detach(i, this);
}

// -----------------------------------------------------------------------------
int
CIRQ::init()
{
  // Initialize function pointers
  REG_INTMAIN    = &::__gba_isr;
#ifdef GBA
#ifndef CONFIG_DIRECT_ACCESS_KERNEL
  REG_MSGSEND    = &::k_msgSend;
  REG_MSGRECEIVE = &::k_msgReceive;
  REG_MSGREPLY   = &::k_msgReply;
#endif // CONFIG_DIRECT_ACCESS_KERNEL
#endif // GBA

  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::attach(i, this);

  return 0;
}
