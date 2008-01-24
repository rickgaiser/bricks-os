#ifndef I386_ATA_H
#define I386_ATA_H


#include "inttypes.h"
#include "kernel/fileSystem.h"
#include "kernel/interrupt.h"


class CATADriver
 : public IBlockDevice
 , public IInterruptServiceRoutine
{
public:
  CATADriver(uint32_t iobase, bool master);
  virtual ~CATADriver();

  virtual int init();

  // Inherited from IBlockDevice
  virtual int read (uint32_t startSector, uint32_t sectorCount,       void * data);
  virtual int write(uint32_t startSector, uint32_t sectorCount, const void * data);

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

private:
  uint32_t iIOBase_;
  uint8_t iMaster_;
};


#endif
