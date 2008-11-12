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


#include "drivers.h"
#include "asm/arch/config.h"
#include "kernel/debug.h"

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER


class CDrivers
{
public:
  CDrivers();

public:
#ifdef CONFIG_FRAMEBUFFER
  CPS2VideoDevice cVideoDevice;
#endif // CONFIG_FRAMEBUFFER
};


CDrivers * pDrivers = NULL;


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CDrivers::CDrivers()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
init_drivers()
{
  pDrivers = new CDrivers;
  if(pDrivers == NULL)
    panic("Out of memory!\n");
}
