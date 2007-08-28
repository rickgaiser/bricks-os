#include "kernel/debug.h"
#include "kernel/srr.h"
#include "pthread.h"

/*
//---------------------------------------------------------------------------
void *
server(void * arg)
{
  printk("SRR Server\n");

  char recvBuffer[20];
  if(msgReceive(scTEST, recvBuffer, 20) >= 0)
  {
    printk("server: %d\n", recvBuffer);
    if(msgReply(scTEST, 0, "SRV", 4) >= 0)
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
    printk("ERROR: Unable to create thread!\n");

  // Send message to server
  char recvBuffer[20];
  if(msgSend(scTEST, "SRR", 4, recvBuffer, 20) >= 0)
  {
    printk("client: %d\n", recvBuffer);
  }
  else
  {
    printk("client: msgSend error\n");
  }

  return 0;
}
*/
