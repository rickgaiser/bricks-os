#ifndef MSGSERVER_H
#define MSGSERVER_H


#include "ace/Task.h"


//---------------------------------------------------------------------------
class CMsgServer
 : public ACE_Task_Base
{
public:
  CMsgServer(const char * name = 0);
  virtual ~CMsgServer();

  int svc();
  virtual int process(int iReceiveID, void * pRcvMsg) = 0;

  int getPID() const;
  int getChannelID() const;

protected:
  const char * sName_;
  int iPID_;
  int iChannelID_;
};


#endif
