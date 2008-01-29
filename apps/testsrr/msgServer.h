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

//protected:
  int iPID_;
  int iChannelID_;
};




#endif
