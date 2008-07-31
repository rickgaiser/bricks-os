#ifndef DSIPC_H
#define DSIPC_H


#include "asm/arch/memory.h"
#include "kernel/fs.h"
#include "kernel/interrupt.h"


class CDSIPC
 : public IInterruptServiceRoutine
 , public CAFileIOBufferedRead
{
public:
  CDSIPC();
  virtual ~CDSIPC();

  int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;

  // Inherited from IFileIO
  virtual ssize_t write(const void * buffer, size_t size, bool block = false);
};


#endif
