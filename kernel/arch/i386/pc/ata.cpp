#include "ata.h"
#include "asm/hal.h"
#include "asm/cpu.h"
#include "kernel/debug.h"
#include "kernel/endian.h"
#include "kernel/interruptManager.h"
#include "stddef.h"


// -----------------------------------------------------------------------------
// Default IBM-PC compatible base addresses:
//   Primary:   0x1F0 - 0x1F7 and 0x3F6 - 0x3F7
//   Secondary: 0x170 - 0x177 and 0x376 - 0x377
enum EATARegisterOffsets
{
  EARO_DATA       = 0x000, // 0x1F0 (Read and Write): Data Register
  EARO_ERROR      = 0x001, // 0x1F1 (Read): Error Register
  EARO_FEATURES   = 0x001, // 0x1F1 (Write): Features Register
  EARO_SECTOR_CNT = 0x002, // 0x1F2 (Read and Write): Sector Count Register
  EARO_LBA_LOW    = 0x003, // 0x1F3 (Read and Write): LBA Low Register
  EARO_LBA_MID    = 0x004, // 0x1F4 (Read and Write): LBA Mid Register
  EARO_LBA_HIGH   = 0x005, // 0x1F5 (Read and Write): LBA High Register
  EARO_DRV_HEAD   = 0x006, // 0x1F6 (Read and Write): Drive/Head Register
  EARO_STATUS     = 0x007, // 0x1F7 (Read): Status Register
  EARO_COMMAND    = 0x007, // 0x1F7 (Write): Command Register
  EARO_ALT_STATUS = 0x206, // 0x3F6 (Read): Alternate Status Register
  EARO_DEV_CTRL   = 0x206  // 0x3F6 (Write): Device Control Register
};

// Status register bits (EARO_STATUS)
#define ATA_STATUS_BSY  0x80 // busy
#define ATA_STATUS_DRDY 0x40 // device ready
#define ATA_STATUS_DF   0x20 // Device Fault
#define ATA_STATUS_DSC  0x10 // seek complete
#define ATA_STATUS_DRQ  0x08 // Data Transfer Requested
#define ATA_STATUS_CORR 0x04 // data corrected
#define ATA_STATUS_IDX  0x02 // index mark
#define ATA_STATUS_ERR  0x01 // error

// Error register bits (EARO_ERROR)
#define ATA_ERROR_BBK   0x80 // Bad Block
#define ATA_ERROR_UNC   0x40 // Uncorrectable data error
#define ATA_ERROR_MC    0x20 // Media Changed
#define ATA_ERROR_IDNF  0x10 // ID mark Not Found
#define ATA_ERROR_MCR   0x08 // Media Change Requested
#define ATA_ERROR_ABRT  0x04 // command aborted
#define ATA_ERROR_TK0NF 0x02 // Track 0 Not Found
#define ATA_ERROR_AMNF  0x01 // Address Mark Not Found

#define ATA_DRV_HEAD_MASTER 0x00
#define ATA_DRV_HEAD_SLAVE  0x10
#define ATA_DRV_HEAD_LBA    0x40

#define ATA_COMMAND_READ_SECTORS      0x20
#define ATA_COMMAND_DEVICE_DIAGNOSTIC 0x90
#define ATA_COMMAND_IDENTIFY_DRIVE    0xec


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CATADrive::CATADrive(uint32_t iobase, bool master)
 : iIOBase_(iobase)
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
  outb(iMaster_, iIOBase_ + EARO_DRV_HEAD);

  // Wait untill device is no longer busy
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_BSY) != 0);

  // Disable interrupts
  unsigned long flags = local_save_flags();
  local_irq_disable();

  // Wait untill device is ready
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_DRDY) == 0);

  // Send "identify drive" command
  outb(ATA_COMMAND_IDENTIFY_DRIVE, iIOBase_ + EARO_COMMAND);

  // Wait for data
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_DRQ) == 0);

  // Transfer data
  uint16_t temp;
  for(int i(0); i < 256; i++)
  {
    temp = inw(iIOBase_ + EARO_DATA);
    temp = ENDIAN_BE_16(temp);
    ((uint16_t *)&info_)[i] = temp;
  }

  // Restore interrupts
  local_irq_restore(flags);

  //info_.sSerialNumber[19] = 0;
  //printk("HDD Serial: %s\n", info_.sSerialNumber);
  //info_.sFirmwareRevision[7] = 0;
  //printk("HDD Firmware: %s\n", info_.sFirmwareRevision);
  //info_.sModelNumber[39] = 0;
  //printk("ATA HDD Detected: %s\n", info_.sModelNumber);

  return 0;
}

// -----------------------------------------------------------------------------
int
CATADrive::read(uint32_t startSector, uint32_t sectorCount, void * data)
{
  //printk("CATADrive::read(%d, %d)\n", startSector, sectorCount);

  //printk("CATADrive::init: waiting for busy\n");
  // Wait untill device is no longer busy
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_BSY) != 0);

  // Disable interrupts
  unsigned long flags = local_save_flags();
  local_irq_disable();

  //printk("CATADrive::init: waiting for ready\n");
  // Wait untill device is ready
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_DRDY) == 0);

  // Set sector count
  outb(sectorCount, iIOBase_ + EARO_SECTOR_CNT);
  // Set LBA addr
  outb((startSector & 0x000000ff)      , iIOBase_ + EARO_LBA_LOW);
  outb((startSector & 0x0000ff00) >>  8, iIOBase_ + EARO_LBA_MID);
  outb((startSector & 0x00ff0000) >> 16, iIOBase_ + EARO_LBA_HIGH);
  // Set device & LBA mode
  outb(iMaster_ | ATA_DRV_HEAD_LBA, iIOBase_ + EARO_DRV_HEAD);

  // Send "read sectors" command
  outb(ATA_COMMAND_READ_SECTORS, iIOBase_ + EARO_COMMAND);

  //printk("CATADrive::init: waiting for data\n");
  // Wait for data
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_DRQ) == 0);

  //printk("CATADrive::init: transferring data...\n");
  // Transfer data
  uint16_t temp;
  for(unsigned int i(0); i < (sectorCount * 256); i++)
  {
    temp = inw(iIOBase_ + EARO_DATA);
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
CATAChannel::CATAChannel(uint32_t iobase)
 : iIOBase_(iobase)
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
  outb(ATA_COMMAND_DEVICE_DIAGNOSTIC, iIOBase_ + EARO_COMMAND);

  // Select master
  outb(ATA_DRV_HEAD_MASTER | ATA_DRV_HEAD_LBA, iIOBase_ + EARO_DRV_HEAD);
  // Read error register
  iError = inb(iIOBase_ + EARO_ERROR);

  // Master present?
  if((iError == 0x01) || (iError == 0x81))
    bMasterPresent = true;

  // Slave present?
  if(iError < 0x80)
  {
    // Select slave
    outb(ATA_DRV_HEAD_SLAVE | ATA_DRV_HEAD_LBA, iIOBase_ + EARO_DRV_HEAD);
    // Read error register
    iError = inb(iIOBase_ + EARO_ERROR);

    if(iError == 0x01)
      bSlavePresent = true;
  }

  if(bMasterPresent == true)
  {
    printk("CATAChannel: Master drive detected\n");
    pMaster_ = new CATADrive(iIOBase_, true);
    pMaster_->init();
    CFileSystem::addBlockDevice(pMaster_);
  }

  if(bSlavePresent == true)
  {
    printk("CATAChannel: Slave drive detected\n");
    pSlave_ = new CATADrive(iIOBase_, false);
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
 : priChannel_(0x1f0)
 , secChannel_(0x170)
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
