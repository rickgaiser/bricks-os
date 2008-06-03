#include "i8042.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "hal.h"


#define I8042_KBD_PORT_NO 0
#define I8042_AUX_PORT_NO 1
#define I8042_MUX_PORT_NO 2
#define I8042_NUM_PORTS   (I8042_NUM_MUX_PORTS + 2)

#define MOUSE_BUTTON_LEFT   0x01
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_CENTER 0x04


// -----------------------------------------------------------------------------
C8042::C8042()
 : bMuxPresent_(false)
 , muxVersion_(0)
 , iMouseByteNr_(0)
{
}

// -----------------------------------------------------------------------------
C8042::~C8042()
{
}

// -----------------------------------------------------------------------------
int
C8042::init()
{
  CInterruptManager::attach(0x21, this); // Keyboard
  CInterruptManager::attach(0x2c, this); // Aux

  // Read CTR register
  if(command(&regCTR_, I8042_CMD_CTL_RCTR) == false)
  {
    printk("C8042: Unable to read CTR register\n");
    return -1;
  }

  // Enable keyboard port
  if(enableKeyboardPort() == false)
  {
    printk("C8042: Unable to enable keyboard port\n");
    return -1;
  }

  // Enable aux(mouse) port
  if(enableAuxPort() == false)
  {
    printk("C8042: Unable to enable aux port\n");
    return -1;
  }

  // Detect MUX
  if(setMuxMode(1, &muxVersion_) == 0)
  {
    if(muxVersion_ != 0xac) // Don't use MUX v10.12
    {
      printk("C8042: MUX Detected, v%d.%d\n", muxVersion_ >> 4, muxVersion_ & 0xf);
//      bMuxPresent_ = true;
    }
  }

  // Enable mux ports
  if(bMuxPresent_ == true)
  {
    if(enableMuxPorts() == false)
    {
      printk("C8042: Unable to enable mux port\n");
      return -1;
    }
  }

  //Tell the mouse to use default settings
  writeAux(0xf6);
  waitRead();
  readData();

  //Enable the mouse
  writeAux(0xf4);
  waitRead();
  readData();

  return 0;
}

