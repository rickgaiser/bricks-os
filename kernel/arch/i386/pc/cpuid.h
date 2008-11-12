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


#ifndef I386_CPUID_H
#define I386_CPUID_H


#include "inttypes.h"


// CPUID Feature Information
#define CPUID_FT_FPU      (1 <<  0)
#define CPUID_FT_VME      (1 <<  1)
#define CPUID_FT_DE       (1 <<  2)
#define CPUID_FT_PSE      (1 <<  3)
#define CPUID_FT_TSC      (1 <<  4)
#define CPUID_FT_MSR      (1 <<  5)
#define CPUID_FT_PAE      (1 <<  6)
#define CPUID_FT_MCE      (1 <<  7)
#define CPUID_FT_CX8      (1 <<  8)
#define CPUID_FT_APIC     (1 <<  9)
#define CPUID_FT_SEP      (1 << 11)
#define CPUID_FT_MTRR     (1 << 12)
#define CPUID_FT_PGE      (1 << 13)
#define CPUID_FT_MCA      (1 << 14)
#define CPUID_FT_MCOV     (1 << 15)
#define CPUID_FT_PAT      (1 << 16)
#define CPUID_FT_PSE_36   (1 << 17)
#define CPUID_FT_PSN      (1 << 18)
#define CPUID_FT_CFLSH    (1 << 19)
#define CPUID_FT_DS       (1 << 21)
#define CPUID_FT_ACPI     (1 << 22)
#define CPUID_FT_MMX      (1 << 23)
#define CPUID_FT_FXSR     (1 << 24)
#define CPUID_FT_SSE      (1 << 25)
#define CPUID_FT_SSE2     (1 << 26)
#define CPUID_FT_SS       (1 << 27)
#define CPUID_FT_HTT      (1 << 28)
#define CPUID_FT_TM       (1 << 29)
#define CPUID_FT_PBE      (1 << 31)
// CPUID Extended Feature Information
#define CPUID_EFT_SSE3    (1 <<  0)
#define CPUID_EFT_MONITOR (1 <<  3)
#define CPUID_EFT_DS_CPL  (1 <<  4)
#define CPUID_EFT_VMX     (1 <<  5)
#define CPUID_EFT_EST     (1 <<  7)
#define CPUID_EFT_TM2     (1 <<  8)
#define CPUID_EFT_SSSE3   (1 <<  9)
#define CPUID_EFT_CNXT_ID (1 << 10)
#define CPUID_EFT_CMPXCHG16B (1 << 13)
#define CPUID_EFT_XTPR    (1 << 14)
#define CPUID_EFT_PDCM    (1 << 16)


namespace CPU
{
  extern uint32_t iCPUFeatures2;
  extern uint32_t iCPUFeatures1;

  void init();

  bool hasCPUID();
  inline bool hasFPU() {return (CPU::iCPUFeatures1 & CPUID_FT_FPU);}
  inline bool hasPAE() {return (CPU::iCPUFeatures1 & CPUID_FT_PAE);}
  inline bool hasPSE() {return (CPU::iCPUFeatures1 & CPUID_FT_PSE);}
  inline bool hasAPIC(){return (CPU::iCPUFeatures1 & CPUID_FT_APIC);}
};


#endif

