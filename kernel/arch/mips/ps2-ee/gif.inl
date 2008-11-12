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


#include "gif.h"


//-------------------------------------------------------------------------
void
CGIFPacket::reset()
{
  CSCDMAPacket::reset();

  pGIFTag_ = NULL;
}

//-------------------------------------------------------------------------
void
CGIFPacket::gifAddPackedAD(uint64_t reg, uint64_t data)
{
  this->add64(data);
  this->add64(reg);
}

//-------------------------------------------------------------------------
void
CGIFPacket::pad128()
{
  CSCDMAPacket::pad128(0);
}
