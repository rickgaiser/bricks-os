#include "dsIPC.h"
#include "kernel/interruptManager.h"
#include "iostream"
#include "asm/arch/registers.h"


// -----------------------------------------------------------------------------
CDSIPC::CDSIPC()
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
  REG_IPC_SYNC = IPC_SYNC_IRQ_ENABLE;

  // fifo init
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ | IPC_FIFO_ERROR; // IPC_FIFO_SEND_IRQ

  return 0;
}

// -----------------------------------------------------------------------------
int
CDSIPC::isr(int irq)
{
  switch(irq)
  {
    case 16:
      std::cout<<"CDSIPC::isr: IPC int received"<<std::endl;
      break;
    case 17:
      std::cout<<"CDSIPC::isr: send buf empty int received"<<std::endl;
      break;
    case 18:
      while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
      {
        char c = REG_IPC_FIFO_RX;
        std::cout<<c;
      }
      break;
    default:
      std::cout<<"CDSIPC::isr: ERROR: unknown interrupt"<<std::endl;
  }

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CDSIPC::read (void * buffer, size_t size, loff_t *)
{
  return -1;
}

// -----------------------------------------------------------------------------
ssize_t
CDSIPC::write(const void * buffer, size_t size, loff_t *)
{
  int iRetVal(0);

  // When interrupts are disabled, this loop will hang, so check
  if(REG_IME != 0)
  {
    for(size_t i(0); i < size;)
    {
      if(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL))
      {
        // fifo send
        REG_IPC_FIFO_TX = ((char *)buffer)[i];
        i++;
      }
    }
  }
  else
  {
    iRetVal = -1;
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
