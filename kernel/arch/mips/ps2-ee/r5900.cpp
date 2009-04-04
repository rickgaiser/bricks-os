/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2009 Maximus32 <Maximus32@bricks-os.org>
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
#include "asm/cpu.h"
#include "registers.h"
#include "r5900.h"


#define MIPS_INTERRUPT_BIT_0       (1<<10)
#define MIPS_INTERRUPT_BIT_1       (1<<11)
#define MIPS_INTERRUPT_BIT_2       (1<<15)


IMIPSInterruptHandler * inthandlers[3];


// Entry point for MIPS_EXCEPTION_COMMON handler
extern "C" void commonExceptionHandler();
// Entry point for MIPS_EXCEPTION_INTERRUPT handler
extern "C" void interruptExceptionHandler();


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
install_exception_handler(int type, void (*func)())
{
  vuint32_t * dest;

  if(type < 5)
  {
    dest = (uint32_t *)(KSEG0_START + type * 0x80);
    dest[0] = 0x00000000; // nop, required to fix CPU bug
    dest[1] = 0x00000000; // nop, required to fix CPU bug
    dest[2] = 0x08000000 | (0x03ffffff & (((uint32_t)func) >> 2)); // jump to function
    dest[3] = 0x00000000; // nop, required because of branch delay execution
  }
}

// -----------------------------------------------------------------------------
extern "C" void
errorHandler(uint32_t * regs)
{
  panic("Exception!\n");
}

// -----------------------------------------------------------------------------
extern "C" uint32_t
unknownSyscall(int nr)
{
  panic("Unknown syscall!\n");

  return 0;
}

// -----------------------------------------------------------------------------
// Called by interruptExceptionHandler
extern "C" void
isr(pt_regs * regs)
{
  // Find out who triggered the interrupt
  uint32_t iCouse;
  iCouse = read_c0_cause();

  // Interrupt pending pin 0 (INTC)
  if(iCouse & MIPS_INTERRUPT_BIT_0)
  {
    if(inthandlers[0] != NULL)
      inthandlers[0]->isr(0, regs);
  }

  // Interrupt pending pin 1 (DMAC)
  if(iCouse & MIPS_INTERRUPT_BIT_1)
  {
    if(inthandlers[1] != NULL)
      inthandlers[1]->isr(1, regs);
  }

  // Interrupt pending pin 2 (not used)
  if(iCouse & MIPS_INTERRUPT_BIT_2)
  {
    if(inthandlers[2] != NULL)
      inthandlers[2]->isr(2, regs);
  }
}

// -----------------------------------------------------------------------------
void
initExceptions()
{
  for(int i(0); i < 3; i++)
  {
    inthandlers[i] = NULL;
  }

  // SubBus interface / SIF
  REG_SIF_SMFLAG = (1 << 8) | (1 << 10);

//  install_exception_handler(MIPS_EXCEPTION_TLB_REFILL, commonExceptionHandler);
//  install_exception_handler(MIPS_EXCEPTION_COUNTER,    commonExceptionHandler);
//  install_exception_handler(MIPS_EXCEPTION_DEBUG,      commonExceptionHandler);
//  install_exception_handler(MIPS_EXCEPTION_COMMON,     commonExceptionHandler);
  install_exception_handler(MIPS_EXCEPTION_INTERRUPT,  interruptExceptionHandler);
  flushDCacheAll();
  invalidateICacheAll();
}

// -----------------------------------------------------------------------------
void
setInterruptHandler(uint32_t nr, IMIPSInterruptHandler & handler)
{
  if(nr < 3)
  {
    inthandlers[nr] = &handler;
  }
  else
  {
    panic("setInterruptHandler: invalid nr: %d", nr);
  }
}
