#ifndef NAMESERVER_H
#define NAMESERVER_H


#include "msgServer.h"
#include "kernel/queue.h"


//---------------------------------------------------------------------------
enum EFunctions
{
  F_REGISTER = 1,
  F_LOOKUP = 2,
};

//---------------------------------------------------------------------------
typedef struct SFunction
{
  int  iFunctionID;
};

//---------------------------------------------------------------------------
typedef struct SRegisterName
{
  int  iFunctionID;
  int  iPID;
  int  iChannelID;
  char sName[20];
};

//---------------------------------------------------------------------------
typedef struct SLookupName
{
  int  iFunctionID;
  char sName[20];
};

typedef struct SLookupNameReturn
{
  int  iPID;
  int  iChannelID;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct SNameEntry
{
  TAILQ_ENTRY(SNameEntry) name_qe;
  int  iPID;
  int  iChannelID;
  char sName[20];
} SNameEntry;

//---------------------------------------------------------------------------
TAILQ_HEAD(SNameQueue, SNameEntry);

//---------------------------------------------------------------------------
class CNameServer
 : public CMsgServer
{
public:
  CNameServer();
  virtual ~CNameServer();

  virtual int process(int iReceiveID, void * pRcvMsg);

private:
  // Instance of the queue
  static SNameQueue nameQueue_;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class CNameServerClient
{
public:
  CNameServerClient(const CNameServer & nameServer);
  virtual ~CNameServerClient();

  int registerName(int channelID, const char * name);
  int lookupName(const char * name, int & pid, int & channelID);

private:
  bool connect();
  void disconnect();

  const CNameServer & nameServer_;
  int iPID_;
  int iConnectID_;
};


#endif
