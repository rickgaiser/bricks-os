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


#include "fileStream.h"
#include "kernel/debug.h"
#include "unistd.h"
#include "fcntl.h"


//------------------------------------------------------------------------------
CFileStream::CFileStream(const char * in, const char * out)
 : pIn_(in)
 , pOut_(out)
 , fdIn_(-1)
 , fdOut_(-1)
{
}

//------------------------------------------------------------------------------
CFileStream::~CFileStream()
{
  if(fdIn_ > 0)
    close(fdIn_);

  if(fdOut_ > 0)
    close(fdOut_);
}

//------------------------------------------------------------------------------
bool
CFileStream::start()
{
  fdIn_ = open(pIn_, O_RDONLY);
  if(fdIn_ < 0)
  {
    printk("Unable to open input stream\n");
    return false;
  }

  fdOut_ = open(pOut_, O_WRONLY);
  if(fdOut_ < 0)
  {
    close(fdIn_);
    fdIn_ = -1;
    printk("Unable to open output stream\n");
    return false;
  }

  this->activate();

  return true;
}

//------------------------------------------------------------------------------
int
CFileStream::svc()
{
  char buffer[80];

  while(true)
  {
    int iSize = read(fdIn_, buffer, 80);
    if(iSize <= 0)
    {
      printk("Unable to read from input stream\n");
      return -1;
    }

    int iSize2 = write(fdOut_, buffer, iSize);
    if(iSize2 != iSize)
    {
      printk("Unable to write to output stream\n");
      return -1;
    }
  }
  return 0;
}
