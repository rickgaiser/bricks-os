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


#include "kernel/srr_k.h"
#include "kernel/debug.h"
#include "kernel/queue.h"
#include "kernel/task.h"
#include "kernel/taskManager.h"
#include "string.h"


typedef struct SNameEntry
{
  TAILQ_ENTRY(SNameEntry) name_qe;
  int  iPID;
  int  iChannelID;
  char sName[20];
} SNameEntry;

TAILQ_HEAD(SNameQueue, SNameEntry);


SNameQueue nameQueue_ = TAILQ_HEAD_INITIALIZER(nameQueue_);


//---------------------------------------------------------------------------
int
k_registerName(int channelID, const char * name)
{
  struct SNameEntry * newEntry = new SNameEntry;

  //printk("k_registerName(%d,%s)\n", channelID, name);

  // Create new entry
  newEntry->iPID       = CTaskManager::pCurrentTask_->iPID_;
  newEntry->iChannelID = channelID;
  strcpy(newEntry->sName, name);
  // Insert entry in the queue
  TAILQ_INSERT_TAIL(&nameQueue_, newEntry, name_qe);

  return 0;
}

//---------------------------------------------------------------------------
int
k_lookupName(const char * name, int * pid, int * channelID)
{
  struct SNameEntry * pEntry;

  //printk("k_lookupName(%s)\n", name);

  // Lookup name in queue
  TAILQ_FOREACH(pEntry, &nameQueue_, name_qe)
  {
    if(strcmp(pEntry->sName, name) == 0)
    {
      *pid       = pEntry->iPID;
      *channelID = pEntry->iChannelID;
      return 0;
    }
  }

  return -1;
}
