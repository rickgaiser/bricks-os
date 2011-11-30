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
 *
 * Based on Deku's gba-mod tutorial: http://deku.gbadev.org/
 */


#ifndef MOD_H
#define MOD_H


#include "stdint.h"


// -----------------------------------------------------------------------------
// This is the layout of the sample info in ROM
struct SAMPLE_HEADER
{
  uint16_t  length;
  uint8_t   finetune;
  uint8_t   vol;
  uint16_t  loopStart;
  uint16_t  loopLength;

  const int8_t * smpData;   // Pointer to sample data in ROM
};

// -----------------------------------------------------------------------------
// This is the MOD data layout in ROM
struct MOD_HEADER
{
  const SAMPLE_HEADER * sample;
  const uint8_t * order;
  const uint8_t ** pattern;

  uint8_t orderCount;
  uint8_t pad[3];
};


#endif
