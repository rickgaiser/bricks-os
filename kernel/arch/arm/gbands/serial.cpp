#include "serial.h"
#include "kernel/interruptManager.h"
#include "iostream"


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
CGBASerial::init(ESerialMode mode, bool master)
{
  eMode_ = mode;
  bMaster_ = master;

  switch(eMode_)
  {
    case SIO_8BIT_MODE:
    case SIO_32BIT_MODE:
    {
      REG_RCNT = 0;
      if(bMaster_ == true)
        REG_SIOCNT = eMode_ | SIO_IRQ_ENABLE | SIO_NORMAL_CLK_256KHZ;
      else
        REG_SIOCNT = eMode_ | SIO_IRQ_ENABLE | SIO_NORMAL_CLK_EXTERNAL;
      REG_SIOCNT |= SIO_START;
      break;
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
  switch(eMode_)
  {
    case SIO_8BIT_MODE:
    {
      std::cout<<(char)REG_SIODATA8;
      break;
    }
    case SIO_32BIT_MODE:
    {
      std::cout<<(char)REG_SIODATA32;
      break;
    }
    case SIO_UART_MODE:
    {
      if((REG_SIOCNT & SIO_UART_RECV_EMPTY) == false)
        std::cout<<(char)REG_SIODATA8;
      break;
    }
    default:
      ;
  };

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CGBASerial::write(const void * buffer, size_t size, loff_t *)
{
  ssize_t iRetVal(0);
  const unsigned char * pData = static_cast<const unsigned char *>(buffer);

  for(size_t i(0); i < size; i++)
  {
    switch(eMode_)
    {
      case SIO_8BIT_MODE:
      {
        // Wait for ACK (ready to send data)
        while(REG_SIOCNT & SIO_NORMAL_ACK_RECV){}
        // Place data to be sent
        REG_SIODATA8 = pData[i];
        // Set start flag
        REG_SIOCNT |= SIO_START;
        // Wait for transfer to complete
        while(REG_SIOCNT & SIO_BUSY){}
        iRetVal++;
        break;
      }
      case SIO_32BIT_MODE:
      {
        // Wait for ACK (ready to send data)
        while(REG_SIOCNT & SIO_NORMAL_ACK_RECV){}
        // Place data to be sent
        REG_SIODATA32 = pData[i];
        // Set start flag
        REG_SIOCNT |= SIO_START;
        // Wait for transfer to complete
        while(REG_SIOCNT & SIO_BUSY){}
        iRetVal++;
        break;
      }
      case SIO_UART_MODE:
      {
        while(REG_SIOCNT & SIO_UART_SEND_FULL){}
        REG_SIODATA8 = pData[i];
        iRetVal++;
        break;
      }
      default:
        return -1;
    };
  }

  return iRetVal;
}
