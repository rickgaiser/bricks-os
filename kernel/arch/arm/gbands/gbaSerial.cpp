#include "gbaSerial.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "string.h"


//#define USE_INTERRUPTS
#define TIMEOUT_COUNT 10000


struct MultiBootParam
{
  uint32_t   reserved1[5];
  uint8_t    handshake_data;
  uint8_t    padding;
  uint16_t   handshake_timeout;
  uint8_t    probe_count;
  uint8_t    client_data[3];
  uint8_t    palette_data;
  uint8_t    response_bit;
  uint8_t    client_bit;
  uint8_t    reserved2;
  uint8_t  * boot_srcp;
  uint8_t  * boot_endp;
  uint8_t  * masterp;
  uint8_t  * reserved3[3];
  uint32_t   system_work2[4];
  uint8_t    sendflag;
  uint8_t    probe_target_bit;
  uint8_t    check_wait;
  uint8_t    server_type;
};

extern "C" uint32_t MultiBoot(MultiBootParam * mp, uint32_t mode);

enum MULTIBOOT_MODES
{
  MODE32_NORMAL,
  MODE16_MULTI,
  MODE32_2MHZ
};

MultiBootParam mbp;
uint8_t * pHeader = (uint8_t *)0x8000000;
uint8_t * pData   = (uint8_t *)0x80000c0;

char * bootModes[] =
{
  "NULL",
  "Joybus",
  "Normal",
  "Multiplay",
};


#ifdef USE_INTERRUPTS
  #define GET_REPLY() \
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (bReceived_ == false); timeout--); \
    if(timeout != 0) \
    { \
      reply = rcvData_; \
      bReceived_ = false; \
    }
  #define GET_REPLY_NORMAL8()   GET_REPLY()
  #define GET_REPLY_NORMAL32()  GET_REPLY()
  #define GET_REPLY_MULTI()     GET_REPLY()
#else
  #define GET_REPLY_NORMAL8() \
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (REG_SIOCNT & SIO_START); timeout--); \
    if(timeout != 0) \
      reply = REG_SIODATA8
  #define GET_REPLY_NORMAL32() \
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (REG_SIOCNT & SIO_START); timeout--); \
    if(timeout != 0) \
      reply = REG_SIODATA32
  #define GET_REPLY_MULTI() \
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (REG_SIOCNT & SIO_START); timeout--); \
    if(timeout != 0) \
    { \
      if(iID_ == 0) reply = REG_SIOMULTI1; \
      else          reply = REG_SIOMULTI0; \
    }
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
#ifdef USE_INTERRUPTS
    CInterruptManager::attach(7, this);
#endif

    bInitialized_ = true;
  }

  return 0;
}

// -----------------------------------------------------------------------------
int
CGBASerial::isr(int irq)
{
  if(bReceived_ == true)
    printk("too fast!\n");

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

  //printk("%c\n", rcvData_);

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
#ifdef USE_INTERRUPTS
      REG_SIOCNT  = eMode_ | SIO_IRQ_ENABLE | SIO_115200_BPS | SIO_UART_CTS_ENABLE | SIO_UART_8BIT | SIO_UART_SEND_ENABLE | SIO_UART_RECV_ENABLE /*| SIO_UART_FIFO_ENABLE*/;
#else
      REG_SIOCNT  = eMode_ | SIO_115200_BPS | SIO_UART_CTS_ENABLE | SIO_UART_8BIT | SIO_UART_SEND_ENABLE | SIO_UART_RECV_ENABLE /*| SIO_UART_FIFO_ENABLE*/;
#endif // USE_INTERRUPTS
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

    return iID_;
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
  uint8_t reply(0);
  vuint32_t timeout;

  if((eMode_ != SIO_8BIT_MODE) && (bConnected_ == true))
    return -1;

  if(bMaster_ == true)
  {
    // Place data to be sent
    REG_SIODATA8 = data;

    // Wait for slave to be ready (SI == 0)
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (REG_SIOCNT & SIO_NORMAL_SI); timeout--); \

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
  uint32_t reply(0);
  vuint32_t timeout;

  if((eMode_ != SIO_32BIT_MODE) && (bConnected_ == true))
    return -1;

  if(bMaster_ == true)
  {
    // Place data to be sent
    REG_SIODATA32 = data;

    // Wait for slave to be ready (SI == 0)
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (REG_SIOCNT & SIO_NORMAL_SI); timeout--); \

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
  uint16_t reply(-1);
  vuint32_t timeout;

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

  // Delay 36us
  // FIXME: How much is this?
  for(vuint32_t i(0); i < 500; i++);

  // Return replied data
  return reply;
}

