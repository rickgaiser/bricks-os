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


#include "kernel/debug.h"
#include "stdint.h"
#include "string.h"
#include "asm/cpu.h"


#define va_list         __builtin_va_list
//#define va_start(v,l)   __builtin_va_start(v,l)
#define va_start(v,l)   __builtin_stdarg_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)


IFileIO * pDebug = 0;


// -----------------------------------------------------------------------------
void
printnk(void (*put)(char), unsigned long ul, int base)
{
  char * p;
  char   buf[(sizeof(long) * 8 / 3) + 1];

  p = buf;

  // Parse in reverse order
  do
  {
    *p++ = "0123456789abcdef"[ul % base];
  } while (ul /= base);

  // Print in correct order
  do
  {
    put(*--p);
  } while (p > buf);
}

// -----------------------------------------------------------------------------
void
doprintk(void (*put)(char), const char *fmt, va_list ap)
{
  char *p;
  int ch, n;
  unsigned long ul;
  int lflag, set;

  for (;;)
  {
    while ((ch = *fmt++) != '%')
    {
      if (ch == '\0')
        return;
      put(ch);
    }
    lflag = 0;
reswitch:
    switch (ch = *fmt++)
    {
    case '\0':
      return;
    case 'l':
      lflag = 1;
      goto reswitch;
    case 'b':
      ul = va_arg(ap, int);
      p = va_arg(ap, char *);
      printnk(put, ul, *p++);

      if (!ul)
        break;

      for (set = 0; (n = *p++);)
      {
        if (ul & (1 << (n - 1)))
        {
          put(set ? ',' : '<');
          for (; (n = *p) > ' '; ++p)
            put(n);
          set = 1;
        }
        else
          for (; *p > ' '; ++p);
      }
      if (set)
        put('>');
      break;
    case 'c':
      ch = va_arg(ap, int);
        put(ch & 0x7f);
      break;
    case 's':
      p = va_arg(ap, char *);
      while ((ch = *p++))
        put(ch);
      break;
    case 'd':
      ul = lflag ? va_arg(ap, long) : va_arg(ap, int);
      if ((long)ul < 0)
      {
        put('-');
        ul = -(long)ul;
      }
      printnk(put, ul, 10);
      break;
    case 'o':
      ul = lflag ? va_arg(ap, uint32_t) : va_arg(ap, uint32_t);
      printnk(put, ul, 8);
      break;
    case 'u':
      ul = lflag ? va_arg(ap, uint32_t) : va_arg(ap, uint32_t);
      printnk(put, ul, 10);
      break;
    case 'p':
      put('0');
      put('x');
      lflag = 1;
      // fall through
    case 'x':
      ul = lflag ? va_arg(ap, uint32_t) : va_arg(ap, uint32_t);
      printnk(put, ul, 16);
      break;
    default:
      put('%');
      if (lflag)
        put('l');
      put(ch);
    }
  }
  va_end(ap);
}

// -----------------------------------------------------------------------------
void
putk(char c)
{
  if(pDebug != 0)
    pDebug->write(&c, 1);
}

// -----------------------------------------------------------------------------
void
vprintk(const char *fmt, va_list ap)
{
  doprintk(putk, fmt, ap);
}

// -----------------------------------------------------------------------------
void
printk(const char * fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vprintk(fmt, ap);
  va_end(ap);
}

// -----------------------------------------------------------------------------
void
panic(const char * fmt, ...)
{
  if(pDebug != 0)
    pDebug->write("Kernel Panic: ", 14);

  va_list ap;
  va_start(ap, fmt);
  vprintk(fmt, ap);
  va_end(ap);

  if(pDebug != 0)
    pDebug->write("\nSystem Halted", 15);

  local_irq_disable();
  ::halt();
}
