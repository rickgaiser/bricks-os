#include "kernel/ibmPartitionDriver.h"
#include "iostream"


// -----------------------------------------------------------------------------
struct SPartitionTableRecord
{
  uint8_t  flag;
  uint8_t  legacy1;
  uint8_t  legacy2;
  uint8_t  legacy3;
  uint8_t  type;
  uint8_t  legacy4;
  uint8_t  legacy5;
  uint8_t  legacy6;
  uint32_t startSector;
  uint32_t sectorCount;
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SPartitionTable
{
  SPartitionTableRecord record[4];
} __attribute__ ((__packed__));


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CIBMPartitionDriver::CIBMPartitionDriver()
{
}

// -----------------------------------------------------------------------------
CIBMPartitionDriver::~CIBMPartitionDriver()
{
}

// -----------------------------------------------------------------------------
bool
CIBMPartitionDriver::init(IBlockDevice * device)
{
  uint8_t * data = new uint8_t[512];
  int iPartitionCount(0);
  bool bRetVal(false);

  std::cout<<"CIBMPartitionDriver::init"<<std::endl;

  if(device->read(0, 1, data) == true)
  {
    if((data[0x1FE] == 0x55) && (data[0x1FF] == 0xAA))
    {
      SPartitionTable * pTable = (SPartitionTable *)(&data[0x1BE]);

      for(int i(0); i < 4; i++)
      {
        if((pTable->record[i].flag == 0x80) || (pTable->record[i].flag == 0x00))
        {
          if(pTable->record[i].startSector > 0)
          {
            iPartitionCount++;
            bRetVal = true;

            // Add partition to file system
            CFileSystem::addBlockDevice(new CPartition(device, pTable->record[i].startSector, pTable->record[i].sectorCount));
          }
          else
            std::cout<<" - ERROR: Invalid start sector"<<std::endl;
        }
      }
    }
    else
      std::cout<<" - ERROR: Invalid boot signature"<<std::endl;
  }
  else
    std::cout<<" - ERROR: Unable to read sector"<<std::endl;

  std::cout<<iPartitionCount<<" valid partitions located"<<std::endl;

  return bRetVal;
}
