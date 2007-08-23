#include "mmap.h"
#include "string.h"


uint8_t       * pMemMap = 0;
unsigned int    iPageCount(0);


// -----------------------------------------------------------------------------
void
init_mmap(uint8_t * addr, unsigned int pageCount)
{
  pMemMap    = addr;
  iPageCount = pageCount;

  memset(pMemMap, 1, iPageCount);
}

// -----------------------------------------------------------------------------
uint64_t
physAllocPage()
{
  // Locate 
  for(unsigned int index(0); index < iPageCount; index++)
  {
    if(pMemMap[index] == 0)
    {
      pMemMap[index] = 1;
      return (index << 12);
    }
  }
  
  return 0;
}

// -----------------------------------------------------------------------------
void
physFreePage(uint64_t addr)
{
  addr >>= 12;
  
  if(addr < iPageCount)
    pMemMap[addr] = 0;
}

// -----------------------------------------------------------------------------
uint64_t
physAllocRange(uint64_t start, uint64_t length)
{
  unsigned int index = PAGE_ALIGN_DOWN_4K(start) >> 12;
  unsigned int end = PAGE_ALIGN_UP_4K(start + length) >> 12;
  
  // Validate range
  if(end > iPageCount)
    return 0;

  // Allocate
  while(index < end)
    pMemMap[index++] = 1;
  
  return start;
}

// -----------------------------------------------------------------------------
void
physFreeRange(uint64_t start, uint64_t length)
{
  unsigned int index = PAGE_ALIGN_DOWN_4K(start) >> 12;
  unsigned int end = PAGE_ALIGN_DOWN_4K(start + length) >> 12;
  
  // Validate range
  if(end > iPageCount)
    return;

  // Free
  while(index < end)
    pMemMap[index++] = 0;
}

// -----------------------------------------------------------------------------
unsigned int
usedPageCount()
{
  unsigned int count(0);
  
  for(unsigned int index(0); index < iPageCount; index++)
    if(pMemMap[index] != 0)
      count++;
  
  return count;
}

// -----------------------------------------------------------------------------
unsigned int
freePageCount()
{
  unsigned int count(0);
  
  for(unsigned int index(0); index < iPageCount; index++)
    if(pMemMap[index] == 0)
      count++;
  
  return count;
}

// -----------------------------------------------------------------------------
unsigned int
pageCount()
{
  return iPageCount;
}

