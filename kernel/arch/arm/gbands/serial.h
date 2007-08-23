#ifndef GBA_SERIAL_H
#define GBA_SERIAL_H


#include "asm/arch/registers.h"
#include "kernel/interrupt.h"
#include "kernel/fs.h"
#include "unistd.h"


class CGBASerial
 : public IInterruptServiceRoutine
 , public IFileIO
{
public:
  CGBASerial();
  virtual ~CGBASerial();

  virtual int init(ESerialMode mode, bool master);

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

  // Inherited from IFileIO
  virtual ssize_t write(const void * buffer, size_t size, loff_t * = 0);

private:
  ESerialMode eMode_;
  bool bMaster_;
};


#endif
