#include "drivers.h"
#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "stddef.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING

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


class CDrivers
{
public:
  CDrivers()
  {
  }

#ifdef CONFIG_NDS_IPC
  CDSIPC            cIPC;
#endif // CONFIG_NDS_IPC

#ifdef CONFIG_GBA_KEYBOARD
  CGBAKeyboard      cKeyboard;
#endif // CONFIG_GBA_KEYBOARD

#ifdef CONFIG_GBA_SERIAL
  CGBASerial        cSerial;
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
};


#if defined(CONFIG_DEBUGGING) && defined(NDS9)
extern CGBADebugScreen cDebugARM7;
#endif

CDrivers * pDrivers = NULL;


// -----------------------------------------------------------------------------
void
init_drivers()
{
  pDrivers = new CDrivers;
  if(pDrivers == NULL)
    panic("Out of memory!\n");

#ifdef CONFIG_NDS_IPC
#ifdef NDS7
  // ARM7 outputs debugging information to IPC
  pDrivers->cIPC.init();
  pDebug = &(pDrivers->cIPC);
#endif // NDS7
#ifdef NDS9
  // ARM9 displays incomming IPC data to bottom screen
  pDrivers->cIPC.init(&cDebugARM7);
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
