#include "sif.h"
#include "bios.h"
#include "stddef.h"
#include "asm/cpu.h"
#include "asm/arch/registers.h"


#define SIF_REG_MAINADDR         1
#define SIF_REG_SUBADDR          2
#define SIF_REG_MSFLAG           3
#define SIF_REG_SMFLAG           4


// -----------------------------------------------------------------------------
CIOP::CIOP()
{
}

// -----------------------------------------------------------------------------
CIOP::~CIOP()
{
}

// -----------------------------------------------------------------------------
int
CIOP::init()
{
  return 0;
}

// -----------------------------------------------------------------------------
bool
CIOP::alive()
{
  return ((bios::SifGetReg(SIF_REG_SMFLAG) & 0x10000) != 0);
}

// -----------------------------------------------------------------------------
bool
CIOP::sync()
{
  if(bios::SifGetReg(SIF_REG_SMFLAG) & 0x40000)
  {
    bios::SifSetReg(SIF_REG_SMFLAG, 0x40000);
    return true;
  }

  return false;
}
