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


#ifndef RASTER_COLOR_H
#define RASTER_COLOR_H


// Load colors
#define COLOR_LOAD_555(c,t,res) \
  c.b = ((t & 0x7c00) >> 10) * ((1<<res) / 31); \
  c.g = ((t & 0x03e0) >>  5) * ((1<<res) / 31); \
  c.r = ((t & 0x001f)      ) * ((1<<res) / 31); \
  c.a = (1<<res)
#define COLOR_LOAD_565(c,t,res) \
  c.r = ((t & 0xf800) >> 11) * ((1<<res) / 31); \
  c.g = ((t & 0x07e0) >>  5) * ((1<<res) / 63); \
  c.b = ((t & 0x001f)      ) * ((1<<res) / 31); \
  c.a = (1<<res)
#define COLOR_LOAD_888(c,t,res) \
  c.r = ((t & 0x00ff0000) >> 16) * ((1<<res) / 255); \
  c.g = ((t & 0x0000ff00) >>  8) * ((1<<res) / 255); \
  c.b = ((t & 0x000000ff)      ) * ((1<<res) / 255); \
  c.a = (1<<res)
#define COLOR_LOAD_8888(c,t,res) \
  c.r = ((t & 0x00ff0000) >> 16) * ((1<<res) / 255); \
  c.g = ((t & 0x0000ff00) >>  8) * ((1<<res) / 255); \
  c.b = ((t & 0x000000ff)      ) * ((1<<res) / 255); \
  c.a = ((t             ) >> 24) * ((1<<res) / 255)

// Save colors
#define COLOR_SAVE_COMP_5(c,res) (((((int32_t)(c))* 31) & (0x1f<<res))>>res)
#define COLOR_SAVE_COMP_6(c,res) (((((int32_t)(c))* 63) & (0x3f<<res))>>res)
#define COLOR_SAVE_COMP_8(c,res) (((((int32_t)(c))*255) & (0xff<<res))>>res)
#define COLOR_SAVE_555(c,res) \
  ((COLOR_SAVE_COMP_5(c.b,res) << 10) | \
   (COLOR_SAVE_COMP_5(c.g,res) <<  5) | \
   (COLOR_SAVE_COMP_5(c.r,res)      ))
#define COLOR_SAVE_565(c,res) \
  ((COLOR_SAVE_COMP_5(c.r,res) << 11) | \
   (COLOR_SAVE_COMP_6(c.g,res) <<  5) | \
   (COLOR_SAVE_COMP_5(c.b,res)      ))
#define COLOR_SAVE_888(c,res) \
  ((COLOR_SAVE_COMP_8(c.r,res) << 16) | \
   (COLOR_SAVE_COMP_8(c.g,res) <<  8) | \
   (COLOR_SAVE_COMP_8(c.b,res)      ))
#define COLOR_SAVE_8888(c,res) \
  ((COLOR_SAVE_COMP_8(c.a,res) << 24) | \
   (COLOR_SAVE_COMP_8(c.r,res) << 16) | \
   (COLOR_SAVE_COMP_8(c.g,res) <<  8) | \
   (COLOR_SAVE_COMP_8(c.b,res)      ))


#endif
