// Memory management, based on sample code from IBM:
// http://www-128.ibm.com/developerworks/linux/library/l-memory/

#include "kernel/memoryManager.h"


struct mem_control_block
{
  bool is_available;
  size_t size;
};


bool has_initialized(false);
char * managed_memory_start;
char * last_valid_address;


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
char *
sbrk(int incr)
{
  static char * pHeapTop = (char *)heaps[0].pStart;
  static char * pHeapEnd = (char *)heaps[0].pStart + heaps[0].iSize;
  char * ret = (char *)-1;

  if((pHeapTop + incr) <= pHeapEnd)
  {
    pHeapTop += incr;
    ret = pHeapTop;
  }

  return ret;
}

// -----------------------------------------------------------------------------
void
kmalloc_init()
{
  // grab the last valid address from the OS
  last_valid_address = sbrk(0);
  // we don't have any memory to manage yet, so
  // just set the beginning to be last_valid_address
  managed_memory_start = last_valid_address;
  // Okay, we're initialized and ready to go
  has_initialized = true;
}

// -----------------------------------------------------------------------------
void *
kmalloc(size_t numbytes)
{
  // Holds where we are looking in memory
  char * current_location;
  // This is the same as current_location, but cast to a
  // memory_control_block
  struct mem_control_block * current_location_mcb;
  // This is the memory location we will return.  It will
  // be set to 0 until we find something suitable
  char * memory_location;
  /* Initialize if we haven't already done so */
  if(has_initialized == false)
    kmalloc_init();
  // The memory we search for has to include the memory
  // control block, but the users of malloc don't need
  // to know this, so we'll just add it in for them.
  numbytes = numbytes + sizeof(struct mem_control_block);
  // Set memory_location to 0 until we find a suitable
  // location
  memory_location = 0;
  // Begin searching at the start of managed memory
  current_location = managed_memory_start;
  // Keep going until we have searched all allocated space
  while(current_location != last_valid_address)
  {
    // current_location and current_location_mcb point
    // to the same address.  However, current_location_mcb
    // is of the correct type, so we can use it as a struct.
    // current_location is a void pointer so we can use it
    // to calculate addresses.
    current_location_mcb = (struct mem_control_block *)current_location;
    if(current_location_mcb->is_available == true)
    {
      if(current_location_mcb->size >= numbytes)
      {
        // Woohoo!  We've found an open,
        // appropriately-size location.

        // It is no longer available
        current_location_mcb->is_available = false;
        // We own it
        memory_location = current_location;
        // Leave the loop
        break;
      }
    }
    // If we made it here, it's because the Current memory
    // block not suitable; move to the next one
    current_location = current_location + current_location_mcb->size;
  }

  // If we still don't have a valid location, we'll
  // have to ask the operating system for more memory
  if(memory_location == 0)
  {
    // Move the program break numbytes further
    sbrk(numbytes);
    // The new memory will be where the last valid
    // address left off
    memory_location = last_valid_address;
    // We'll move the last valid address forward
    // numbytes
    last_valid_address = last_valid_address + numbytes;
    // We need to initialize the mem_control_block
    current_location_mcb = (struct mem_control_block *)memory_location;
    current_location_mcb->is_available = false;
    current_location_mcb->size = numbytes;
  }
  // Now, no matter what (well, except for error conditions),
  // memory_location has the address of the memory, including
  // the mem_control_block
  // Move the pointer past the mem_control_block
  memory_location = memory_location + sizeof(struct mem_control_block);
  // Return the pointer
  return memory_location;
 }

// -----------------------------------------------------------------------------
void
kfree(void * firstbyte)
{
  struct mem_control_block * mcb;
  // Backup from the given pointer to find the
  // mem_control_block
  mcb = (struct mem_control_block *)((char *)firstbyte - sizeof(struct mem_control_block));
  // Mark the block as being available
  mcb->is_available = true;
  // That's It!  We're done.
  return;
}
