#include "kernel/bricks.h"
#include "asm/cpu.h"
#include "asm/irq.h"

#ifdef CONFIG_MULTITASKING
#include "kernel/task.h"
#include "timer.h"
#endif // CONFIG_MULTITASKING

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

#include "iostream"


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

  if(cIRQ.init() == -1)
    iRetVal = -1;

#ifdef CONFIG_GBA_CONSOLE
  if(cVideo.init() == -1)
    iRetVal = -1;
  CTaskManager::setStandardOutput(&cVideo);
#endif // CONFIG_GBA_CONSOLE

#ifdef CONFIG_GBA_KEYBOARD
  if(cKeyboard.init() == -1)
    iRetVal = -1;
  CTaskManager::setStandardInput(&cKeyboard);
#endif // CONFIG_GBA_KEYBOARD

#ifdef CONFIG_GBA_SERIAL
  if(cSerial.init() == -1)
    iRetVal = -1;
#endif // CONFIG_GBA_SERIAL

#ifdef CONFIG_NDS_IPC
  if(cIPC.init() == -1)
    iRetVal = -1;
#ifdef NDS7
  CTaskManager::setStandardOutput(&cIPC);
#endif // NDS7
#ifdef NDS9
  CTaskManager::setStandardInput(&cIPC);
#endif // NDS9
#endif // CONFIG_NDS_IPC

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CGBAVideoDevice;
#endif // CONFIG_FRAMEBUFFER

#ifdef CONFIG_MULTITASKING
  // Create task structure
  CTask * pTask = getNewTask(0, 0, 0);
  pTask->eState_ = TS_RUNNING;
  CTaskManager::addTask(pTask);

  setTimerFrequency(0, 100.0f);
  cIRQ.enable(3);
#endif // CONFIG_MULTITASKING

  return bricks_main();
}
