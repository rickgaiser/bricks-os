#ifndef PS2ARCH_H
#define PS2ARCH_H


#include "kernel/arch.h"
#include "ps2Video.h"


class CPS2Arch
 : public arch::IArch
{
public:
  CPS2Arch();
  virtual ~CPS2Arch();

  virtual int init();

private:
  IFileIO           cDummy_;
  CPS2Video         cVideo_;
};


#endif
