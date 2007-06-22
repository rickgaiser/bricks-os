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

// Page Directory/Table Entry
typedef uint32_t  pde_t;
typedef uint32_t  pte_t;

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
inline void setESP(void * addr){ __asm__ ("movl %0, %%esp"::"r" (addr));}
inline void setEBP(void * addr){ __asm__ ("movl %0, %%ebp"::"r" (addr));}
#define getESP() ({ uint32_t _v; __asm__ __volatile__ ("movl %%esp, %%eax":"=a" (_v):); _v; })
#define getEBP() ({ uint32_t _v; __asm__ __volatile__ ("movl %%ebp, %%eax":"=a" (_v):); _v; })


#endif
