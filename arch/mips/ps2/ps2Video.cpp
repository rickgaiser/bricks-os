#include "ps2Video.h"
#include <kernel.h>
#include <debug.h>


// -----------------------------------------------------------------------------
CPS2Video::CPS2Video()
{
}

// -----------------------------------------------------------------------------
CPS2Video::~CPS2Video()
{
}

// -----------------------------------------------------------------------------
int 
CPS2Video::init()
{
  init_scr();

  this->cls();

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CPS2Video::write(const void * buffer, size_t size, loff_t *)
{
  for(size_t i(0); i < size; i++)
    scr_printf("%c", ((char *)buffer)[i]);

  return 0;
}

// -----------------------------------------------------------------------------
void
CPS2Video::cls()
{
  scr_clear();
}
