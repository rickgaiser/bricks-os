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


#include "mod.h"
#include "stdint.h"


extern const uint8_t dMod0Pattern0[];
extern const uint8_t dMod0Pattern1[];
extern const uint8_t dMod0Pattern2[];
extern const uint8_t dMod0Pattern3[];
extern const uint8_t dMod0Pattern4[];
extern const uint8_t dMod0Pattern5[];
extern const uint8_t dMod0Pattern6[];
extern const uint8_t dMod0Pattern7[];
extern const uint8_t dMod0Pattern8[];
extern const uint8_t dMod0Pattern9[];
extern const uint8_t dMod0Pattern10[];
extern const uint8_t dMod0Pattern11[];
extern const uint8_t dMod0Pattern12[];

extern const int8_t dSmpData0[];
extern const int8_t dSmpData1[];
extern const int8_t dSmpData2[];
extern const int8_t dSmpData3[];
extern const int8_t dSmpData4[];
extern const int8_t dSmpData5[];
extern const int8_t dSmpData6[];
extern const int8_t dSmpData7[];


const SAMPLE_HEADER dMod0SmpTable[] =
{
  { 17, 0, 64, 0, 16, dSmpData0 },
  { 65, 0, 64, 0, 64, dSmpData1 },
  { 8, 0, 32, 0, 8, dSmpData2 },
  { 8, 0, 16, 0, 8, dSmpData3 },
  { 8, 0, 64, 0, 8, dSmpData4 },
  { 1123, 0, 48, 0, 0, dSmpData5 },
  { 1505, 0, 64, 0, 0, dSmpData6 },
  { 1463, 0, 64, 0, 0, dSmpData7 },
};

const uint8_t * dMod0PatternTable[] =
{
  dMod0Pattern0, dMod0Pattern1, dMod0Pattern2, dMod0Pattern3, dMod0Pattern4, dMod0Pattern5, dMod0Pattern6, dMod0Pattern7,
  dMod0Pattern8, dMod0Pattern9, dMod0Pattern10, dMod0Pattern11, dMod0Pattern12,
};

const uint8_t dMod0OrderTable[] =
{
  0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
};

MOD_HEADER dModTable[] =
{
  {dMod0SmpTable, dMod0OrderTable, dMod0PatternTable, 15, {13, 0, 0}},
};

const SAMPLE_HEADER dSfxTable[] =
{
  {0, 0, 0, 0, 0, 0} // Placeholder, no SFX specified
};
