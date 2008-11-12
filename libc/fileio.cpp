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


#include "kernel/srr.h"
#include "kernel/fileDriver.h"
#include "fcntl.h"
#include "string.h"
#include "unistd.h"


#define va_list         __builtin_va_list
//#define va_start(v,l)   __builtin_va_start(v,l)
#define va_start(v,l)   __builtin_stdarg_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)


// -----------------------------------------------------------------------------
int
vopen(const char * path, int oflag, va_list ap)
{
  int iRetVal, iNID = 0, iPID, iChannelID;

  iRetVal = lookupName(path, &iPID, &iChannelID);

  if(iRetVal >= 0)
    iRetVal = channelConnectAttach(iNID, iPID, iChannelID, 0);

  return iRetVal;
}

// -----------------------------------------------------------------------------
int
open(const char * path, int oflag, ...)
{
  va_list ap;
  int     fd;

  va_start(ap, oflag);
  fd = vopen(path, oflag, ap);
  va_end(ap);

  return fd;
}

// -----------------------------------------------------------------------------
int
close(int fd)
{
  return channelConnectDetach(fd);
}

// -----------------------------------------------------------------------------
ssize_t
read(int fd, void * buffer, size_t size)
{
  ssize_t iRetVal(0);

  uint32_t iMsgSize = sizeof(SFileReadHeader);
  uint8_t * pMessage = new uint8_t[iMsgSize];
  SFileReadHeader * pHeader = (SFileReadHeader *)pMessage;

  pHeader->commonHeader.command = FC_READ;
  pHeader->size = size;

  iRetVal = msgSend(fd, pMessage, iMsgSize, buffer, size);

  delete pMessage;

  return iRetVal;
}

// -----------------------------------------------------------------------------
ssize_t
write(int fd, const void * buffer, size_t size)
{
  ssize_t iRetVal(0);

  uint32_t iMsgSize = size + sizeof(SFileWriteHeader);
  uint8_t * pMessage = new uint8_t[iMsgSize];
  SFileWriteHeader * pHeader = (SFileWriteHeader *)pMessage;
  uint8_t * pData = pMessage + sizeof(SFileWriteHeader);

  pHeader->commonHeader.command = FC_WRITE;
  pHeader->size = size;
  memcpy(pData, buffer, size);

  iRetVal = msgSend(fd, pMessage, iMsgSize, 0, 0);

  delete pMessage;

  return iRetVal;
}
