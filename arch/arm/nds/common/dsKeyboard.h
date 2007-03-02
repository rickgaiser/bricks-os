#ifndef DSKEYBOARD_H
#define DSKEYBOARD_H


#include "kernel/fs.h"
#include "kernel/interrupt.h"


class CDSKeyboard
 : public IFileIO
 , public IInterruptServiceRoutine
{
public:
  CDSKeyboard();
  virtual ~CDSKeyboard();

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

  // Inherited from IDriver
  virtual ssize_t read(void * buffer, size_t size, loff_t * = 0);

private:
  volatile int iBufferCount_;
  int iKey_;
};


#endif
