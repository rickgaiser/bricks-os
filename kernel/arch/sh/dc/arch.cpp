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
#include "asm/arch/config.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER


extern unsigned long end;
#define _HEAP_START ((uint32_t)(&end))
#define HEAP_START ((_HEAP_START+0x3)&(~0x3)) // 4byte aligned
#define HEAP_END   ((uint32_t)(0x8c000000 + (16*1024*1024)))

#ifdef CONFIG_DEBUGGING
CDCDebugScreen cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
CDCVideoDevice * pVideoDevice;
#endif // CONFIG_FRAMEBUFFER


// -----------------------------------------------------------------------------
int
arch_main(int, char *[])
{
#ifdef CONFIG_BUILTIN_MM
  init_heap((void *)HEAP_START, HEAP_END - HEAP_START);
#endif // CONFIG_BUILTIN_MM

#ifdef CONFIG_DEBUGGING
  cDebug.init();
  pDebug = &cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CDCVideoDevice;
#endif // CONFIG_FRAMEBUFFER

#ifdef CONFIG_BUILTIN_MM
  printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);
#endif // CONFIG_BUILTIN_MM

  printk("DC arch ready\n");

  return bricks_main();
}
