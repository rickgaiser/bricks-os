#include "kernel/driverMountPoint.h"
#include "kernel/srr_k.h"
#include "kernel/debug.h"


// -----------------------------------------------------------------------------
CMountPointDriver::CMountPointDriver(IMountPoint * mountPoint)
 : pMountPoint_(0)
{
  setMountPoint(mountPoint);
}

// -----------------------------------------------------------------------------
CMountPointDriver::~CMountPointDriver()
{
  setMountPoint(0);
}

// -----------------------------------------------------------------------------
void
CMountPointDriver::setMountPoint(IMountPoint * mountPoint)
{
//  if(pMountPoint_ != 0)
//    removeKernelDriver(this);

  pMountPoint_ = mountPoint;

//  if(pMountPoint_ != 0)
//    addKernelDriver(this);
}

// -----------------------------------------------------------------------------
int
CMountPointDriver::msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  int iRetVal(-1);

  if(pMountPoint_ != 0)
  {
    SKernelMessageHeader * pHeader = (SKernelMessageHeader *)pSndMsg;

    if((pHeader->iHeaderSize == sizeof(SKernelMessageHeader)) &&
       (pHeader->iVersion == INTERFACE_VERSION(1, 0)))
    {
      switch(pHeader->iFunctionID)
      {
        unwrapfunc3r(open,  pMountPoint_->open,  pSndMsg, const char *, path, int, flags, int, mode);
        unwrapfunc1r(close, pMountPoint_->close, pSndMsg, int, fd);
        unwrapfunc3r(read,  pMountPoint_->read,  pSndMsg, int, fd, char *, ptr, int, len);
        unwrapfunc3r(write, pMountPoint_->write, pSndMsg, int, fd, const char *, ptr, int, len);

        default:
          printk("CMountPointDriver: Invalid function id: %d\n", pHeader->iFunctionID);
      };
    }
    else
    {
      printk("CMountPointDriver: Invalid header(0x%x, %d, %d, 0x%x)\n", pHeader, pHeader->iHeaderSize, iSndSize, pHeader->iVersion);
    }
  }

  return iRetVal;
}
