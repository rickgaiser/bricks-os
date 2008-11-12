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


#ifndef TWL_RECT_H
#define TWL_RECT_H


namespace twl
{


class CRect
{
public:
  CRect();
  CRect(const CRect & rect);
  CRect(int x, int y, int width, int height);
  ~CRect();

  CRect & operator=(const CRect & rect);

  // Get sizes
  int x     () const;
  int y     () const;
  int width () const;
  int height() const;

  int left  () const;
  int top   () const;
  int right () const;
  int bottom() const;

  // Set sizes
  void x     (int x);
  void y     (int y);
  void width (int width);
  void height(int height);
  void rect  (int x, int y, int width, int height);

  bool valid();
  bool contains(const CRect & rect);
  bool contains(int x, int y);
  bool intersects(const CRect & rect);
  void clip(const CRect & rect);

private:
  int iX_;
  int iY_;
  int iWidth_;
  int iHeight_;
};


};


#include "rect.inl"


#endif
