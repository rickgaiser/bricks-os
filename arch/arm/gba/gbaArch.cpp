#include "gbaArch.h"


extern IFileIO * pKeyboard;
extern IFileIO * pVideo;


// -----------------------------------------------------------------------------
CGBAArch::CGBAArch()
{
  pVideo    = &cVideo_;
  pKeyboard = &cKeyboard_;
}

// -----------------------------------------------------------------------------
CGBAArch::~CGBAArch()
{
}

// -----------------------------------------------------------------------------
int
CGBAArch::init()
{
  int iRetVal(0);

  if(cIRQ_.init() == -1)
    iRetVal = -1;
  if(cVideo_.init() == -1)
    iRetVal = -1;
  if(cKeyboard_.init() == -1)
    iRetVal = -1;

  return(iRetVal);
}
