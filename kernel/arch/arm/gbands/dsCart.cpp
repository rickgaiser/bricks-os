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


#include "dsCart.h"
#include "superCardDriver.h"
#include "kernel/debug.h"


// -----------------------------------------------------------------------------
CDSCart::CDSCart()
{
}

// -----------------------------------------------------------------------------
CDSCart::~CDSCart()
{
  CInterruptManager::detach(13, this);
}

// -----------------------------------------------------------------------------
int
CDSCart::init()
{
  CInterruptManager::attach(13, this);

  CSuperCardDriver * pDriver = new CSuperCardDriver();
  pDriver->init();
  CFileSystem::addBlockDevice(pDriver);

  return 0;
}

// -----------------------------------------------------------------------------
int
CDSCart::isr(int irq)
{
  printk("CDSCart::isr: gba cart ejected\n");

  return 0;
}
