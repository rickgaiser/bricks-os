#include "superCardDriver.h"
#include "kernel/debug.h"


#define inb(addr)        (*(vuint16_t *)(addr))
#define inw(addr)        (*(vuint16_t *)(addr))
#define outb(data,addr) ((*(vuint16_t *)(addr)) = data)
#define outw(data,addr) ((*(vuint16_t *)(addr)) = data)

// Values for changing mode
#define SC_MODE_RAM    0x5
#define SC_MODE_MEDIA  0x3
#define SC_MODE_RAM_RO 0x1

// SC CF Addresses
#define REG_SCCF_DATA       ((vuint16_t*)0x09000000) // Pointer to buffer of CF data transered from card
#define REG_SCCF_ERR        ((vuint16_t*)0x09020000) // Errors / Features
#define REG_SCCF_SEC        ((vuint16_t*)0x09040000) // Number of sector to transfer
#define REG_SCCF_LBA1       ((vuint16_t*)0x09060000) // 1st byte of sector address
#define REG_SCCF_LBA2       ((vuint16_t*)0x09080000) // 2nd byte of sector address
#define REG_SCCF_LBA3       ((vuint16_t*)0x090A0000) // 3rd byte of sector address
#define REG_SCCF_LBA4       ((vuint16_t*)0x090C0000) // last nibble of sector address | 0xE0
#define REG_SCCF_CMD        ((vuint16_t*)0x090E0000) // Commands sent to control chip and status return
#define REG_SCCF_STS        ((vuint16_t*)0x098C0000) // Status of the CF Card / Device control

// CF Card status
#define CF_STS_INSERTED     0x50
#define CF_STS_REMOVED      0x00
#define CF_STS_READY        0x58

// CF Card commands
#define CF_CMD_LBA          0xE0

#define CF_CARD_TIMEOUT 10000000


const SATARegisters supercardRegs =
{
  (uint32_t)REG_SCCF_DATA,
  {(uint32_t)REG_SCCF_ERR},
  (uint32_t)REG_SCCF_SEC,
  (uint32_t)REG_SCCF_LBA1,
  (uint32_t)REG_SCCF_LBA2,
  (uint32_t)REG_SCCF_LBA3,
  (uint32_t)REG_SCCF_LBA4,
  {(uint32_t)REG_SCCF_CMD},
  {(uint32_t)REG_SCCF_STS},
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
  uint16_t temp = inw(regs_.lbaLow);
  outw(~temp & 0xff, regs_.lbaLow);
  temp = ~temp & 0xff;
  if(inw(regs_.lbaLow) != temp)
    return -1;

  // Make sure it is 8 bit
  outw(0xaa55, regs_.lbaLow);
  if(inw(regs_.lbaLow) == 0xaa55)
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
  outw(CF_STS_INSERTED, regs_.devControl);
  return ((inw(regs_.altStatus) & 0xff) == CF_STS_INSERTED);
}

// -----------------------------------------------------------------------------
int
CSuperCardDriver::read(uint32_t startSector, uint32_t sectorCount, void * data)
{
  int i;

  uint16_t * buff = (uint16_t *)data;

  // Wait until CF card is finished previous commands
  i=0;
  while((inw(regs_.status) & ATA_STATUS_BSY) && (i < CF_CARD_TIMEOUT))
    i++;
  if(i >= CF_CARD_TIMEOUT)
    return -1;

  // Wait until card is ready for commands
  i = 0;
  while((!(inw(regs_.altStatus) & CF_STS_INSERTED)) && (i < CF_CARD_TIMEOUT))
    i++;
  if(i >= CF_CARD_TIMEOUT)
    return -1;

  // Set number of sectors to read
  outw((sectorCount < 256) ? sectorCount : 0, regs_.sectorCount); // Read a maximum of 256 sectors, 0 means 256

  // Set read sector
  outw(((startSector      ) & 0xff),             regs_.lbaLow);  // 1st byte of sector number
  outw(((startSector >>  8) & 0xff),             regs_.lbaMid);  // 2nd byte of sector number
  outw(((startSector >> 16) & 0xff),             regs_.lbaHigh); // 3rd byte of sector number
  outw(((startSector >> 24) & 0x0f)| CF_CMD_LBA, regs_.device);  // last nibble of sector number

  // Set command to read
  outw(ATA_COMMAND_READ_SECTORS, regs_.command);

  for(unsigned int iSector(0); iSector < sectorCount; iSector++)
  {
    // Wait until card is ready for reading
    i = 0;
    while(((inw(regs_.altStatus) & 0xff)!= CF_STS_READY) && (i < CF_CARD_TIMEOUT))
      i++;
    if(i >= CF_CARD_TIMEOUT)
      return -1;

    // Read data
    i=256;
    while(i--)
      *buff++ = inw(regs_.data);
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
