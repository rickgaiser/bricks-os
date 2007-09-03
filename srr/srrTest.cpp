#include "kernel/debug.h"
#include "kernel/srr.h"
#include "kernel/srrChannel.h"
#include "pthread.h"


volatile int iCHID(0);


//---------------------------------------------------------------------------
void *
server(void * arg)
{
  printk("server: Thread Running\n");

  iCHID = channelCreate(0);
  if(iCHID > 1)
  {
    int rcvid;
    char recvBuffer[20];
    rcvid = msgReceive(iCHID, recvBuffer, 20);
    if(rcvid > 0)
    {
      //printk("server: %s\n", recvBuffer);
      printk("server: ");
      printk(recvBuffer);
      printk("\n");
      if(msgReply(rcvid, 0, "SRV", 4) >= 0)
      {
      }
      else
      {
        printk("server: msgReply error\n");
      }
    }
    else
    {
      printk("server: msgReceive error\n");
    }
  }
  else
  {
     printk("server: Can't get channel\n");
  }

  while(true){}
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

  //while(true){}
  while(iCHID == 0){}
  // Connect to server
  int iCID = channelConnectAttach(0, 1, iCHID, 0);
  if(iCID > 1)
  {
    printk("client: connected\n");

    // Send message to server
    char recvBuffer[20];
    if(msgSend(iCID, "SRR", 4, recvBuffer, 20) >= 0)
    {
      //printk("client: %s\n", recvBuffer);
      printk("client: ");
      printk(recvBuffer);
      printk("\n");
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
