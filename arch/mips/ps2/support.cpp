#include "ps2Arch.h"
#include "kernel/arch.h"
#include "kernel/bricks.h"


CPS2Arch    cArch;
//CProcessor  cProcessor;


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  arch::pArch      = &cArch;
  //arch::pProcessor = &cProcessor;

  return(CBricks::instance()->main());
}
