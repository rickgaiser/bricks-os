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


#ifndef BWM_WINDOWMANAGER_H
#define BWM_WINDOWMANAGER_H


#include "window.h"
#include "kernel/videoManager.h"
#include "kernel/2dRenderer.h"
#include "vector"


namespace bwm
{


class CWindowManager
{
public:
  CWindowManager();
  virtual ~CWindowManager();

  int init(CAVideoDevice * device);

  CWindow * requestNewWindow();
  int exec();

private:
  void setActive(CWindow * window);
  void drawCursor();
  void undrawCursor();

private:
  // Windows
  std::vector<CWindow *> pWindows_;
  CWindow * pActiveWindow_;

  // Cursor
  color_t underCursor_[16 * 16];
  bool bCursorActive_;
  int xOff_;
  int yOff_;

  // Display surface
  CAVideoDevice * pDevice_;
  I2DRenderer   * pRenderer_;
  CSurface      * pSurface_;
};


extern CWindowManager windowManager;


};


#endif
