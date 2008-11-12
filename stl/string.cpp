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


#include "string"


namespace std
{


// -----------------------------------------------------------------------------
string::string(const char * string)
 : iLength_(0)
{
  while((string[iLength_] != 0) && (iLength_ < (STRING_SIZE - 1)))
  {
    string_[iLength_] = string[iLength_];
    iLength_++;
  }

  string_[iLength_] = 0;
}

// -----------------------------------------------------------------------------
ostream &
operator<<(ostream & os, const string & s)
{
  return (os<<s.c_str());
}


}
