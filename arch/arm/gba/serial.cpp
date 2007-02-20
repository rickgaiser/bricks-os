#include "serial.h"
#include "asm/arch/registers.h"
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
CGBASerial::init()
{
  REG_RCNT = 0;
  REG_SIOCNT = SIO_UART_MODE | SIO_115200_BPS | SIO_CTS_ENABLE | SIO_UART_8BIT | SIO_TRANS_ENABLE | SIO_RECV_ENABLE | /* SIO_FIFO_ENABLE |*/ SIO_IF_ENABLE;
  CInterruptManager::attach(7, this);

  return 0;
}

// -----------------------------------------------------------------------------
int
CGBASerial::isr(int irq)
{
  std::cout<<"SERIAL Port interrupt: "<<std::endl;

  if((REG_SIOCNT & SIO_RECV_DATA_EMPTY) == false)
  {
    std::cout<<(char)REG_SIODATA8;
  }
  std::cout<<std::endl;
  
  return 0;
}

// -----------------------------------------------------------------------------
int
CGBASerial::write(const void * data, size_t size)
{
  const unsigned char * pData = static_cast<const unsigned char *>(data);

  for(int i(0); i < size; i++)
  {
    while((REG_SIOCNT & SIO_TRANS_DATA_FULL) == true)
      ;
    REG_SIODATA8 = pData[i];
  }
}
