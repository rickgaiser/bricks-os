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
// Intel 8237 DMA controller
// IBM PC Compatible has two DMA controllers:
//  - 1x  8bit (4channels)
//    - Channel-0: Reserved (was used for DRAM refresh)
//    - Channel-1: Free
//    - Channel-2: Free
//    - Channel-3: Free
//  - 1x 16bit (4channels)
//    - Channel-4: Reserved (cascaded to other controller)
//    - Channel-5: Free
//    - Channel-6: Free
//    - Channel-7: Free


#ifndef I8237_H
#define I8237_H


#include "inttypes.h"


// Transfer type
#define DMA_MODE_TEST     0x00
#define DMA_MODE_IN       0x04
#define DMA_MODE_OUT      0x08
// Auto init
#define DMA_MODE_AUTO     0x10
// Tranfer data in reverse?
#define DMA_MODE_DOWN     0x20
// Transfer mode
#define DMA_MODE_DEMAND   0x00
#define DMA_MODE_SINGLE   0x40
#define DMA_MODE_BLOCK    0x80
#define DMA_MODE_CASCADE  0xc0


void dmaEnable(unsigned int channel);
void dmaDisable(unsigned int channel);

void dmaSetMode(unsigned int channel, uint8_t mode);
void dmaSetAddress(unsigned int channel, uint32_t addr);
void dmaSetCount(unsigned int channel, uint16_t count);


#endif
