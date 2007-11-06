#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "video.h"


CNGCVideo      cVideo;


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

  // Initialize the memory manager so we can use new/delete/malloc/free
  //init_heap(&__heap_start, (uint32_t)(&__heap_end - &__heap_start));

  if(cVideo.init() == -1)
    iRetVal = -1;
  pDebug = &cVideo;

//  printf("Bricks-OS\n");
//  while(1);

  return bricks_main();
}
