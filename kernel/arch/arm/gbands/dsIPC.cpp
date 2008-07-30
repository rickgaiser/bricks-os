#include "dsIPC.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/arch/registers.h"
#include "stddef.h"


// -----------------------------------------------------------------------------
CDSIPC::CDSIPC()
 : buffer_()
{
}

// -----------------------------------------------------------------------------
CDSIPC::~CDSIPC()
{
}

// -----------------------------------------------------------------------------
int
CDSIPC::init()
{
  // Register ISRs
  CInterruptManager::attach(16, this); // ipc
  CInterruptManager::attach(17, this); // send buf empty
  CInterruptManager::attach(18, this); // recv buf not empty

  // Enable interrupts from ARM7
//  REG_IPC_SYNC = IPC_SYNC_IRQ_ENABLE;

  // fifo init
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ | IPC_FIFO_ERROR; // IPC_FIFO_SEND_IRQ

  return 0;
}

// -----------------------------------------------------------------------------
int
CDSIPC::isr(int irq)
{
  uint8_t data;
  int iCount(0);

//  printk("CDSIPC::isr\n");

  switch(irq)
  {
    case 16:
      printk("CDSIPC::isr: IPC int received\n");
      break;
    case 17:
      printk("CDSIPC::isr: send buf empty int received\n");
      break;
    case 18:
      while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
      {
        data = REG_IPC_FIFO_RX;

        //printk("%c", data);
        if(buffer_.put(data) == false)
          printk("CDSIPC::isr: Buffer overflow\n");

        iCount++;
      }
      break;
    default:
      printk("CDSIPC::isr: ERROR: unknown interrupt\n");
  }

  if(iCount > 0)
    buffer_.notifyGetters();

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CDSIPC::read(void * buffer, size_t size, bool block)
{
  int iRetVal(0);
  uint8_t * data((uint8_t *)buffer);

  for(size_t i(0); i < size; i++)
  {
    if((i == 0) && (block == true))
      buffer_.get(data, true);
    else if(buffer_.get(data, false) == false)
      break;
    data++;
    iRetVal++;
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
ssize_t
CDSIPC::write(const void * buffer, size_t size, bool block)
{
  int iRetVal(0);

  for(size_t i(0); i < size; i++)
  {
    // Wait untill fifo is not full
    while(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL);

    // Send
    REG_IPC_FIFO_TX = ((char *)buffer)[i];

    iRetVal++;
  }

  return iRetVal;


  /*
  // --- FIFO TEST ---
  // fifo init
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;

  int count(0);
  while(true)
  {
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL))
    {
      // fifo send
      REG_IPC_FIFO_TX = ++count;

      // Wait
      for(int i(0); i != 1000000; i++){}
    }

    // Wait
    for(int i(0); i != 1000000; i++){}
  }
*/


/*
  // --- SHARED MEMORY TEST ---
  char * string("Hallo vanuit de ARM7 processor!");

  int i;
  for(i = 0; string[i] != 0; i++)
  {
    SH_MEM[i] = string[i];
  }
  SH_MEM[i] = 0;
*/

/*
  // --- IPC TRIGGER TEST ---
  // Enable interrupts from ARM9
  REG_IPC_SYNC = IPC_SYNC_IRQ_ENABLE;

  while(true)
  {
    // Trigger int
    REG_IPC_SYNC = IPC_SYNC_IRQ_REQUEST;
    // Wait
    for(int i(0); i != 1000000; i++){}
  }
*/
}
