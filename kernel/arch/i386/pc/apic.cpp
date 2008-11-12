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


#include "apic.h"
#include "cpuid.h"
#include "asm/arch/config.h"
#include "asm/hal.h"
#include "asm/task.h"
#include "kernel/task.h"
#include "kernel/debug.h"


uint32_t * pApicID = (uint32_t *)(0xfee00020);
uint32_t * pApicVersion = (uint32_t *)(0xfee00030);

void
init_apic()
{
  if(CPU::hasAPIC())
  {
#ifdef CONFIG_MMU
    pMainTask->aspace().mapIdentity((void *)(0xfee00000), 4 * 1024);  // Identity Map APIC
#endif

    printk("APIC:\n");
    printk(" - ID:      %d\n", *pApicID >> 24);
    printk(" - Version: %d\n", *pApicVersion & 0xff);
  }
}
