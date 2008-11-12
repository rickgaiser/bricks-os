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


#ifndef KERNEL_DRIVERMOUNTPOINT_H
#define KERNEL_DRIVERMOUNTPOINT_H


#include "kernel/driverKernel.h"


// -----------------------------------------------------------------------------
struct DIR_ITER
{
};

// -----------------------------------------------------------------------------
struct stat
{
};

// -----------------------------------------------------------------------------
// Mount point interface class
class IMountPoint
{
public:
  virtual ~IMountPoint(){}

  virtual int open(const char * path, int flags, int mode) = 0;
  virtual int close(int fd) = 0;
  virtual int read(int fd, char * ptr, int len) = 0;
  virtual int write(int fd, const char * ptr, int len) = 0;
/*
  virtual int seek(int fd, int pos, int dir) = 0;
  virtual int fstat(int fd, struct stat * st) = 0;
  virtual int stat(const char * file, struct stat * st) = 0;
  virtual int link(const char * existing, const char * newLink) = 0;
  virtual int unlink(const char * name) = 0;
  virtual int chdir(const char * name) = 0;

  virtual int rename(const char * oldName, const char * newName) = 0;
  virtual int mkdir(const char * path, int mode) = 0;
*/
  virtual DIR_ITER * diropen(const char * path) = 0;
  virtual int dirreset(DIR_ITER * dirState) = 0;
  virtual int dirnext(DIR_ITER * dirState, char * filename, struct stat * filestat) = 0;
  virtual int dirclose(DIR_ITER * dirState) = 0;
};

// -----------------------------------------------------------------------------
// Kernel driver for converting messages to IMountPoint calls
class CMountPointDriver
 : public IKernelDriver
{
public:
  CMountPointDriver(IMountPoint * mountPoint = 0);
  virtual ~CMountPointDriver();

  void setMountPoint(IMountPoint * mountPoint);

  virtual int msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);

private:
  IMountPoint * pMountPoint_;
};


#endif
