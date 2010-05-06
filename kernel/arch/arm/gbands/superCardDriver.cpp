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


#include "superCardDriver.h"
#include "kernel/debug.h"


#define ata_inb(addr)        (*(vuint16_t *)(addr))
#define ata_inw(addr)        (*(vuint16_t *)(addr))
#define ata_outb(addr,data) ((*(vuint16_t *)(addr)) = data)
#define ata_outw(addr,data) ((*(vuint16_t *)(addr)) = data)

// Values for changing mode
#define SC_MODE_RAM    0x5
#define SC_MODE_MEDIA  0x3
#define SC_MODE_RAM_RO 0x1

// CF Card status
#define CF_STS_INSERTED     0x50
#define CF_STS_REMOVED      0x00
#define CF_STS_READY        0x58

#define CF_CARD_TIMEOUT 10000000


// -----------------------------------------------------------------------------
const SATARegisters supercardRegs =
{
   0x09000000,
  {0x09020000},
   0x09040000,
   0x09060000,
   0x09080000,
   0x090A0000,
   0x090C0000,
  {0x090E0000},
  {0x098C0000},
};


// -----------------------------------------------------------------------------
CSuperCardDriver::CSuperCardDriver()
{
}

// -----------------------------------------------------------------------------
CSuperCardDriver::~CSuperCardDriver()
{
}

// -----------------------------------------------------------------------------
int
CSuperCardDriver::init()
{
  changeMode(SC_MODE_MEDIA);

  regs_ = supercardRegs;

  // See if there is a read/write register
  uint16_t temp = ata_inw(regs_.lbaLow);
  ata_outw(regs_.lbaLow, ~temp & 0xff);
  temp = ~temp & 0xff;
  if(ata_inw(regs_.lbaLow) != temp)
    return -1;

  // Make sure it is 8 bit
  ata_outw(regs_.lbaLow, 0xaa55);
  if(ata_inw(regs_.lbaLow) == 0xaa55)
    return -1;

  printk("Supercard Detected\n");
  CFileSystem::addBlockDevice(this);

  return 0;
}

// -----------------------------------------------------------------------------
bool
CSuperCardDriver::inserted()
{
  // Change register, then check if value did change
  ata_outw(regs_.devControl, CF_STS_INSERTED);
  return ((ata_inw(regs_.altStatus) & 0xff) == CF_STS_INSERTED);
}

// -----------------------------------------------------------------------------
int
CSuperCardDriver::read(uint32_t startSector, uint32_t sectorCount, void * data)
{
  int i;

  uint16_t * buff = (uint16_t *)data;

  // Wait until CF card is finished previous commands
  i=0;
  while((ata_inw(regs_.status) & ATA_STATUS_BSY) && (i < CF_CARD_TIMEOUT))
    i++;
  if(i >= CF_CARD_TIMEOUT)
    return -1;

  // Wait until card is ready for commands
  i = 0;
  while((!(ata_inw(regs_.altStatus) & CF_STS_INSERTED)) && (i < CF_CARD_TIMEOUT))
    i++;
  if(i >= CF_CARD_TIMEOUT)
    return -1;

  // Set number of sectors to read
  ata_outw(regs_.sectorCount, (sectorCount < 256) ? sectorCount : 0); // Read a maximum of 256 sectors, 0 means 256

  // Set read sector
  ata_outw(regs_.lbaLow,  ((startSector      ) & 0xff)); // 1st byte of sector number
  ata_outw(regs_.lbaMid,  ((startSector >>  8) & 0xff)); // 2nd byte of sector number
  ata_outw(regs_.lbaHigh, ((startSector >> 16) & 0xff)); // 3rd byte of sector number
  ata_outw(regs_.device,  ((startSector >> 24) & 0x0f) | ATA_DRV_HEAD_LBA28); // last nibble of sector number

  // Set command to read
  ata_outw(regs_.command, ATA_COMMAND_READ_SECTORS);

  for(unsigned int iSector(0); iSector < sectorCount; iSector++)
  {
    // Wait until card is ready for reading
    i = 0;
    while(((ata_inw(regs_.altStatus) & 0xff) != CF_STS_READY) && (i < CF_CARD_TIMEOUT))
      i++;
    if(i >= CF_CARD_TIMEOUT)
      return -1;

    // Read data
    i=256;
    while(i--)
      *buff++ = ata_inw(regs_.data);
  }

  return sectorCount;
}

// -----------------------------------------------------------------------------
int
CSuperCardDriver::write(uint32_t startSector, uint32_t sectorCount, const void * data)
{
  return -1;
}

// -----------------------------------------------------------------------------
void
CSuperCardDriver::changeMode(uint8_t mode)
{
  vuint16_t *unlockAddress = (vuint16_t *)0x09FFFFFE;
  *unlockAddress = 0xA55A;
  *unlockAddress = 0xA55A;
  *unlockAddress = mode;
  *unlockAddress = mode;
}
