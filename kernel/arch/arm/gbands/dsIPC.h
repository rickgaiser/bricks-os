#ifndef DSIPC_H
#define DSIPC_H


#include "kernel/interrupt.h"
#include "kernel/fs.h"
#include "asm/arch/memory.h"


class CDSIPC
 : public IInterruptServiceRoutine
 , public IFileIO
{
public:
  CDSIPC();
  virtual ~CDSIPC();

  int init(IFileIO * output = 0);

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;

  // Inherited from IFileIO
  virtual ssize_t read (      void * buffer, size_t size, loff_t * = 0);
  virtual ssize_t write(const void * buffer, size_t size, loff_t * = 0);

private:
  IFileIO * pOutput_;
  volatile int iBufferCount_;
  int iKey_;
};


#endif
