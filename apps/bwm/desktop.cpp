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


#include "desktop.h"


namespace bwm
{


using namespace twl;


//---------------------------------------------------------------------------
CDesktop::CDesktop()
 : CWidget(0)
 , taskBar_(this)
{
}

//---------------------------------------------------------------------------
CDesktop::~CDesktop()
{
}

//---------------------------------------------------------------------------
bool
CDesktop::eventRedraw(const CEventRedraw & event)
{
  pWindow_->setColor(clDesktop);
  pWindow_->fill();
  //pWindow_->drawText(0, 0, "Bricks-OS", clWhite);

  return true;
}

//---------------------------------------------------------------------------
bool
CDesktop::eventResize(const CEventResize & event)
{
  if(rect().height() < 480)
    taskBar_.rect(0, rect().height() - 14, rect().width(), 14); // Tiny mode
  else
    taskBar_.rect(0, rect().height() - 28, rect().width(), 28); // Normal mode

  taskBar_.visible(true);

  return true;
}


};
