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


#ifndef ASM_ARCH_MEMORY_H
#define ASM_ARCH_MEMORY_H


// EWRAM:  4MiB (default)
// ITCM:  32KiB (instruction cache)
// DTCM:  16KiB (data cache)
#define EWRAM_CODE __attribute__((section(".ewram"), long_call))
#define EWRAM_DATA __attribute__((section(".ewram")))
#define ITCM_CODE  __attribute__((section(".itcm"), long_call))
#define DTCM_DATA  __attribute__((section(".dtcm")))
#define INTERRUPT_CODE ITCM_CODE
#define FAST_CODE      INTERRUPT_CODE
#define FAST_DATA      DTCM_DATA


#endif
