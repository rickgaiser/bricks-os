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


#ifndef HAL_H
#define HAL_H


#include "stdint.h"


// CPU flags
#define I386_ON_FLAGS      (0x00000002) // Always on flags
#define I386_IE_FLAG       (1<< 9)
#define I386_IOPL_VALUE(v) (((v)&3)<<12)
#define I386_NT_FLAG       (1<<14)
#define I386_VM_FLAG       (1<<17)

// v86 addr to linear arrd
#define from_v86_addr(seg,off) (((seg & 0xffff) << 4)+off)

// Descriptor Types (bits for 6th byte (access) of descriptor)
typedef enum
{
  // Code/Data
    dtDataR         = 0x10
  , dtDataRW        = 0x12
  , dtStackR        = 0x14
  , dtStackRW       = 0x16
  , dtCode          = 0x18
  , dtCodeR         = 0x1a
  , dtCodeUnsafe    = 0x1c
  , dtCodeUnsafeR   = 0x1e
  // System
  , dtTaskGate      = 0x05
  , dtTSS           = 0x09
  , dtCallGate      = 0x0c
  , dtInterruptGate = 0x0e
  , dtTrapGate      = 0x0f
} EDescriptorType;
#define DESC_PRESENT  0x80

// Selector type (selects a descriptor in the LDT/GDT)
typedef uint16_t  selector_t;

// Descriptor struct
typedef struct
{
  // Byte 0 and 1
  union
  {
    uint16_t limit;      // Segment
    uint16_t offset_l;   // Call/Interrupt/Trap Gate
    uint16_t reserved1;  // Task Gate
  };
  // Byte 2 and 3
  union
  {
    uint16_t base_l;     // Segment
    selector_t selector; // Call/Interrupt/Trap/Task Gate
  };
  // Byte 4
  union
  {
    uint8_t base_m;      // Segment
    uint8_t param_cnt;   // Call Gate
    uint8_t reserved2;   // Interrupt/Trap/Task Gate (bits 5..7 need to be 0 for Interrupt and Trap)
  };
  // Byte 5
  uint8_t access;
  // Byte 6 and 7
  union
  {
    struct               // Segment
    {
      uint8_t attribs;
      uint8_t base_h;
    };
    uint16_t offset_h;   // Call/Interrupt/Trap Gate
    uint16_t reserved3;  // Task Gate
  };
} __attribute__ ((__packed__)) SDescriptor;

// Descriptor Table Register struct for:
//  - Interrupt Descriptor Table Register (IDTR)
//  - Global Descriptor Table Register    (GDTR)
typedef struct
{
  uint16_t limit;
  uint32_t base;
} __attribute__ ((__packed__)) SIDTR, SGDTR;

// Descriptor Table Register struct for:
//  - Local Descriptor Table Register     (LDTR)
//  - Task Register                       (TR)
typedef struct
{
  uint16_t limit;
  uint32_t base;
} __attribute__ ((__packed__)) SDescriptorTableReg;

// PDE Flags
#define PDE_FLAG_PRESENT       (1<<0) // Present
#define PDE_FLAG_WRITABLE      (1<<1) // Read/Write
#define PDE_FLAG_USER          (1<<2) // User/Supervisor
#define PDE_FLAG_WRITE_THROUGH (1<<3) // Write-Through
#define PDE_FLAG_CACHE_DISABLE (1<<4) // Cache Disabled
#define PDE_FLAG_ACCESSED      (1<<5) // Accessed
#define PDE_FLAG_RESERVED      (1<<6) // Reserved (set to 0)
#define PDE_FLAG_LARGE_SIZE    (1<<7) // Page Size (0=4KiB, 1=4MiB)
#define PDE_FLAG_GLOBAL        (1<<8) // Global Page (ignored)                - Since "Pentium Pro"
// PTE Flags
#define PTE_FLAG_PRESENT       (1<<0) // Present
#define PTE_FLAG_WRITABLE      (1<<1) // Read/Write
#define PTE_FLAG_USER          (1<<2) // User/Supervisor
#define PTE_FLAG_WRITE_THROUGH (1<<3) // Write-Through
#define PTE_FLAG_CACHE_DISABLE (1<<4) // Cache Disabled
#define PTE_FLAG_ACCESSED      (1<<5) // Accessed
#define PTE_FLAG_DIRTY         (1<<6) // Dirty
#define PTE_FLAG_PAT           (1<<7) // Page Table Attribute Index (PAT)
#define PTE_FLAG_GLOBAL        (1<<8) // Global Page                          - Since "Pentium Pro"

