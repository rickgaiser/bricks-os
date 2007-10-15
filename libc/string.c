#include "string.h"
#include "inttypes.h"


#define RAND_A     9301
#define RAND_C    49297


uint32_t seed = 0x12345678;


// -----------------------------------------------------------------------------
int
rand(void)
{
  seed = (seed*RAND_A+RAND_C) % RAND_MAX;

  return seed;
}

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

  while(s[iLength] != 0)
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
char *
strncpy(char * s1, const char * s2, size_t n)
{
  // Copy n-1 so we can NULL terminate the string
  n--;

  // Copy string
  while((*s2 != 0) && (n != 0))
  {
    *s1 = *s2;
    s1++;
    s2++;
    n--;
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
