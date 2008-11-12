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


#ifndef I386_ATA_H
#define I386_ATA_H


#include "inttypes.h"
#include "kernel/fileSystem.h"
#include "kernel/interrupt.h"


struct SATADeviceInformation
{
  uint16_t iGeneralConfig;
  uint16_t words001_009[9];
  char     sSerialNumber[20];
  uint16_t words020_022[3];
  char     sFirmwareRevision[8];
  char     sModelNumber[40];
  uint16_t words047_079[33];
  uint16_t major_rev_num;
  uint16_t minor_rev_num;
  uint16_t command_set_1;
  uint16_t command_set_2;
  uint16_t command_set_extension;
  uint16_t cfs_enable_1;
  uint16_t word086;
  uint16_t csf_default;
  uint16_t words088_255[168];
} __attribute__ ((__packed__));


// -----------------------------------------------------------------------------
class CATADrive
 : public IBlockDevice
 , public IInterruptServiceRoutine
{
public:
  CATADrive(uint32_t iobase, bool master);
  virtual ~CATADrive();

  virtual int init();

  // Inherited from IBlockDevice
  virtual int read (uint32_t startSector, uint32_t sectorCount,       void * data);
  virtual int write(uint32_t startSector, uint32_t sectorCount, const void * data);

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

private:
  uint32_t iIOBase_;
  uint8_t iMaster_;

  SATADeviceInformation info_;
};

// -----------------------------------------------------------------------------
class CATAChannel
 : public IInterruptServiceRoutine
{
public:
  CATAChannel(uint32_t iobase);
  virtual ~CATAChannel();

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

private:
  uint32_t iIOBase_;

  CATADrive * pMaster_;
  CATADrive * pSlave_;
};

// -----------------------------------------------------------------------------
class CATADriver
{
public:
  CATADriver();
  virtual ~CATADriver();

  virtual int init();

private:
  CATAChannel priChannel_;
  CATAChannel secChannel_;
};


#endif
