#ifndef PARTITION_H
#define PARTITION_H


#include "kernel/fileManager.h"


class CPartition
 : public IBlockDevice
{
public:
  CPartition(IBlockDevice * pPartition);
  virtual ~CPartition();

  virtual int init();

  virtual int read (uint32_t startSector, uint32_t sectorCount,       void * data);
  virtual int write(uint32_t startSector, uint32_t sectorCount, const void * data);

private:
  IBlockDevice * pBlock_;
  uint32_t iStartSector_;
  uint32_t iSectorCount_;
};


#endif
