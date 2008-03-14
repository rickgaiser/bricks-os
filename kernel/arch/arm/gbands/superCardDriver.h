#ifndef SUPERCARDDRIVER_H
#define SUPERCARDDRIVER_H


#include "kernel/fileSystem.h"
#include "kernel/ata.h"
#include "inttypes.h"


class CSuperCardDriver
 : public IBlockDevice
{
public:
  CSuperCardDriver();
  virtual ~CSuperCardDriver();

  virtual int init();
  virtual bool inserted();

  virtual int read (uint32_t startSector, uint32_t sectorCount,       void * data);
  virtual int write(uint32_t startSector, uint32_t sectorCount, const void * data);

private:
  void changeMode(uint8_t mode);

  SATARegisters regs_;
};


#endif
