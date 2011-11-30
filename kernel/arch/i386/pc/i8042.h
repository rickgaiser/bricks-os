/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#ifndef I8042_H
#define I8042_H


#include "kernel/interrupt.h"
#include "stdint.h"


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
#define I8042_NUM_PORTS       (I8042_NUM_MUX_PORTS + 2)


class I8042CallBack
{
public:
  virtual ~I8042CallBack(){}

  virtual void i8042_callBack(uint8_t data) = 0;
};

// -----------------------------------------------------------------------------
class C8042
 : public IInterruptServiceRoutine
{
public:
  C8042();
  ~C8042();

  int  init();

  bool registerHandler(uint8_t portNr, I8042CallBack * handler);
  bool writeData(uint8_t port, uint8_t data);
  bool readData(uint8_t port, uint8_t * data);

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

private:
  bool enablePort(uint8_t portNr);
  bool enableKeyboardPort();
  bool enableAuxPort();
  bool enableMuxPorts();
  bool setMuxMode(unsigned int mode, unsigned char * mux_version);
  bool command(uint8_t * param, int command);
  void flush();

  bool writeKeyboard(uint8_t data);
  bool writeAux(uint8_t data);
  bool writeMux(uint8_t port, uint8_t data);
  bool readDataW(uint8_t * data);

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
  uint8_t iPortCount_;
  bool bPortEnabled_[I8042_NUM_PORTS];

  // Callback handlers
  I8042CallBack * pHandler_[I8042_NUM_PORTS];
};


#endif
