#ifndef GBAARCH_H
#define GBAARCH_H


#include "kernel/arch.h"
#include "gbaIRQ.h"
#include "gbaVideo.h"
#include "gbaKeyboard.h"


class CGBAArch
 : public arch::IArch
{
public:
  CGBAArch();
  virtual ~CGBAArch();

  virtual int init();

private:
  CGBAIRQ         cIRQ_;
  CGBAVideo       cVideo_;
  CGBAKeyboard    cKeyboard_;
};


#endif
