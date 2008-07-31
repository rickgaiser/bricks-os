#include "vu.h"
#include "dma.h"


// -----------------------------------------------------------------------------
void
vu1_wait()
{
  //2 nops before coprocessor 2 wait?
  asm __volatile__
  (
    "  nop\n"
    "  nop\n"
    "0:\n"
    "  bc2t 0b\n"
    "  nop\n"
    "  nop\n"
  );
}

// -----------------------------------------------------------------------------
void
vu_upload(void * dst, const void * src, unsigned int size)
{
  for(unsigned int i(0); i < (size + 8-1) / 8; i++)
  {
    *((uint64_t *)dst)++ = *((uint64_t *)src)++;
  }
}

// -----------------------------------------------------------------------------
void
vu1_run()
{
  asm __volatile__
  (
    "ctc2.i $0,$31"
  );
}
