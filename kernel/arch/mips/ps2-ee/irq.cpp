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
#include "irq.h"

#ifdef CONFIG_KERNEL_MODE
#include "registers.h"
#else
#include "bios.h"
#endif

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

#ifdef CONFIG_KERNEL_MODE
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
#endif // CONFIG_KERNEL_MODE


#ifndef CONFIG_KERNEL_MODE
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int32_t
biosINTHandler(int32_t irq)
{
  if(irq < MAX_INTERRUPTS)
  {
    // Handle
    CInterruptManager::isr(irq, 0);
  }

  return 0;
}
#endif // CONFIG_KERNEL_MODE

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
#ifdef CONFIG_KERNEL_MODE
  iINTMask_ = 0;

  // Clear INT mask/status
  REG_INT_STAT = REG_INT_STAT;
  REG_INT_MASK = REG_INT_MASK;

  // INT controller is connected to MIPS interrupt PIN0
  setInterruptHandler(MIPS_INTERRUPT_0, *this);
#else
  for(int i(0); i < MAX_INTERRUPTS; i++)
    handle_[i] = 0;
#endif // CONFIG_KERNEL_MODE

  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::attach(i, this);

  return 0;
}

#ifdef CONFIG_KERNEL_MODE
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
#endif // CONFIG_KERNEL_MODE

// -----------------------------------------------------------------------------
void
CIRQ::enable(unsigned int irq)
{
//  printk("CIRQ::enable(%d)\n", irq);

  if(irq < MAX_INTERRUPTS)
  {
#ifdef CONFIG_KERNEL_MODE
    if((iINTMask_ & (1 << irq)) == 0)
    {
      iINTMask_    |= (1 << irq);
      REG_INT_MASK |= (1 << irq);
    }
#else
    if(handle_[irq] == 0)
    {
      handle_[irq] = bios::AddIntcHandler(irq, ::biosINTHandler, 0);
      bios::EnableIntc(irq);
    }
#endif // CONFIG_KERNEL_MODE
  }
}

// -----------------------------------------------------------------------------
void
CIRQ::disable(unsigned int irq)
{
//  printk("CIRQ::disable(%d)\n", irq);

  if(irq < MAX_INTERRUPTS)
  {
#ifdef CONFIG_KERNEL_MODE
    if((iINTMask_ & (1 << irq)) != 0)
    {
      iINTMask_    &= ~(1 << irq);
      REG_INT_MASK |=  (1 << irq);
    }
#else
    if(handle_[irq] != 0)
    {
      bios::DisableIntc(irq);
      bios::RemoveIntcHandler(irq, handle_[irq]);
      handle_[irq] = 0;
    }
#endif // CONFIG_KERNEL_MODE
  }
}
