#ifndef HAL_H
#define HAL_H


#include "inttypes.h"


// 6th byte of a Descriptor
#define TASK_GATE      0x05
#define INT_GATE       0x0E
#define TRAP_GATE      0x0F
#define TYPE_TSS       0x09
#define CODE           0x08      // Execute only
#define CODER          0x0A      // Execute, Read
#define DATA           0x02      // Read/Write, Expand-Up
#define STACK          0x02      // Read/Write, Expand-up

#define SD_PRESENT     0x80
#define SD_DPL0        0x00
#define SD_DPL1        0x20
#define SD_DPL2        0x40
#define SD_DPL3        0x60
#define SD_SYSTEM      0x00
#define SD_CODE_DATA   0x10


// Page Directory/Table Entry
typedef uint32_t  PDE;
typedef uint32_t  PTE;

// Descriptor (for Global/Local Desctiptor Table (GDT/LDT))
typedef struct
{
  uint16_t limit;
  uint16_t base_l;
  uint8_t  base_m;
  uint8_t  access;
  uint8_t  attribs;
  uint8_t  base_h;
} __attribute__ ((__packed__)) SGlobalDescriptor;

// Desciptor (for Interrupt Descriptor Table (IDT))
typedef struct
{
  uint16_t offset_l;
  uint16_t selector;
  uint8_t  param_cnt;
  uint8_t  access;
  uint16_t offset_h;
} __attribute__ ((__packed__)) SInterruptDescriptor;

// Global Descriptor Table Register    (GDTR)
// Local Descriptor Table Register     (LDTR)
// Interrupt Descriptor Table Register (IDTR)
typedef struct
{
  uint16_t limit;
  uint32_t base;
} __attribute__ ((__packed__)) SDescriptorTableReg;

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
#define inb(port) ({ uint8_t _v;  __asm__ volatile ("inb %%dx, %%al":"=a" (_v):"d" (port)); _v; })
#define inw(port) ({ uint16_t _v; __asm__ volatile ("inw %%dx, %%ax":"=a" (_v):"d" (port)); _v; })
#define ind(port) ({ uint32_t _v; __asm__ volatile ("inl %%dx,%%eax":"=a" (_v):"d" (port)); _v; })

// PROCESSOR INSTRUCTIONS
inline void sti(){__asm__ ("sti"::);}
inline void cli(){__asm__ ("cli"::);}
inline void halt(){__asm__ ("hlt"::);}

// CONTROL REGISTERS
#define write_cr0(data) __asm__ ("movl %0, %%cr0"::"r" (data))
#define write_cr1(data) __asm__ ("movl %0, %%cr1"::"r" (data))
#define write_cr2(data) __asm__ ("movl %0, %%cr2"::"r" (data))
#define write_cr3(data) __asm__ ("movl %0, %%cr3"::"r" (data))
#define write_cr4(data) __asm__ ("movl %0, %%cr4"::"r" (data))
#define read_cr0() ({ uint32_t _v; __asm__ ("movl %%cr0, %%eax":"=a" (_v):); _v; })
#define read_cr1() ({ uint32_t _v; __asm__ ("movl %%cr1, %%eax":"=a" (_v):); _v; })
#define read_cr2() ({ uint32_t _v; __asm__ ("movl %%cr2, %%eax":"=a" (_v):); _v; })
#define read_cr3() ({ uint32_t _v; __asm__ ("movl %%cr3, %%eax":"=a" (_v):); _v; })
#define read_cr4() ({ uint32_t _v; __asm__ ("movl %%cr4, %%eax":"=a" (_v):); _v; })

// MEMORY-MANAGEMENT REGISTERS
#define setTR(data)  __asm__ ("ltr   %0" ::"r" (data))
#define setLDTR(data) __asm__ ("lltd  %0" ::"r" (data))
//#define setGDTR(gdtr) __asm__ ("lgdt (%0)"::"r" (gdtr))
//#define setIDTR(idtr) __asm__ ("lidt (%0)"::"r" (idtr))
//inline void setTR(data){ __asm__ ("ltr   %0" ::"r" (data));}
//inline void setLDTR(SDescriptorTableReg * dtr){ __asm__ ("lldt  %0 "::"r" (dtr));}
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
inline void jumpSelector(uint16_t selector){ __asm__ __volatile__ ("ljmp  %0"::"m"(*(((char *)&selector)-4)):"memory");}
inline void callSelector(uint16_t selector){ __asm__ __volatile__ ("lcall %0"::"m"(*(((char *)&selector)-4)):"memory");}

// SEGMENT REGISTERS
#define write_ds(sel) __asm__ ("movw %0, %%ds"::"r" (sel))
#define write_ss(sel) __asm__ ("movw %0, %%ss"::"r" (sel))
#define write_es(sel) __asm__ ("movw %0, %%es"::"r" (sel))
#define write_fs(sel) __asm__ ("movw %0, %%fs"::"r" (sel))
#define write_gs(sel) __asm__ ("movw %0, %%gs"::"r" (sel))
#define read_ds() ({ word _v; __asm__ ("movw %%ds, %%ax":"=a" (_v):); _v; })
#define read_ss() ({ word _v; __asm__ ("movw %%ss, %%ax":"=a" (_v):); _v; })
#define read_es() ({ word _v; __asm__ ("movw %%es, %%ax":"=a" (_v):); _v; })
#define read_fs() ({ word _v; __asm__ ("movw %%fs, %%ax":"=a" (_v):); _v; })
#define read_gs() ({ word _v; __asm__ ("movw %%gs, %%ax":"=a" (_v):); _v; })

// POINTER REGISTERS
#define write_esp(addr) __asm__ ("movl %0, %%esp"::"r" (addr))
#define write_ebp(addr) __asm__ ("movl %0, %%ebp"::"r" (addr))
#define read_esp() ({ uint32_t _v; __asm__ volatile ("movl %%esp, %%eax":"=a" (_v):); _v; })
#define read_ebp() ({ uint32_t _v; __asm__ volatile ("movl %%ebp, %%eax":"=a" (_v):); _v; })


#endif
