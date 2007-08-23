#include "superCardDriver.h"


// Values for changing mode
#define SC_MODE_RAM    0x5
#define SC_MODE_MEDIA  0x3
#define SC_MODE_RAM_RO 0x1

// SC CF Addresses
#define REG_SCCF_STS        ((vuint16_t*)0x098C0000) // Status of the CF Card / Device control
#define REG_SCCF_CMD        ((vuint16_t*)0x090E0000) // Commands sent to control chip and status return
#define REG_SCCF_ERR        ((vuint16_t*)0x09020000) // Errors / Features

#define REG_SCCF_SEC        ((vuint16_t*)0x09040000) // Number of sector to transfer
#define REG_SCCF_LBA1       ((vuint16_t*)0x09060000) // 1st byte of sector address
#define REG_SCCF_LBA2       ((vuint16_t*)0x09080000) // 2nd byte of sector address
#define REG_SCCF_LBA3       ((vuint16_t*)0x090A0000) // 3rd byte of sector address
#define REG_SCCF_LBA4       ((vuint16_t*)0x090C0000) // last nibble of sector address | 0xE0

#define REG_SCCF_DATA       ((vuint16_t*)0x09000000) // Pointer to buffer of CF data transered from card

// CF Card status
#define CF_STS_INSERTED     0x50
#define CF_STS_REMOVED      0x00
#define CF_STS_READY        0x58

#define CF_STS_DRQ          0x08
#define CF_STS_BUSY         0x80

// CF Card commands
#define CF_CMD_LBA          0xE0
#define CF_CMD_READ         0x20
#define CF_CMD_WRITE        0x30

#define CF_CARD_TIMEOUT 10000000


const CF_REGISTERS _SCCF_Registers =
{
  REG_SCCF_DATA,
  REG_SCCF_STS,
  REG_SCCF_CMD,
  REG_SCCF_ERR,
  REG_SCCF_SEC,
  REG_SCCF_LBA1,
  REG_SCCF_LBA2,
  REG_SCCF_LBA3,
  REG_SCCF_LBA4
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

  cfRegisters_ = _SCCF_Registers;
  // See if there is a read/write register
  uint16_t temp = *(cfRegisters_.lba1);
  *(cfRegisters_.lba1) = (~temp & 0xFF);
  temp = (~temp & 0xFF);
  if(!(*(cfRegisters_.lba1) == temp))
  {
    return false;
  }
  // Make sure it is 8 bit
  *(cfRegisters_.lba1) = 0xAA55;
  if(*(cfRegisters_.lba1) == 0xAA55)
    return -1;

  return 0;
}

// -----------------------------------------------------------------------------
bool
CSuperCardDriver::inserted()
{
  // Change register, then check if value did change
  *(cfRegisters_.status) = CF_STS_INSERTED;
  return ((*(cfRegisters_.status) & 0xff) == CF_STS_INSERTED);
}

// -----------------------------------------------------------------------------
int
CSuperCardDriver::read(uint32_t startSector, uint32_t sectorCount, void * data)
{
  int i;

  uint16_t * buff = (uint16_t *)data;

  // Wait until CF card is finished previous commands
  i=0;
  while((*(cfRegisters_.command) & CF_STS_BUSY) && (i < CF_CARD_TIMEOUT))
    i++;
  if(i >= CF_CARD_TIMEOUT)
    return 0;

  // Wait until card is ready for commands
  i = 0;
  while((!(*(cfRegisters_.status) & CF_STS_INSERTED)) && (i < CF_CARD_TIMEOUT))
    i++;
  if(i >= CF_CARD_TIMEOUT)
    return 0;

  // Set number of sectors to read
  *(cfRegisters_.sectorCount) = (sectorCount < 256 ? sectorCount : 0);	// Read a maximum of 256 sectors, 0 means 256

  // Set read sector
  *(cfRegisters_.lba1) = startSector & 0xFF;						// 1st byte of sector number
  *(cfRegisters_.lba2) = (startSector >> 8) & 0xFF;					// 2nd byte of sector number
  *(cfRegisters_.lba3) = (startSector >> 16) & 0xFF;				// 3rd byte of sector number
  *(cfRegisters_.lba4) = ((startSector >> 24) & 0x0F )| CF_CMD_LBA;	// last nibble of sector number

  // Set command to read
  *(cfRegisters_.command) = CF_CMD_READ;

  for(unsigned int iSector(0); iSector < sectorCount; iSector++)
  {
    // Wait until card is ready for reading
    i = 0;
    while(((*(cfRegisters_.status) & 0xff)!= CF_STS_READY) && (i < CF_CARD_TIMEOUT))
      i++;
    if(i >= CF_CARD_TIMEOUT)
      return 0;

    // Read data
    i=256;
    while(i--)
      *buff++ = *(cfRegisters_.data);
  }

  return sectorCount;
}

// -----------------------------------------------------------------------------
int
CSuperCardDriver::write(uint32_t startSector, uint32_t sectorCount, const void * data)
{
  return 0;
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
