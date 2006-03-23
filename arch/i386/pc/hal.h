#ifndef HAL_H
#define HAL_H


typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned long      uint32_t;
typedef unsigned long long uint64_t;
typedef char               int8_t;
typedef short              int16_t;
typedef long               int32_t;
typedef long long          int64_t;


// 6th byte of a Descriptor
#define TASK_GATE 0x85
#define INT_GATE  0x8E
#define TRAP_GATE 0x8F
#define TYPE_TSS  0x89
#define CODE      0x98      // Execute only
#define CODER     0x9A      // Execute, Read
#define DATA      0x92      // Read/Write, Expand-Up
#define STACK     0x92      // Read/Write, Expand-up

// Page Directory/Table Entry
#define PDE       uint32_t
#define PTE       uint32_t


// (start) align structures to a byte boundary
#pragma pack (push, 1)

// Global Descriptor
typedef struct
{
  uint16_t limit, base_l;
  uint8_t  base_m, access, attribs, base_h;
} SGlobalDescriptor;

// Interrupt Desciptor
typedef struct
{
  uint16_t offset_l, selector;
  uint8_t  param_cnt, access;
  uint16_t offset_h;
} SInterruptDescriptor;

// Global Descriptor Table Register
// Local Descriptor Table Register
// Interrupt Descriptor Table Register
typedef struct
{
  uint16_t limit;
  uint32_t base;
} SDescriptorTableReg;

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
  uint32_t  cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
  uint16_t  es, res5,
            cs, res6,
            ss, res7,
            ds, res8,
            fs, res9,
            gs, resA,
            ldtr, resB;
  uint16_t  trace, io_map_addr;
} STaskStateSegment;

// (end) align structures to a byte boundary
#pragma pack (pop)


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
#define load_tr(data)  __asm__ ("ltr   %0" ::"r" (data))
#define load_ldt(data) __asm__ ("lltd  %0" ::"r" (data))
#define load_gdt(gdtr) __asm__ ("lgdt (%0)"::"r" (gdtr))
#define load_idt(idtr) __asm__ ("lidt (%0)"::"r" (idtr))
//inline void load_ldt(SDescriptorTableReg * dtr){ __asm__ ("lldt  %0 "::"r" (dtr));}
//inline void load_gdt(SDescriptorTableReg * dtr){ __asm__ ("lgdt (%0)"::"r" (dtr));}
//inline void load_idt(SDescriptorTableReg * dtr){ __asm__ ("lidt (%0)"::"r" (dtr));}

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
