#include "kernel/srr.h"
#include "iostream"
#include "pthread.h"


//---------------------------------------------------------------------------
void *
server(void * arg)
{
  std::cout<<"SRR Server"<<std::endl;

  char recvBuffer[20];
  if(msgReceive(scTEST, recvBuffer, 20) >= 0)
  {
    std::cout<<"server: "<<recvBuffer<<std::endl;
    if(msgReply(scTEST, 0, "SRV", 4) >= 0)
    {
    }
    else
    {
      std::cout<<"server: msgReply error"<<std::endl;
    }
  }
  else
  {
    std::cout<<"server: msgReceive error"<<std::endl;
  }

  return 0;
}

//---------------------------------------------------------------------------
int
srrTest(int argc, char * argv[])
{
  std::cout<<"SRR Client"<<std::endl;

  // Create server thread
  pthread_t thrServer;
  if(pthread_create(&thrServer, 0, server, 0) != 0)
    std::cout<<"ERROR: Unable to create thread!"<<std::endl;

  // Send message to server
  char recvBuffer[20];
  if(msgSend(scTEST, "SRR", 4, recvBuffer, 20) >= 0)
  {
    std::cout<<"client: "<<recvBuffer<<std::endl;
  }
  else
  {
    std::cout<<"client: msgSend error"<<std::endl;
  }

  return 0;
}
