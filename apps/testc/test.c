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
#include "unistd.h"
#include "pthread.h"


//---------------------------------------------------------------------------
// Test sleeping
void
testSleep()
{
  int i;

  printk(" - Sleep 5x 1s:");
  for(i = 0; i < 5; i++)
  {
    sleep(1);
    printk(".");
  }
  printk("done\n");
}

//---------------------------------------------------------------------------
// Main test thread
void *
testThread(void * arg)
{
  printk(" - Test Thread running\n");

  testSleep();

  // Thread exit
  pthread_exit(NULL);

  return 0;
}

//---------------------------------------------------------------------------
int
appMain(int argc, char * argv[])
{
  printk("Bricks-OS system test:\n");

  // Start test thread since we're called from the kernel thread, and
  // sleeping here will couse the system to die.
  printk(" - Starting test thread: ");
  pthread_t thr;
  if(pthread_create(&thr, 0, testThread, 0) == 0)
    printk("done\n");
  else
    printk("error\n");

  while(1);

  return 0;
}
