#include "partition.h"
#include "iostream"


#define ENDIANSWAP16(x)  (((x<<8)&0xff00)|((x>>8)&0x00ff))
#define ENDIANSWAP32(x)  (((x<<24)&0xff000000)|((x<<8)&0x00ff0000)|((x>>8)&0x0000ff00)|((x>>24)&0x000000ff))
//#define LE16(x)  ENDIANSWAP16(x)
//#define LE32(x)  ENDIANSWAP32(x)
#define LE16(x)  (x)
#define LE32(x)  (x)


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

struct SPartitionTable
{
  SPartitionTableRecord record[4];
} __attribute__ ((__packed__));


// -----------------------------------------------------------------------------
CPartition::CPartition(IBlockDevice * pPartition)
 : pBlock_(pPartition)
 , iStartSector_(0)
 , iSectorCount_(0)
{
}

// -----------------------------------------------------------------------------
CPartition::~CPartition()
{
}

// -----------------------------------------------------------------------------
int
CPartition::init()
{
  uint8_t data[512];

//  std::cout<<"Partition initialization:"<<std::endl;

  if(pBlock_->read(0, 1, data) == true)
  {
//    std::cout<<" - Sector read OK"<<std::endl;
    if((data[0x1FE] == 0x55) && (data[0x1FF] == 0xAA))
    {
      SPartitionTable * pTable = (SPartitionTable *)(&data[0x1BE]);

//      std::cout<<" - Boot signature OK"<<std::endl;

      for(int i(0); i < 1; i++)
      {
        if((pTable->record[i].flag == 0x80) || (pTable->record[i].flag == 0x00))
        {
/*
          // Partition found
          std::cout<<" - Partition "<<i+1<<" ok"<<std::endl;
          std::cout<<"   - type:   ";
          switch(pTable->record[i].type)
          {
            case 0x01:
              std::cout<<"FAT12"<<std::endl;
              break;
            case 0x04:
            case 0x06:
            case 0x0e: // FAT16, LBA Mapped
              std::cout<<"FAT16"<<std::endl;
              break;
            case 0x0b: // FAT32
            case 0x0c: // FAT32, LBA Mapped
              std::cout<<"FAT32"<<std::endl;
              break;
            default:
              std::cout<<pTable->record[i].type<<std::endl;
          };
          std::cout<<"   - offset: "<<LE32(pTable->record[i].startSector)<<" sect."<<std::endl;
          std::cout<<"   - size:   "<<LE32(pTable->record[i].sectorCount) / 2048<<" MiB"<<std::endl;
*/
          iStartSector_ = LE32(pTable->record[i].startSector);
          iSectorCount_ = LE32(pTable->record[i].sectorCount);
        }
      }
    }
    else
      std::cout<<" - ERROR: Invalid boot signature"<<std::endl;
  }
  else
    std::cout<<" - ERROR: Unable to read sector"<<std::endl;


  return 0;
}

// -----------------------------------------------------------------------------
int
CPartition::read(uint32_t startSector, uint32_t sectorCount, void * data)
{
  if(iSectorCount_ != 0)
    return pBlock_->read(startSector + iStartSector_, sectorCount, data);
  else
    return -1;
}

// -----------------------------------------------------------------------------
int
CPartition::write(uint32_t startSector, uint32_t sectorCount, const void * data)
{
  if(iSectorCount_ != 0)
    return pBlock_->write(startSector + iStartSector_, sectorCount, data);
  else
    return -1;
}
