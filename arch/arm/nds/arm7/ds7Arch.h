#ifndef DS7ARCH_H
#define DS7ARCH_H


#include "kernel/arch.h"
#include "ds7IRQ.h"
#include "dsKeyboard.h"
#include "dsIPC.h"


class CDS7Arch
 : public arch::IArch
{
public:
  CDS7Arch();
  virtual ~CDS7Arch();

  virtual int init();

private:
  CDS7IRQ           cIRQ_;
  CDSKeyboard       cKeyboard_;
  CDSIPC            cIPC_;
};


#endif
