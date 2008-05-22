#ifndef KERNEL_DRIVERKERNEL_H
#define KERNEL_DRIVERKERNEL_H


// -----------------------------------------------------------------------------
// Interface for kernel drivers
class IKernelDriver
{
public:
  virtual ~IKernelDriver(){}

  virtual int msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize) = 0;
};


#endif
