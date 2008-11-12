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


#ifndef STRING_H
#define STRING_H


#include "stddef.h"


#ifdef __cplusplus
extern "C" {
#endif


void    * memccpy(void * s1, const void * s2, int c, size_t n);
void    * memchr(const void * s, int c, size_t n);
int       memcmp(const void * s1, const void * s2, size_t n);
void    * memcpy(void * s1, const void * s2, size_t n);
void    * memmove(void * s1, const void * s2, size_t n);
void    * memset(void * s, int c, size_t n);
char    * strcat(char * s1, const char * s2);
char    * strchr(const char * s, int c);
int       strcmp(const char * s1, const char * s2);
//int       strcoll(const char *, const char *);
char    * strcpy(char * s1, const char * s2);
//size_t    strcspn(const char *, const char *);
//char    * strdup(const char *);
char    * strerror(int errnum);
size_t    strlen(const char * s);
//char    * strncat(char *, const char *, size_t);
//int       strncmp(const char *, const char *, size_t);
char    * strncpy(char * s1, const char * s2, size_t n);
//char    * strpbrk(const char *, const char *);
//char    * strrchr(const char *, int);
//size_t    strspn(const char *, const char *);
char    * strstr(const char * s1, const char * s2);
//char    * strtok(char *, const char *);
//char    * strtok_r(char *, const char *, char **);
//size_t    strxfrm(char *, const char *, size_t);


#ifdef __cplusplus
}
#endif


#endif
