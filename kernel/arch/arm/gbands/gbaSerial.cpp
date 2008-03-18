#include "gbaSerial.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"


//#define USE_INTERRUPTS


#ifdef USE_INTERRUPTS
  #define GET_REPLY() \
    while(bReceived_ == false); \
    reply = rcvData_; \
    bReceived_ = false
  #define GET_REPLY_NORMAL8()   GET_REPLY()
  #define GET_REPLY_NORMAL32()  GET_REPLY()
  #define GET_REPLY_MULTI()     GET_REPLY()
#else
  #define GET_REPLY_NORMAL8() \
    while(REG_SIOCNT & SIO_START); \
    reply = REG_SIODATA8
  #define GET_REPLY_NORMAL32() \
    while(REG_SIOCNT & SIO_START); \
    reply = REG_SIODATA32
  #define GET_REPLY_MULTI() \
    while(REG_SIOCNT & SIO_START); \
    if(iID_ == 0) reply = REG_SIOMULTI1; \
    else          reply = REG_SIOMULTI0
#endif // USE_INTERRUPTS


// -----------------------------------------------------------------------------
CGBASerial::CGBASerial()
 : bInitialized_(false)
 , bModeSet_(false)
 , bConnected_(false)
 , bReceived_(false)
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
  if(bInitialized_ == false)
  {
    CInterruptManager::attach(7, this);

    bInitialized_ = true;
  }

  return 0;
}

// -----------------------------------------------------------------------------
int
CGBASerial::isr(int irq)
{
  // Receive data
#ifdef USE_INTERRUPTS
  switch(eMode_)
  {
    case SIO_8BIT_MODE:
      rcvData_ = REG_SIODATA8;
      bReceived_ = true;
      break;
    case SIO_32BIT_MODE:
      rcvData_ = REG_SIODATA32;
      bReceived_ = true;
      break;
    case SIO_MULTI_MODE:
      switch(iID_)
      {
        case 0: rcvData_ = REG_SIOMULTI1; break;
        case 1: rcvData_ = REG_SIOMULTI0; break;
        case 2: rcvData_ = REG_SIOMULTI0; break;
        case 3: rcvData_ = REG_SIOMULTI0; break;
      };
      bReceived_ = true;
      break;
    case SIO_UART_MODE:
      if((REG_SIOCNT & SIO_UART_RECV_EMPTY) == false)
        rcvData_ = REG_SIODATA8;
      bReceived_ = true;
      break;
    default:
      ;
  };
#else
  if((eMode_ != SIO_UART_MODE) || (bModeSet_ == false))
    return 0;

  if((REG_SIOCNT & SIO_UART_RECV_EMPTY) == false)
    rcvData_ = REG_SIODATA8;
  bReceived_ = true;
#endif // USE_INTERRUPTS

  //printk("0x%x", rcvData_);

  return 0;
}

// -----------------------------------------------------------------------------
ssize_t
CGBASerial::write(const void * buffer, size_t size, loff_t *)
{
  if((eMode_ != SIO_UART_MODE) || (bModeSet_ == false))
    return -1;

  for(size_t i(0); i < size; i++)
  {
    while(REG_SIOCNT & SIO_UART_SEND_FULL){}
    REG_SIODATA8 = ((const uint8_t *)buffer)[i];
  }

  return size;
}

// -----------------------------------------------------------------------------
int
CGBASerial::setMode(ESerialMode mode)
{
  bModeSet_     = false;
  bConnected_   = false;
  eMode_        = mode;

  if(bInitialized_ == false)
    return -1;

  switch(eMode_)
  {
    case SIO_8BIT_MODE:
    case SIO_32BIT_MODE:
      // Initialize in connectNormal
      break;
    case SIO_MULTI_MODE:
      REG_RCNT    = 0;
#ifdef USE_INTERRUPTS
      REG_SIOCNT  = eMode_ | SIO_IRQ_ENABLE | SIO_115200_BPS;
#else
      REG_SIOCNT  = eMode_ | SIO_115200_BPS;
#endif // USE_INTERRUPTS
      REG_SIOCNT |= SIO_START;
      break;
    case SIO_UART_MODE:
      REG_RCNT    = 0;
      REG_SIOCNT  = eMode_ | SIO_IRQ_ENABLE | SIO_115200_BPS | SIO_UART_CTS_ENABLE | SIO_UART_8BIT | SIO_UART_SEND_ENABLE | SIO_UART_RECV_ENABLE /*| SIO_UART_FIFO_ENABLE*/;
      REG_SIOCNT |= SIO_START;
      break;
    default:
      return -1;
  };

  bModeSet_ = true;

  return 0;
}

