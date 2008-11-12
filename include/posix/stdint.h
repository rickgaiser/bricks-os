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


#ifndef STDINT_H
#define STDINT_H


typedef signed char                 int8_t;
typedef signed short                int16_t;
typedef signed int                  int32_t;
typedef signed long long            int64_t;
typedef unsigned char               uint8_t;
typedef unsigned short              uint16_t;
typedef unsigned int                uint32_t;
typedef unsigned long long          uint64_t;

typedef int8_t                      __s8;
typedef int16_t                     __s16;
typedef int32_t                     __s32;
typedef int64_t                     __s64;
typedef uint8_t                     __u8;
typedef uint16_t                    __u16;
typedef uint32_t                    __u32;
typedef uint64_t                    __u64;

typedef volatile int8_t             vint8_t;
typedef volatile int16_t            vint16_t;
typedef volatile int32_t            vint32_t;
typedef volatile int64_t            vint64_t;
typedef volatile uint8_t            vuint8_t;
typedef volatile uint16_t           vuint16_t;
typedef volatile uint32_t           vuint32_t;
typedef volatile uint64_t           vuint64_t;

typedef signed int                  intptr_t;
typedef unsigned int                uintptr_t;


#endif
