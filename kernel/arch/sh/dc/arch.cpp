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


// -----------------------------------------------------------------------------
// C++ support function
extern "C"
void
__cxa_pure_virtual()
{
}

// -----------------------------------------------------------------------------
// C++ support function
extern "C"
void
exit()
{
}

// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  //init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

  //printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);

  printk("DC arch ready\n");

  return bricks_main();
}
