/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "stdlib.h"
#include "stdint.h"
#include "errno.h"
#include "ctype.h"
#include "limits.h"
#include "kernel/memoryManager.h"
#include "asm/arch/config.h"


#define RAND_A     9301
#define RAND_C    49297


uint32_t seed = 0x12345678;


// -----------------------------------------------------------------------------
int
atoi(const char * str)
{
  return (int) strtol (str, (char **) NULL, 10);
}

#ifdef CONFIG_BUILTIN_MM
// -----------------------------------------------------------------------------
void
free(void * ptr)
{
  kfree(ptr);
}

// -----------------------------------------------------------------------------
void *
malloc(size_t size)
{
  return kmalloc(size);
}
#endif // CONFIG_BUILTIN_MM

// -----------------------------------------------------------------------------
int
rand(void)
{
  seed = (seed*RAND_A+RAND_C) % RAND_MAX;

  return seed;
}

#ifndef NDS9
// -----------------------------------------------------------------------------
long int
strtol(const char * str, char ** endptr, int base)
{
  register const char *s = str;
  register unsigned long acc;
  register int c;
  register unsigned long cutoff;
  register int neg = 0, any, cutlim;

  /*
   * Skip white space and pick up leading +/- sign if any.
   * If base is 0, allow 0x for hex and 0 for octal, else
   * assume decimal; if base is already 16, allow 0x.
   */
  do
  {
      c = *s++;
  } while (isspace(c));
  if(c == '-')
  {
      neg = 1;
      c = *s++;
  }
  else if(c == '+')
      c = *s++;
  if((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X'))
  {
      c = s[1];
      s += 2;
      base = 16;
  }
  if(base == 0)
      base = c == '0' ? 8 : 10;

  /*
   * Compute the cutoff value between legal numbers and illegal
   * numbers.  That is the largest legal value, divided by the
   * base.  An input number that is greater than this value, if
   * followed by a legal input character, is too big.  One that
   * is equal to this value may be valid or not; the limit
   * between valid and invalid numbers is then based on the last
   * digit.  For instance, if the range for longs is
   * [-2147483648..2147483647] and the input base is 10,
   * cutoff will be set to 214748364 and cutlim to either
   * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
   * a value > 214748364, or equal but the next digit is > 7 (or 8),
   * the number is too big, and we will return a range error.
   *
   * Set any if any `digits' consumed; make it negative to indicate
   * overflow.
   */
  cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
  cutlim = cutoff % (unsigned long)base;
  cutoff /= (unsigned long)base;
  for (acc = 0, any = 0;; c = *s++)
  {
    if (isdigit(c))
      c -= '0';
    else if (isalpha(c))
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base)
      break;
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
      any = -1;
    else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if(any < 0)
  {
    acc = neg ? LONG_MIN : LONG_MAX;
    errno = ERANGE;
  }
  else if(neg)
    acc = -acc;
  if(endptr != 0)
    *endptr = (char *) (any ? s - 1 : str);

  return (acc);
}
#endif
