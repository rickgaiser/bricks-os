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


#ifndef FATDRIVER_H
#define FATDRIVER_H


#include "kernel/fileSystem.h"
#include "kernel/driverMountPoint.h"


// -----------------------------------------------------------------------------
struct SBPB
{
  uint8_t  BS_jmpBoot[3];
  char     BS_OEMName[8];
  uint16_t BPB_BytsPerSec;
  uint8_t  BPB_SecPerClus;
  uint16_t BPB_RsvdSecCnt;
  uint8_t  BPB_NumFATs;
  uint16_t BPB_RootEntCnt;
  uint16_t BPB_TotSec16;
  uint8_t  BPB_Media;
  uint16_t BPB_FATSz16;
  uint16_t BPB_SecPerTrk;
  uint16_t BPB_NumHeads;
  uint32_t BPB_HiddSec;
  uint32_t BPB_TotSec32;
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SBPB16
{
  uint8_t  BS_DrvNum;
  uint8_t  BS_Reserved1;
  uint8_t  BS_BootSig;
  uint32_t BS_VolID;
  char     BS_VolLab[11];
  char     BS_FilSysType[8];
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SBPB32
{
  uint32_t BPB_FATSz32;
  uint16_t BPB_ExtFlags;
  uint16_t BPB_FSVer;
  uint32_t BPB_RootClus;
  uint16_t BPB_FSInfo;
  uint16_t BPB_BkBootSec;
  uint8_t  BPB_Reserved[12];
  uint8_t  BS_DrvNum;
  uint8_t  BS_Reserved1;
  uint8_t  BS_BootSig;
  uint32_t BS_VolID;
  char     BS_VolLab[11];
  char     BS_FilSysType[8];
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SFATEntry
{
  uint8_t  name[11];
  uint8_t  attr;
  uint8_t  reserved1;
  uint8_t  createTime10ms;
  uint16_t createTime;
  uint16_t createDate;
  uint16_t accessDate;
  uint16_t firstClusterHi;
  uint16_t writeTime;
  uint16_t writeDate;
  uint16_t firstClusterLo;
  uint32_t fileSize;
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
class CFATDriver
 : public IFileSystemDriver
{
public:
  CFATDriver();
  virtual ~CFATDriver();

  virtual bool init(IBlockDevice * device);
};

// -----------------------------------------------------------------------------
class CFATVolume
 : public IMountPoint
{
public:
  CFATVolume(IBlockDevice * device);
  virtual ~CFATVolume();

  virtual int open(const char * path, int flags, int mode);
  virtual int close(int fd);
  virtual int read(int fd, char * ptr, int len);
  virtual int write(int fd, const char * ptr, int len);

  virtual DIR_ITER * diropen(const char * path);
  virtual int dirreset(DIR_ITER * dirState);
  virtual int dirnext(DIR_ITER * dirState, char * filename, struct stat * filestat);
  virtual int dirclose(DIR_ITER * dirState);

private:
  IBlockDevice * device_;

  uint8_t * pBootSector_;
  SBPB    * pBPB_;
  SBPB16  * pBPB16_;
  SBPB32  * pBPB32_;

  uint32_t iClusterCount_;
  uint32_t iRootDirSectors_;
  uint32_t iDataSectors_;
  uint32_t iFATSz_;
  uint32_t iTotSec_;
  uint32_t iFirstDataSector_;
  uint32_t iRootDirSec_;
};


#endif
