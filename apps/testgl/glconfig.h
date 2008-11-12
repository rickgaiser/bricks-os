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


#ifndef GLCONFIG_H
#define GLCONFIG_H


#include "asm/arch/config.h"


// Use floating point interface instead of fixed point
#ifdef CONFIG_FPU
  #define USE_FLOATING_POINT
#endif

// Enable double buffering, should be enabled on most devices
// NOTE: ngc/wii currently uses the flush function to copy from RGB to YUV
//       framebuffer. Therefore a double RGB buffer is not needed.
#define ENABLE_DOUBLE_BUFFERED

// Enable vertical sync, works on most platforms, doesn't hurt on the rest
#define ENABLE_VSYNC

// Enable fog, doesn't hurt
//#define ENABLE_FOG

// Enable ligting in test, currently experimental
#define ENABLE_LIGHTING

// Enable textures, currently experimental
#define ENABLE_TEXTURES

// Enable deph testing (z-buffer), some platforms only
//#define ENABLE_DEPTH_TEST


#endif
