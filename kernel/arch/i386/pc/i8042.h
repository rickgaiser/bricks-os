#ifndef I8042_H
#define I8042_H


#include "kernel/interrupt.h"
#include "inttypes.h"


#define I8042_DATA_REG        0x60
#define I8042_COMMAND_REG     0x64
#define I8042_STATUS_REG      0x64

// Status register bits
#define I8042_STR_PARITY      0x80
#define I8042_STR_TIMEOUT     0x40
#define I8042_STR_AUXDATA     0x20
#define I8042_STR_KEYLOCK     0x10
#define I8042_STR_CMDDAT      0x08
#define I8042_STR_MUXERR      0x04
#define I8042_STR_IBF         0x02
#define I8042_STR_OBF         0x01

// Control register bits.
#define I8042_CTR_KBDINT      0x01
#define I8042_CTR_AUXINT      0x02
#define I8042_CTR_IGNKEYLOCK  0x08
#define I8042_CTR_KBDDIS      0x10
#define I8042_CTR_AUXDIS      0x20
#define I8042_CTR_XLATE       0x40

// Commands.
#define I8042_CMD_CTL_RCTR    0x0120
#define I8042_CMD_CTL_WCTR    0x1060
#define I8042_CMD_CTL_TEST    0x01aa
#define I8042_CMD_KBD_DISABLE 0x00ad
#define I8042_CMD_KBD_ENABLE  0x00ae
#define I8042_CMD_KBD_TEST    0x01ab
#define I8042_CMD_KBD_LOOP    0x11d2
#define I8042_CMD_AUX_DISABLE 0x00a7
#define I8042_CMD_AUX_ENABLE  0x00a8
#define I8042_CMD_AUX_TEST    0x01a9
#define I8042_CMD_AUX_SEND    0x10d4
#define I8042_CMD_AUX_LOOP    0x11d3
#define I8042_CMD_MUX_PFX     0x0090
#define I8042_CMD_MUX_SEND    0x1090

#define I8042_BUFFER_SIZE     16
#define I8042_CTL_TIMEOUT     500000
#define I8042_NUM_MUX_PORTS   4


// -----------------------------------------------------------------------------
class C8042
 : public IInterruptServiceRoutine
{
public:
  C8042();
  ~C8042();

  int  init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

private:
  bool enableKeyboardPort();
  bool enableAuxPort();
  bool enableMuxPorts();
  bool setMuxMode(unsigned int mode, unsigned char * mux_version);
  bool command(uint8_t * param, int command);
  void flush();

  bool writeKeyboard(uint8_t data);
  bool writeAux(uint8_t data);
  bool writeMux(uint8_t port, uint8_t data);
  bool waitRead();
  bool waitWrite();
  uint8_t readData();
  uint8_t readStatus();
  void writeData(uint8_t data);
  void writeCommand(uint8_t cmd);

private:
  uint8_t regCTR_;
  bool bMuxPresent_;
  uint8_t muxVersion_;

  // Mouse
  uint8_t iMouseData_[3];
  uint8_t iMouseByteNr_;
};


#endif
