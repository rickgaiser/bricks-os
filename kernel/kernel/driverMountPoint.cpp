#include "kernel/driverMountPoint.h"


// -----------------------------------------------------------------------------
CMountPointDriver::CMountPointDriver(IMountPoint * mountPoint)
 : pMountPoint_(mountPoint)
{
}

// -----------------------------------------------------------------------------
CMountPointDriver::~CMountPointDriver()
{
}

// -----------------------------------------------------------------------------
int
CMountPointDriver::msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  return -1;
}
