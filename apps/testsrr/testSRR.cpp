#include "kernel/debug.h"
#include "kernel/srr.h"
#include "unistd.h"
#include "pthread.h"
#include "msgServer.h"


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

  // Activate the server thread
  server.activate();

  // FIXME: Wait for server to activate
  sleep(1);

  // Connect to server
  printk(" - Channel Connect...");
  int iConnectID = channelConnectAttach(0, server.getPID(), server.getChannelID(), 0);
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
void
testNamed()
{
  // Register "test" @ channel 1234
  if(registerName(1234, "test") >= 0)
  {
    // Lookup "test"
    int iTestPID, iTestChannelID;
    if(lookupName("test", &iTestPID, &iTestChannelID) >= 0)
      printk("\"test\" @ %d-%d\n", iTestPID, iTestChannelID);
    else
      printk("ERROR Trying to lookup \"test\"\n");
  }
  else
    printk("ERROR Trying to register \"test\"\n");
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
  printk(" - Thread Running...OK\n");

  printk(" - Thread arg...");
  if(arg == (void *)0x21436587)
    printk("OK\n");
  else
    printk("ERROR\n");

  testSleep();

  // Only test SRR if we properly pass arguments to the thread
  // becouse ACE_Task_Base needs it.
  if(arg == (void *)0x21436587)
  {
    testSRR();
    testNamed();
  }

  // Thread exit
  pthread_exit((void *)0x78563412);

  return 0;
}

//---------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  printk("Bricks-OS system test:\n");

  // Start test thread since we're called from the kernel thread, and
  // sleeping here will couse the system to die.
  pthread_t thr;
  if(pthread_create(&thr, 0, testThread, (void *)0x21436587) != 0)
  {
    printk(" - Thread Creation...ERROR\n");
  }
  else
  {
    printk(" - Thread Creation...OK\n");

    void * status;
    if(pthread_join(thr, &status) < 0)
    {
      printk(" - Thread Wait...ERROR\n");
    }
    else
    {
      printk(" - Thread Wait...OK\n");
      printk(" - Thread Return Value...");
      if(status == (void *)0x78563412)
        printk("OK\n");
      else
        printk("ERROR\n");
    }
  }

  while(1);

  return 0;
}
