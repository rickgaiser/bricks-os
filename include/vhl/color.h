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


#ifndef GL_COLOR_H
#define GL_COLOR_H


//---------------------------------------------------------------------------
template <class T>
class TColor
{
public:
  TColor();
  TColor(const TColor & c);
  TColor(const T * c);
  TColor(T _r, T _g, T _b, T _a);

  TColor & operator= (const TColor & c);
  TColor & operator= (const T * c);

  TColor   operator+ (const TColor & c) const;
  TColor   operator- (const TColor & c) const;
  TColor   operator* (const TColor & c) const;
  TColor   operator/ (const TColor & c) const;
  TColor   operator+ (T s) const;
  TColor   operator- (T s) const;
  TColor   operator* (T s) const;
  TColor   operator/ (T s) const;
  TColor   getClamped() const;

  TColor & operator+=(const TColor & c);
  TColor & operator-=(const TColor & c);
  TColor & operator*=(const TColor & c);
  TColor & operator/=(const TColor & c);
  TColor & operator+=(T s);
  TColor & operator-=(T s);
  TColor & operator*=(T s);
  TColor & operator/=(T s);
  TColor & clamp();

public:
  T r;
  T g;
  T b;
  T a;
};


#include "color.inl"


#endif // GL_COLOR_H
