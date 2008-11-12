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


#include "pushButton.h"


namespace twl
{


//---------------------------------------------------------------------------
CPushButton::CPushButton(CWidget * parent, const char * string)
 : CFrame(parent, CFrame::ftRaised)
 , bPressed_(false)
 , string_(string)
{
}

//---------------------------------------------------------------------------
CPushButton::~CPushButton()
{
}

//---------------------------------------------------------------------------
bool
CPushButton::eventKey(const CEventKey & event)
{
  CWidget::eventKey(event);

  switch(event.key())
  {
    case kcA:
      if(event.pressed() == true)
      {
        setFrameType(CFrame::ftSunken);
        bPressed_ = true;
        redraw();
        pressed();
      }
      else
      {
        setFrameType(CFrame::ftRaised);
        bPressed_ = false;
        redraw();
        released();
        clicked();
      }
      break;
  };

  return true;
}

//---------------------------------------------------------------------------
bool
CPushButton::eventRedraw(const CEventRedraw & event)
{
  CFrame::eventRedraw(event);
/*
  if(string_ != 0)
  {
    if(bPressed_ == false)
      pWindow_->drawText(rectAbsolute().left() + 2, rectAbsolute().top() + 2, string_);
    else
      pWindow_->drawText(rectAbsolute().left() + 3, rectAbsolute().top() + 3, string_);
  }
*/
  return true;
}

//---------------------------------------------------------------------------
void
CPushButton::pressed()
{
}

//---------------------------------------------------------------------------
void
CPushButton::released()
{
}

//---------------------------------------------------------------------------
void
CPushButton::clicked()
{
  static CFrame * widget = 0;
  if(widget == 0)
  {
    widget = new CFrame(0, CFrame::ftSunken);
    widget->rect(60, 50, 120, 80);
  }
}


};
