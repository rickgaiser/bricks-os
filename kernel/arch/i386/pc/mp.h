/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2010 Maximus32 <Maximus32@bricks-os.org>
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


#ifndef I386_MP_H
#define I386_MP_H


#include "stdint.h"


#define MP_FLOATING_SIGNATURE 0x5f504d5f // "_MP_"


struct SMPFloatingPointer
{
  uint32_t iSignature;
  uint32_t pAddr;
  uint8_t iLength;
  uint8_t iSpecRev;
  uint8_t iChecksum;
} __attribute__ ((__packed__));

struct SMPTableHeader
{
  uint32_t iSignature;
  uint16_t iBaseTableLength;
  uint8_t iSpecRev;
  uint8_t iBaseTableChecksum;
  char sOemId[8];
  char sProdId[12];
  uint32_t iOemTablePointer;
  uint16_t iOemTableSize;
  uint16_t iEntryCount;
  uint32_t iLocalAPICBase;
  uint16_t iExtendedTableLength;
  uint8_t iExtendedTableChecksum;
  uint8_t reserved1;
} __attribute__ ((__packed__));

struct SMPTableEntryProcessor
{
  uint8_t iEntryType;
  uint8_t iLocalApicId;
  uint8_t iLocalApicVersion;
  uint8_t iCPUFlags;
  uint32_t iCPUSignature;
  uint32_t iFeatureFlags;
  uint32_t reserved1;
  uint32_t reserved2;
} __attribute__ ((__packed__));


void init_mp();


#endif
