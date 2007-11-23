#ifndef LIMITS_H
#define LIMITS_H


#ifndef ULONG_MAX
#define ULONG_MAX ((unsigned long) ~(unsigned long) 0)
#define ULONG_MIN ((unsigned long) 0 - ULONG_MAX)
#endif

#ifndef LONG_MAX
#define LONG_MAX ((long) (ULONG_MAX >> 1))
#define LONG_MIN ((long) (0 - LONG_MAX))
#endif


#endif
