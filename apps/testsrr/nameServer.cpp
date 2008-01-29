#include "nameServer.h"
#include "kernel/debug.h"


//---------------------------------------------------------------------------
CNameServer::CNameServer()
 : CMsgServer()
{
}

//---------------------------------------------------------------------------
CNameServer::~CNameServer()
{
}

//---------------------------------------------------------------------------
int
CNameServer::process(int iReceiveID, void * pRcvMsg)
{
  printk("CNameServer::process: Message Received\n");

  return 0;
}
