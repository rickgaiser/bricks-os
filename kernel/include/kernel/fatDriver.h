#ifndef FATDRIVER_H
#define FATDRIVER_H


#include "fileSystem.h"


class CFATDriver
 : public IFileSystemDriver
{
public:
  CFATDriver();
  virtual ~CFATDriver();

  virtual bool init(IBlockDevice * device);
};


#endif
