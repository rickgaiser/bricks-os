/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2009 Maximus32 <Maximus32@bricks-os.org>
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


typedef void (*fptr)(void);

extern void (*__ctors)(void);
extern void (*__ctors_end)(void);
extern void (*__dtors)(void);
extern void (*__dtors_end)(void);

extern int arch_main(int, char *[]);


// -----------------------------------------------------------------------------
// Execute all constructors
void
__do_global_ctors()
{
  fptr * fpp;

  // FIXME: Should run backward?
  for(fpp = &__ctors; *fpp != __ctors_end; ++fpp)
    (**fpp)();
}

// -----------------------------------------------------------------------------
// Execute all destructors
void
__do_global_dtors()
{
  fptr * fpp;

  // Run forward
  for(fpp = &__dtors; *fpp != __dtors_end; ++fpp)
    (**fpp)();
}

// -----------------------------------------------------------------------------
void
__init()
{
  __do_global_ctors();
}

// -----------------------------------------------------------------------------
void
__fini()
{
  __do_global_dtors();
}

// -----------------------------------------------------------------------------
extern "C"
void
__cxa_pure_virtual()
{
}

// -----------------------------------------------------------------------------
extern "C"
void
exit()
{
}

// -----------------------------------------------------------------------------
int
main(int argc, char * argv[])
{
  int iRetVal;

  __init();

  iRetVal = arch_main(argc, argv);

  __fini();

  return iRetVal;
}
