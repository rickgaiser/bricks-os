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


#ifndef ATA_H
#define ATA_H


#include "stdint.h"
#include "kernel/fileSystem.h"
#include "kernel/interrupt.h"


// Status register bits
#define ATA_STATUS_BSY  0x80 // busy
#define ATA_STATUS_DRDY 0x40 // device ready
#define ATA_STATUS_DF   0x20 // Device Fault
#define ATA_STATUS_DSC  0x10 // seek complete
#define ATA_STATUS_DRQ  0x08 // Data Transfer Requested
#define ATA_STATUS_CORR 0x04 // data corrected
#define ATA_STATUS_IDX  0x02 // index mark
#define ATA_STATUS_ERR  0x01 // error

// Error register bits
#define ATA_ERROR_BBK   0x80 // Bad Block
#define ATA_ERROR_UNC   0x40 // Uncorrectable data error
#define ATA_ERROR_MC    0x20 // Media Changed
#define ATA_ERROR_IDNF  0x10 // ID mark Not Found
#define ATA_ERROR_MCR   0x08 // Media Change Requested
#define ATA_ERROR_ABRT  0x04 // command aborted
#define ATA_ERROR_TK0NF 0x02 // Track 0 Not Found
#define ATA_ERROR_AMNF  0x01 // Address Mark Not Found

// Drive/Head register bits
#define ATA_DRV_HEAD_MASTER 0x00
#define ATA_DRV_HEAD_SLAVE  0x10
#define ATA_DRV_HEAD_CHS    0xa0
#define ATA_DRV_HEAD_LBA28  0xe0
#define ATA_DRV_HEAD_LBA48  0x40

// Commands
#define ATA_COMMAND_READ_SECTORS      0x20
#define ATA_COMMAND_DEVICE_DIAGNOSTIC 0x90
#define ATA_COMMAND_IDENTIFY_DRIVE    0xec


// -----------------------------------------------------------------------------
struct SATADeviceInformation
{
  uint16_t words000_009[10];
  uint8_t  serial_no[20];
  uint16_t words020_022[3];
  uint8_t  fw_rev[8];
  uint8_t  model[40];
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
struct SATARegisters
{
  uint32_t data;         // (rw) Data
  union{
    uint32_t error;      // (r)  Error
    uint32_t features;   // (w)  Features
  };
  uint32_t sectorCount;  // (rw) Sector Count
  uint32_t lbaLow;       // (rw) LBA Low
  uint32_t lbaMid;       // (rw) LBA Mid
  uint32_t lbaHigh;      // (rw) LBA High
  uint32_t device;       // (rw) Device (Drive/Head)
  union{
    uint32_t status;     // (r)  Status
    uint32_t command;    // (w)  Command
  };
  union{
    uint32_t altStatus;  // (r)  Alternate Status
    uint32_t devControl; // (w)  Device Control
  };
};


#endif
