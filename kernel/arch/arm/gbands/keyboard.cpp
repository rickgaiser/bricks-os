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


#include "keyboard.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/arch/registers.h"


// -----------------------------------------------------------------------------
CGBAKeyboard::CGBAKeyboard()
 : CAFileIOBufferedRead()
 , CAControllerDevice()
 , iKeys_(0x0000)
#ifdef NDS7
 , iKeysXY_(0x0000 | KEY_LID) // Lid is opened by default
#endif
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CGBAKeyboard::~CGBAKeyboard()
{
}

// -----------------------------------------------------------------------------
int
CGBAKeyboard::init()
{
/*
  // NOTE: Interrupt ONLY come when a button is PRESSED!
  // NOTE: Interrupt is intended for waking up from (the low power) stop mode
  // NOTE: Use VBlank instead
  // Attach to keypad IRQ
  CInterruptManager::attach(12, this);
  // Enable interrupt from all keys
  REG_KEYCNT = KEY_CNT_OR
             | KEY_CNT_IRQ
             | KEY_A
             | KEY_B
             | KEY_SELECT
             | KEY_START
             | KEY_RIGHT
             | KEY_LEFT
             | KEY_UP
             | KEY_DOWN
             | KEY_R
             | KEY_L;
*/

  // Attach to VBlank IRQ
  CInterruptManager::attach(0, this);
  // Enable VBlank interrupt
  REG_DISPSTAT |= IRQ_VBLANK_ENABLE;

  return 0;
}

// -----------------------------------------------------------------------------
int
CGBAKeyboard::isr(int irq)
{
  uint16_t iNewKeys       = ~REG_KEYS;
  uint16_t iChangedKeys   = iNewKeys   ^ iKeys_;
  uint16_t iChangedUp     = iNewKeys   & iChangedKeys;
#ifdef NDS7
  uint16_t iNewKeysXY     = ~REG_KEYSXY;
  uint16_t iChangedKeysXY = iNewKeysXY ^ iKeysXY_;
  uint16_t iChangedUpXY   = iNewKeysXY & iChangedKeysXY;
#endif

  if(iChangedUp & KEY_A)       bufferRead_.put('A');
  if(iChangedUp & KEY_B)       bufferRead_.put('B');
  if(iChangedUp & KEY_SELECT)  bufferRead_.put('S');
  if(iChangedUp & KEY_START)   bufferRead_.put('S');
  if(iChangedUp & KEY_RIGHT)   bufferRead_.put('R');
  if(iChangedUp & KEY_LEFT)    bufferRead_.put('L');
  if(iChangedUp & KEY_UP)      bufferRead_.put('U');
  if(iChangedUp & KEY_DOWN)    bufferRead_.put('D');
  if(iChangedUp & KEY_R)       bufferRead_.put('>');
  if(iChangedUp & KEY_L)       bufferRead_.put('<');
#ifdef NDS7
  if(iChangedUpXY & KEY_X)     bufferRead_.put('X');
  if(iChangedUpXY & KEY_Y)     bufferRead_.put('Y');
  if(iChangedUpXY & KEY_TOUCH) bufferRead_.put('T');
  if(iChangedUpXY & KEY_LID)   bufferRead_.put('L');
#endif

  bufferRead_.notifyGetters();

  iKeys_   = iNewKeys;
#ifdef NDS7
  iKeysXY_ = iNewKeysXY;
#endif

  return 0;
}

// -----------------------------------------------------------------------------
uint32_t
CGBAKeyboard::getButtonState()
{
  uint32_t iRetVal(0);

  uint16_t iKeys   = ~REG_KEYS;
#ifdef NDS7
  uint16_t iKeysXY = ~REG_KEYSXY;
#endif

  if(iKeys & KEY_UP)      iRetVal |= CTRL_BTN_UP;
  if(iKeys & KEY_DOWN)    iRetVal |= CTRL_BTN_DOWN;
  if(iKeys & KEY_LEFT)    iRetVal |= CTRL_BTN_LEFT;
  if(iKeys & KEY_RIGHT)   iRetVal |= CTRL_BTN_RIGHT;

  if(iKeys & KEY_START)   iRetVal |= CTRL_BTN_START;
  if(iKeys & KEY_SELECT)  iRetVal |= CTRL_BTN_SELECT;

  if(iKeys & KEY_A)       iRetVal |= CTRL_BTN_ACTION_4;
  if(iKeys & KEY_B)       iRetVal |= CTRL_BTN_ACTION_2;
#ifdef NDS7
  if(iKeysXY & KEY_X)     iRetVal |= CTRL_BTN_ACTION_1;
  if(iKeysXY & KEY_Y)     iRetVal |= CTRL_BTN_ACTION_3;
#endif

  if(iKeys & KEY_R)       iRetVal |= CTRL_BTN_TRIGGER_R1;
  if(iKeys & KEY_L)       iRetVal |= CTRL_BTN_TRIGGER_L1;

  return iRetVal;
}
