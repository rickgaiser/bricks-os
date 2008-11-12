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


#ifndef TWL_PUSHBUTTON_H
#define TWL_PUSHBUTTON_H


#include "frame.h"
#include "widget.h"
#include "event.h"
#include "eventKey.h"
#include "eventRedraw.h"


namespace twl
{


class CPushButton
 : public CFrame
{
public:
  CPushButton(CWidget * parent = 0, const char * string = 0);
  virtual ~CPushButton();

  virtual bool eventKey(const CEventKey & event);
  virtual bool eventRedraw(const CEventRedraw & event);

  virtual void pressed(); // Button pressed down
  virtual void released(); // Button released again
  virtual void clicked(); // Button pressed and then released

private:
  bool bPressed_;
  const char * string_;
};


};


#endif
