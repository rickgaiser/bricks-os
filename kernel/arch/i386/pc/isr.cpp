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


#include "i8259.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "kernel/srr_k.h"
#include "kernel/task.h"
#include "kernel/taskManager.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "asm/hal.h"
#include "asm/vmm.h"


extern CIRQ c8259;


#define EC_FLAGS_P    0x00000001
#define EC_FLAGS_WR   0x00000002
#define EC_FLAGS_US   0x00000004
// -----------------------------------------------------------------------------
// Page Fault Handler
//  - Not implemented: Automatic stack growing
//  - Not implemented: Stack guard page to detect stack overflow
//  - Not implemented: Copy on Write (for sharing memory)
//  - Not implemented: Backingstore (restore from disk or ???)
void
PageFault(pt_regs * regs)
{
  bool bKillTask = false;
  uint32_t cr2 = getCR2();
  
/*
  if((cr2 >= MM_KERNEL_BASE) && (regs->eip >= MM_KERNEL_BASE))
  {
    // Kernel page fault. This could be a:
    //  - Stack in need for some growth
    //  - Fatal error
  }
  else
  {
    // User page fault
  }
*/
  printk("Page Fault:\n");
  printk(" - Trying to ");
  if(regs->iErrorCode & EC_FLAGS_WR)
    printk("write to");
  else
    printk("read from");
  printk(" 0x%x (from 0x%x)\n", cr2, regs->eip);

  if(regs->iErrorCode & EC_FLAGS_P)
    printk(" - Not allowed\n");
  else
    printk(" - Page not present\n");

  CArea * pArea = CTaskManager::pCurrentTask_->aspace().findArea(cr2);
  if(pArea != NULL)
  {
    printk("Mapping found for location 0x%x\n", cr2);
    
    // Check read/write permissions
    if(regs->iErrorCode & EC_FLAGS_WR)
    {
      if(pArea->writable() == false)
      {
        printk("Security Error: Writing is NOT allowed at addr 0x%x!\n", cr2);
        bKillTask = true;
      }
    }
    else
    {
      if(pArea->readable() == false)
      {
        printk("Security Error: Reading is NOT allowed at addr 0x%x!\n", cr2);
        bKillTask = true;
      }
    }

    if(bKillTask == false)
    {
      if(regs->iErrorCode & EC_FLAGS_P)
      {
        // Page present, but protected
        panic("Page should be unprotected!");
      }
      else
      {
        // Page not present, make page present
        //panic("Page should be made present!");
        pArea->map(cr2);
        //panic("Page should be present?");
      }
    }
  }
  else
  {
    printk("Security Error: No mapping found at addr 0x%x\n", cr2);
    bKillTask = true;
  }

  if(bKillTask == true)
  {
    panic("Task should be killed!");
  }
}

// -----------------------------------------------------------------------------
void
deadThreadPanic()
{
  printk("running a dead thread\n");

  // Busy waiting for the scheduler to delete this thread
  ::local_irq_enable();
  while(true)
  {
    ::halt();
  }
}

// -----------------------------------------------------------------------------
extern "C" void
isr(pt_regs * regs)
{
  bool bTimeout = false;

  //printk("isr 0x%x\n", regs->iIntNumber);

  static const char * msg[] =
  {
    // (0x00 - 0x1f) IA-32 Reserved Interrupts
    "Divide Error",
    "RESERVED",
    "NMI",
    "Breakpoint",              // INT3 instruction
    "Overflow",                // INTO instruction
    "BOUND Range Exceeded",    // BOUND instruction
    "Invalid Opcode",
    "Device Not Available",    // No coprocessor
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection",
    "Page Fault",
    "RESERVED",
    "x87 FPU Floating-Point Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    // (0x20 - 0x2f, IRQ0 - IRQ15) Intel 8259 IRQ interrupts
    "Timer",                   // Intel 8253/8254
    "Keyboard",                // Intel 8042
    "Cascade",
    "COM 2/4",                 // 8250
    "COM 1/3",                 // 8250
    "LPT2",                    // Intel 8255
    "Floppy",                  // Intel 82072A
    "LPT1",                    // Intel 8255
    "RTC",
    "?",
    "?",
    "?",
    "Mouse",                   // Intel 8042
    "Math Coprocessor",
    "Pimary IDE",
    "Secondary IDE",
    // (0x30 - 0x30) Soft interrupts
    "Syscall"
  };
  
  switch(regs->iIntNumber)
  {
    // Handle CPU interrupts
    case 0x07: // Device Not Available
/*
    {
      // FIXME: Save floating point registers
      // ...

      // Clear the TS (task switch) flag if present
      uint32_t cr0 = getCR0();
      if(cr0 & CR0_TS)
        setCR0(cr0 & (~CR0_TS));
      else
        panic("Device Not Available\n");

      break;
    }
*/
    case 0x0e:
      PageFault(regs);
      break;
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d: // GPF, handled by TSS task
    case 0x0f:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
      panic("CPU Interrupt(0x%x): %s, addr: 0x%x, error: 0x%x\n", regs->iIntNumber, msg[regs->iIntNumber], regs->eip, regs->iErrorCode);
      break;

    // Handle IRQs
    case 0x20:  // Timer
      // Timer interrupt handled at end of this function
      //c8259.ack(regs->iIntNumber);
      //break;
      bTimeout = true;
    case 0x21: // Keyboard
    case 0x22: // Cascade
    case 0x23: // COM 2/4
    case 0x24: // COM 1/3
    case 0x25: // LPT2
    case 0x26: // Floppy
    case 0x27: // LPT1
    case 0x28: // RTC
    case 0x29: // Free
    case 0x2a: // Free
    case 0x2b: // Free
    case 0x2c: // Mouse
    case 0x2d: // Math Coprocessor
    case 0x2e: // Pimary IDE
    case 0x2f: // Secondary IDE
      //printk("IRQ Interrupt(0x%x): %s\n", regs->iIntNumber, msg[regs->iIntNumber]);
      c8259.ack(regs->iIntNumber);
      CInterruptManager::isr(regs->iIntNumber, regs);
      break;

    // Bricks System Calls
    case 0x30:
    {
      switch(regs->eax)
      {
        case 1:
          regs->eax = k_msgSend((int)regs->ebx, (const void *)regs->ecx, (int)regs->edx, (void *)regs->esi, (int)regs->edi);
          break;
        case 2:
          regs->eax = k_msgReceive((int)regs->ebx, (void *)regs->ecx, (int)regs->edx);
          break;
        case 3:
          regs->eax = k_msgReply((int)regs->ebx, (int)regs->ecx, (const void *)regs->edx, (int)regs->esi);
          break;
        default:
          panic("ERROR: Unknown int 0x30 System Call(eax=0x%x)\n", regs->eax);
      };
      break;
    }

    // Linux System Calls
    case 0x80:
    {
      switch(regs->eax)
      {
        case 1:
          // exit
          //  - Destroy task
          CTaskManager::pCurrentThread_->state(TS_DEAD);
          //  - Schedule another task/thread
          CTaskManager::schedule();
          //  - We will never get here
          deadThreadPanic();
          break;
        case 4:
          // write
          printk((char *)regs->ecx);
          break;
        default:
          panic("Unknown int 0x80 System Call(eax=0x%x)", regs->eax);
      };
      break;
    }

    // Handle unknown interrupts
    default:
      panic("Unknown Interrupt(0x%x)", regs->iIntNumber);
  };

  // Run the scheduler
  CTaskManager::schedule(bTimeout);
}
