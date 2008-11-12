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


#include "i8042.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/hal.h"
#include "stddef.h"


#define I8042_KBD_PORT_NO 0
#define I8042_AUX_PORT_NO 1
#define I8042_MUX_PORT_NO 2


// -----------------------------------------------------------------------------
C8042::C8042()
 : bMuxPresent_(false)
 , muxVersion_(0)
 , iPortCount_(I8042_NUM_PORTS)
{
  for(int i(0); i < iPortCount_; i++)
  {
    bPortEnabled_[i] = false;
    pHandler_[i]     = NULL;
  }
}

// -----------------------------------------------------------------------------
C8042::~C8042()
{
}

// -----------------------------------------------------------------------------
int
C8042::init()
{
  CInterruptManager::attach(0x21, this); // Keyboard
  CInterruptManager::attach(0x2c, this); // Aux

  // Read CTR register
  if(command(&regCTR_, I8042_CMD_CTL_RCTR) == false)
  {
    printk("C8042: Unable to read CTR register\n");
    return -1;
  }

  // Disable keyboard interface and interrupt
  regCTR_ |=  I8042_CTR_KBDDIS;
  regCTR_ &= ~I8042_CTR_KBDINT;

  // Disable aux interface and interrupt
  regCTR_ |=  I8042_CTR_AUXDIS;
  regCTR_ &= ~I8042_CTR_AUXINT;

  // Write CTR register
  if(command(&regCTR_, I8042_CMD_CTL_WCTR) == false)
  {
    printk("C8042: Unable to write CTR register\n");
    return -1;
  }

  // Detect MUX
  if(setMuxMode(1, &muxVersion_) == 0)
  {
    if(muxVersion_ != 0xac) // Don't use MUX v10.12
    {
      printk("C8042: MUX Detected, v%d.%d\n", muxVersion_ >> 4, muxVersion_ & 0xf);
      //bMuxPresent_ = true;
    }
  }

  // Number of ports
  if(bMuxPresent_ == true)
    iPortCount_ = I8042_NUM_PORTS;
  else
    iPortCount_ = 2;

  return 0;
}

// -----------------------------------------------------------------------------
bool
C8042::registerHandler(uint8_t portNr, I8042CallBack * handler)
{
  bool bRetVal(false);

  if(portNr < iPortCount_)
  {
    if(pHandler_[portNr] == NULL)
    {
      if(enablePort(portNr) == true)
      {
        pHandler_[portNr] = handler;
        bRetVal = true;
      }
    }
  }

  return bRetVal;
}

// -----------------------------------------------------------------------------
bool
C8042::writeData(uint8_t port, uint8_t data)
{
  switch(port)
  {
    case I8042_KBD_PORT_NO:     return writeKeyboard(data);
    case I8042_AUX_PORT_NO:     return writeAux(     data);
    case I8042_MUX_PORT_NO + 0: return writeMux(0,   data);
    case I8042_MUX_PORT_NO + 1: return writeMux(1,   data);
    case I8042_MUX_PORT_NO + 2: return writeMux(2,   data);
    case I8042_MUX_PORT_NO + 3: return writeMux(3,   data);
  };

  return false;
}

// -----------------------------------------------------------------------------
bool
C8042::readData(uint8_t port, uint8_t * data)
{
  switch(port)
  {
    case I8042_KBD_PORT_NO:     return readDataW(data);
    case I8042_AUX_PORT_NO:     return readDataW(data);
    case I8042_MUX_PORT_NO + 0: return readDataW(data);
    case I8042_MUX_PORT_NO + 1: return readDataW(data);
    case I8042_MUX_PORT_NO + 2: return readDataW(data);
    case I8042_MUX_PORT_NO + 3: return readDataW(data);
  };

  return false;
}

// -----------------------------------------------------------------------------
int
C8042::isr(int irq)
{
  uint8_t status = readStatus();
  uint8_t data   = readData();
  uint8_t portNr;

  if(status & I8042_STR_AUXDATA)
  {
    if(bMuxPresent_ == true)
      portNr = I8042_MUX_PORT_NO + ((status >> 6) & 3);
    else
      portNr = I8042_AUX_PORT_NO;
  }
  else
    portNr = I8042_KBD_PORT_NO;

  if(pHandler_[portNr] != NULL)
    pHandler_[portNr]->i8042_callBack(data);

  return 0;
}

// -----------------------------------------------------------------------------
bool
C8042::enablePort(uint8_t portNr)
{
  if(portNr == I8042_KBD_PORT_NO)
    return enableKeyboardPort();
  else if(portNr == I8042_AUX_PORT_NO)
    return enableAuxPort();
  else
    return enableMuxPorts();
}

// -----------------------------------------------------------------------------
bool
C8042::enableKeyboardPort()
{
  if(bPortEnabled_[I8042_KBD_PORT_NO] == false)
  {
    regCTR_ &= ~I8042_CTR_KBDDIS;
    regCTR_ |=  I8042_CTR_KBDINT;

    bPortEnabled_[I8042_KBD_PORT_NO] = command(&regCTR_, I8042_CMD_CTL_WCTR);
  }

  return bPortEnabled_[I8042_KBD_PORT_NO];
}

