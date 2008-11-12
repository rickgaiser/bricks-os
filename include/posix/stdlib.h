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


#ifndef STDLIB_H
#define STDLIB_H


#include "stddef.h"


#define RAND_MAX 233280


#ifdef __cplusplus
extern "C" {
#endif


//long       a64l(const char *);
//void       abort(void);
//int        abs(int);
#define abs(i) (i >= 0 ? i : -i)
//int        atexit(void (*)(void));
//double     atof(const char *);
int        atoi(const char * str);
//long int   atol(const char *);
//void     * bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
//void     * calloc(size_t, size_t);
//div_t      div(int, int);
//double     drand48(void);
//char     * ecvt(double, int, int *, int *);
//double     erand48(unsigned short int[3]);
//void       exit(int);
//char     * fcvt (double, int, int *, int *);
void       free(void * ptr);

//char     * gcvt(double, int, char *);
//char     * getenv(const char *);
//int        getsubopt(char **, char *const *, char **);
//int        grantpt(int);
//char     * initstate(unsigned int, char *, size_t);
//long int   jrand48(unsigned short int[3]);
//char     * l64a(long);
//long int   labs(long int);
//void       lcong48(unsigned short int[7]);
//ldiv_t     ldiv(long int, long int);
//long int   lrand48(void);
void     * malloc(size_t size);
//int        mblen(const char *, size_t);
//size_t     mbstowcs(wchar_t *, const char *, size_t);
//int        mbtowc(wchar_t *, const char *, size_t);
//char     * mktemp(char *);
//int        mkstemp(char *);
//long int   mrand48(void);
//long int   nrand48(unsigned short int [3]);
//char     * ptsname(int);
//int        putenv(char *);
//void       qsort(void *, size_t, size_t, int (*)(const void *, const void *));
int        rand(void);
//int        rand_r(unsigned int *);
//long       random(void);
//void     * realloc(void *, size_t);
//char     * realpath(const char *, char *);
//unsigned short int seed48(unsigned short int[3]);
//void       setkey(const char *);
//char     * setstate(const char *);
//void       srand(unsigned int);
//void       srand48(long int);
//void       srandom(unsigned);
//double     strtod(const char *, char **);
long int   strtol(const char * str, char ** endptr, int base);
//unsigned long int strtoul(const char *, char **, int);
//int        system(const char *);
//int        ttyslot(void); (LEGACY)
//int        unlockpt(int);
//void     * valloc(size_t); (LEGACY)
//size_t     wcstombs(char *, const wchar_t *, size_t);
//int        wctomb(char *, wchar_t);


#ifdef __cplusplus
}
#endif


#endif
