#include "string.h"


// -----------------------------------------------------------------------------
void *
memset(void * data, int fill_char, size_t size)
{
  unsigned char * pData = (unsigned char *)data;
  int i;

  for(i = 0; i < size; i++)
  {
    pData[i] = fill_char;
  }

  return(data);
}
