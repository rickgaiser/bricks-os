#ifndef GBA_SERIAL_H
#define GBA_SERIAL_H


#include "kernel/interrupt.h"
#include "kernel/file.h"
#include "unistd.h"


class CGBASerial
 : public IInterruptServiceRoutine
 , public IFileIO
{
public:
  CGBASerial();
  virtual ~CGBASerial();

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

  // Inherited from IFileIO
  virtual int write(const void * data, size_t size);

private:
};


#endif
