#include "kernel/debug.h"
#include "kernel/srr.h"
#include "kernel/srrChannel.h"
#include "pthread.h"


volatile bool bServerRunning(false);
int iChannelID;
int iServerPID;


//---------------------------------------------------------------------------
void *
server(void * arg)
{
  printk("server: Thread Running\n");

  iServerPID = 1;//getPID();
  iChannelID = channelCreate(0);
  bServerRunning = true;
  if(iChannelID > 1)
  {
    int rcvid;
    char recvBuffer[20];
    while(true)
    {
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
  }
  else
     printk("server: Can't get channel\n");

  return 0;
}

//---------------------------------------------------------------------------
int
srrTest(int argc, char * argv[])
{
  printk("SRR Client\n");

  // Create server thread
  pthread_t thrServer;
  if(pthread_create(&thrServer, 0, server, 0) != 0)
  {
    printk("ERROR: Unable to create thread for server!\n");
    return -1;
  }

  while(bServerRunning != true);
  // Connect to server
  int iConnectID = channelConnectAttach(0, iServerPID, iChannelID, 0);
  if(iConnectID > 1)
  {
    printk("client: connected\n");

    // Send message to server
    char recvBuffer[20];
    if(msgSend(iConnectID, "SRR", 4, recvBuffer, 20) >= 0)
    {
      printk("client: %s\n", recvBuffer);
    }
    else
    {
      printk("client: msgSend error\n");
      return -1;
    }
  }
  else
  {
    printk("client: Unable to connect\n");
    return -1;
  }

  return 0;
}
