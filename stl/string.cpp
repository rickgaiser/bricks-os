#include "string"


namespace std
{


// -----------------------------------------------------------------------------
string::string(const char * string)
 : iLength_(0)
{
  while((string[iLength_] != 0) && (iLength_ < (STRING_SIZE - 1)))
  {
    string_[iLength_] = string[iLength_];
    iLength_++;
  }

  string_[iLength_] = 0;
}

// -----------------------------------------------------------------------------
ostream &
operator<<(ostream & os, const string & s)
{
  return (os<<s.c_str());
}


}
