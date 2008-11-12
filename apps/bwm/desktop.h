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


#ifndef BWM_DESKTOP_H
#define BWM_DESKTOP_H


#include "frame.h"
#include "taskBar.h"


namespace bwm
{


class CDesktop
 : public twl::CWidget
{
public:
  CDesktop();
  virtual ~CDesktop();

  virtual bool eventRedraw(const twl::CEventRedraw & event);
  virtual bool eventResize(const twl::CEventResize & event);

private:
  CTaskBar taskBar_;
};


};


#endif
