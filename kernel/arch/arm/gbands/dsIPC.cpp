/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "dsIPC.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "registers.h"
#include "stddef.h"


// -----------------------------------------------------------------------------
CDSIPC::CDSIPC()
 : CAFileIOBufferedRead()
 , bufferWrite_()
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
//  CInterruptManager::attach(16, this); // ipc
  CInterruptManager::attach(17, this); // send buf empty
  CInterruptManager::attach(18, this); // recv buf not empty

  // Enable interrupts from ARM7
//  REG_IPC_SYNC = IPC_SYNC_IRQ_ENABLE;

  // fifo init
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ | IPC_FIFO_ERROR | IPC_FIFO_SEND_IRQ;

  return 0;
}

// -----------------------------------------------------------------------------
int
CDSIPC::isr(int irq)
{
//  printk("CDSIPC::isr(%d)\n", irq);

  switch(irq)
  {
/*
    case 16:
    {
      // IPC
      printk("CDSIPC::isr: IPC\n");

      break;
    }
*/
    case 17:
    {
      // Send buffer empty
      //  - Fill fifo when there is data waiting to be sent
      //printk("CDSIPC::isr: send buf empty\n");

      uint8_t data;

      while((REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL) == 0)
      {
        if(bufferWrite_.get(&data) == false)
        {
          // Nothing more to send
          break;
        }

        // Send
        REG_IPC_FIFO_TX = data;
      }

      break;
    }
    case 18:
    {
      // Receive buffer not empty
      //  - Read data from fifo and place into read buffer
      //printk("CDSIPC::isr: receive buf not empty\n");

      uint8_t data;
      int iCount(0);

      while((REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY) == 0)
      {
        data = REG_IPC_FIFO_RX;

        //printk("%c", data);
        if(bufferRead_.put(data) == false)
          printk("CDSIPC::isr: Buffer overflow\n");

        iCount++;
      }

      if(iCount > 0)
        bufferRead_.notifyGetters();

      break;
    }
    default:
      printk("CDSIPC::isr: ERROR: unknown interrupt\n");
  }

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CDSIPC::write(const void * buffer, size_t size, bool block)
{
  size_t iRetVal(0);

#if 0
  for(size_t i(0); i < size; i++)
  {
    if(block == true)
    {
      // Wait untill fifo is not full
      while(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL);
    }
    else
    {
      // Break when fifo is full
      if(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL)
        break;
    }

    // Send
    REG_IPC_FIFO_TX = ((char *)buffer)[i];

    iRetVal++;
  }
#else
  // Place data in fifo
  while(iRetVal < size)
  {
    // Break when fifo is full
    if(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL)
      break;

    REG_IPC_FIFO_TX = ((char *)buffer)[iRetVal];

    iRetVal++;
  }

  // Place remaining data in buffer
  while(iRetVal < size)
  {
    // Break if unable to place data in buffer
    if(bufferWrite_.put(((char *)buffer)[iRetVal], block) == false)
      break;

    iRetVal++;
  }
#endif

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
