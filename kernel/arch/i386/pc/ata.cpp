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


#include "ata.h"
#include "asm/hal.h"
#include "asm/cpu.h"
#include "kernel/debug.h"
#include "kernel/endian.h"
#include "kernel/interruptManager.h"
#include "stddef.h"


#define ata_inb(addr)       (inb(addr))
#define ata_inw(addr)       (inw(addr))
#define ata_outb(addr,data) (outb(data,addr))
#define ata_outw(addr,data) (outw(data,addr))


// -----------------------------------------------------------------------------
const SATARegisters sATARegs[] =
{
  {0x1f0, {0x1f1}, 0x1f2, 0x1f3, 0x1f4, 0x1f5, 0x1f6, {0x1f7}, {0x3f6}},
  {0x170, {0x171}, 0x172, 0x173, 0x174, 0x175, 0x176, {0x177}, {0x376}},
};


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CATADrive::CATADrive(const SATARegisters * regs, bool master)
 : pRegs_(regs)
 , iMaster_(master ? ATA_DRV_HEAD_MASTER : ATA_DRV_HEAD_SLAVE)
{
}

// -----------------------------------------------------------------------------
CATADrive::~CATADrive()
{
}

// -----------------------------------------------------------------------------
int
CATADrive::init()
{
  // Select master/slave drive
  ata_outb(pRegs_->device, iMaster_);

  // Wait until device is no longer busy
  while((ata_inb(pRegs_->status) & ATA_STATUS_BSY) != 0);

  // Disable interrupts
  unsigned long flags = local_save_flags();
  local_irq_disable();

  // Wait until device is ready
  while((ata_inb(pRegs_->status) & ATA_STATUS_DRDY) == 0);

  // Send "identify drive" command
  ata_outb(pRegs_->command, ATA_COMMAND_IDENTIFY_DRIVE);

  // Wait for data
  while((ata_inb(pRegs_->status) & ATA_STATUS_DRQ) == 0);

  // Transfer data
  uint16_t temp;
  for(int i(0); i < 256; i++)
  {
    temp = ata_inw(pRegs_->data);
    temp = ENDIAN_BE_16(temp);
    ((uint16_t *)&info_)[i] = temp;
  }

  // Restore interrupts
  local_irq_restore(flags);

  //info_.serial_no[19] = 0;
  //printk("HDD Serial: %s\n", info_.serial_no);
  //info_.fw_rev[7] = 0;
  //printk("HDD Firmware: %s\n", info_.fw_rev);
  //info_.model[39] = 0;
  //printk("ATA HDD Detected: %s\n", info_.model);

  return 0;
}

// -----------------------------------------------------------------------------
int
CATADrive::read(uint32_t startSector, uint32_t sectorCount, void * data)
{
  //printk("CATADrive::read(%d, %d)\n", startSector, sectorCount);

  //printk("CATADrive::init: waiting for busy\n");
  // Wait untill device is no longer busy
  while((ata_inb(pRegs_->status) & ATA_STATUS_BSY) != 0);

  // Disable interrupts
  unsigned long flags = local_save_flags();
  local_irq_disable();

  //printk("CATADrive::init: waiting for ready\n");
  // Wait untill device is ready
  while((ata_inb(pRegs_->status) & ATA_STATUS_DRDY) == 0);

  // Set sector count
  ata_outb(pRegs_->sectorCount, sectorCount);
  // Set LBA addr
  ata_outb(pRegs_->lbaLow,  (startSector & 0x000000ff)      );
  ata_outb(pRegs_->lbaMid,  (startSector & 0x0000ff00) >>  8);
  ata_outb(pRegs_->lbaHigh, (startSector & 0x00ff0000) >> 16);
  // Set device & LBA mode
  ata_outb(pRegs_->device, iMaster_ | ATA_DRV_HEAD_LBA);

  // Send "read sectors" command
  ata_outb(pRegs_->command, ATA_COMMAND_READ_SECTORS);

  //printk("CATADrive::init: waiting for data\n");
  // Wait for data
  while((ata_inb(pRegs_->status) & ATA_STATUS_DRQ) == 0);

  //printk("CATADrive::init: transferring data...\n");
  // Transfer data
  uint16_t temp;
  for(unsigned int i(0); i < (sectorCount * 256); i++)
  {
    temp = ata_inw(pRegs_->data);
    ((uint16_t *)data)[i] = temp;
  }

  // Restore interrupts
  local_irq_restore(flags);

  return sectorCount;
}

// -----------------------------------------------------------------------------
int
CATADrive::write(uint32_t startSector, uint32_t sectorCount, const void * data)
{
  printk("CATADrive::write(%d, %d)\n", startSector, sectorCount);

  return -1;
}

// -----------------------------------------------------------------------------
int
CATADrive::isr(int irq)
{
  panic("CATADrive::isr(0x%x)\n", irq);

  return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CATAChannel::CATAChannel(const SATARegisters * regs)
 : pRegs_(regs)
 , pMaster_(NULL)
 , pSlave_(NULL)
{
}

// -----------------------------------------------------------------------------
CATAChannel::~CATAChannel()
{
  if(pMaster_ != NULL)
    delete pMaster_;
  if(pSlave_ != NULL)
    delete pSlave_;
}

// -----------------------------------------------------------------------------
int
CATAChannel::init()
{
  uint8_t iError;
  bool bMasterPresent(false);
  bool bSlavePresent(false);

  // Send "device diagnostic" command to both devices
  ata_outb(pRegs_->command, ATA_COMMAND_DEVICE_DIAGNOSTIC);

  // Select master
  ata_outb(pRegs_->device, ATA_DRV_HEAD_MASTER | ATA_DRV_HEAD_LBA);
  // Read error register
  iError = ata_inb(pRegs_->error);

  // Master present?
  if((iError == 0x01) || (iError == 0x81))
    bMasterPresent = true;

  // Slave present?
  if(iError < 0x80)
  {
    // Select slave
    ata_outb(pRegs_->device, ATA_DRV_HEAD_SLAVE | ATA_DRV_HEAD_LBA);
    // Read error register
    iError = ata_inb(pRegs_->error);

    if(iError == 0x01)
      bSlavePresent = true;
  }

  if(bMasterPresent == true)
  {
    printk("CATAChannel: Master drive detected\n");
    pMaster_ = new CATADrive(pRegs_, true);
    pMaster_->init();
    CFileSystem::addBlockDevice(pMaster_);
  }

  if(bSlavePresent == true)
  {
    printk("CATAChannel: Slave drive detected\n");
    pSlave_ = new CATADrive(pRegs_, false);
    pSlave_->init();
    CFileSystem::addBlockDevice(pSlave_);
  }

  return 0;
}

// -----------------------------------------------------------------------------
int
CATAChannel::isr(int irq)
{
  if((pMaster_ == NULL) && (pSlave_ == NULL))
    panic("CATAChannel::isr(0x%x)\n", irq);

  if(pMaster_ != NULL)
    pMaster_->isr(irq);

  if(pSlave_ != NULL)
    pSlave_->isr(irq);

  return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CATADriver::CATADriver()
 : priChannel_(&sATARegs[0])
 , secChannel_(&sATARegs[1])
{
}

// -----------------------------------------------------------------------------
CATADriver::~CATADriver()
{
}

// -----------------------------------------------------------------------------
int
CATADriver::init()
{
  //CInterruptManager::attach(0x2e, &priChannel_); // Pimary IDE
  //CInterruptManager::attach(0x2f, &secChannel_); // Secondary IDE

  priChannel_.init();
  secChannel_.init();

  return 0;
}
