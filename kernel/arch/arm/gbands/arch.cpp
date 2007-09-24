#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "kernel/task.h"
#include "asm/arch/config.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "timer.h"

#ifdef CONFIG_GBA_CONSOLE
#include "video.h"
#endif // CONFIG_GBA_CONSOLE

#ifdef CONFIG_GBA_KEYBOARD
#include "keyboard.h"
#endif // CONFIG_GBA_KEYBOARD

#ifdef CONFIG_GBA_SERIAL
#include "serial.h"
#endif // CONFIG_GBA_SERIAL

#ifdef CONFIG_NDS_IPC
#include "dsIPC.h"
#endif // CONFIG_NDS_IPC

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER

#ifdef CONFIG_FILESYSTEM
#include "kernel/fileSystem.h"
#include "kernel/ibmPartitionDriver.h"
#include "kernel/fatDriver.h"
#include "superCardDriver.h"
#endif // #ifdef CONFIG_FILESYSTEM


#ifdef GBA
extern char __iheap_start, __iwram_top;
extern char __eheap_start, __eheap_end;
#endif // GBA
#ifdef NDS7
extern char _end, __iwram_top;
#endif // NDS7
#ifdef NDS9
extern char _end, __eheap_end;
#endif // NDS9


CIRQ           cIRQ;

#ifdef CONFIG_GBA_CONSOLE
CGBAVideo      cVideo;
#endif // CONFIG_GBA_CONSOLE

#ifdef CONFIG_GBA_KEYBOARD
CGBAKeyboard   cKeyboard;
#endif // CONFIG_GBA_KEYBOARD

#ifdef CONFIG_GBA_SERIAL
CGBASerial     cSerial;
#endif // CONFIG_GBA_SERIAL

#ifdef CONFIG_NDS_IPC
CDSIPC         cIPC;
#endif // CONFIG_NDS_IPC

#ifdef CONFIG_FRAMEBUFFER
CGBAVideoDevice * pVideoDevice;
#endif // CONFIG_FRAMEBUFFER


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

#ifdef GBA
  //init_heap(&__iheap_start, &__iwram_top - &__iheap_start);
  init_heap(&__eheap_start, (uint32_t)(&__eheap_end - &__eheap_start));
#endif // GBA
#ifdef NDS7
  init_heap(&__iwram_top, (uint32_t)(&_end - &__iwram_top));
#endif // NDS7
#ifdef NDS9
  init_heap(&__eheap_end, (uint32_t)(&_end - &__eheap_end));
#endif // NDS9

  if(cIRQ.init() == -1)
    iRetVal = -1;

#ifdef CONFIG_GBA_CONSOLE
  if(cVideo.init() == -1)
    iRetVal = -1;
  pDebug = &cVideo;
#endif // CONFIG_GBA_CONSOLE

#ifdef CONFIG_GBA_KEYBOARD
  if(cKeyboard.init() == -1)
    iRetVal = -1;
  //CTaskManager::setStandardInput(&cKeyboard);
#endif // CONFIG_GBA_KEYBOARD

#ifdef CONFIG_GBA_SERIAL
  if(cSerial.init() == -1)
    iRetVal = -1;
#endif // CONFIG_GBA_SERIAL

#ifdef CONFIG_NDS_IPC
  if(cIPC.init() == -1)
    iRetVal = -1;
#ifdef NDS7
  pDebug = &cIPC;
#endif // NDS7
#ifdef NDS9
  //CTaskManager::setStandardInput(&cIPC);
#endif // NDS9
#endif // CONFIG_NDS_IPC

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CGBAVideoDevice;
#endif // CONFIG_FRAMEBUFFER

  // Create task structure
  CTask * pTask = new CTask(0, 0, 0);
  pTask->thr_->state(TS_RUNNING);

  setTimerFrequency(0, 100.0f);
  cIRQ.enable(3);

#ifdef CONFIG_FILESYSTEM
  CIBMPartitionDriver ibmPartitionDriver;
  CFATDriver fatDriver;
  CSuperCardDriver scDriver;

  CFileSystem::addPartitionDriver(&ibmPartitionDriver);
  CFileSystem::addFileSystemDriver(&fatDriver);
  scDriver.init();
  CFileSystem::addBlockDevice(&scDriver);
#endif // CONFIG_FILESYSTEM

  // Let CPU flag control interrupt state
  local_irq_disable();  // Disable in cpu interrupt enable flag
  REG_IME = 1;          // Enable REG_IME interrupt enable flag

  return bricks_main();
}
