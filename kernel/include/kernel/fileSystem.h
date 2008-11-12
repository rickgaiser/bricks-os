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


#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include "inttypes.h"
#include "vector"


// -----------------------------------------------------------------------------
class IBlockDevice
{
public:
  virtual ~IBlockDevice(){}

  virtual int read (uint32_t startSector, uint32_t sectorCount,       void * data) = 0;
  virtual int write(uint32_t startSector, uint32_t sectorCount, const void * data) = 0;
};

// -----------------------------------------------------------------------------
class CPartition
 : public IBlockDevice
{
public:
  CPartition(IBlockDevice * blockDevice, uint32_t startSector, uint32_t sectorCount);
  virtual ~CPartition();

  virtual int read (uint32_t startSector, uint32_t sectorCount,       void * data);
  virtual int write(uint32_t startSector, uint32_t sectorCount, const void * data);

protected:
  IBlockDevice * pBlockDevice_;
  uint32_t iStartSector_;
  uint32_t iEndSector_;
  uint32_t iSectorCount_;
};

// -----------------------------------------------------------------------------
class IPartitionDriver
{
public:
  virtual ~IPartitionDriver(){}

  virtual bool init(IBlockDevice * device) = 0;
};

// -----------------------------------------------------------------------------
class IFileSystemDriver
{
public:
  virtual ~IFileSystemDriver(){}

  virtual bool init(IBlockDevice * device) = 0;
};

// -----------------------------------------------------------------------------
class IMountPoint;
// -----------------------------------------------------------------------------
class CFileSystem
{
public:
  static void addBlockDevice(IBlockDevice * device);
  static void remBlockDevice(IBlockDevice * device);

  static void addPartitionDriver(IPartitionDriver * driver);
  static void remPartitionDriver(IPartitionDriver * driver);

  static void addFileSystemDriver(IFileSystemDriver * driver);
  static void remFileSystemDriver(IFileSystemDriver * driver);

  static void mount(IMountPoint * point, const char * location);
  static void unmount(IMountPoint * point);
  static void unmount(const char * location);

private:
  typedef std::vector<IBlockDevice *>       blockdevicelist_t;
  typedef std::vector<IPartitionDriver *>   partitiondriverlist_t;
  typedef std::vector<IFileSystemDriver *>  filesystemdriverlist_t;
  struct SMountPoint
  {
    char * location;
    IMountPoint * point;
  };
  typedef std::vector<SMountPoint *>        mountpointlist_t;

  static blockdevicelist_t       listBlockDevices_;
  static partitiondriverlist_t   listPartitionDrivers_;
  static filesystemdriverlist_t  listFileSystemDrivers_;
  static mountpointlist_t        listMountPoints_;
};


#endif
