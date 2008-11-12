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


#ifndef TWL_EVENT_H
#define TWL_EVENT_H


namespace twl
{


class CEvent
{
public:
  enum EEventType
  {
      etError    = 0
    , etKeyboard = 1
    , etMouse    = 2
    , etRedraw   = 3
    , etResize   = 4
  };

  CEvent(EEventType type = etError);
  virtual ~CEvent();

  EEventType type() const;

private:
  EEventType eType_;
};

class IWindowEventHandler
{
public:
  virtual ~IWindowEventHandler(){}

  virtual bool event(const CEvent & event) = 0;
};


};


#endif
