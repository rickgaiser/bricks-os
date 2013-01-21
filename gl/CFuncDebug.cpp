#include "CFuncDebug.h"
#include <stdio.h>


int CFuncDebug::iIndent_ = 0;


CFuncDebug::CFuncDebug(const char * sFunc) : sFunc_(sFunc)
{
  for(int i = 0; i < iIndent_; i++) printf(" ");
  printf(">> %s\n", sFunc_);
  iIndent_ += 2;
}

CFuncDebug::~CFuncDebug()
{
  iIndent_ -= 2;
  for(int i = 0; i < iIndent_; i++) printf(" ");
  printf("<< %s\n", sFunc_);
}