// -----------------------------------------------------------------------------
bool
C8042::enableAuxPort()
{
  if(bPortEnabled_[I8042_AUX_PORT_NO] == false)
  {
    regCTR_ &= ~I8042_CTR_AUXDIS;
    regCTR_ |=  I8042_CTR_AUXINT;

    bPortEnabled_[I8042_AUX_PORT_NO] = command(&regCTR_, I8042_CMD_CTL_WCTR);
  }

  return bPortEnabled_[I8042_AUX_PORT_NO];
}

// -----------------------------------------------------------------------------
bool
C8042::enableMuxPorts()
{
  if(bPortEnabled_[I8042_MUX_PORT_NO] == false)
  {
    uint8_t dummy;

    for(int i(0); i < I8042_NUM_MUX_PORTS; i++)
    {
      command(&dummy, I8042_CMD_MUX_PFX + i);
      command(&dummy, I8042_CMD_AUX_ENABLE);
    }

    bPortEnabled_[I8042_MUX_PORT_NO] = enableAuxPort();
  }

  return bPortEnabled_[I8042_MUX_PORT_NO];
}

// -----------------------------------------------------------------------------
// checks whether the controller has an active
// multiplexor and puts the chip into Multiplexed (1) or Legacy (0) mode.
bool
C8042::setMuxMode(unsigned int mode, unsigned char * mux_version)
{
  uint8_t param;

  // Get rid of bytes in the queue.
  flush();

  // Internal loopback test - send three bytes, they should come back from the
  // mouse interface, the last should be version.
  param = 0xf0;
  if(!command(&param, I8042_CMD_AUX_LOOP) || param != 0xf0)
    return false;
  param = mode ? 0x56 : 0xf6;
  if(!command(&param, I8042_CMD_AUX_LOOP) || param != (mode ? 0x56 : 0xf6))
    return false;
  param = mode ? 0xa4 : 0xa5;
  if(!command(&param, I8042_CMD_AUX_LOOP) || param == (mode ? 0xa4 : 0xa5))
    return false;

  if(mux_version)
    *mux_version = param;

  return true;
}

// -----------------------------------------------------------------------------
bool
C8042::command(uint8_t * param, int command)
{
//  if(i8042_noloop && command == I8042_CMD_AUX_LOOP)
//    return false;

  if(waitWrite() == false)
    return false;
  writeCommand(command & 0xff);

  for(int i(0); i < ((command >> 12) & 0xf); i++)
  {
    if(waitWrite() == false)
      return false;
    writeData(param[i]);
  }

  for(int i(0); i < ((command >> 8) & 0xf); i++)
  {
    if(waitRead() == false)
      return false;
    param[i] = readData();
  }

  return true;
}

// -----------------------------------------------------------------------------
void
C8042::flush()
{
  for(int i(0); i < I8042_BUFFER_SIZE; i++)
    if(readStatus() & I8042_STR_OBF)
      readData();
}

// -----------------------------------------------------------------------------
bool
C8042::writeKeyboard(uint8_t data)
{
  if(waitWrite() == false)
    return false;

  writeData(data);

  return true;
}

// -----------------------------------------------------------------------------
bool
C8042::writeAux(uint8_t data)
{
  if(waitWrite() == false)
    return false;

  // AUX port
  writeCommand(0xd4);

  if(waitWrite() == false)
    return false;

  writeData(data);

  return true;
}

// -----------------------------------------------------------------------------
bool
C8042::writeMux(uint8_t port, uint8_t data)
{
  if(port >= I8042_NUM_MUX_PORTS)
    return false;

  if(waitWrite() == false)
    return false;

  // MUX port (0, 1, 2, or 3)
  writeCommand(0x90 + port);

  if(waitWrite() == false)
    return false;

  writeData(data);

  return true;
}

// -----------------------------------------------------------------------------
bool
C8042::readDataW(uint8_t * data)
{
  if(waitRead() == false)
    return false;

  *data = readData();

  return true;
}

// -----------------------------------------------------------------------------
bool
C8042::waitRead()
{
  for(int iTimeout(0); iTimeout < I8042_CTL_TIMEOUT; iTimeout++)
  {
    if(readStatus() & I8042_STR_OBF)
      return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
bool
C8042::waitWrite()
{
  for(int iTimeout(0); iTimeout < I8042_CTL_TIMEOUT; iTimeout++)
  {
    if(~readStatus() & I8042_STR_IBF)
      return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
uint8_t
C8042::readData()
{
  return inb(I8042_DATA_REG);
}

// -----------------------------------------------------------------------------
uint8_t
C8042::readStatus()
{
  return inb(I8042_STATUS_REG);
}

// -----------------------------------------------------------------------------
void
C8042::writeData(uint8_t data)
{
  outb(data, I8042_DATA_REG);
}

// -----------------------------------------------------------------------------
void
C8042::writeCommand(uint8_t cmd)
{
  outb(cmd, I8042_COMMAND_REG);
}
