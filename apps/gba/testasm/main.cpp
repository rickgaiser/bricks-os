/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2009 Maximus32 <Maximus32@bricks-os.org>
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


#include "kernel/debug.h"
#include "inttypes.h"


extern "C" int32_t asm_fx_mul_64(int32_t a, int32_t b);


//---------------------------------------------------------------------------
int32_t
fx_mul_32(int32_t a, int32_t b)
{
  int32_t result;

  result = (a * b) >> 16;

  return result;
}

//---------------------------------------------------------------------------
int32_t
fx_mul_64(int32_t a, int32_t b)
{
  return (((int64_t)a * b) >> 16);
}

//---------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  // 5.5 * 5.5 = 30.25
  int32_t a = (5<<16) + (1<<15); // 5.5
  int32_t b = (5<<16) + (1<<15); // 5.5
  int32_t result;

  printk("64bit mul->shr test\n");

  result = fx_mul_32(a, b);
  printk("5.5 x 5.5 = %d.%d\n", result >> 16, ((result & 0xffff) * 10) / (1<<16));
  result = fx_mul_64(a, b);
  printk("5.5 x 5.5 = %d.%d\n", result >> 16, ((result & 0xffff) * 10) / (1<<16));
  result = asm_fx_mul_64(a, b);
  printk("5.5 x 5.5 = %d.%d (0x%x)\n", result >> 16, ((result & 0xffff) * 10) / (1<<16), result);

  while(1);

  return 0;
}
