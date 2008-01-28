#include "kernel/debug.h"
#include "kernel/srr.h"
#include "kernel/srrChannel.h"
#include "unistd.h"
#include "pthread.h"


pthread_mutex_t mut_run = PTHREAD_MUTEX_INITIALIZER;
int iChannelID;
int iServerPID;


//---------------------------------------------------------------------------
void *
server(void * arg)
{
  printk(" - Channel Creation...");

  iServerPID = getpid();
  iChannelID = channelCreate(0);
  if(iChannelID > 1)
  {
    int rcvid;
    char recvBuffer[20];

    printk("OK\n");

    // Notify client we're ready
    pthread_mutex_unlock(&mut_run);

    rcvid = msgReceive(iChannelID, recvBuffer, 20);
    if((rcvid > 0) &&
       (recvBuffer[0] == 'S') &&
       (recvBuffer[1] == 'R') &&
       (recvBuffer[2] == 'R'))
    {
      printk(" - Message Receive...OK\n");
      if(msgReply(rcvid, 0, "SRV", 4) < 0)
        printk(" - Message Reply...ERROR\n");
    }
    else
      printk(" - Message Receive...ERROR\n");
  }
  else
    printk("ERROR\n");

  channelDestroy(iChannelID);
  pthread_exit(NULL);

  return 0;
}

//---------------------------------------------------------------------------
int
testSRR()
{
  int iRetVal(-1);

  //printk(" - SRR IPC Test\n");

  // Lock mutex before creating the thread
  pthread_mutex_lock(&mut_run);

  // Create server thread
  //printk(" - Starting server thread...");
  pthread_t thrServer;
  if(pthread_create(&thrServer, 0, server, 0) == 0)
  {
    //printk("OK\n");

    // Wait for server to free mutex
    pthread_mutex_lock(&mut_run);

    // Connect to server
    printk(" - Channel Connect...");
    int iConnectID = channelConnectAttach(0, iServerPID, iChannelID, 0);
    if(iConnectID > 1)
    {
      printk("OK\n");

      // Send message to server
      char recvBuffer[20];
      if((msgSend(iConnectID, "SRR", 4, recvBuffer, 20) >= 0) &&
         (recvBuffer[0] == 'S') &&
         (recvBuffer[1] == 'R') &&
         (recvBuffer[2] == 'V'))
      {
        printk(" - Message Send...OK\n");
        iRetVal = 0;
      }
      else
        printk(" - Message Send...ERROR\n");

      channelConnectDetach(iConnectID);
    }
    else
      printk("ERROR\n");
  }
  else
    printk("ERROR\n");

  return iRetVal;
}

//---------------------------------------------------------------------------
// Test sleeping
void
testSleep()
{
  printk(" - Sleep 3x 1s");
  for(int i(0); i < 3; i++)
  {
    sleep(1);
    printk(".");
  }
  printk("OK\n");
/*
  printk(" - Sleep 5x 1000000us:");
  for(int i(0); i < 5; i++)
  {
    usleep(1000000);
    printk(".");
  }
  printk("done\n");
*/
}

//---------------------------------------------------------------------------
// Main test thread
void *
testThread(void * arg)
{
  printk("OK\n");

  printk(" - Thread arg...");
  if(arg == (void *)0x21436587)
    printk("OK\n");
  else
    printk("ERROR\n");

  testSleep();
  testSRR();

  // Thread exit
  pthread_exit(NULL);

  return 0;
}

//---------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  printk("Bricks-OS system test:\n");

  // Start test thread since we're called from the kernel thread, and
  // sleeping here will couse the system to die.
  printk(" - Thread Creation...");
  pthread_t thr;
  if(pthread_create(&thr, 0, testThread, (void *)0x21436587) != 0)
    printk("ERROR\n");

  while(1);

  return 0;
}
