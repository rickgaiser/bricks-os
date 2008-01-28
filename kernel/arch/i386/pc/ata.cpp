#include "ata.h"
#include "hal.h"
#include "asm/cpu.h"
#include "kernel/debug.h"


// -----------------------------------------------------------------------------
#define LE16(x)  (((x<<8)&0xff00)|((x>>8)&0x00ff))
#define LE32(x)  (((x<<24)&0xff000000)|((x<<8)&0x00ff0000)|((x>>8)&0x0000ff00)|((x>>24)&0x000000ff))
//#define LE16(x)  (x)
//#define LE32(x)  (x)

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

#define ATA_COMMAND_READ_SECTORS   0x20
#define ATA_COMMAND_IDENTIFY_DRIVE 0xec

#pragma pack(1)
struct ata_identify_device
{
  unsigned short words000_009[10];
  unsigned char  serial_no[20];
  unsigned short words020_022[3];
  unsigned char  fw_rev[8];
  unsigned char  model[40];
  unsigned short words047_079[33];
  unsigned short major_rev_num;
  unsigned short minor_rev_num;
  unsigned short command_set_1;
  unsigned short command_set_2;
  unsigned short command_set_extension;
  unsigned short cfs_enable_1;
  unsigned short word086;
  unsigned short csf_default;
  unsigned short words088_255[168];
};
#pragma pack()


// -----------------------------------------------------------------------------
CATADriver::CATADriver(uint32_t iobase, bool master)
 : iIOBase_(iobase)
 , iMaster_(master ? ATA_DRV_HEAD_MASTER : ATA_DRV_HEAD_SLAVE)
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
  ata_identify_device data;

  //printk("CATADriver::init: waiting for busy\n");
  // Wait untill device is no longer busy
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_BSY) != 0);

  // Disable interrupts
  unsigned long flags = local_save_flags();
  local_irq_disable();

  //printk("CATADriver::init: waiting for ready\n");
  // Wait untill device is ready
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_DRDY) == 0);

  // Select master/slave drive
  outb(iMaster_, iIOBase_ + EARO_DRV_HEAD);

  // Send "identify drive" command
  outb(ATA_COMMAND_IDENTIFY_DRIVE, iIOBase_ + EARO_COMMAND);

  //printk("CATADriver::init: waiting for data\n");
  // Wait for data
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_DRQ) == 0);

  //printk("CATADriver::init: transferring data...\n");
  // Transfer data
  uint16_t temp;
  for(int i(0); i < 256; i++)
  {
    temp = inw(iIOBase_ + EARO_DATA);
    temp = LE16(temp);
    ((uint16_t *)&data)[i] = temp;
  }

  // Restore interrupts
  local_irq_restore(flags);

  //data.serial_no[19] = 0;
  //printk("HDD Serial: %s\n", data.serial_no);
  //data.fw_rev[7] = 0;
  //printk("HDD Firmware: %s\n", data.fw_rev);
  data.model[39] = 0;
  printk("ATA HDD Detected: %s\n", data.model);

  CFileSystem::addBlockDevice(this);

  return 0;
}

// -----------------------------------------------------------------------------
int
CATADriver::read(uint32_t startSector, uint32_t sectorCount, void * data)
{
  //printk("CATADriver::read(%d, %d)\n", startSector, sectorCount);

  //printk("CATADriver::init: waiting for busy\n");
  // Wait untill device is no longer busy
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_BSY) != 0);

  // Disable interrupts
  unsigned long flags = local_save_flags();
  local_irq_disable();

  //printk("CATADriver::init: waiting for ready\n");
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

  //printk("CATADriver::init: waiting for data\n");
  // Wait for data
  while((inb(iIOBase_ + EARO_STATUS) & ATA_STATUS_DRQ) == 0);

  //printk("CATADriver::init: transferring data...\n");
  // Transfer data
  uint16_t temp;
  for(unsigned int i(0); i < (sectorCount * 256); i++)
  {
    temp = inw(iIOBase_ + EARO_DATA);
    //temp = LE16(temp);
    ((uint16_t *)data)[i] = temp;
  }

  // Restore interrupts
  local_irq_restore(flags);

  return 0;
}

// -----------------------------------------------------------------------------
int
CATADriver::write(uint32_t startSector, uint32_t sectorCount, const void * data)
{
  printk("CATADriver::write(%d, %d)\n", startSector, sectorCount);

  return -1;
}

// -----------------------------------------------------------------------------
int
CATADriver::isr(int irq)
{
  return 0;
}
