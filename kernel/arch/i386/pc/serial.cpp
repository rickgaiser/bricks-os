#include "serial.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "hal.h"


// Register Offsets
#define I8250_DATA              0
#define I8250_INTERRUPT_ENABLE  1
#define I8250_INTERRUPT_ID      2 // Read
#define I8250_FIFO_CTRL         2 // Write
#define I8250_LINE_CTRL         3
#define I8250_MODEM_CTRL        4
#define I8250_LINE_STATUS       5
#define I8250_MODEM_STATUS      6
#define I8250_SCRATCH           7


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CI8250::CI8250(unsigned int baseAddr)
 : iBaseAddr_(baseAddr)
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
  uint8_t bits;

  outb(0xe7, iBaseAddr_ + I8250_FIFO_CTRL);
  bits = inb(iBaseAddr_ + I8250_INTERRUPT_ID);

  // Check if the chip supports fifo (bits 6&7)
  if((bits & (1<<6)) != 0)
  {
    if((bits & (1<<7)) != 0)
    {
      // Check for 64byte fifo
      if((bits & (1<<5)) != 0)
        printk("CI8250::init: 16750 detected\n");
      else
        printk("CI8250::init: 16550A detected\n");
    }
    else
      printk("CI8250::init: 16550 detected\n");
  }
  else
  {
    // Check if the chip supports scratch
    outb(0xaa, iBaseAddr_ + I8250_SCRATCH);
    bits = inb(iBaseAddr_ + I8250_SCRATCH);
    if(bits == 0xaa)
      printk("CI8250::init: 16450 detected\n");
    else
      printk("CI8250::init: 8250 detected\n");
  }

  return 0;
}

// -----------------------------------------------------------------------------
int
CI8250::isr(int irq)
{
  printk("CI8250::isr\n");

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CI8250::read(void * buffer, size_t size, loff_t *)
{
  printk("CI8250::read\n");

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CI8250::write(const void * buffer, size_t size, loff_t *)
{
  printk("CI8250::write\n");

  return 0;
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
