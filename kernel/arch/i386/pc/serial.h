// Classes for handling Intel 8250 and compatible UART's
#ifndef ARCH_SERIAL_H
#define ARCH_SERIAL_H


#include "kernel/fs.h"
#include "kernel/interrupt.h"
#include "sys/types.h"


// -----------------------------------------------------------------------------
class CI8250
 : public IFileIO
 , public IInterruptServiceRoutine
{
public:
  CI8250(unsigned int baseAddr);
  virtual ~CI8250();

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

  // Inherited from IFileIO
  virtual ssize_t read (      void * buffer, size_t size, loff_t * = 0);
  virtual ssize_t write(const void * buffer, size_t size, loff_t * = 0);

private:
  unsigned int iBaseAddr_;
};

// -----------------------------------------------------------------------------
class CI386Serial
{
public:
  CI386Serial();
  virtual ~CI386Serial();

  virtual int init();

private:
  CI8250 cCom1_;
  CI8250 cCom2_;
  CI8250 cCom3_;
  CI8250 cCom4_;
};


#endif
