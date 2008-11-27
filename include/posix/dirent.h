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


#ifndef DIRENT_H
#define DIRENT_H


#include "sys/types.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef void * DIR;

typedef struct dirent
{
  ino_t  d_ino;        // file serial number
  char   d_name[256];  // name of entry
};


int             closedir (DIR * dirp);
DIR           * opendir  (const char * dirname);
struct dirent * readdir  (DIR * dirp);
int             readdir_r(DIR * dirp, struct dirent * entry, struct dirent ** result);
void            rewinddir(DIR * dirp);
void            seekdir  (DIR * dirp, long int loc);
long int        telldir  (DIR * dirp);


#ifdef __cplusplus
}
#endif


#endif
