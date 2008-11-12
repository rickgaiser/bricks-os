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


#ifndef I8042_KEYBOARD_H
#define I8042_KEYBOARD_H


#include "i8042.h"
#include "kernel/fs.h"
#include "inttypes.h"


#define KBD_DATA_REG       0x60
#define KBD_ACK_REG        0x61
#define KBD_COMMAND_REG    0x64
#define KBD_STATUS_REG     0x64

#define KBD_LEDS           0xed
#define KBD_DIAGNOSTICS    0xee
#define KBD_SELECT_SCANSET 0xf0
#define KBD_IDENTIFY       0xf2
#define KBD_RATE           0xf3


// -----------------------------------------------------------------------------
enum EKeyCode
{
  KEY_Escape,
  KEY_Backspace,
  KEY_CapsLock,
  KEY_Shift,
  KEY_Ctrl,
  KEY_Alt,
  KEY_Enter,

  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12,

  KEY_1 = 0x31,
  KEY_2 = 0x32,
  KEY_3 = 0x33,
  KEY_4 = 0x34,
  KEY_5 = 0x35,
  KEY_6 = 0x36,
  KEY_7 = 0x37,
  KEY_8 = 0x38,
  KEY_9 = 0x39,
  KEY_0 = 0x30,

  KEY_a = 'a',
  KEY_b = 'b',
  KEY_c = 'c',
  KEY_d = 'd',
  KEY_e = 'e',
  KEY_f = 'f',
  KEY_g = 'g',
  KEY_h = 'h',
  KEY_i = 'i',
  KEY_j = 'j',
  KEY_k = 'k',
  KEY_l = 'l',
  KEY_m = 'm',
  KEY_n = 'n',
  KEY_o = 'o',
  KEY_p = 'p',
  KEY_q = 'q',
  KEY_r = 'r',
  KEY_s = 's',
  KEY_t = 't',
  KEY_u = 'u',
  KEY_v = 'v',
  KEY_w = 'w',
  KEY_x = 'x',
  KEY_y = 'y',
  KEY_z = 'z',

  KEY_A = 'A',
  KEY_B = 'B',
  KEY_C = 'C',
  KEY_D = 'D',
  KEY_E = 'E',
  KEY_F = 'F',
  KEY_G = 'G',
  KEY_H = 'H',
  KEY_I = 'I',
  KEY_J = 'J',
  KEY_K = 'K',
  KEY_L = 'L',
  KEY_M = 'M',
  KEY_N = 'N',
  KEY_O = 'O',
  KEY_P = 'P',
  KEY_Q = 'Q',
  KEY_R = 'R',
  KEY_S = 'S',
  KEY_T = 'T',
  KEY_U = 'U',
  KEY_V = 'V',
  KEY_W = 'W',
  KEY_X = 'X',
  KEY_Y = 'Y',
  KEY_Z = 'Z',
};


// -----------------------------------------------------------------------------
class CI8042Keyboard
 : public CAFileIOBufferedRead
 , public I8042CallBack
{
public:
  CI8042Keyboard(C8042 & driver);
  virtual ~CI8042Keyboard();

  int init();

  // Inherited from ...
  virtual void i8042_callBack(uint8_t scancode);

private:
  void updateLeds();

private:
  C8042 & driver_;

  // Special keycode
  bool bE0_;
  // State keys
  bool bShift_;
  bool bCtrl_;
  bool bAlt_;
  // Locked state keys (with leds on keyboard)
  bool bCapsLock_;
  bool bNumLock_;
  bool bScrollLock_;
};


#endif
