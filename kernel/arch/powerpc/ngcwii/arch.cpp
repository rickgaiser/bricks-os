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

#include "serial.h"
#include "pad.h"

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER


extern char __ArenaLo, __ArenaHi;
#define HEAP_START ((uint32_t)(&__ArenaLo))
#define HEAP_END   ((uint32_t)(&__ArenaHi))


#ifdef CONFIG_DEBUGGING
CNGCDebugScreen cDebug;
#endif // CONFIG_DEBUGGING

CNGCSerial   cSerial;
CNGCPad    * pPAD;

#ifdef CONFIG_FRAMEBUFFER
CNGCVideoDevice * pVideoDevice;
#endif // CONFIG_FRAMEBUFFER


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  //init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

#ifdef CONFIG_DEBUGGING
  cDebug.init();
  pDebug = &cDebug;
#endif // CONFIG_DEBUGGING

  cSerial.init();
  pPAD = new CNGCPad;
  pPAD->init();

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CNGCVideoDevice;
#endif // CONFIG_FRAMEBUFFER

  //printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);

#ifdef NGC
  printk("NGC arch ready\n");
#else
  printk("Wii arch ready\n");
#endif

  return bricks_main();
}
