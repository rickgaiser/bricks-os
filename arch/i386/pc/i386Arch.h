#ifndef I386ARCH_H
#define I386ARCH_H


#include "kernel/arch.h"
#include "i386IRQ.h"
#include "i386Video.h"
#include "i386Keyboard.h"
#include "gdt.h"
#include "idt.h"


class CI386Arch
 : public arch::IArch
{
public:
  CI386Arch();
  virtual ~CI386Arch();

  virtual int init();

private:
  CI386IRQ         cIRQ_;
  CI386Video       cVideo_;
  CI386Keyboard    cKeyboard_;
  CGDT             cGDT_;
  CIDT             cIDT_;
};


#endif
