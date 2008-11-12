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


#ifndef FILESERVER_H
#define FILESERVER_H


#include "msgServer.h"
#include "kernel/fs.h"
#include "kernel/fileDriver.h"


//---------------------------------------------------------------------------
class CReadServer
 : public ACE_Task_Base
{
public:
  CReadServer(IFileIO & file);
  virtual ~CReadServer();

  int svc();
  void read(int iReceiveID, size_t size, bool block);

private:
  void _read(int iReceiveID, size_t size, bool block);

  IFileIO & file_;
  volatile int iReceiveID_;
  volatile size_t size_;
  pthread_mutex_t mutex_;
  volatile bool bExit_;
};

//---------------------------------------------------------------------------
class CWriteServer
 : public ACE_Task_Base
{
public:
  CWriteServer(IFileIO & file);
  virtual ~CWriteServer();

  int svc();
  void write(int iReceiveID, const void * data, size_t size, bool block);

private:
  void _write(int iReceiveID, const void * data, size_t size, bool block);

  IFileIO & file_;
  volatile int iReceiveID_;
  const void * data_;
  volatile size_t size_;
  pthread_mutex_t mutex_;
  volatile bool bExit_;
};

//---------------------------------------------------------------------------
// Our user space file server needs 3 threads:
//  1 - Process incomming messages      (CMsgServer)
//  2 - Process blocking read requests  (CReadServer)
//  3 - Process blocking write requests (CWriteServer)
class CFileServer
 : public CMsgServer
{
public:
  CFileServer(IFileIO & file, const char * name);
  virtual ~CFileServer();

  virtual int process(int iReceiveID, void * pRcvMsg);

private:
  IFileIO & file_;
  CReadServer thrRead_;
  CWriteServer thrWrite_;
  uint32_t flags_[MAX_OPEN_FILES];
};


#endif
