#ifndef DS9ARCH_H
#define DS9ARCH_H


#include "kernel/arch.h"
#include "ds9Video.h"
#include "ds9IRQ.h"
#include "dsKeyboard.h"
//#include "dsCart.h"
//#include "dsTimer.h"
#include "dsIPC.h"


class CDS9Arch
 : public arch::IArch
{
public:
  CDS9Arch();
  virtual ~CDS9Arch();

  virtual int init();

private:
  CDS9Video         cDS9Video_;
  CDS9IRQ           cDSIRQ_;
  CDSKeyboard       cDSKeyboard_;
//  CDSCart           cDSCart_;
//  CDSTimerManager   cDSTimerManager_;
  CDSIPC            cDSIPC_;
};


#endif
