/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2010 Maximus32 <Maximus32@bricks-os.org>
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


#include "i8237.h"
#include "asm/hal.h"


// 8bit DMA controller
#define REG_DMA8_STATUS         0x08 // R
#define REG_DMA8_COMMAND        0x08 // W
#define REG_DMA8_REQUEST        0x09 // W
#define REG_DMA8_MASK           0x0a // W
#define REG_DMA8_MODE           0x0b // W
#define REG_DMA8_CLEAR          0x0c // W

// 16bit DMA controller
#define REG_DMA16_STATUS        0xd0 // R
#define REG_DMA16_COMMAND       0xd0 // W
#define REG_DMA16_REQUEST       0xd2 // W
#define REG_DMA16_MASK          0xd4 // W
#define REG_DMA16_MODE          0xd6 // W
#define REG_DMA16_CLEAR         0xd8 // W


struct SDMAChannel
{
  uint16_t start;
  uint16_t count;
  uint16_t page;
};

SDMAChannel dmaChannel[] =
{
  {0x00, 0x01, 0x87},
  {0x02, 0x03, 0x83},
  {0x04, 0x05, 0x81},
  {0x06, 0x07, 0x82},

  {0xc0, 0xc2, 0x8f},
  {0xc4, 0xc6, 0x8b},
  {0xc8, 0xca, 0x89},
  {0xcc, 0x07, 0x8a},
};


// -----------------------------------------------------------------------------
uint8_t
dmaIn8(uint16_t addr)
{
  return inb(addr);
}

// -----------------------------------------------------------------------------
void
dmaOut8(uint16_t addr, uint8_t data)
{
  outb(data, addr);
}

// -----------------------------------------------------------------------------
void
dmaEnable(unsigned int channel)
{
  dmaOut8((channel < 4) ? REG_DMA8_MASK : REG_DMA16_MASK, (channel & 3));
}

// -----------------------------------------------------------------------------
void
dmaDisable(unsigned int channel)
{
  dmaOut8((channel < 4) ? REG_DMA8_MASK : REG_DMA16_MASK, (channel & 3) | 0x04);
}

// -----------------------------------------------------------------------------
void
dmaSetMode(unsigned int channel, uint8_t mode)
{
  dmaOut8((channel < 4) ? REG_DMA8_MODE : REG_DMA16_MODE, (channel & 3) | mode);
}

// -----------------------------------------------------------------------------
void
dmaSetAddress(unsigned int channel, uint32_t addr)
{
  // Clear Flip-Flop
  dmaOut8(REG_DMA16_CLEAR, 0);
  // Set addr
  if(channel < 4)
  {
    // Addr
    dmaOut8(dmaChannel[channel].start, (addr >>  0) & 0xff);
    dmaOut8(dmaChannel[channel].start, (addr >>  8) & 0xff);
    // 64KiB page
    dmaOut8(dmaChannel[channel].page,  (addr >> 16) & 0xff);
  }
  else
  {
    // Addr
    dmaOut8(dmaChannel[channel].start, (addr >>  1) & 0xff);
    dmaOut8(dmaChannel[channel].start, (addr >>  9) & 0x7f);
    // 64KiB page
    dmaOut8(dmaChannel[channel].page,  (addr >> 16) & 0xff);
  }
}

// -----------------------------------------------------------------------------
void
dmaSetCount(unsigned int channel, uint16_t count)
{
  // Clear Flip-Flop
  dmaOut8(REG_DMA16_CLEAR, 0);
  // Set Count
  dmaOut8(dmaChannel[channel].count, (count - 1) >> 0);
  dmaOut8(dmaChannel[channel].count, (count - 1) >> 8);
}
