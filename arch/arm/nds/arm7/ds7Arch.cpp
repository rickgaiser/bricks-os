#include "ds7Arch.h"


extern IFileIO * pKeyboard;
extern IFileIO * pVideo;


// -----------------------------------------------------------------------------
CDS7Arch::CDS7Arch()
{
  pVideo    = &cIPC_;
  pKeyboard = &cKeyboard_;
}

// -----------------------------------------------------------------------------
CDS7Arch::~CDS7Arch()
{
}

// -----------------------------------------------------------------------------
int
CDS7Arch::init()
{
  int iRetVal(0);

  if(cIRQ_.init() == -1)
    iRetVal = -1;
  if(cKeyboard_.init() == -1)
    iRetVal = -1;
  if(cIPC_.init() == -1)
    iRetVal = -1;

  return(iRetVal);
}
