#include "kernel/debug.h"
#include "inttypes.h"
#include "string.h"
#include "asm/cpu.h"


IFileIO * pDebug = 0;


// -----------------------------------------------------------------------------
void
printnk(void (*put)(char), unsigned long ul, int base)
{
  char * p;
  char * buf[(sizeof(long) * 8 / 3) + 1];

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
  pDebug->write(&c, 1);
}

// -----------------------------------------------------------------------------
void
vprintk(const char *fmt, va_list ap)
{
  if(pDebug != 0)
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
  pDebug->write("Kernel Panic: ", 14);

  va_list ap;
  va_start(ap, fmt);
  vprintk(fmt, ap);
  va_end(ap);

  pDebug->write("\nSystem Halted", 15);

  CCPU::cli();
  CCPU::halt();
}
