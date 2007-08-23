#include "string.h"
#include "inttypes.h"


// -----------------------------------------------------------------------------
int
strcmp(const char * s1, const char * s2)
{
  int i = 0;
  while((s1[i] != 0) && (s2[i] != 0) && (s1[i] == s2[i]))
    i++;
  return (s1[i] - s2[i]);
}

// -----------------------------------------------------------------------------
size_t
strlen(const char * s)
{
  int iLength = 0;

  while(s[iLength] == 0)
    iLength++;

  return iLength;
}

// -----------------------------------------------------------------------------
char *
strcpy(char * s1, const char * s2)
{
  // Copy string
  while(*s2 != 0)
  {
    *s1 = *s2;
    s1++;
    s2++;
  }

  // Null terminate
  *s1 = 0;

  return s1;
}

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

// -----------------------------------------------------------------------------
void *
memcpy(void * s1, const void * s2, size_t n)
{
  int i;
  for(i = 0; i < n; i++)
    ((uint8_t *)s1)[i] = ((uint8_t *)s2)[i];

  return s1;
}
