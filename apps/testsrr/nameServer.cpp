#include "nameServer.h"
#include "kernel/debug.h"
#include "kernel/srr.h"
#include "kernel/srrChannel.h"
#include "string.h"
#include "unistd.h"


SNameQueue CNameServer::nameQueue_ = TAILQ_HEAD_INITIALIZER(CNameServer::nameQueue_);


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CNameServer::CNameServer()
 : CMsgServer()
{
}

//---------------------------------------------------------------------------
CNameServer::~CNameServer()
{
  // remove queue
  struct SNameEntry * pEntry;
  struct SNameEntry * pEntryTemp;
  TAILQ_FOREACH_SAFE(pEntry, &CNameServer::nameQueue_, name_qe, pEntryTemp)
  {
    delete pEntry;
  }
}

//---------------------------------------------------------------------------
int
CNameServer::process(int iReceiveID, void * pRcvMsg)
{
  //printk("CNameServer::process\n");

  switch(((SFunction *)pRcvMsg)->iFunctionID)
  {
    case F_REGISTER:
    {
      printk("Register(%d,%d,%s)\n", ((SRegisterName *)pRcvMsg)->iPID, ((SRegisterName *)pRcvMsg)->iChannelID, ((SRegisterName *)pRcvMsg)->sName);
      // Create new entry
      struct SNameEntry * newEntry = new SNameEntry;
      newEntry->iPID       = ((SRegisterName *)pRcvMsg)->iPID;
      newEntry->iChannelID = ((SRegisterName *)pRcvMsg)->iChannelID;
      strcpy(newEntry->sName, ((SRegisterName *)pRcvMsg)->sName);
      // Insert entry in the queue
      TAILQ_INSERT_TAIL(&CNameServer::nameQueue_, newEntry, name_qe);
      // Reply to caller
      msgReply(iReceiveID, 0, 0, 0);
      break;
    }
    case F_LOOKUP:
    {
      printk("Lookup(%s)\n", ((SLookupName *)pRcvMsg)->sName);
      struct SLookupNameReturn retdata = {-1, -1};
      struct SNameEntry * pEntry;
      // Lookup name in queue
      TAILQ_FOREACH(pEntry, &CNameServer::nameQueue_, name_qe)
      {
        if(strcmp(pEntry->sName, ((SLookupName *)pRcvMsg)->sName) == 0)
        {
          retdata.iPID       = pEntry->iPID;
          retdata.iChannelID = pEntry->iChannelID;
          break;
        }
      }
      // Reply to caller
      msgReply(iReceiveID, 0, &retdata, sizeof(SLookupNameReturn));
      break;
    }
    default:
      // Reply to caller
      msgReply(iReceiveID, 0, 0, 0);
  };

  return 0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CNameServerClient::CNameServerClient(const CNameServer & nameServer)
 : nameServer_(nameServer)
 , iConnectID_(-1)
{
  iPID_ = getpid(); // <-- Kernel Function Call

  connect();
}

//---------------------------------------------------------------------------
CNameServerClient::~CNameServerClient()
{
  disconnect();
}

//---------------------------------------------------------------------------
int
CNameServerClient::registerName(int channelID, const char * name)
{
  int iRetVal(0);

  if(connect() == true)
  {
    struct SRegisterName data;

    data.iFunctionID = F_REGISTER;
    data.iPID        = iPID_;
    data.iChannelID  = channelID;
    strcpy(data.sName, name);

    // Send message to name server
    if(msgSend(iConnectID_, &data, sizeof(SRegisterName), 0, 0) < 0) // <-- Kernel Function Call
      iRetVal = -1;
  }
  else
    iRetVal = -1;

  return iRetVal;
}

//---------------------------------------------------------------------------
int
CNameServerClient::lookupName(const char * name, int & pid, int & channelID)
{
  int iRetVal(0);

  if(connect() == true)
  {
    struct SLookupName data;
    struct SLookupNameReturn retdata;

    data.iFunctionID = F_LOOKUP;
    strcpy(data.sName, name);

    // Send message to name server
    if(msgSend(iConnectID_, &data, sizeof(SRegisterName), &retdata, sizeof(SLookupNameReturn)) < 0) // <-- Kernel Function Call
      iRetVal = -1;
    else
    {
      pid = retdata.iPID;
      channelID = retdata.iChannelID;
    }
  }
  else
    iRetVal = -1;

  return iRetVal;
}

//---------------------------------------------------------------------------
bool
CNameServerClient::connect()
{
  // Connect Name Server (ourselves in this case)
  if(iConnectID_ <= 1)
    iConnectID_ = channelConnectAttach(0, nameServer_.getPID(), nameServer_.getChannelID(), 0); // <-- Kernel Function Call

  return (iConnectID_ > 1);
}

//---------------------------------------------------------------------------
void
CNameServerClient::disconnect()
{
  // Detach connection
  if(iConnectID_ > 1)
  {
    channelConnectDetach(iConnectID_); // <-- Kernel Function Call
    iConnectID_ = -1;
  }
}
