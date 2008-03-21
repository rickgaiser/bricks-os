#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "kernel/task.h"
#include "asm/arch/config.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "timer.h"
#include "task.h"

#ifdef CONFIG_GBA_SOUND
#include "gbaSound.h"
#endif // CONFIG_GBA_SOUND

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_GBA_KEYBOARD
#include "keyboard.h"
#endif // CONFIG_GBA_KEYBOARD

#ifdef CONFIG_GBA_SERIAL
#include "gbaSerial.h"
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
#define HEAP_START ((uint32_t)(&__eheap_start))
#define HEAP_END   ((uint32_t)(&__eheap_end))
#elif NDS7
extern char _end, __iwram_top;
#define HEAP_START ((uint32_t)(&_end))
#define HEAP_END   ((uint32_t)(&__iwram_top))
#elif NDS9
extern char _end, __eheap_end;
#define HEAP_START ((uint32_t)(&_end))
#define HEAP_END   ((uint32_t)(&__eheap_end))
#endif


CIRQ           cIRQ;

#ifdef CONFIG_GBA_SOUND
CGBASound      cSound;
#endif // CONFIG_GBA_SOUND

#ifdef CONFIG_DEBUGGING
#if defined(GBA) || defined(NDS9)
CGBADebugScreen cDebug;
#ifdef NDS9
CGBADebugScreen cDebugARM7;
#endif // NDS9
#endif // defined(GBA) || defined(NDS9)
#endif // CONFIG_DEBUGGING

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


extern "C"
{
  void __gxx_personality_v0()
  {
  }
  void __aeabi_atexit()
  {
  }
  void __cxa_pure_virtual()
  {
  }
}

// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

  init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

  if(cIRQ.init() == -1)
    iRetVal = -1;

#ifdef CONFIG_DEBUGGING
#if defined(GBA) || defined(NDS9)
  if(cDebug.init() == -1)
    iRetVal = -1;
  pDebug = &cDebug;
#endif // defined(GBA) || defined(NDS9)
#endif // CONFIG_DEBUGGING

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
#ifdef NDS7
  if(cIPC.init() == -1)
    iRetVal = -1;
  pDebug = &cIPC;
#endif // NDS7
#ifdef NDS9
  if(cDebugARM7.init(BOTTOM_SCREEN) == -1)
    iRetVal = -1;
  if(cIPC.init(&cDebugARM7) == -1)
    iRetVal = -1;
#endif // NDS9
#endif // CONFIG_NDS_IPC

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CGBAVideoDevice;
#endif // CONFIG_FRAMEBUFFER

  CGBANDSThread::init();
  setTimerFrequency(2, 100.0f);
  cIRQ.enable(5);

#ifdef CONFIG_GBA_SOUND
  cSound.init();
#endif // CONFIG_GBA_SOUND

#ifdef CONFIG_FILESYSTEM
  CIBMPartitionDriver ibmPartitionDriver;
  CFATDriver fatDriver;
  CSuperCardDriver scDriver;

  CFileSystem::addPartitionDriver(&ibmPartitionDriver);
  CFileSystem::addFileSystemDriver(&fatDriver);
  scDriver.init();
#endif // CONFIG_FILESYSTEM

  // Let CPU flag control interrupt state
  local_irq_disable();  // Disable in cpu interrupt enable flag
  REG_IME = 1;          // Enable REG_IME interrupt enable flag

  printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);

  // Enable interrupts
  local_irq_enable();
  printk("Interrupts...OK\n");

#if defined(CONFIG_NDS_IPC) && defined(NDS9)
  // Notify ARM7 CPU that we are ready to receive debugging information
  cIPC.write("ready", 6);
#endif

#ifdef GBA
  printk("GBA arch ready\n");
#elif NDS7
  printk("NDS-ARM7 arch ready\n");
#elif NDS9
  printk("NDS-ARM9 arch ready\n");
#endif

  return bricks_main();
}
