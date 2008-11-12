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


#ifndef TIME_H
#define TIME_H


#include "sys/types.h"


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


#endif
