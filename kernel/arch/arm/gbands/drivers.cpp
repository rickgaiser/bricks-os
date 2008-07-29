#include "drivers.h"
#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "kernel/fileDriver.h"
#include "stddef.h"

#include "debugScreen.h"

#ifdef CONFIG_NDS_IPC
#include "dsIPC.h"
#endif // CONFIG_NDS_IPC

#ifdef CONFIG_GBA_KEYBOARD
#include "keyboard.h"
#endif // CONFIG_GBA_KEYBOARD

#ifdef CONFIG_GBA_SERIAL
#include "gbaSerial.h"
#endif // CONFIG_GBA_SERIAL

#ifdef CONFIG_GBA_SOUND
#include "gbaSound.h"
#endif // CONFIG_GBA_SOUND

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER

#ifdef CONFIG_FILESYSTEM
#include "kernel/fileSystem.h"
#include "kernel/ibmPartitionDriver.h"
#include "kernel/fatDriver.h"
#include "superCardDriver.h"
#endif // #ifdef CONFIG_FILESYSTEM


#ifndef NDS7
extern CGBADebugScreen cDebug;
#endif // !NDS7
#ifdef NDS9
extern CGBADebugScreen cDebugARM7;
#endif // NDS9


class CDrivers
{
public:
  CDrivers();

public:
  int iDummyStart_;

#ifndef NDS7
  CKernelFileDriver fdDebug;
#endif // !NDS7
#ifdef NDS9
  CKernelFileDriver fdDebug2;
#endif // NDS9

#ifdef CONFIG_NDS_IPC
  CDSIPC            cIPC;
  CKernelFileDriver fdIPC;
#endif // CONFIG_NDS_IPC

#ifdef CONFIG_GBA_KEYBOARD
  CGBAKeyboard      cKeyboard;
  CKernelFileDriver fdKeyboard;
#endif // CONFIG_GBA_KEYBOARD

#ifdef CONFIG_GBA_SERIAL
  CGBASerial        cSerial;
  CKernelFileDriver fdSerial;
#endif // CONFIG_GBA_SERIAL

#ifdef CONFIG_GBA_SOUND
  CGBASound         cSound;
#endif // CONFIG_GBA_SOUND

#ifdef CONFIG_FRAMEBUFFER
  CGBAVideoDevice   cVideoDevice;
#endif // CONFIG_FRAMEBUFFER

#ifdef CONFIG_FILESYSTEM
  CIBMPartitionDriver cIBMPartitionDriver;
  CFATDriver        cFATDriver;
  CSuperCardDriver  cSCDriver;
#endif // CONFIG_FILESYSTEM

  int iDummyEnd_;
};


CDrivers * pDrivers = NULL;


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CDrivers::CDrivers()
 : iDummyStart_(0)
#ifndef NDS7
 , fdDebug(cDebug, "/dev/debug")
#endif // !NDS7
#ifdef NDS9
 , fdDebug2(cDebugARM7, "/dev/debug2")
#endif // NDS9
#ifdef CONFIG_NDS_IPC
 , cIPC()
 , fdIPC(cIPC, "/dev/ipc")
#endif // CONFIG_NDS_IPC
#ifdef CONFIG_GBA_KEYBOARD
 , cKeyboard()
 , fdKeyboard(cKeyboard, "/dev/keyboard")
#endif // CONFIG_GBA_KEYBOARD
#ifdef CONFIG_GBA_SERIAL
 , cSerial()
 , fdSerial(cSerial, "/dev/serial")
#endif // CONFIG_GBA_SERIAL
 , iDummyEnd_(0)
{
#ifdef CONFIG_NDS_IPC
#ifdef NDS7
  // ARM7 outputs debugging information to IPC
  pDrivers->cIPC.init();
  pDebug = &(pDrivers->cIPC);
#endif // NDS7
#ifdef NDS9
  // ARM9 displays incomming IPC data to bottom screen
  pDrivers->cIPC.init();
  pDrivers->cIPC.write("ready", 6);
#endif // NDS9
#endif // CONFIG_NDS_IPC

#ifdef CONFIG_GBA_KEYBOARD
  pDrivers->cKeyboard.init();
#endif // CONFIG_GBA_KEYBOARD

#ifdef CONFIG_GBA_SERIAL
  pDrivers->cSerial.init();
#endif // CONFIG_GBA_SERIAL

#ifdef CONFIG_GBA_SOUND
  pDrivers->cSound.init();
#endif // CONFIG_GBA_SOUND

#ifdef CONFIG_FILESYSTEM
  CFileSystem::addPartitionDriver(&(pDrivers->cIBMPartitionDriver));
  CFileSystem::addFileSystemDriver(&(pDrivers->cFATDriver));
  pDrivers->cSCDriver.init();
#endif // CONFIG_FILESYSTEM
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
