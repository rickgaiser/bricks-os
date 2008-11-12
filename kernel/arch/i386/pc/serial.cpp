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


#include "serial.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/hal.h"


// From Linux (include/linux/serial_reg.h)
#define UART_RX                 0 // In:  Receive buffer
#define UART_TX                 0 // Out: Transmit buffer
#define UART_IER                1 // Out: Interrupt Enable Register
#define UART_IIR                2 // In:  Interrupt ID Register
#define UART_FCR                2 // Out: FIFO Control Register
#define UART_LCR                3 // Out: Line Control Register
#define UART_LCR_DLAB           0x80 // Divisor latch access bit
#define UART_LCR_SBC            0x40 // Set break control
#define UART_LCR_SPAR           0x20 // Stick parity (?)
#define UART_LCR_EPAR           0x10 // Even parity select
#define UART_LCR_PARITY         0x08 // Parity Enable
#define UART_LCR_STOP           0x04 // Stop bits: 0=1 bit, 1=2 bits
#define UART_LCR_WLEN5          0x00 // Wordlength: 5 bits
#define UART_LCR_WLEN6          0x01 // Wordlength: 6 bits
#define UART_LCR_WLEN7          0x02 // Wordlength: 7 bits
#define UART_LCR_WLEN8          0x03 // Wordlength: 8 bits
#define UART_MCR                4 // Out: Modem Control Register
#define UART_LSR                5 // In:  Line Status Register
#define UART_LSR_TEMT           0x40 // Transmitter empty
#define UART_LSR_THRE           0x20 // Transmit-hold-register empty
#define UART_LSR_BI             0x10 // Break interrupt indicator
#define UART_LSR_FE             0x08 // Frame error indicator
#define UART_LSR_PE             0x04 // Parity error indicator
#define UART_LSR_OE             0x02 // Overrun error indicator
#define UART_LSR_DR             0x01 // Receiver data ready
#define UART_MSR                6 // In:  Modem Status Register
#define UART_SCR                7 // I/O: Scratch Register

#define UART_DLL                0 // Out: Divisor Latch Low
#define UART_DLM                1 // Out: Divisor Latch High

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)
#define whileTxNotReady() while((inb(iBaseAddr_ + UART_LSR) & BOTH_EMPTY) != BOTH_EMPTY)


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CI8250::CI8250(unsigned int baseAddr)
 : CAFileIOBufferedRead()
 , iBaseAddr_(baseAddr)
{
}

// -----------------------------------------------------------------------------
CI8250::~CI8250()
{
}

// -----------------------------------------------------------------------------
int
CI8250::init()
{
  // Disable interrupts
  outb(0x00, iBaseAddr_ + UART_IER);

  // Set DLAB on
  outb(0x80, iBaseAddr_ + UART_LCR);

  // Set baudrate to 9600
  uint16_t divisor = 115200 / 9600;
  outb(((divisor & 0x00ff)     ), iBaseAddr_ + UART_DLL);
  outb(((divisor & 0xff00) >> 8), iBaseAddr_ + UART_DLM);

  // 8 Bits, No Parity, 1 Stop Bit (Set DLAB off)
  outb(0x03, iBaseAddr_ + UART_LCR);

  // Enable fifo, interrupt after receiving 1 bytes
  outb(0x07, iBaseAddr_ + UART_FCR);

  // Set DTR + RTS + AUX2
  outb(0x0b, iBaseAddr_ + UART_MCR);

  // Enable data receive interrupt
  outb(0x01, iBaseAddr_ + UART_IER);

  return 0;
}

// -----------------------------------------------------------------------------
int
CI8250::isr(int irq)
{
  //printk("CI8250::isr\n");

  uint8_t status, data;
  int iCount(0);

  while(true)
  {
    status = inb(iBaseAddr_ + UART_LSR);
    // FIXME: What is status 0xff?
    if((status != 0xff) && ((status & UART_LSR_DR) == UART_LSR_DR))
    {
      data = inb(iBaseAddr_ + UART_RX);
      //printk("CI8250::read: status = 0x%x, data = %c\n", status, data);
      //printk("%c", data);
      if(bufferRead_.put(data) == false)
        printk("CI8250::isr: Buffer overflow\n");

      iCount++;
    }
    else
      break;
  }

  if(iCount > 0)
    bufferRead_.notifyGetters();

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CI8250::write(const void * buffer, size_t size, bool block)
{
  for(size_t i(0); i < size; i++)
  {
    whileTxNotReady();
    outb(((const char *)buffer)[i], iBaseAddr_ + UART_TX);
  }

  return size;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CI386Serial::CI386Serial()
 : cCom1_(0x03f8)
 , cCom2_(0x02f8)
 , cCom3_(0x03e8)
 , cCom4_(0x02e8)
{
}

// -----------------------------------------------------------------------------
CI386Serial::~CI386Serial()
{
  // Detach com ports from IRQ
  CInterruptManager::detach(0x24, &cCom1_); // IRQ4, COM1 & COM3
  CInterruptManager::detach(0x23, &cCom2_); // IRQ3, COM2 & COM4
  CInterruptManager::detach(0x24, &cCom3_); // IRQ4, COM1 & COM3
  CInterruptManager::detach(0x23, &cCom4_); // IRQ3, COM2 & COM4
}

// -----------------------------------------------------------------------------
int
CI386Serial::init()
{
  // Initialize com ports
  cCom1_.init();
  cCom2_.init();
  cCom3_.init();
  cCom4_.init();

  // Attach com ports to IRQ
  CInterruptManager::attach(0x24, &cCom1_); // IRQ4, COM1 & COM3
  CInterruptManager::attach(0x23, &cCom2_); // IRQ3, COM2 & COM4
  CInterruptManager::attach(0x24, &cCom3_); // IRQ4, COM1 & COM3
  CInterruptManager::attach(0x23, &cCom4_); // IRQ3, COM2 & COM4

  return 0;
}
