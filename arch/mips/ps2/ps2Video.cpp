#include "ps2Video.h"

//#include <stdio.h>
//#include <tamtypes.h>
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

  return(0);
}

// -----------------------------------------------------------------------------
int
CPS2Video::write(const void * data, size_t size)
{
  scr_printf("%s", (char *)data);

  return(0);
}

// -----------------------------------------------------------------------------
void
CPS2Video::cls()
{
  scr_clear();
}