// -----------------------------------------------------------------------------
int
CGBASerial::multiBoot()
{
  uint8_t bootMode = *((uint8_t *)0x20000c4);
  uint8_t deviceID = *((uint8_t *)0x20000c5);

  printk("Boot mode: %s\n", bootModes[bootMode & 0x03]);
  printk("Device ID: %d\n", deviceID);

  if(deviceID != 0)
  {
    printk("invalid device\n");
    return -1;
  }

  if(bInitialized_ == false)
  {
    printk("not initialized\n");
    return -1;
  }

  if(setMode(SIO_MULTI_MODE) == -1)
  {
    printk("unable to set multi mode\n");
    return -1;
  }

  bMaster_ = true;
  iID_ = 0;

  REG_RCNT = 0; // non-general purpose comms

  uint16_t data;
  int client_bit(-1);

  // Wait for slave to enter multiplay/normal mode
  while(true)
  {
    data = sendMulti(0x6200);
    if(data == 0x0000)
    {
      break;
    }
    else
    {
      printk("Wait: Got 0x%x\n", data);
      for(vuint32_t i(0); i < 50000; i++);
    }
  }

  // Detect slaves
  for(int i(0); i < 15; i++)
  {
    uint16_t data = sendMulti(0x6200);
    if((data & 0xfff0) == 0x7200)
    {
      client_bit = data & 0x000f;
      printk("Slave %d found\n", client_bit);
      break;
    }
    else
    {
      printk("Detect: Got 0x%x\n", data);
      // Wait 1/16 sec
      // FIXME: How much is this?
      for(vuint32_t i(0); i < 50000; i++);
    }
  }
  if(client_bit == -1)
  {
    printk("No slaves!\n");
    return -1;
  }

  data = sendMulti(0x6100 + client_bit);
  if(data == (0x7200 + client_bit))
    printk("Slave ACK ok\n");
  else
  {
    printk("Slave ACK error (0x%x)\n", data);
    return -1;
  }

  // Send multiboot header
  for(int i(0); i < 0x60; i++)
  {
    data = sendMulti((pHeader[i*2+1] << 8) + pHeader[i*2]);
    if(data != (((0x60 - i) << 8) | client_bit))
    {
      printk("header invalid response (0x%x)\n", data);
      return -1;
    }
  }

  data = sendMulti(0x6200);
  if(data == (0x0000 + client_bit))
    printk("Slave ACK ok\n");
  else
  {
    printk("Slave ACK error (0x%x)\n", data);
    return -1;
  }

  data = sendMulti(0x6200 + client_bit);
  if(data == 0x7200 + client_bit)
    printk("Slave ACK ok\n");
  else
  {
    printk("Slave ACK error (0x%x)\n", data);
    return -1;
  }

  uint8_t palette_data(0xc1);
  uint8_t client_data(0);
  printk("Slave palette mode...");
  while(true)
  {
    data = sendMulti(0x6300 + palette_data);
    if((data & 0xff00) == 0x7300)
    {
      client_data = data & 0x00ff;
      break;
    }
  }
  printk("ok\n");

  uint8_t handshake_data = 0x11 + client_data + 0xff + 0xff;
  data = sendMulti(0x6400 + handshake_data);
  if((data & 0xff00) == 0x7300)
    printk("Slave ACK ok\n");
  else
  {
    printk("Slave ACK error (0x%x)\n", data);
    return -1;
  }

  printk("Sending...");

  uint32_t length = (256*1024) - 0xc0;
  length = (length + 0xf) & ~0xf; // 16 byte units
  if(length < 0x1c0)
   length = 0x1c0;

  memset(&mbp, 0, sizeof(MultiBootParam));
  mbp.client_bit     = client_bit;
  mbp.probe_count    = 0xd1;
  mbp.boot_srcp      = pData;
  mbp.boot_endp      = (uint8_t *)pData + length;
  mbp.palette_data   = palette_data;
  mbp.client_data[0] = client_data;
  mbp.client_data[1] = 0xff;
  mbp.client_data[2] = 0xff;
  mbp.handshake_data = handshake_data;

  if(MultiBoot(&mbp, MODE16_MULTI) != 0)
  {
    printk("failed to boot\n");
    return -1;
  }

  return 0;
}