// -----------------------------------------------------------------------------
int
C8042::isr(int irq)
{
  uint8_t status = readStatus();
  uint8_t data   = readData();
  uint8_t portNr;

  if(status & I8042_STR_AUXDATA)
  {
    if(bMuxPresent_ == true)
      portNr = I8042_MUX_PORT_NO + ((status >> 6) & 3);
    else
      portNr = I8042_AUX_PORT_NO;
  }
  else
    portNr = I8042_KBD_PORT_NO;

  //printk("C8042::isr: 0x%x @ port %d\n", data, portNr);

  if(portNr == I8042_KBD_PORT_NO)
  {
    // Keyboard
  }
  else if(portNr == I8042_AUX_PORT_NO)
  {
    // Mouse
    iMouseData_[iMouseByteNr_] = data;
    iMouseByteNr_++;

    if(iMouseByteNr_ == 3)
    {
      iMouseByteNr_ = 0;
      printk("Mouse keys=0x%x, x=%d, y=%d\n", iMouseData_[0], (int8_t)iMouseData_[1], (int8_t)iMouseData_[2]);
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
bool
C8042::enableKeyboardPort()
{
  regCTR_ &= ~I8042_CTR_KBDDIS;
  regCTR_ |=  I8042_CTR_KBDINT;

  return command(&regCTR_, I8042_CMD_CTL_WCTR);
}

// -----------------------------------------------------------------------------
bool
C8042::enableAuxPort()
{
  regCTR_ &= ~I8042_CTR_AUXDIS;
  regCTR_ |=  I8042_CTR_AUXINT;

  return command(&regCTR_, I8042_CMD_CTL_WCTR);
}

// -----------------------------------------------------------------------------
bool
C8042::enableMuxPorts()
{
  uint8_t param;

  for(int i(0); i < I8042_NUM_MUX_PORTS; i++)
  {
    command(&param, I8042_CMD_MUX_PFX + i);
    command(&param, I8042_CMD_AUX_ENABLE);
  }

  return enableAuxPort();
}

// -----------------------------------------------------------------------------
// checks whether the controller has an active
// multiplexor and puts the chip into Multiplexed (1) or Legacy (0) mode.
bool
C8042::setMuxMode(unsigned int mode, unsigned char * mux_version)
{
  uint8_t param;

  // Get rid of bytes in the queue.
  flush();

  // Internal loopback test - send three bytes, they should come back from the
  // mouse interface, the last should be version.
  param = 0xf0;
  if(!command(&param, I8042_CMD_AUX_LOOP) || param != 0xf0)
    return false;
  param = mode ? 0x56 : 0xf6;
  if(!command(&param, I8042_CMD_AUX_LOOP) || param != (mode ? 0x56 : 0xf6))
    return false;
  param = mode ? 0xa4 : 0xa5;
  if(!command(&param, I8042_CMD_AUX_LOOP) || param == (mode ? 0xa4 : 0xa5))
    return false;

  if(mux_version)
    *mux_version = param;

  return true;
}

// -----------------------------------------------------------------------------
bool
C8042::command(uint8_t * param, int command)
{
//  if(i8042_noloop && command == I8042_CMD_AUX_LOOP)
//    return false;

  if(waitWrite() == false)
    return false;
  writeCommand(command & 0xff);

  for(int i(0); i < ((command >> 12) & 0xf); i++)
  {
    if(waitWrite() == false)
      return false;
    writeData(param[i]);
  }

  for(int i(0); i < ((command >> 8) & 0xf); i++)
  {
    if(waitRead() == false)
      return false;
    param[i] = readData();
  }

  return true;
}

// -----------------------------------------------------------------------------
void
C8042::flush()
{
  for(int i(0); i < I8042_BUFFER_SIZE; i++)
    if(readStatus() & I8042_STR_OBF)
      readData();
}

// -----------------------------------------------------------------------------
bool
C8042::writeKeyboard(uint8_t data)
{
  if(waitWrite() == false)
    return false;

  writeData(data);

  return true;
}

// -----------------------------------------------------------------------------
bool
C8042::writeAux(uint8_t data)
{
  if(waitWrite() == false)
    return false;

  // AUX port
  writeCommand(0xd4);

  if(waitWrite() == false)
    return false;

  writeData(data);

  return true;
}

// -----------------------------------------------------------------------------
bool
C8042::writeMux(uint8_t port, uint8_t data)
{
  if(port >= I8042_NUM_MUX_PORTS)
    return false;

  if(waitWrite() == false)
    return false;

  // MUX port (0, 1, 2, or 3)
  writeCommand(0x90 + port);

  if(waitWrite() == false)
    return false;

  writeData(data);

  return true;
}

// -----------------------------------------------------------------------------
bool
C8042::waitRead()
{
  for(int iTimeout(0); iTimeout < I8042_CTL_TIMEOUT; iTimeout++)
  {
    if(readStatus() & I8042_STR_OBF)
      return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
bool
C8042::waitWrite()
{
  for(int iTimeout(0); iTimeout < I8042_CTL_TIMEOUT; iTimeout++)
  {
    if(~readStatus() & I8042_STR_IBF)
      return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
uint8_t
C8042::readData()
{
  return inb(I8042_DATA_REG);
}

// -----------------------------------------------------------------------------
uint8_t
C8042::readStatus()
{
  return inb(I8042_STATUS_REG);
}

// -----------------------------------------------------------------------------
void
C8042::writeData(uint8_t data)
{
  outb(data, I8042_DATA_REG);
}

// -----------------------------------------------------------------------------
void
C8042::writeCommand(uint8_t cmd)
{
  outb(cmd, I8042_COMMAND_REG);
}
