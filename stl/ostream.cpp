#include "ostream"
#include "unistd.h"


const char charMinus('-');
#define SIGNED_TO_UNSIGNED(i) \
if(i < 0){ \
  ::write(STDOUT, &charMinus, 1); \
  i = -i; \
}


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
  return *this;
}

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(char c)
{
  ::write(STDOUT, &c, 1);
  return *this;
}

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(int i)
{
  SIGNED_TO_UNSIGNED(i);
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
ostream::operator<<(long i)
{
  SIGNED_TO_UNSIGNED(i);
  return operator<<(static_cast<unsigned long>(i));
}

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(unsigned long i)
{
  bool bPrint(false);

  for(unsigned long iWalker(1000000000); iWalker > 0; i %= iWalker, iWalker /= 10)
  {
    unsigned long iTemp(i / iWalker);

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

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(long long i)
{
  SIGNED_TO_UNSIGNED(i);
  return operator<<(static_cast<unsigned long long>(i));
}

// -----------------------------------------------------------------------------
ostream &
ostream::operator<<(unsigned long long i)
{
  // FIXME
  return operator<<(static_cast<unsigned long>(i));
}


}
