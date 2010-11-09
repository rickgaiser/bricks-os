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

#include "drivers.h"
#include "timer.h"
#include "irq.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING


#ifdef GBA
extern char __iheap_start, __iwram_top;
extern char __eheap_start, __eheap_end;
#define HEAP_START ((uint32_t)(&__eheap_start))
#define HEAP_END   ((uint32_t)(&__eheap_end))
#elif NDS7
extern char _end, __iwram_top;
#define HEAP_START ((uint32_t)(&_end))
#define HEAP_END   ((uint32_t)(&__iwram_top))
#elif NDS9
extern char _end, __eheap_end;
#define HEAP_START ((uint32_t)(&_end))
#define HEAP_END   ((uint32_t)(&__eheap_end))
#endif


CIRQ           cIRQ;

#ifdef CONFIG_DEBUGGING
#if defined(GBA) || defined(NDS9)
CGBADebugScreen cDebug;
#ifdef NDS9
CGBADebugScreen cDebugARM7;
#endif // NDS9
#endif // defined(GBA) || defined(NDS9)
#endif // CONFIG_DEBUGGING


extern "C"
{
  void __gxx_personality_v0()
  {
  }
  void __aeabi_atexit()
  {
  }
  void __cxa_pure_virtual()
  {
  }
#ifdef NDS
  void initSystem(void)
  {
  }
  void __libnds_exit(int rc)
  {
  }
#endif // NDS
}

// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

#ifdef GBA
  // Change the Gamepak ROM waitstates. Buscycles 8/16/32:
  // from: 5/5/8
  // to:   4/4/6
  REG_WAITCNT = 0x4317;
#endif

  init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

  if(cIRQ.init() == -1)
    iRetVal = -1;

#ifdef CONFIG_DEBUGGING
#if defined(GBA) || defined(NDS9)
  cDebug.init();
  pDebug = &cDebug;
#ifdef NDS9
  cDebugARM7.init(BOTTOM_SCREEN);
#endif // NDS9
#endif // defined(GBA) || defined(NDS9)
#endif // CONFIG_DEBUGGING

  task_init();
  setTimerFrequency(2, 100.0f);
  cIRQ.enable(5);

  // Let CPU flag control interrupt state
  local_irq_disable();  // Disable in cpu interrupt enable flag
  REG_IME = 1;          // Enable REG_IME interrupt enable flag

#ifdef GBA
  printk("iwram: %dKiB free\n", ((uint32_t)(&__iwram_top) - (uint32_t)(&__iheap_start)) / 1024);
  printk("ewram: %dKiB heap\n", ((uint32_t)(&__eheap_end) - (uint32_t)(&__eheap_start)) / 1024);
#else
  printk("heap: %dKiB\n", ((uint32_t)(HEAP_END - HEAP_START)) / 1024);
#endif

  // Enable interrupts
  local_irq_enable();
  printk("Interrupts...OK\n");

  // Initialize drivers
  init_drivers();

#ifdef GBA
  printk("GBA arch ready\n");
#elif NDS7
  printk("NDS-ARM7 arch ready\n");
#elif NDS9
  printk("NDS-ARM9 arch ready\n");
#endif

  return bricks_main();
}
