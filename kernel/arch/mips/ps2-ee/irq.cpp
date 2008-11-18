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
#include "kernel/interruptManager.h"
#include "asm/arch/registers.h"
#include "asm/arch/memory.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "cache.h"
#include "irq.h"


// Exceptions
#define V_TLB_REFILL      0
#define V_COUNTER         1
#define V_DEBUG           2
#define V_COMMON          3
#define V_INTERRUPT       4

// Interrupts Pins (exception V_INTERRUPT)
#define COUSE_INT_0       (1<<10)
#define COUSE_INT_1       (1<<11)
#define COUSE_INT_2       (1<<15)


const char * sINTSource[] =
{
  "GS",
  "SBUS",
  "VBL_START",
  "VBL_END",
  "VIF0",
  "VIF1",
  "VU0",
  "VU1",
  "IPU",
  "TIMER0",
  "TIMER1"
};

IInterruptHandler * inthandlers[3];

uint32_t syscallTable[130] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*
  // 0
  0, 0, syscallSetCrtc, 0, syscallExit, 0, 0, 0, 0, 0,
  // 10
  0, 0, 0, 0, 0, 0, 0, 0, syscallAddDmacHandler, 0,
  // 20
  0, 0, syscallEnableDmac, 0, 0, 0, 0, 0, 0, 0,
  // 30
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 40
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 50
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 60
  syscallRFU60, syscallRFU61, 0, 0, syscallCreateSema, iDeleteSema, iSignalSema, iSignalSema, WaitSema, 0,
  // 70
  0, 0, 0, iDeleteSema, 0, 0, 0, 0, 0, 0,
  // 80
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 90
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 100
  syscallRFU100, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 110
  0, 0, 0, syscallSetGsIMR, 0, 0, 0, 0, 0, syscallSifSetDma,
  // 120
  syscallSifSetDChain, syscallSifSetReg, syscallSifGetReg, 0, 0, 0, 0, 0, 0, 0,
*/
};

// Entry point for V_COMMON exception handler
extern "C" void commonExceptionHandler();
// Entry point for V_INTERRUPT exception handler
extern "C" void interruptExceptionHandler();

// Function called by interruptExceptionHandler
extern "C" void isr(pt_regs * regs) INTERRUPT_CODE;


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
extern "C" void
isr(pt_regs * regs)
{
  // Find out who triggered the interrupt
  uint32_t iCouse;
  iCouse = read_c0_cause();

  // Interrupt pending pin 0 (INTC)
  if(iCouse & COUSE_INT_0)
  {
    if(inthandlers[0] != NULL)
      inthandlers[0]->isr(0, regs);
  }

  // Interrupt pending pin 1 (DMAC)
  if(iCouse & COUSE_INT_1)
  {
    if(inthandlers[1] != NULL)
      inthandlers[1]->isr(1, regs);
  }

  // Interrupt pending pin 2 (not used)
  if(iCouse & COUSE_INT_2)
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

//  install_exception_handler(V_TLB_REFILL, commonExceptionHandler);
//  install_exception_handler(V_COUNTER,    commonExceptionHandler);
//  install_exception_handler(V_DEBUG,      commonExceptionHandler);
//  install_exception_handler(V_COMMON,     commonExceptionHandler);
  install_exception_handler(V_INTERRUPT,  interruptExceptionHandler);
  flushDCacheAll();
  invalidateICacheAll();
}

// -----------------------------------------------------------------------------
void
setInterruptHandler(uint32_t nr, IInterruptHandler & handler)
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
  iINTMask_ = 0;

  // Clear INT mask/status
  REG_INT_STAT = REG_INT_STAT;
  REG_INT_MASK = REG_INT_MASK;

  // INT controller is connected to MIPS interrupt PIN0
  setInterruptHandler(MIPS_INT_0, *this);

  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::attach(i, this);

  return 0;
}

// -----------------------------------------------------------------------------
void
CIRQ::isr(unsigned int irq, pt_regs * regs)
{
  uint64_t status = REG_INT_STAT;

//  printk("interrupt from INTC(0x%x)\n", status);

  for(int i(0); i < MAX_INTERRUPTS; i++)
  {
    if((iINTMask_ & status) & (1 << i))
    {
//      printk("isr(%d)\n", i);

      // ACK
      REG_INT_STAT = (1 << i);

      // Handle
      CInterruptManager::isr(i, 0);
    }
  }
}

// -----------------------------------------------------------------------------
void
CIRQ::enable(unsigned int irq)
{
//  printk("CIRQ::enable(%d)\n", irq);

  if(irq < MAX_INTERRUPTS)
  {
    if((iINTMask_ & (1 << irq)) == 0)
    {
      iINTMask_    |= (1 << irq);
      REG_INT_MASK |= (1 << irq);
    }
  }
}

// -----------------------------------------------------------------------------
void
CIRQ::disable(unsigned int irq)
{
//  printk("CIRQ::disable(%d)\n", irq);

  if(irq < MAX_INTERRUPTS)
  {
    if((iINTMask_ & (1 << irq)) != 0)
    {
      iINTMask_    &= ~(1 << irq);
      REG_INT_MASK |=  (1 << irq);
    }
  }
}
