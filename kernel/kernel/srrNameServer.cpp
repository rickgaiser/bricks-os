#include "kernel/srrNameServer_k.h"
#include "kernel/debug.h"
#include "kernel/queue.h"
#include "kernel/task.h"
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

  printk("Register(%d,%s)\n", channelID, name);

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
k_lookupName(const char * name, int & pid, int & channelID)
{
  struct SNameEntry * pEntry;

  printk("k_lookupName(%s)\n", name);

  // Lookup name in queue
  TAILQ_FOREACH(pEntry, &nameQueue_, name_qe)
  {
    if(strcmp(pEntry->sName, name) == 0)
    {
      pid       = pEntry->iPID;
      channelID = pEntry->iChannelID;
      return 0;
    }
  }

  return -1;
}
