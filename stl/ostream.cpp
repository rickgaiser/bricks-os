#include "ostream"
#include "unistd.h"


namespace std
{


ostream cout;


// -----------------------------------------------------------------------------
ostream::ostream()
{
}

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(const char *cp)
{
  int iStrLen;

  for(iStrLen = 0; cp[iStrLen] != 0; iStrLen++){}

  ::write(STDOUT, cp, iStrLen);
  return(*this); 
}

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(char c)
{
  ::write(STDOUT, &c, 1);
  return(*this);
}

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(int i)
{
  return operator<<(static_cast<long>(i));
}

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(long i)
{
  if(i < 0)
  {
    char c('-');
    ::write(STDOUT, &c, 1);
    i = -i;
  }

  return operator<<(static_cast<unsigned int>(i));
}

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(unsigned int i)
{
  return operator<<(static_cast<unsigned long>(i));
}

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(unsigned long i)
{
  bool bPrint(false);

  for(int iWalker(1000000000); iWalker > 0; iWalker /= 10)
  {
    int iTemp;

    iTemp = i / iWalker;
    i     = i % iWalker;

    if(iTemp > 0)
      bPrint = true;

    if((bPrint == true) || (iWalker == 1))
    {
      char c('0' + iTemp);
      ::write(STDOUT, &c, 1);
    }
  }

  return *this;
}


}
