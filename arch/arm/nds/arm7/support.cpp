#include "ds7Arch.h"
#include "../../common/processor.h"
#include "kernel/arch.h"
#include "kernel/bricks.h"


CDS7Arch    cArch;
CProcessor  cProcessor;


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  arch::pArch      = &cArch;
  arch::pProcessor = &cProcessor;

  return(CBricks::instance()->main());
}
