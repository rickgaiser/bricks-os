#ifndef I386_MMAP_H
#define I386_MMAP_H


#include "inttypes.h"


#define PAGE_ALIGN_DOWN_4K(addr) ((uint32_t)addr & 0xfffff000)
#define PAGE_ALIGN_DOWN_2M(addr) ((uint32_t)addr & 0xffe00000)
#define PAGE_ALIGN_DOWN_4M(addr) ((uint32_t)addr & 0xffc00000)

#define PAGE_ALIGN_UP_4K(addr)   (((uint32_t)addr & 0x00000fff) ? (PAGE_ALIGN_DOWN_4K(addr) + 0x00001000) : ((uint32_t)addr))
#define PAGE_ALIGN_UP_2M(addr)   (((uint32_t)addr & 0x001fffff) ? (PAGE_ALIGN_DOWN_2M(addr) + 0x00200000) : ((uint32_t)addr))
#define PAGE_ALIGN_UP_4M(addr)   (((uint32_t)addr & 0x003fffff) ? (PAGE_ALIGN_DOWN_4M(addr) + 0x00400000) : ((uint32_t)addr))


void     init_mmap(uint8_t * addr, unsigned int pageCount);

uint64_t physAllocPage();
void     physFreePage(uint64_t addr);
uint64_t physAllocRange(uint64_t start, uint64_t length);
void     physFreeRange(uint64_t start, uint64_t length);

unsigned int usedPageCount();
unsigned int freePageCount();
unsigned int pageCount();


#endif

