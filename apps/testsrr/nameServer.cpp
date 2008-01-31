#include "nameServer.h"
#include "kernel/debug.h"
#include "kernel/srr.h"
#include "kernel/srrChannel.h"
#include "string.h"
#include "unistd.h"


//---------------------------------------------------------------------------
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
  // FIXME: We need a list!
  static int pid(-1), channelID(-1);

  //printk("CNameServer::process\n");

  switch(((SFunction *)pRcvMsg)->iFunctionID)
  {
    case F_REGISTER:
      printk("Register(%d,%d,%s)\n", ((SRegisterName *)pRcvMsg)->iPID, ((SRegisterName *)pRcvMsg)->iChannelID, ((SRegisterName *)pRcvMsg)->sName);
      pid       = ((SRegisterName *)pRcvMsg)->iPID;
      channelID = ((SRegisterName *)pRcvMsg)->iChannelID;
      msgReply(iReceiveID, 0, 0, 0);
      break;
    case F_LOOKUP:
      printk("Lookup(%s)\n", ((SLookupName *)pRcvMsg)->sName);
      struct SLookupNameReturn retdata;
      retdata.iPID       = pid;
      retdata.iChannelID = channelID;
      msgReply(iReceiveID, 0, &retdata, sizeof(SLookupNameReturn));
      break;
    default:
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
