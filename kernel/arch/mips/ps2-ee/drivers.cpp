#include "drivers.h"
#include "asm/arch/config.h"
#include "kernel/debug.h"

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER


class CDrivers
{
public:
  CDrivers();

public:
#ifdef CONFIG_FRAMEBUFFER
  CPS2VideoDevice cVideoDevice;
#endif // CONFIG_FRAMEBUFFER
};


CDrivers * pDrivers = NULL;


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CDrivers::CDrivers()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
init_drivers()
{
  pDrivers = new CDrivers;
  if(pDrivers == NULL)
    panic("Out of memory!\n");
}
