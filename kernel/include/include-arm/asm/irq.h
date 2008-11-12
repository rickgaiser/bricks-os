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


#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#include "inttypes.h"
#include "asm/arch/irq.h"


//---------------------------------------------------------------------------
//#define sp r13
//#define lr r14
//#define pc r15
struct pt_regs
{
  // System/User mode state
  uint32_t r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r0;
  uint32_t pc, cpsr, lr, sp;
};

#define CPU_MODE_USER        0x00000010
#define CPU_MODE_FIQ         0x00000011
#define CPU_MODE_IRQ         0x00000012
#define CPU_MODE_SUPERVISOR  0x00000013
#define CPU_MODE_ABORT       0x00000017
#define CPU_MODE_UNDEFINED   0x0000001b
#define CPU_MODE_SYSTEM      0x0000001f

#define CPU_MODE_THUMB       0x00000020
#define CPU_MODE_ARM         0x00000000

#define CPU_FIQ_DISABLE      0x00000040
#define CPU_FIQ_ENABLE       0x00000000

#define CPU_IRQ_DISABLE      0x00000080
#define CPU_IRQ_ENABLE       0x00000000


#endif
