#include "string"


namespace std
{


// -----------------------------------------------------------------------------
string::string()
 : iLength_(0)
{
  string_[iLength_] = 0;
}

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
const char *
string::c_str() const
{
  return(string_);
}

// -----------------------------------------------------------------------------
bool
string::operator==(const string & string) const
{
  return(strcmp(string.string_));
}

// -----------------------------------------------------------------------------
bool
string::operator==(const char * string) const
{
  return(strcmp(string));
}

// -----------------------------------------------------------------------------
bool
string::strcmp(const char * string) const
{
  if(this->iLength_ != strlen(string))
  {
    return(false);
  }

  for(int i(0); i != this->iLength_; i++)
  {
    if(this->string_[i] != string[i])
    {
      return(false);
    }
  }

  return(true);
}

// -----------------------------------------------------------------------------
int
string::strlen(const char * string) const
{
  int iStringLength(0);

  while(string[iStringLength] == 0)
  {
    iStringLength++;
  }

  return(iStringLength);
}

// -----------------------------------------------------------------------------
ostream &
operator<<(ostream & os, const string & s)
{
  return(os<<s.c_str());
}


}
