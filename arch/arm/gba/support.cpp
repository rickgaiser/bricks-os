#include "kernel/bricks.h"
#include "kernel/memoryManager.h"
#include "stddef.h"


// -----------------------------------------------------------------------------
void *
operator new(size_t size)
{
  return kmalloc(size);
}

// -----------------------------------------------------------------------------
void
operator delete(void * mem)
{
  kfree(mem);
}

// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  return CBricks::instance()->main();
}
