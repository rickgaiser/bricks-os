#include "dsCart.h"
#include "superCardDriver.h"
#include "kernel/debug.h"


// -----------------------------------------------------------------------------
CDSCart::CDSCart()
{
}

// -----------------------------------------------------------------------------
CDSCart::~CDSCart()
{
  CInterruptManager::detach(13, this);
}

// -----------------------------------------------------------------------------
int
CDSCart::init()
{
  CInterruptManager::attach(13, this);

  CSuperCardDriver * pDriver = new CSuperCardDriver();
  pDriver->init();
  CFileSystem::addBlockDevice(pDriver);

  return 0;
}

// -----------------------------------------------------------------------------
int
CDSCart::isr(int irq)
{
  printk("CDSCart::isr: gba cart ejected\n");

  return 0;
}
