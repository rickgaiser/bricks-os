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


#ifndef RASTER_BITRESOLUTION_H
#define RASTER_BITRESOLUTION_H


#define SHIFT_XY      16 // 1.15.16 (same as fixed point pipeline)
#define DEPTH_Z       16 // 16bit depth buffer
#define SHIFT_Z       14 // 1.17.14
#define SHIFT_COLOR   14 // 1.17.14 (allows 32bit multiply, with some additions)
#define SHIFT_TEX     16 // 1.15.16

// The subdivided affine block specs
#define AFFINE_SHIFT      5
#define AFFINE_LENGTH     (1 << AFFINE_SHIFT)
#define MIN_AFFINE_LENGTH (AFFINE_LENGTH + (AFFINE_LENGTH >> 1)) // 1.5x AFFINE_LENGTH

#define i_mul_shr_64(a,b,shr)       ((int32_t)(((int64_t)(a)*(int64_t)(b)             )>>shr))
#define i_mul_round_shr_64(a,b,shr) ((int32_t)(((int64_t)(a)*(int64_t)(b)+(1<<(shr-1)))>>shr))
#define i_mul_shr(a,b,shr)          ((                   (a)*         (b)             )>>shr)
#define i_mul_round_shr(a,b,shr)    ((                   (a)*         (b)+(1<<(shr-1)))>>shr)

#define fx_ceil(fx,sh)  (((fx)+((1<<sh)-1)) >> sh)
#define fx_floor(fx,sh) ( (fx)              >> sh)
#define fx_round(fx,sh) (((fx)+(1<<(sh-1))) >> sh)
#define int_abs(i)      ((i) > 0 ? (i) : -(i))


#endif
