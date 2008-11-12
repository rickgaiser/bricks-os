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


#ifndef NGC_GX_H
#define NGC_GX_H


#include "inttypes.h"


namespace GX
{
  namespace REG
  {
    enum
    {
      zmode       = 0x40,
      cmode0      = 0x41,
      cmode1      = 0x42,
      pixfmt      = 0x43,
      cp_src_off  = 0x49,
      cp_src_size = 0x4a,
      cp_dst_addr = 0x4b,
      cp_dst_w    = 0x4d,
      cp_yscale   = 0x4e,
      cp_clr_ar   = 0x4f,
      cp_clr_gb   = 0x50,
      cp_clr_z    = 0x51,
      cp_ctrl     = 0x52,
    };
  };
};


#define EFB_ADDR       ((void *)0xc8000000)

#define REG_WGPIPE     (0xcc008000)

#define FIFO_PUTU8(x)  ((*(vuint8_t  *)REG_WGPIPE) = (uint8_t )(x))
#define FIFO_PUTS8(x)  ((*(vint8_t   *)REG_WGPIPE) = (int8_t  )(x))
#define FIFO_PUTU16(x) ((*(vuint16_t *)REG_WGPIPE) = (uint16_t)(x))
#define FIFO_PUTS16(x) ((*(vint16_t  *)REG_WGPIPE) = (int16_t )(x))
#define FIFO_PUTU32(x) ((*(vuint32_t *)REG_WGPIPE) = (uint32_t)(x))
#define FIFO_PUTS32(x) ((*(vint32_t  *)REG_WGPIPE) = (int32_t )(x))
#define FIFO_PUTF32(x) ((*(volatile float *)REG_WGPIPE) = (float)(x))

#define GX_LOAD_BP_REG(reg,x) \
  FIFO_PUTU8(0x61); \
  FIFO_PUTU32(((reg)<<24)|(x)); \

#define GX_LOAD_CP_REG(x, y) \
  FIFO_PUTU8(0x08); \
  FIFO_PUTU8((x)); \
  FIFO_PUTU32((y)); \

#define GX_LOAD_XF_REG(x, y) \
  FIFO_PUTU8(0x10); \
  FIFO_PUTU32(((x)&0xffff)); \
  FIFO_PUTU32((y)); \

#define GX_LOAD_XF_REGS(x, n) \
  FIFO_PUTU8(0x10); \
  FIFO_PUTU32((((((n)&0xffff)-1)<<16)|((x)&0xffff))); \

#define GX_XY(x,y) \
  (((x)      ) | \
   ((y) << 10))


#endif
