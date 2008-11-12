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


#include "frame.h"


namespace twl
{


//---------------------------------------------------------------------------
CFrame::CFrame(CWidget * parent, EFrameType type)
 : CWidget(parent)
 , eType_(type)
{
}

//---------------------------------------------------------------------------
CFrame::~CFrame()
{
}

//---------------------------------------------------------------------------
void
CFrame::setFrameType(EFrameType type)
{
  eType_ = type;
}

//---------------------------------------------------------------------------
bool
CFrame::eventRedraw(const CEventRedraw & event)
{
  const CRect & rect = this->rectAbsolute();
  bool bRaised(false);

  switch(eType_)
  {
    case ftNone:
      pWindow_->setColor(clPanelFill);
      pWindow_->fillRect(rect.x(), rect.y(), rect.width(), rect.height());
      break;
    case ftPlain:
      pWindow_->setColor(clPanelShade1);
      // left/top
      pWindow_->drawVLine(rect.x()    , rect.y(), rect.height() - 1);
      pWindow_->drawHLine(rect.x() + 1, rect.y(), rect.width()  - 2);
      // right/bottom
      pWindow_->drawVLine(rect.right() - 1, rect.top()       , rect.height()    );
      pWindow_->drawHLine(rect.left()     , rect.bottom() - 1, rect.width()  - 1);
      // Fill client area
      pWindow_->setColor(clPanelFill);
      pWindow_->fillRect(rect.x() + 1, rect.y() + 1, rect.width() - 2, rect.height() - 2);
      break;
    case ftRaised:
      bRaised = true;
    case ftSunken:
      // left/top outer
      pWindow_->setColor(bRaised ? clPanelFill : clPanelShade1);
      pWindow_->drawVLine(rect.x()    , rect.y()    , rect.height() - 1);
      pWindow_->drawHLine(rect.x() + 1, rect.y()    , rect.width()  - 2);
      // left/top inner
      pWindow_->setColor(bRaised ? clPanelLight : clPanelShade2);
      pWindow_->drawVLine(rect.x() + 1, rect.y() + 1, rect.height() - 2);
      pWindow_->drawHLine(rect.x() + 2, rect.y() + 1, rect.width()  - 3);
      // right/bottom outer
      pWindow_->setColor(bRaised ? clPanelShade1 : clPanelFill);
      pWindow_->drawVLine(rect.right() - 1, rect.top()       , rect.height()    );
      pWindow_->drawHLine(rect.left()     , rect.bottom() - 1, rect.width()  - 1);
      // right/bottom inner
      pWindow_->setColor(bRaised ? clPanelShade2 : clPanelLight);
      pWindow_->drawVLine(rect.right() - 2, rect.top()    + 1, rect.height() - 2);
      pWindow_->drawHLine(rect.left()  + 1, rect.bottom() - 2, rect.width()  - 3);
      // Fill client area
      pWindow_->setColor(clPanelFill);
      pWindow_->fillRect(rect.x() + 2, rect.y() + 2, rect.width() - 4, rect.height() - 4);
      break;
  };

  return true;
}


};
