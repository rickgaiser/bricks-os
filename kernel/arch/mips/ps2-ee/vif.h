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


#ifndef PS2_VIF_H
#define PS2_VIF_H


#include "stdint.h"
#include "dma.h"


// VIF Commands
#define VIF_CMD_INTERRUPT     0x80
#define VIF_CMD_NOP           0x00
#define VIF_CMD_STCYCL        0x01
#define VIF_CMD_OFFSET        0x02
#define VIF_CMD_BASE          0x03
#define VIF_CMD_ITOP          0x04
#define VIF_CMD_STMOD         0x05
#define VIF_CMD_MSKPATH3      0x06
#define VIF_CMD_MARK          0x07
#define VIF_CMD_FLUSHE        0x10
#define VIF_CMD_FLUSH         0x11
#define VIF_CMD_FLUSHA        0x13
#define VIF_CMD_MSCAL         0x14
#define VIF_CMD_MSCNT         0x17
#define VIF_CMD_MSCALF        0x15
#define VIF_CMD_STMASK        0x20
#define VIF_CMD_STROW         0x30
#define VIF_CMD_STCOL         0x31
#define VIF_CMD_MPG           0x4a
#define VIF_CMD_DIRECT        0x50
#define VIF_CMD_DIRECTHL      0x51
// UNPACK Part 1
#define VIF_CMD_UNPACK        0x60
// UNPACK Part 2
#define VIF_CMD_UNPACK_SIGNED (0<<4)
#define VIF_CMD_UNPACK_UNSIGNED (1<<4)
// UNPACK Part 3
#define VIF_CMD_UNPACK_V1_32  (0<<2|0)
#define VIF_CMD_UNPACK_V1_16  (0<<2|1)
#define VIF_CMD_UNPACK_V1_8   (0<<2|2)
#define VIF_CMD_UNPACK_V2_32  (1<<2|0)
#define VIF_CMD_UNPACK_V2_16  (1<<2|1)
#define VIF_CMD_UNPACK_V2_8   (1<<2|2)
#define VIF_CMD_UNPACK_V3_32  (2<<2|0)
#define VIF_CMD_UNPACK_V3_16  (2<<2|1)
#define VIF_CMD_UNPACK_V3_8   (2<<2|2)
#define VIF_CMD_UNPACK_V4_32  (3<<2|0)
#define VIF_CMD_UNPACK_V4_16  (3<<2|1)
#define VIF_CMD_UNPACK_V4_8   (3<<2|2)
#define VIF_CMD_UNPACK_V4_5   (3<<2|3)
// UNPACK Example:
//   Unpack 4 x int32_t
//   (VIF_CMD_UNPACK | VIF_CMD_UNPACK_SIGNED | VIF_CMD_UNPACK_V4_32)

// NUM:
//  - Data: number of 128bit fields
//  - Code: number of  64bit fields
#define VIF_CODE(imm, num, cmd) \
  (((uint32_t)(imm) <<  0) | \
   ((uint32_t)(num) << 16) | \
   ((uint32_t)(cmd) << 24))

// VU memory mapped to the EE addr space
#define VU0_CODE_START ((void *)0x11000000)
#define VU0_DATA_START ((void *)0x11004000)
#define VU1_CODE_START ((void *)0x11008000)
#define VU1_DATA_START ((void *)0x1100c000)


// -----------------------------------------------------------------------------
class CVIFPacket
 : public CSCDMAPacket
{
public:
  CVIFPacket(uint32_t qwSize, EDMAChannel channelId);
  CVIFPacket(void * buffer, uint32_t qwSize, EDMAChannel channelId);
  ~CVIFPacket();

  // Reset packet
  inline void reset();

  // Add VIF code
  inline void vifAddCommand(uint8_t command, uint8_t num, uint8_t immediate);

  // Send code
  void vifAddSendCode(const void * code, uint32_t size);

  // Pad to 128bit (for faster operation)
  inline void pad128();
};

// -----------------------------------------------------------------------------
void vu_upload(void * dst, const void * src, unsigned int size);
void vu1_wait();
void vu1_run();

// -----------------------------------------------------------------------------
class CVUData
{
public:
  CVUData(const void * data, uint32_t size) : pData_(data), iSize_(size) {}

  inline void upload(void * addr){vu1_wait();vu_upload(addr, pData_, iSize_);}

private:
  const void * pData_;
  uint32_t iSize_;
};

// -----------------------------------------------------------------------------
class CVUCode
{
public:
  CVUCode(const void * data, uint32_t size) : pData_(data), iSize_(size) {}

  inline void upload(void * addr){vu1_wait();vu_upload(addr, pData_, iSize_);}
  inline void run(){vu1_run();}
  inline void wait(){vu1_wait();}

private:
  const void * pData_;
  uint32_t iSize_;
};


#include "vif.inl"


#endif // PS2_VIF_H
