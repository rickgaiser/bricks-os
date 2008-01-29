#ifndef NAMESERVER_H
#define NAMESERVER_H


#include "msgServer.h"


class CNameServer
 : public CMsgServer
{
public:
  CNameServer();
  virtual ~CNameServer();

  virtual int process(int iReceiveID, void * pRcvMsg);
};


#endif