// Page Directory/Table Entry
typedef uint32_t  pde32_t;
typedef uint32_t  pte32_t;

// Page Directory/Table Entry with PAE enabled
typedef uint64_t  pdpte64_t;
typedef struct
{
  pdpte64_t entry[4];
} __attribute__ ((aligned (32))) pdpt64_t;
typedef uint64_t  pde64_t;
typedef uint64_t  pte64_t;

// CR0
#define CR0_PG           0x80000000
#define CR0_TS           0x00000008
// CR4
#define CR4_PSE          0x00000010
#define CR4_PAE          0x00000020

// Task State Segment
typedef struct
{
  uint16_t  link, res1;
  uint32_t  esp0;
  uint16_t  ss0, res2;
  uint32_t  esp1;
  uint16_t  ss1, res3;
  uint32_t  esp2;
  uint16_t  ss2, res4;
  uint32_t  cr3;
  uint32_t  eip;
  uint32_t  eflags;
  uint32_t  eax;
  uint32_t  ecx;
  uint32_t  edx;
  uint32_t  ebx;
  uint32_t  esp;
  uint32_t  ebp;
  uint32_t  esi;
  uint32_t  edi;
  uint16_t  es, res5;
  uint16_t  cs, res6;
  uint16_t  ss, res7;
  uint16_t  ds, res8;
  uint16_t  fs, res9;
  uint16_t  gs, resA;
  uint16_t  ldtr, resB;
  uint16_t  trace;
  uint16_t  io_map_addr;
} __attribute__ ((__packed__)) STaskStateSegment;


// I/O INSTRUCTIONS
inline void outb(unsigned char  data, unsigned short addr){__asm__ ("outb %%al,  %%dx"::"a" (data),"d" (addr));}
inline void outw(unsigned short data, unsigned short addr){__asm__ ("outw %%ax,  %%dx"::"a" (data),"d" (addr));}
inline void outd(unsigned long  data, unsigned short addr){__asm__ ("outl %%eax, %%dx"::"a" (data),"d" (addr));}
#define inb(port) ({ uint8_t _v;  __asm__ __volatile__ ("inb %%dx, %%al":"=a" (_v):"d" (port)); _v; })
#define inw(port) ({ uint16_t _v; __asm__ __volatile__ ("inw %%dx, %%ax":"=a" (_v):"d" (port)); _v; })
#define ind(port) ({ uint32_t _v; __asm__ __volatile__ ("inl %%dx,%%eax":"=a" (_v):"d" (port)); _v; })

// CONTROL REGISTERS
inline void setCR0(uint32_t data){ __asm__ ("movl %0, %%cr0"::"r" (data));}
inline void setCR1(uint32_t data){ __asm__ ("movl %0, %%cr1"::"r" (data));}
inline void setCR2(uint32_t data){ __asm__ ("movl %0, %%cr2"::"r" (data));}
inline void setCR3(uint32_t data){ __asm__ ("movl %0, %%cr3"::"r" (data));}
inline void setCR4(uint32_t data){ __asm__ ("movl %0, %%cr4"::"r" (data));}
#define getCR0() ({ uint32_t _v; __asm__ ("movl %%cr0, %%eax":"=a" (_v):); _v; })
#define getCR1() ({ uint32_t _v; __asm__ ("movl %%cr1, %%eax":"=a" (_v):); _v; })
#define getCR2() ({ uint32_t _v; __asm__ ("movl %%cr2, %%eax":"=a" (_v):); _v; })
#define getCR3() ({ uint32_t _v; __asm__ ("movl %%cr3, %%eax":"=a" (_v):); _v; })
#define getCR4() ({ uint32_t _v; __asm__ ("movl %%cr4, %%eax":"=a" (_v):); _v; })

