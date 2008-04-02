#include "string.h"
#include "stdlib.h"
#include "inttypes.h"


const char * sStrError = "strerror not implemented";


// -----------------------------------------------------------------------------
void *
memccpy(void * s1, const void * s2, int c, size_t n)
{
  int i;
  for(i = 0; (i < n) && (((uint8_t *)s2)[i] != c); i++)
    ((uint8_t *)s1)[i] = ((uint8_t *)s2)[i];

  return s1;
}

// -----------------------------------------------------------------------------
void *
memchr(const void * s, int c, size_t n)
{
  int i;
  void * retval = NULL;

  for(i = 0; i < n; i++)
  {
    if(((uint8_t *)s)[i] == c)
    {
      retval = &((uint8_t *)s)[i];
      break;
    }
  }

  return retval;
}

// -----------------------------------------------------------------------------
int
memcmp(const void * s1, const void * s2, size_t n)
{
  int i, retval = 0;

  for(i = 0; i < n; i++)
    retval += ((uint8_t *)s1)[i] - ((uint8_t *)s2)[i];

  return retval;
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

// -----------------------------------------------------------------------------
void *
memmove(void * s1, const void * s2, size_t n)
{
  int i;
  uint8_t * buf = (uint8_t *)malloc(n);

  // Copy to buf
  for(i = 0; i < n; i++)
    buf[i] = ((uint8_t *)s2)[i];
  // Copy to s1
  for(i = 0; i < n; i++)
    ((uint8_t *)s1)[i] = buf[i];

  free(buf);

  return s1;
}

// -----------------------------------------------------------------------------
void *
memset(void * data, int fill_char, size_t size)
{
  int i;

  for(i = 0; i < size; i++)
    ((unsigned char *)data)[i] = fill_char;

  return data;
}

// -----------------------------------------------------------------------------
char *
strcat(char * s1, const char * s2)
{
  return strcpy(&s1[strlen(s1)], s2);
}

// -----------------------------------------------------------------------------
char *
strchr(const char * s, int c)
{
  int i;
  char * retval = NULL;

  for(i = 0; s[i] != 0; i++)
  {
    if(s[i] == c)
    {
      retval = (char *)&s[i];
      break;
    }
  }

  return retval;
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
strerror(int errnum)
{
  return (char *)sStrError;
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
char *
strstr(const char * s1, const char * s2)
{
  int i;
  const char * p1;
  const char * p2;
  const char * s = s1;

  for(p2 = s2, i = 0; *s; p2 = s2, i++, s++)
  {
    for(p1 = s; *p1 && *p2 && *p1 == *p2; p1++, p2++)
      ;
    if (!*p2)
      break;
  }

  if (!*p2)
    return (char *)(s1 + i);

  return 0;
}
