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


#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "unistd.h"
#include "fcntl.h"


#define TEST_FILE  "/dev/debug"
//#define TEST_READ
#define TEST_WRITE


// -----------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  int iRetVal;
  int fd;

  printk("FileSystem Test\n");

  printk(" - Open File...");
  fd = open(TEST_FILE, O_RDWR);
  if(fd >= 0)
  {
    printk("OK\n");

#ifdef TEST_READ
    char c;
    printk(" - Read File...");
    iRetVal = read(fd, &c, 1);
    if(iRetVal == 1)
      printk("OK");
    else
      printk("ERROR(%d)\n", iRetVal);
#endif

#ifdef TEST_WRITE
    printk(" - Write File...");
    iRetVal = write(fd, "OK\n", 4);
    if(iRetVal != 4)
      printk("ERROR(%d)\n", iRetVal);
#endif

    printk(" - Close File...");
    iRetVal = close(fd);
    if(iRetVal >= 0)
      printk("OK\n");
    else
      printk("ERROR(%d)\n", iRetVal);
  }
  else
    printk("ERROR(%d)\n", fd);

  printk("---done---\n");
  while(1);

  return 0;
}
