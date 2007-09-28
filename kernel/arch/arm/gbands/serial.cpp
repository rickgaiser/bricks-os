#include "serial.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"


// -----------------------------------------------------------------------------
CGBASerial::CGBASerial()
{
}

// -----------------------------------------------------------------------------
CGBASerial::~CGBASerial()
{
}

// -----------------------------------------------------------------------------
int
CGBASerial::init(ESerialMode mode)
{
  eMode_ = mode;

  switch(eMode_)
  {
    case SIO_8BIT_MODE:
    case SIO_32BIT_MODE:
    {
      REG_RCNT = 0;
      REG_SIOCNT = SIO_8BIT_MODE | SIO_NORMAL_CLK_EXTERNAL;
      REG_SIOCNT = SIO_8BIT_MODE | SIO_NORMAL_CLK_EXTERNAL | SIO_START;
      bMaster_ = ((REG_SIOCNT & SIO_NORMAL_SI) == 0);

      if(bMaster_ == true)
      {
        REG_SIOCNT = SIO_8BIT_MODE | SIO_IRQ_ENABLE |  SIO_NORMAL_CLK_256KHZ | SIO_NORMAL_SO;
        REG_SIOCNT = SIO_8BIT_MODE | SIO_IRQ_ENABLE |  SIO_NORMAL_CLK_256KHZ | SIO_NORMAL_SO | SIO_START;
        for(volatile int i(0); i < 100000; i++);
        REG_SIOCNT = SIO_8BIT_MODE | SIO_IRQ_ENABLE |  SIO_NORMAL_CLK_256KHZ;
        REG_SIOCNT = SIO_8BIT_MODE | SIO_IRQ_ENABLE |  SIO_NORMAL_CLK_256KHZ | SIO_START;
        printk("master\n");
      }
      else
      {
        REG_SIOCNT = SIO_8BIT_MODE | SIO_IRQ_ENABLE | SIO_NORMAL_CLK_EXTERNAL;
        REG_SIOCNT = SIO_8BIT_MODE | SIO_IRQ_ENABLE | SIO_NORMAL_CLK_EXTERNAL | SIO_START;
        printk("slave\n");
      }
    }
    case SIO_UART_MODE:
    {
      REG_RCNT = 0;
      REG_SIOCNT = eMode_ | SIO_IRQ_ENABLE | SIO_115200_BPS | SIO_UART_CTS_ENABLE | SIO_UART_8BIT | SIO_UART_SEND_ENABLE | SIO_UART_RECV_ENABLE /*| SIO_UART_FIFO_ENABLE*/;
      REG_SIOCNT |= SIO_START;
      break;
    }
    default:
      return -1;
  };

  CInterruptManager::attach(7, this);

  return 0;
}

// -----------------------------------------------------------------------------
int
CGBASerial::isr(int irq)
{
  uint8_t data('s');

  // Receive data
  switch(eMode_)
  {
    case SIO_8BIT_MODE:
      data = REG_SIODATA8;
      break;
    case SIO_32BIT_MODE:
      data = REG_SIODATA32;
      break;
    case SIO_UART_MODE:
      if((REG_SIOCNT & SIO_UART_RECV_EMPTY) == false)
        data = REG_SIODATA8;
      break;
    default:
      ;
  };

  printk("%c", data);

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CGBASerial::write(const void * buffer, size_t size, loff_t *)
{
  ssize_t iRetVal(0);
  int timeout;

  for(size_t i(0); i < size; i++)
  {
    switch(eMode_)
    {
      case SIO_8BIT_MODE:
      case SIO_32BIT_MODE:
      {
        // Place data to be sent
        if(eMode_ == SIO_8BIT_MODE)
          REG_SIODATA8  = ((const unsigned char *)buffer)[i];
        else
          REG_SIODATA32 = ((const unsigned char *)buffer)[i];

        // Wait for slave to be ready (SI == 0)
        for(timeout = 10000; (timeout > 0) && (REG_SIOCNT & SIO_NORMAL_SI); timeout--);
        if(timeout == 0)
        {
          printk("Timeout waiting for slave to be ready\n");
          return -1;
        }

        // Set start flag
        REG_SIOCNT |= SIO_START;

        // Wait for start bit to become zero (transmission complete)
        for(timeout = 10000; (timeout > 0) && (REG_SIOCNT & SIO_START); timeout--);
        if(timeout == 0)
        {
          printk("Timeout waiting for slave to be ready\n");
          return -1;
        }
      }
      case SIO_UART_MODE:
      {
        while(REG_SIOCNT & SIO_UART_SEND_FULL){}
        REG_SIODATA8 = ((const unsigned char *)buffer)[i];
        iRetVal++;
        break;
      }
      default:
        return -1;
    };
  }

  return iRetVal;
}
