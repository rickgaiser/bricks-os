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


#ifndef VIDEO_H
#define VIDEO_H


#include "kernel/fs.h"


class CAVideo
 : public IFileIO
{
public:
  CAVideo(unsigned int iX, unsigned int iY);
  virtual ~CAVideo();

  // Inherited from IFileIO
  virtual ssize_t write(const void * buffer, size_t size, bool block = false);

  virtual void cls();
  virtual void put(char c);
  virtual void put(const char * s);

private:
  virtual void put(int iX, int iY, char c) = 0;
  virtual char get(int iX, int iY) = 0;
  virtual void scrollUp();

  unsigned int iX_;
  unsigned int iY_;

  unsigned int iCurrentX_;
  unsigned int iCurrentY_;
};


#endif
