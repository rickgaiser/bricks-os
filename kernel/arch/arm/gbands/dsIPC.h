#ifndef DSIPC_H
#define DSIPC_H


#include "asm/arch/memory.h"
#include "kernel/fs.h"
#include "kernel/interrupt.h"
#include "kernel/ringBuffer.h"


class CDSIPC
 : public IInterruptServiceRoutine
 , public IFileIO
{
public:
  CDSIPC();
  virtual ~CDSIPC();

  int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;

  // Inherited from IFileIO
  virtual ssize_t read (      void * buffer, size_t size, bool block = false);
  virtual ssize_t write(const void * buffer, size_t size, bool block = false);

private:
  CRingBuffer buffer_;
};


#endif
