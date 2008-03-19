#ifndef GBA_SERIAL_H
#define GBA_SERIAL_H


// Normal mode:
//  - Connect 2 GBA's
//  - Speed of 256Kbit/s (or 2Mbit/s for directly connected expansions)
//  - Send 8/32 bits at a time and get 8/32 bits returned (start_bit -> 0 + irq)
// Multiplayer mode:
//  - Connect 2-4 GBA's
//  - Speed of 9600/38400/57600/115200 bit/s
//  - Send 16 bits at a time and get 16 bits returned (from up to 3 GBA's)
// UART mode:
//  - Connect 2 GBA's
//  - Speed of 9600/38400/57600/115200 bit/s
//  - Send 8 bits at a time
//  - Fifo can hold 4 bytes


#include "asm/arch/registers.h"
#include "asm/arch/memory.h"
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

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) IWRAM_CODE;

  // Inherited from IFileIO (UART mode only)
  virtual ssize_t write(const void * buffer, size_t size, loff_t * = 0);

  int setMode(ESerialMode mode);
  int connectNormal(bool master);       // NORMAL mode
  int connectMulti();                   // MULTI  mode

  bool isMaster();                      // NORMAL & MULTI mode

  uint8_t  sendNormal8 (uint8_t  data); // NORMAL mode ( 8bit)
  uint32_t sendNormal32(uint32_t data); // NORMAL mode (32bit)
  uint16_t sendMulti   (uint16_t data); // MULTI  mode (16bit)

  int multiBoot();

private:
  bool bInitialized_;
  bool bModeSet_;
  bool bConnected_;                     // NORMAL & MULTI mode
  bool bMaster_;                        // NORMAL & MULTI mode
  uint8_t iID_;                         // MULTI mode

  ESerialMode eMode_;

  volatile bool bReceived_;
  uint32_t rcvData_;
};


#endif
