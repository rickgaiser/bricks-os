#ifndef FAT_H
#define FAT_H


#include "kernel/fileManager.h"


class CFAT
 : public IFileSystemDriver
{
public:
  CFAT(IBlockDevice * pPartition);
  virtual ~CFAT();

  int init();

private:
  IBlockDevice * pPartition_;
};

class CFATFactory
 : public IFileSystemDriverFactory
{
public:
  CFATFactory();
  virtual ~CFATFactory();

  virtual bool check(IBlockDevice * partition);
  virtual IFileSystemDriver * newFileSystemDriver(IBlockDevice * partition);

private:
};


#endif