// -----------------------------------------------------------------------------
int
CGBASerial::connectNormal(bool master)
{
  if(((eMode_ != SIO_8BIT_MODE) && (eMode_ != SIO_32BIT_MODE)) || (bModeSet_ == false))
    return -1;

  if(bConnected_ == true)
    return 0;

  bMaster_ = master;
  if(bMaster_ == true)
  {
    REG_RCNT    = 0;
#ifdef USE_INTERRUPTS
    REG_SIOCNT  = eMode_ | SIO_IRQ_ENABLE | SIO_NORMAL_CLK_256KHZ;
#else
    REG_SIOCNT  = eMode_ | SIO_NORMAL_CLK_256KHZ;
#endif // USE_INTERRUPTS
    REG_SIOCNT |= SIO_START;
  }
  else
  {
    REG_RCNT    = 0;
#ifdef USE_INTERRUPTS
    REG_SIOCNT  = eMode_ | SIO_IRQ_ENABLE | SIO_NORMAL_CLK_EXTERNAL;
#else
    REG_SIOCNT  = eMode_ | SIO_NORMAL_CLK_EXTERNAL;
#endif // USE_INTERRUPTS
    REG_SIOCNT |= SIO_START;
  }

  bConnected_ = true;

  return 0;
}

// -----------------------------------------------------------------------------
int
CGBASerial::connectMulti()
{
  if((eMode_ != SIO_MULTI_MODE) || (bModeSet_ == false))
    return -1;

  if(bConnected_ == true)
    return 0;

  if((REG_SIOCNT & SIO_MULTI_CONNECTED) == SIO_MULTI_CONNECTED)
  {
//    printk("Cable connected, we are the ");
    bMaster_ = ((REG_SIOCNT & SIO_MULTI_CHILD) != SIO_MULTI_CHILD);
//    if(bMaster_ == true)
//      printk("master\n");
//    else
//      printk("slave\n");

    bConnected_ = true;

    // Send to find out our id
    sendMulti(0);

    iID_ = (REG_SIOCNT & SIO_MULTI_ID_MASK) >> SIO_MULTI_ID_SHIFT;

    return 0;
  }

//  printk("Cable not connected\n");
  return -1;
}

// -----------------------------------------------------------------------------
bool
CGBASerial::isMaster()
{
  return bMaster_;
}

// -----------------------------------------------------------------------------
uint8_t
CGBASerial::sendNormal8(uint8_t data)
{
  uint8_t reply;

  if((eMode_ != SIO_8BIT_MODE) && (bConnected_ == true))
    return -1;

  if(bMaster_ == true)
  {
    // Place data to be sent
    REG_SIODATA8 = data;

    // Wait for slave to be ready (SI == 0)
    while((REG_SIOCNT & SIO_NORMAL_SI) != 0);

    // Set start flag
    REG_SIOCNT |= SIO_START;

    // Wait for, and get reply
    GET_REPLY_NORMAL8();
  }
  else
  {
    // Place data to be sent
    REG_SIODATA8 = data;

    // Set start flag & clear SO
    REG_SIOCNT = (REG_SIOCNT | SIO_START) & ~SIO_NORMAL_SO;

    // Wait for, and get reply
    GET_REPLY_NORMAL8();

    // Set SO
    REG_SIOCNT |= SIO_NORMAL_SO;
  }

  // Return replied data
  return reply;
}

// -----------------------------------------------------------------------------
uint32_t
CGBASerial::sendNormal32(uint32_t data)
{
  uint32_t reply;

  if((eMode_ != SIO_32BIT_MODE) && (bConnected_ == true))
    return -1;

  if(bMaster_ == true)
  {
    // Place data to be sent
    REG_SIODATA32 = data;

    // Wait for slave to be ready (SI == 0)
    while((REG_SIOCNT & SIO_NORMAL_SI) != 0);

    // Set start flag
    REG_SIOCNT |= SIO_START;

    // Wait for, and get reply
    GET_REPLY_NORMAL32();
  }
  else
  {
    // Place data to be sent
    REG_SIODATA32 = data;

    // Set start flag & clear SO
    REG_SIOCNT = (REG_SIOCNT | SIO_START) & ~SIO_NORMAL_SO;

    // Wait for, and get reply
    GET_REPLY_NORMAL32();

    // Set SO
    REG_SIOCNT |= SIO_NORMAL_SO;
  }

  // Return replied data
  return reply;
}

// -----------------------------------------------------------------------------
uint16_t
CGBASerial::sendMulti(uint16_t data)
{
  uint16_t reply;

  if((eMode_ != SIO_MULTI_MODE) && (bConnected_ == true))
    return -1;

  // Place data to be sent
  REG_SIOMLT_SEND = data;

  if(bMaster_ == true)
  {
    // Set start flag
    REG_SIOCNT |= SIO_START;
  }

  // Wait for, and get reply
  GET_REPLY_MULTI();

  // Return replied data
  return reply;
}
