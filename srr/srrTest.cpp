#include "kernel/debug.h"
#include "kernel/srr.h"
#include "kernel/srrChannel.h"
#include "unistd.h"
#include "pthread.h"


volatile bool bServerRunning(false);
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
    bServerRunning = true;

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

  // Create server thread
  pthread_t thrServer;
  if(pthread_create(&thrServer, 0, server, 0) == 0)
  {
    while(bServerRunning != true);
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
    }
    else
      printk("client: Unable to connect\n");
  }
  else
    printk("ERROR: Unable to create thread for server!\n");

  return iRetVal;
}
