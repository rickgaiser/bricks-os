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
  printk("server: Thread Running\n");

  iServerPID = getpid();
  iChannelID = channelCreate(0);
  if(iChannelID > 1)
  {
    int rcvid;
    char recvBuffer[20];

    printk("server: Ready\n");

    // Notify client we're ready
    pthread_mutex_unlock(&mut_run);

    rcvid = msgReceive(iChannelID, recvBuffer, 20);
    if(rcvid > 0)
    {
      printk("server: %s\n", recvBuffer);
      if(msgReply(rcvid, 0, "SRV", 4) < 0)
        printk("server: msgReply error\n");
    }
    else
      printk("server: msgReceive error\n");
  }
  else
     printk("server: Can't get channel\n");

  printk("server: Done\n");
  channelDestroy(iChannelID);
  pthread_exit(NULL);

  return 0;
}

//---------------------------------------------------------------------------
int
srrTest(int argc, char * argv[])
{
  int iRetVal(-1);

  printk("SRR Client\n");

  // Lock mutex before creating the thread
  pthread_mutex_lock(&mut_run);

  // Create server thread
  pthread_t thrServer;
  if(pthread_create(&thrServer, 0, server, 0) == 0)
  {
    // Wait for server to free mutex
    pthread_mutex_lock(&mut_run);

    // Connect to server
    int iConnectID = channelConnectAttach(0, iServerPID, iChannelID, 0);
    if(iConnectID > 1)
    {
      printk("client: Connected\n");

      // Send message to server
      char recvBuffer[20];
      if(msgSend(iConnectID, "SRR", 4, recvBuffer, 20) >= 0)
      {
        printk("client: %s\n", recvBuffer);
        iRetVal = 0;
      }
      else
        printk("client: msgSend error\n");

      channelConnectDetach(iConnectID);
    }
    else
      printk("client: Unable to connect\n");
  }
  else
    printk("ERROR: Unable to create thread for server!\n");

  return iRetVal;
}
