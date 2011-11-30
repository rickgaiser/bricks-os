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


#include "stdint.h"


#define MAX_INTERRUPTS 0x0b


//---------------------------------------------------------------------------
struct pt_regs
{
#ifdef PS2
  // 32 128-bit General Purpose Registers
  uint128_t cpu_r00;
  uint128_t cpu_r01;
  uint128_t cpu_r02;
  uint128_t cpu_r03;
  uint128_t cpu_r04;
  uint128_t cpu_r05;
  uint128_t cpu_r06;
  uint128_t cpu_r07;
  uint128_t cpu_r08;
  uint128_t cpu_r09;
  uint128_t cpu_r10;
  uint128_t cpu_r11;
  uint128_t cpu_r12;
  uint128_t cpu_r13;
  uint128_t cpu_r14;
  uint128_t cpu_r15;
  uint128_t cpu_r16;
  uint128_t cpu_r17;
  uint128_t cpu_r18;
  uint128_t cpu_r19;
  uint128_t cpu_r20;
  uint128_t cpu_r21;
  uint128_t cpu_r22;
  uint128_t cpu_r23;
  uint128_t cpu_r24;
  uint128_t cpu_r25;
  uint128_t cpu_r26;
  uint128_t cpu_r27;
  uint128_t cpu_r28;
  uint128_t cpu_r29;
  uint128_t cpu_r30;
  uint128_t cpu_r31;

  uint64_t fpu_r00;
  uint64_t fpu_r01;
  uint64_t fpu_r02;
  uint64_t fpu_r03;
  uint64_t fpu_r04;
  uint64_t fpu_r05;
  uint64_t fpu_r06;
  uint64_t fpu_r07;
  uint64_t fpu_r08;
  uint64_t fpu_r09;
  uint64_t fpu_r10;
  uint64_t fpu_r11;
  uint64_t fpu_r12;
  uint64_t fpu_r13;
  uint64_t fpu_r14;
  uint64_t fpu_r15;
  uint64_t fpu_r16;
  uint64_t fpu_r17;
  uint64_t fpu_r18;
  uint64_t fpu_r19;
  uint64_t fpu_r20;
  uint64_t fpu_r21;
  uint64_t fpu_r22;
  uint64_t fpu_r23;
  uint64_t fpu_r24;
  uint64_t fpu_r25;
  uint64_t fpu_r26;
  uint64_t fpu_r27;
  uint64_t fpu_r28;
  uint64_t fpu_r29;
  uint64_t fpu_r30;
  uint64_t fpu_r31;

  uint32_t fp_acc;
  uint32_t reserved;
  uint32_t t0;
  uint32_t t1;
  uint32_t t2;
  uint32_t t3;
  uint32_t t4;
  uint32_t fpu_status;
  uint32_t epc;
#endif
};


#endif
