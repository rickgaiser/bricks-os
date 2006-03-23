#include "ds9Arch.h"


extern IFileIO * pKeyboard;
extern IFileIO * pVideo;


// -----------------------------------------------------------------------------
CDS9Arch::CDS9Arch()
{
  pVideo    = &cDS9Video_;
  pKeyboard = &cDSKeyboard_;
}

// -----------------------------------------------------------------------------
CDS9Arch::~CDS9Arch()
{
}

// -----------------------------------------------------------------------------
int
CDS9Arch::init()
{
  int iRetVal(0);

  if(cDSIRQ_.init() == -1)
    iRetVal = -1;
//  if(cDSTimerManager_.init() == -1)
//    iRetVal = -1;
  if(cDS9Video_.init() == -1)
    iRetVal = -1;
  if(cDSKeyboard_.init() == -1)
    iRetVal = -1;
//  if(cDSCart_.init() == -1)
//    iRetVal = -1;
  if(cDSIPC_.init() == -1)
    iRetVal = -1;

  return(iRetVal);
}
