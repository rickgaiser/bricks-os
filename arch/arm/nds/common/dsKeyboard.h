#ifndef DSKEYBOARD_H
#define DSKEYBOARD_H


#include "kernel/file.h"
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
  virtual int read(void * data, size_t size);

private:
  volatile int iBufferCount_;
  int iKey_;
};


#endif
