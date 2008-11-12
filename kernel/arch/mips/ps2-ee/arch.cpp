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
#include "kernel/memoryManager.h"
#include "kernel/task.h"

#include "asm/arch/config.h"
#include "asm/cpu.h"
#include "asm/task.h"

#include "irq.h"
#include "dma.h"
#include "sif.h"
#include "cache.h"
#include "drivers.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING


#ifdef CONFIG_BUILTIN_MM
extern char _end;
extern char _heap_size;
#define HEAP_START (KSEG0_START | (uint32_t)(&_end))
#define HEAP_END   (KSEG0_START | (32*1024*1024))
#endif // CONFIG_BUILTIN_MM

CIRQ           cIRQ;  // INT-Controller
CDMAC          cDMAC; // DMA-Controller
CIOP           cIOP;

#ifdef CONFIG_DEBUGGING
CPS2DebugScreen cDebug;
#endif // CONFIG_DEBUGGING


extern "C" void jumpToSegment(unsigned int segment);


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  // Disable interrupts
  local_irq_disable();
  // Enter cpu's kernel mode
  ee_kmode_enter();
  // Disable performance counters
  write_c0_perfctrl0(0);
  // Flush cache
  flushDCacheAll();
  invalidateICacheAll();
  // Jump to KSEG0 (kernel space)
  jumpToSegment(KSEG0_START);
  // Setup interrupts and exceptions
  initExceptions(); // Install CPU exception handlers
  cIRQ.init();      // Interrupt Controller
  cDMAC.init();     // DMA Controller

#ifdef CONFIG_BUILTIN_MM
  init_heap((void *)HEAP_START, HEAP_END - HEAP_START);
#endif // CONFIG_BUILTIN_MM

#ifdef CONFIG_DEBUGGING
  cDebug.init();
  pDebug = &cDebug;
  printk("Debugging ok\n");
#endif // CONFIG_DEBUGGING

  task_init();

  // Initialize IOP
  cIOP.init();
  // Wait for IOP
  printk("Waiting for IOP...");
  while(cIOP.sync());
  printk("ready\n");

#ifdef CONFIG_BUILTIN_MM
  printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);
#endif // CONFIG_BUILTIN_MM

  // Enable interrupts
  local_irq_enable();
  printk("Interrupts...OK\n");

  // Initialize drivers
  init_drivers();

  printk("PS2 arch ready\n");

  return bricks_main();
}
