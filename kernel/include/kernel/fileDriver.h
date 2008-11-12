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


#ifndef KERNEL_FILEDRIVER_H
#define KERNEL_FILEDRIVER_H


#include "kernel/srrChannel.h"


#define MAX_OPEN_FILES 10


// -----------------------------------------------------------------------------
enum EFileCommand
{
  FC_OPEN  = 1,
  FC_CLOSE = 2,
  FC_READ  = 3,
  FC_WRITE = 4,
};

// -----------------------------------------------------------------------------
struct SFileHeader
{
  uint32_t command;
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SFileOpenHeader
{
  SFileHeader commonHeader;
  uint32_t flags;
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SFileCloseHeader
{
  SFileHeader commonHeader;
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SFileReadHeader
{
  SFileHeader commonHeader;
  uint32_t size; // Max Read Size
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SFileWriteHeader
{
  SFileHeader commonHeader;
  uint32_t size; // Max Write Size
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
class CKernelFileDriver
 : public CAChannelKernel
{
public:
  CKernelFileDriver(IFileIO & file, const char * name);
  virtual ~CKernelFileDriver();

  virtual int msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);

private:
  IFileIO & file_;
  uint32_t flags_[MAX_OPEN_FILES];
};


#endif
