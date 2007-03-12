#ifndef DSIPC_H
#define DSIPC_H


#include "kernel/interrupt.h"
#include "kernel/fs.h"


class CDSIPC
 : public IInterruptServiceRoutine
 , public IFileIO
{
public:
  CDSIPC();
  virtual ~CDSIPC();

  int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

  // Inherited from IFileIO
  virtual ssize_t read (      void * buffer, size_t size, loff_t * = 0);
  virtual ssize_t write(const void * buffer, size_t size, loff_t * = 0);

private:
  volatile int iBufferCount_;
  int iKey_;
};


#endif
