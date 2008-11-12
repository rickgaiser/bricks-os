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


#ifndef TWL_FRAME_H
#define TWL_FRAME_H


#include "widget.h"
#include "eventRedraw.h"


namespace twl
{


class CFrame
 : public CWidget
{
public:
  enum EFrameType
  {
      ftNone
    , ftPlain
    , ftRaised
    , ftSunken
  };

  CFrame(CWidget * parent = 0, EFrameType type = ftNone);
  virtual ~CFrame();

  void setFrameType(EFrameType type);

  virtual bool eventRedraw(const CEventRedraw & event);

private:
  EFrameType eType_;
};


};


#endif
