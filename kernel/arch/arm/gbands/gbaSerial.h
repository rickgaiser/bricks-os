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
 , public CAFileIOBufferedRead
{
public:
  CGBASerial();
  virtual ~CGBASerial();

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;

  // Inherited from IFileIO (UART mode only)
  virtual ssize_t write(const void * buffer, size_t size, bool block = false);

  int setMode(ESerialMode mode);
  int connectNormal(bool master);       // NORMAL mode
  int connectMulti();                   // MULTI  mode

  bool isMaster();                      // NORMAL & MULTI mode

  uint8_t  sendNormal8 (uint8_t  data); // NORMAL mode ( 8bit)
  uint32_t sendNormal32(uint32_t data); // NORMAL mode (32bit)
  void     sendMulti   (uint16_t data, uint16_t * slave0, uint16_t * slave1, uint16_t * slave2, uint16_t * slave3);

  uint8_t getBootMode();
  uint8_t getDeviceID();
  uint8_t locateMultiBootSlaves();
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
