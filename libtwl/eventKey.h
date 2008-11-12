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


#ifndef TWL_EVENTKEY_H
#define TWL_EVENTKEY_H


#include "event.h"


namespace twl
{


enum EKeyCodes
{
    kcUP
  , kcDOWN
  , kcLEFT
  , kcL1         // Game Console Key
  , kcL2         // Game Console Key
  , kcRIGHT
  , kcR1         // Game Console Key
  , kcR2         // Game Console Key
  , kcSTART      // Game Console Key
  , kcSELECT     // Game Console Key
  , kcTRIANGLE   // Game Console Key (Sony)
  , kcCROSS      // Game Console Key (Sony)
  , kcSQUARE     // Game Console Key (Sony)
  , kcCIRCEL     // Game Console Key (Sony)
  , kcA
  , kcB
  , kcX
  , kcY
};

class CEventKey
 : public CEvent
{
public:
  CEventKey(int key, bool bPressed);
  virtual ~CEventKey();

  int key() const;
  bool pressed() const;

private:
  int iKey_;
  bool bPressed_;
};


};


#endif
