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


#include "icxxabi.h"
#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "multiboot.h"


extern char start_ctors, end_ctors, start_dtors, end_dtors;
extern void arch_main(unsigned long magic, multiboot_info_t * mbi);


// -----------------------------------------------------------------------------
// Called when a (C++) pure virtual function is called
extern "C"
void
__cxa_pure_virtual()
{
  panic("pure virtual function called");
}

// -----------------------------------------------------------------------------
extern "C"
void
__main(unsigned long magic, multiboot_info_t * mbi)
{
  // Static constructors
  for(unsigned long * call = (unsigned long *)(&start_ctors); call < (unsigned long *)(&end_ctors); call++)
  {
    ((void (*)(void))*call)();
  }

  arch_main(magic, mbi);

  // We should never get here
  panic("ERROR: Trying to leave kernel");
  
  // Static destructors
  for(unsigned long * call = (unsigned long *)(&start_dtors); call < (unsigned long *)(&end_dtors); call++)
  {
    ((void (*)(void))*call)();
  }
}

