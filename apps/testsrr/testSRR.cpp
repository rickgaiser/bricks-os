#include "kernel/debug.h"
#include "kernel/srr.h"
#include "kernel/srrChannel.h"
#include "unistd.h"
#include "pthread.h"
#include "ace/Task.h"


pthread_mutex_t mut_run = PTHREAD_MUTEX_INITIALIZER;


//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
CMsgServer::CMsgServer()
 : iChannelID_(0)
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
    char recvBuffer[20];

    // Notify client we're ready
    pthread_mutex_unlock(&mut_run);

    while(true)
    {
      rcvid = msgReceive(iChannelID_, recvBuffer, 20);
      if(rcvid <= 0)
        break;
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
//---------------------------------------------------------------------------
class CTestServer
 : public CMsgServer
{
public:
  CTestServer(){}
  virtual ~CTestServer(){}
  
  virtual int process(int iReceiveID, void * pRcvMsg);
};

//---------------------------------------------------------------------------
int
CTestServer::process(int iReceiveID, void * pRcvMsg)
{
  if((((char *)pRcvMsg)[0] == 'S') &&
     (((char *)pRcvMsg)[1] == 'R') &&
     (((char *)pRcvMsg)[2] == 'R'))
  {
    printk(" - Message Receive...OK\n");
    if(msgReply(iReceiveID, 0, "SRV", 4) < 0)
      printk(" - Message Reply...ERROR\n");
  }
  else
    printk(" - Message Receive...ERROR\n");

  return 0;
}

//---------------------------------------------------------------------------
void
testSRR()
{
  // Create server thread
  CTestServer server;

  // Lock mutex before activating the thread
  pthread_mutex_lock(&mut_run);
  // Activate the server thread
  server.activate();
  // Wait for server to free mutex
  pthread_mutex_lock(&mut_run);

  // Connect to server
  printk(" - Channel Connect...");
  int iConnectID = channelConnectAttach(0, server.iPID_, server.iChannelID_, 0);
  if(iConnectID > 1)
  {
    printk("OK\n");

    // Send message to server
    char recvBuffer[20];
    if((msgSend(iConnectID, "SRR", 4, recvBuffer, 20) >= 0) &&
       (recvBuffer[0] == 'S') &&
       (recvBuffer[1] == 'R') &&
       (recvBuffer[2] == 'V'))
    {
      printk(" - Message Send...OK\n");
    }
    else
      printk(" - Message Send...ERROR\n");

    channelConnectDetach(iConnectID);
  }
  else
    printk("ERROR\n");
}

//---------------------------------------------------------------------------
// Test sleeping
void
testSleep()
{
  printk(" - Sleep 3x 1s");
  for(int i(0); i < 3; i++)
  {
    sleep(1);
    printk(".");
  }
  printk("OK\n");
}

//---------------------------------------------------------------------------
// Main test thread
void *
testThread(void * arg)
{
  printk("OK\n");

  printk(" - Thread arg...");
  if(arg == (void *)0x21436587)
    printk("OK\n");
  else
    printk("ERROR\n");

  testSleep();
  testSRR();

  // Thread exit
  pthread_exit(NULL);

  return 0;
}

//---------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  printk("Bricks-OS system test:\n");

  // Start test thread since we're called from the kernel thread, and
  // sleeping here will couse the system to die.
  printk(" - Thread Creation...");
  pthread_t thr;
  if(pthread_create(&thr, 0, testThread, (void *)0x21436587) != 0)
    printk("ERROR\n");

  while(1);

  return 0;
}
