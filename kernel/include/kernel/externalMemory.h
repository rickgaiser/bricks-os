#ifndef EXTERNALMEMORY_H
#define EXTERNALMEMORY_H


#include "kernel/queue.h"
#include "inttypes.h"
#include "stddef.h"


//---------------------------------------------------------------------------
struct SExternalMemoryBlock
{
  uint32_t base; // Base addr of memory allocation
  uint32_t size; // Size in bytes of memory allocation
  bool     allocated;
  TAILQ_ENTRY(SExternalMemoryBlock) queueEntry;
};

TAILQ_HEAD(SExternalMemoryBlockQueue, SExternalMemoryBlock);

//---------------------------------------------------------------------------
class CExternalMemory
{
public:
  CExternalMemory(uint32_t base, uint32_t size, uint32_t blockSize);
  ~CExternalMemory();

  void * malloc(uint32_t size);
  void   free(void * base);

private:
  SExternalMemoryBlockQueue queueHead_;
  uint32_t base_;
  uint32_t size_;
  uint32_t blockSize_;
};


#endif
