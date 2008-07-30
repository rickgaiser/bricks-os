#include "drivers.h"
#include "debugScreen.h"
#include "i8042.h"
#include "keyboard.h"
#include "mouse.h"
#include "serial.h"
#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "kernel/fileDriver.h"

#ifdef CONFIG_FILESYSTEM
#include "kernel/fileSystem.h"
#include "kernel/ibmPartitionDriver.h"
#include "kernel/fatDriver.h"
#include "ata.h"
#endif // #ifdef CONFIG_FILESYSTEM

#ifdef CONFIG_FRAMEBUFFER
#include "vesa.h"
#endif // CONFIG_FRAMEBUFFER


#ifdef CONFIG_DEBUGGING
extern CI386DebugScreen  cDebug;
#endif // #ifdef CONFIG_DEBUGGING


class CDrivers
{
public:
  CDrivers();

public:
  CKernelFileDriver fdDebug;

  C8042             c8042;
  CI8042Keyboard    cKeyboard;
  CKernelFileDriver fdKeyboard;
  CI8042Mouse       cMouse;
  CKernelFileDriver fdMouse;
  CI386Serial       cSerial;
  CKernelFileDriver fdSerial;

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
// -----------------------------------------------------------------------------
CDrivers::CDrivers()
 : fdDebug(cDebug, "/dev/debug")
 , c8042()
 , cKeyboard(c8042)
 , fdKeyboard(cKeyboard, "/dev/keyboard")
 , cMouse(c8042)
 , fdMouse(cMouse, "/dev/mouse")
 , cSerial()
 , fdSerial(cSerial.cCom1_, "/dev/serial")
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
