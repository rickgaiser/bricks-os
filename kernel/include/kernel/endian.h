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


#ifndef ENDIAN_H
#define ENDIAN_H


#include "asm/arch/config.h"


#define ENDIAN_SWAP_16(v) \
  ((((v) >>  8) & 0x00ff) | \
   (((v) <<  8) & 0xff00))
#define ENDIAN_SWAP_32(v) \
  ((((v) >> 24) & 0x000000ff) | \
   (((v) >>  8) & 0x0000ff00) | \
   (((v) <<  8) & 0x00ff0000) | \
   (((v) << 24) & 0xff000000))
#define ENDIAN_SWAP_64(v) \
  ((((v) >> 56) & 0x00000000000000ff) | \
   (((v) >> 40) & 0x000000000000ff00) | \
   (((v) >> 24) & 0x0000000000ff0000) | \
   (((v) >>  8) & 0x00000000ff000000) | \
   (((v) <<  8) & 0x000000ff00000000) | \
   (((v) << 24) & 0x0000ff0000000000) | \
   (((v) << 40) & 0x00ff000000000000) | \
   (((v) << 56) & 0xff00000000000000))

#if defined(__LITTLE_ENDIAN__)
  #define ENDIAN_LE_16(v) (v)
  #define ENDIAN_LE_32(v) (v)
  #define ENDIAN_LE_64(v) (v)
  #define ENDIAN_BE_16(v) ENDIAN_SWAP_16(v)
  #define ENDIAN_BE_32(v) ENDIAN_SWAP_32(v)
  #define ENDIAN_BE_64(v) ENDIAN_SWAP_64(v)
#elif defined(__BIG_ENDIAN__)
  #define ENDIAN_LE_16(v) ENDIAN_SWAP_16(v)
  #define ENDIAN_LE_32(v) ENDIAN_SWAP_32(v)
  #define ENDIAN_LE_64(v) ENDIAN_SWAP_64(v)
  #define ENDIAN_BE_16(v) (v)
  #define ENDIAN_BE_32(v) (v)
  #define ENDIAN_BE_64(v) (v)
#else
  #error BYTE ORDER NOT DEFINED!
#endif


#endif
