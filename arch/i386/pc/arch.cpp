#include "kernel/bricks.h"
#include "kernel/task.h"
#include "asm/irq.h"
#include "video.h"
#include "keyboard.h"
#include "gdt.h"
#include "idt.h"


CIRQ             cIRQ;
CI386Video       cVideo;
CI386Keyboard    cKeyboard;
CGDT             cGDT;
CIDT             cIDT;


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

  if(cGDT.init() == -1)
    iRetVal = -1;
  if(cIDT.init() == -1)
    iRetVal = -1;
  if(cIRQ.init() == -1)
    iRetVal = -1;
  if(cVideo.init() == -1)
    iRetVal = -1;
  if(cKeyboard.init() == -1)
    iRetVal = -1;

  CTask::setStandardOutput(&cVideo);
  CTask::setStandardInput(&cKeyboard);

  return bricks_main();
}

