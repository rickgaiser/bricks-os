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


#ifndef PS2_SIF_H
#define PS2_SIF_H


#include "stdint.h"


class CIOP
{
public:
  CIOP();
  ~CIOP();

  int  init();
  bool alive();
  bool sync();

private:
  uint32_t sendCmd(int cid, void * pkt, uint32_t pktsize, void * src, void * dest, int size);

  // Callback functions
  static int32_t  callBackHandler(int32_t channel);
  static void     changeAddr(void * packet, void * harg);
  static void     setSReg   (void * packet, void * harg);

  bool bInitialized_;
  int  iDMAHandlerID_;
};


#endif
