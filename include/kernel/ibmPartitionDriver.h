#ifndef IBMPARTITIONDRIVER_H
#define IBMPARTITIONDRIVER_H


#include "fileSystem.h"


class CIBMPartitionDriver
 : public IPartitionDriver
{
public:
  CIBMPartitionDriver();
  virtual ~CIBMPartitionDriver();

  virtual bool init(IBlockDevice * device);
};


#endif
