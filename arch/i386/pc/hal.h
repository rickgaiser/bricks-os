#ifndef HAL_H
#define HAL_H


#include "inttypes.h"


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


#define PG_PRESENT       0x001
#define PG_WRITABLE      0x002
#define PG_USER          0x004
#define PG_WRITE_THROUGH 0x008
#define PG_DISABLE_CACH  0x010
#define PG_ACCESSED      0x020
#define PG_DIRTY         0x040
#define PG_LARGE_SIZE    0x080
#define PG_GLOBAL        0x100

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
inline void outw(unsigned short data, unsigned short addr){__asm__ ("outb %%ax,  %%dx"::"a" (data),"d" (addr));}
inline void outd(unsigned long  data, unsigned short addr){__asm__ ("outb %%eax, %%dx"::"a" (data),"d" (addr));}
#define inb(port) ({ uint8_t _v;  __asm__ __volatile__ ("inb %%dx, %%al":"=a" (_v):"d" (port)); _v; })
#define inw(port) ({ uint16_t _v; __asm__ __volatile__ ("inw %%dx, %%ax":"=a" (_v):"d" (port)); _v; })
#define ind(port) ({ uint32_t _v; __asm__ __volatile__ ("inl %%dx,%%eax":"=a" (_v):"d" (port)); _v; })

// PROCESSOR INSTRUCTIONS
inline void sti(){__asm__ ("sti"::);}
inline void cli(){__asm__ ("cli"::);}
inline void halt(){__asm__ ("hlt"::);}

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
/*
inline void jumpSelector(uint16_t selector)
{
  static struct {
  unsigned eip : 32; // 32 bit
  unsigned cs  : 16; // 16 bit
  } __attribute__ ((packed)) tss_link = {0, 0};
  // Set the TSS link
  tss_link.cs = selector;
  // Jump to the task
  __asm__ __volatile__ ("ljmp *(%0)"::"m" (tss_link));
}
*/
inline void jumpSelector(selector_t selector){ __asm__ __volatile__ ("ljmp  %0"::"m"(*(((char *)&selector)-4)):"memory");}
inline void callSelector(selector_t selector){ __asm__ __volatile__ ("lcall %0"::"m"(*(((char *)&selector)-4)):"memory");}

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
// CPUID presence detection
inline bool cpuidPresent()
{
  unsigned int f1, f2;

  __asm__("pushfl\n\t"
          "pushfl\n\t"
          "popl %0\n\t"
          "movl %0,%1\n\t"
          "xorl %2,%0\n\t"
          "pushl %0\n\t"
          "popfl\n\t"
          "pushfl\n\t"
          "popl %0\n\t"
          "popfl"
          : "=&r" (f1), "=&r" (f2)
          : "ir" (0x00200000));

  return ((f1^f2) & 0x00200000) != 0;
}
// CPUID Function
inline void cpuid(int op, uint32_t * eax, uint32_t * ebx, uint32_t * ecx, uint32_t * edx)
{
  __asm__("cpuid" : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx) : "a" (op) : "cc");
}


#endif
