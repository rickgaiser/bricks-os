/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "kernel/debug.h"
#include "kernel/srr.h"
#include "unistd.h"
#include "fcntl.h"
#include "pthread.h"
#include "msgServer.h"
#include "fileStream.h"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class CTestServer
 : public CMsgServer
{
public:
  CTestServer(const char * name = 0) : CMsgServer(name) {}
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
void
testNamed2()
{
  // Create named server thread
  CTestServer server("CTestServer");

  // Activate the server thread
  server.activate();

  // FIXME: Wait for server to activate
  sleep(1);

  printk(" - Open File...");
  int fd = open("CTestServer", 0/*O_RDWR*/);

  if(fd >= 0)
  {
    printk("OK\n");

    printk(" - Close File...");
    if(close(fd) >= 0)
      printk("OK\n");
    else
      printk("ERROR\n");
  }
  else
    printk("ERROR\n");
}

//---------------------------------------------------------------------------
void
testFile()
{
  char c;
  int iRetVal;
  int fd;

  printk(" - Open File...");
  fd = open("/dev/debug", O_RDWR);
  if(fd >= 0)
  {
    printk("OK\n");

    printk(" - Read File...");
    iRetVal = read(fd, &c, 1);
    if(iRetVal == 1)
      printk("OK");
    else
      printk("ERROR(%d)\n", iRetVal);

    printk(" - Write File...");
    iRetVal = write(fd, "OK\n", 4);
    if(iRetVal != 4)
      printk("ERROR(%d)\n", iRetVal);

    printk(" - Close File...");
    iRetVal = close(fd);
    if(iRetVal >= 0)
      printk("OK\n");
    else
      printk("ERROR(%d)\n", iRetVal);
  }
  else
    printk("ERROR(%d)\n", fd);
}

//---------------------------------------------------------------------------
void
testFile2()
{
#ifdef NDS7
  CFileStream stream("/dev/keyboard", "/dev/ipc");
#elif NDS9
  CFileStream stream("/dev/ipc", "/dev/debug2");
#else
  CFileStream stream("/dev/keyboard", "/dev/debug");
#endif

  if(stream.start() == true)
  {
    printk("Running:\n");
    while(true)
      sleep(1);
  }
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
    testNamed2();
    testFile();
    testFile2();
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

  // Forever sleep
  while(true)
  {
    sleep(1);
  }

  return 0;
}
