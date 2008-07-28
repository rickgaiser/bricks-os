#include "drivers.h"
#include "i8042.h"
#include "keyboard.h"
#include "mouse.h"
#include "serial.h"
#include "asm/arch/config.h"
#include "kernel/debug.h"

#ifdef CONFIG_FILESYSTEM
#include "kernel/fileSystem.h"
#include "kernel/ibmPartitionDriver.h"
#include "kernel/fatDriver.h"
#include "ata.h"
#endif // #ifdef CONFIG_FILESYSTEM

#ifdef CONFIG_FRAMEBUFFER
#include "vesa.h"
#endif // CONFIG_FRAMEBUFFER


class CDrivers
{
public:
  CDrivers()
   : c8042()
   , cKeyboard(c8042)
   , cMouse(c8042)
  {
  }

  C8042             c8042;
  CI8042Keyboard    cKeyboard;
  CI8042Mouse       cMouse;
  CI386Serial       cSerial;

#ifdef CONFIG_FILESYSTEM
  CIBMPartitionDriver cIBMPartitionDriver;
  CFATDriver        cFATDriver;
  CATADriver        cATADriver;
#endif // CONFIG_FILESYSTEM

#ifdef CONFIG_FRAMEBUFFER
  CVesaVideoDevice  cVideoDevice;
#endif // CONFIG_FRAMEBUFFER
};


CDrivers * pDrivers = NULL;


// -----------------------------------------------------------------------------
void
init_drivers()
{
  pDrivers = new CDrivers;
  if(pDrivers == NULL)
    panic("Out of memory!\n");

  pDrivers->c8042.init();
  pDrivers->cKeyboard.init();
  pDrivers->cMouse.init();
  pDrivers->cSerial.init();

#ifdef CONFIG_FILESYSTEM
  CFileSystem::addPartitionDriver(&(pDrivers->cIBMPartitionDriver));
  CFileSystem::addFileSystemDriver(&(pDrivers->cFATDriver));
  pDrivers->cATADriver.init();
#endif // CONFIG_FILESYSTEM
}
