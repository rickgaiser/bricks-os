#include "msgServer.h"
#include "kernel/debug.h"
#include "kernel/srr.h"
#include "unistd.h"


//---------------------------------------------------------------------------
CMsgServer::CMsgServer(const char * name)
 : sName_(name)
 , iChannelID_(0)
{
  iPID_ = getpid();
}

//---------------------------------------------------------------------------
CMsgServer::~CMsgServer()
{
}

//---------------------------------------------------------------------------
int
CMsgServer::svc()
{
  iChannelID_ = channelCreate(0);
  if(iChannelID_ > 1)
  {
    int rcvid;
    char recvBuffer[80];

    // Register name is one was passed
    if(sName_ != 0)
    {
      if(registerName(iChannelID_, sName_) < 0)
        printk("CMsgServer::svc: ERROR: Unable to register name\n");
    }

    while(true)
    {
      rcvid = msgReceive(iChannelID_, recvBuffer, 80);
//      if(rcvid <= 0)
//        break;
      if(process(rcvid, recvBuffer) < 0)
        break;
    }

    channelDestroy(iChannelID_);
  }
  else
    printk("CMsgServer::svc: ERROR: Unable to get channel\n");

  return 0;
}

//---------------------------------------------------------------------------
int
CMsgServer::getPID() const
{
  return iPID_;
}

//---------------------------------------------------------------------------
int
CMsgServer::getChannelID() const
{
  return iChannelID_;
}
