#ifndef TIME_H
#define TIME_H


#include "sys/types.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifndef NULL
#define NULL (0)
#endif


typedef struct tm
{
  int    tm_sec;   // seconds [0,61]
  int    tm_min;   // minutes [0,59]
  int    tm_hour;  // hour [0,23]
  int    tm_mday;  // day of month [1,31]
  int    tm_mon;   // month of year [0,11]
  int    tm_year;  // years since 1900
  int    tm_wday;  // day of week [0,6] (Sunday = 0)
  int    tm_yday;  // day of year [0,365]
  int    tm_isdst; // daylight savings flag
} tm;

typedef struct timespec
{
  time_t tv_sec;   // Seconds
  long   tv_nsec;  // Nanoseconds
} timespec;


#ifdef __cplusplus
}
#endif


#endif
