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


#include "debugScreen.h"


// -----------------------------------------------------------------------------
CI386DebugScreen::CI386DebugScreen()
 : CAVideo(CHARS_PER_LINE, LINE_COUNT)
 , pVideo_(VIDEO_START)
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CI386DebugScreen::~CI386DebugScreen()
{
}

// -----------------------------------------------------------------------------
int
CI386DebugScreen::init()
{
  this->cls();

  return 0;
}

// -----------------------------------------------------------------------------
void
CI386DebugScreen::put(int iX, int iY, char c)
{
  pVideo_[(iY * CHARS_PER_LINE + iX) * 2] = c;
}

// -----------------------------------------------------------------------------
char
CI386DebugScreen::get(int iX, int iY)
{
  return pVideo_[(iY * CHARS_PER_LINE + iX) * 2];
}
