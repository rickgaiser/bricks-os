#include "serial.h"
#include "videoDevice.h"
#include "asm/arch/registers.h"


//---------------------------------------------------------------------------
// Real dirty: Wait for line 200 TWICE
#define BUSY_WAIT_VSYNC() \
  while(REG_VI_HLINE != 200); \
  while(REG_VI_HLINE <= 200)

// -----------------------------------------------------------------------------
CNGCSerial::CNGCSerial()
{
}

// -----------------------------------------------------------------------------
CNGCSerial::~CNGCSerial()
{
}

// -----------------------------------------------------------------------------
int
CNGCSerial::init()
{
  REG_SI_POLL            = 0x00000000;
  REG_SI_STATUS          = 0x80000000;
  REG_SI_POLL            = 0x00f70200;
  REG_SI_STATUS          = 0x80000000;
  REG_SI_IO_BUFFER       = 0x00000000;
  REG_SI_COMCSR          = 0xc0010301;
  REG_SI_STATUS          = 0x00000000;

  (void)REG_SI_COMCSR;
  REG_SI_POLL |= 0xF0; // enable all four controller ports

  REG_SI_CHANNEL0_OUTBUF = 0x00400300;
  REG_SI_CHANNEL1_OUTBUF = 0x00400300;
  REG_SI_CHANNEL2_OUTBUF = 0x00400300;
  REG_SI_CHANNEL3_OUTBUF = 0x00400300;

  BUSY_WAIT_VSYNC();

  REG_SI_POLL            = 0x00000000;
  REG_SI_STATUS          = 0x80000000;
  REG_SI_POLL            = 0x00f70200;
  REG_SI_STATUS          = 0x80000000;
  REG_SI_IO_BUFFER       = 0x00000000;
  REG_SI_COMCSR          = 0xc0010301;
  REG_SI_STATUS          = 0x00000000;

  (void)REG_SI_COMCSR;
  REG_SI_POLL |= 0xF0; // enable all four controller ports

  REG_SI_CHANNEL0_OUTBUF = 0x00400300;
  REG_SI_CHANNEL1_OUTBUF = 0x00400300;
  REG_SI_CHANNEL2_OUTBUF = 0x00400300;
  REG_SI_CHANNEL3_OUTBUF = 0x00400300;

  return 0;
}
