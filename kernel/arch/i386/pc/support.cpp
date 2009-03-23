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
#include "kernel/memoryManager.h"
#include "multiboot.h"


extern int arch_main(unsigned long magic, multiboot_info_t * mbi);
void *__dso_handle;


// -----------------------------------------------------------------------------
extern "C" void
_init()
{
  extern void (*__CTOR_LIST__)();               //the ctor list is defined in the linker script
  void (**constructor)() = &__CTOR_LIST__;      //hold current constructor in list
  int total = *(int *)constructor;              //the first int is the number of constructors

  constructor++;                                //increment to first constructor

  while(total)
  {
    (*constructor)();
    total--;
    constructor++;
  }
}

// -----------------------------------------------------------------------------
extern "C" void
__cxa_atexit()                                  //Walk and call the deconstructors in the dtor_list
{
  extern void (*__DTOR_LIST__)();               //the dtor list is defined in the linker script
  void (**deconstructor)() = &__DTOR_LIST__;    //hold current deconstructor in list
  int total = *(int *)deconstructor;            //the first int is the number of deconstructors

  deconstructor++;                              //increment to first deconstructor

  while(total)
  {
    (*deconstructor)();
    total--;
    deconstructor++;
  }
}

// -----------------------------------------------------------------------------
// C++ support function
extern "C"
void
__cxa_pure_virtual()
{
}

// -----------------------------------------------------------------------------
extern "C"
void
_alloca()
{
}

// -----------------------------------------------------------------------------
extern "C"
int
__main(unsigned long magic, multiboot_info_t * mbi)
{
  int iRetVal;

  _init();

  iRetVal = arch_main(magic, mbi);

  __cxa_atexit();

  return iRetVal;
}

