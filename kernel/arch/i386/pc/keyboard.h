#ifndef I386KEYBOARD_H
#define I386KEYBOARD_H


#include "kernel/fs.h"
#include "kernel/interrupt.h"


#define KBD_DATA_REG       0x60
#define KBD_ACK_REG        0x61
#define KBD_COMMAND_REG    0x64
#define KBD_STATUS_REG     0x64

#define KBD_LEDS           0xED
#define KBD_DIAGNOSTICS    0xEE
#define KBD_SELECT_SCANSET 0xF0
#define KBD_IDENTIFY       0xF2
#define KBD_RATE           0xF3


class CI386Keyboard
 : public IFileIO
 , public IInterruptServiceRoutine
{
public:
  CI386Keyboard();
  virtual ~CI386Keyboard();

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

  // Inherited from IFileIO
  virtual int read(void * data, size_t size);

private:
  void outb(unsigned char data, unsigned short addr);
  void updateLeds();

  bool bE0_;
  bool bDown_;
  bool bShift_;
  bool bCtrl_;
  bool bAlt_;
  bool bCaps_;
  unsigned char iLeds_;
};


#endif