// MEMORY-MANAGEMENT REGISTERS
inline void setTR(selector_t sel)             { __asm__ ("ltr   %0" ::"r" (sel));}
inline void setLDTR(selector_t sel)           { __asm__ ("lldt  %0 "::"r" (sel));}
inline void setGDTR(SDescriptorTableReg * dtr){ __asm__ ("lgdt (%0)"::"r" (dtr));}
inline void setIDTR(SDescriptorTableReg * dtr){ __asm__ ("lidt (%0)"::"r" (dtr));}

// Task Management
inline void jumpSelector(selector_t selector)
{
  static struct {
    uint32_t eip;
    uint16_t cs;
  } __attribute__ ((packed)) tss_link = {0, 0};
  tss_link.cs = selector;
  __asm__ __volatile__ ("ljmp *(%0)"::"m" (tss_link));
}
inline void callSelector(selector_t selector)
{
  static struct {
    uint32_t eip;
    uint16_t cs;
  } __attribute__ ((packed)) tss_link = {0, 0};
  tss_link.cs = selector;
  __asm__ __volatile__ ("lcall *(%0)"::"m" (tss_link));
}

// SEGMENT REGISTERS
inline void setDS(selector_t sel){ __asm__ ("movw %0, %%ds"::"r" (sel));}
inline void setSS(selector_t sel){ __asm__ ("movw %0, %%ss"::"r" (sel));}
inline void setES(selector_t sel){ __asm__ ("movw %0, %%es"::"r" (sel));}
inline void setFS(selector_t sel){ __asm__ ("movw %0, %%fs"::"r" (sel));}
inline void setGS(selector_t sel){ __asm__ ("movw %0, %%gs"::"r" (sel));}
#define getDS() ({ word _v; __asm__ ("movw %%ds, %%ax":"=a" (_v):); _v; })
#define getSS() ({ word _v; __asm__ ("movw %%ss, %%ax":"=a" (_v):); _v; })
#define getES() ({ word _v; __asm__ ("movw %%es, %%ax":"=a" (_v):); _v; })
#define getFS() ({ word _v; __asm__ ("movw %%fs, %%ax":"=a" (_v):); _v; })
#define getGS() ({ word _v; __asm__ ("movw %%gs, %%ax":"=a" (_v):); _v; })

// POINTER REGISTERS
inline void setESP(uint32_t addr){ __asm__ ("movl %0, %%esp"::"r" (addr));}
inline void setEBP(uint32_t addr){ __asm__ ("movl %0, %%ebp"::"r" (addr));}
#define getESP() ({ uint32_t _v; __asm__ __volatile__ ("movl %%esp, %%eax":"=a" (_v):); _v; })
#define getEBP() ({ uint32_t _v; __asm__ __volatile__ ("movl %%ebp, %%eax":"=a" (_v):); _v; })

// CPUID Function
inline void cpuid(int op, uint32_t * eax, uint32_t * ebx, uint32_t * ecx, uint32_t * edx)
{
  __asm__("cpuid" : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx) : "a" (op) : "cc");
}

#define getMSR(msr,val1,val2) __asm__ __volatile__ ("rdmsr" : "=a" (val1), "=d" (val2) : "c" (msr))
#define setMSR(msr,val1,val2) __asm__ __volatile__ ("wrmsr" :: "c" (msr), "a" (val1), "d" (val2))


#endif
