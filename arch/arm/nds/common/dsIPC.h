#ifndef DSIPC_H
#define DSIPC_H


#include "kernel/interrupt.h"
#include "kernel/file.h"


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
  virtual int read (      void * data, size_t size);
  virtual int write(const void * data, size_t size);
};


#endif
