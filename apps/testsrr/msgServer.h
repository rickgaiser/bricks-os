#ifndef MSGSERVER_H
#define MSGSERVER_H


#include "ace/Task.h"


//---------------------------------------------------------------------------
class CMsgServer
 : public ACE_Task_Base
{
public:
  CMsgServer();
  virtual ~CMsgServer();

  int svc();
  virtual int process(int iReceiveID, void * pRcvMsg) = 0;

  int getPID() const;
  int getChannelID() const;

protected:
  int iPID_;
  int iChannelID_;
};


#endif
