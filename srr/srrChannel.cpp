#include "kernel/srr.h"
#include "kernel/srr_s.h"
#include "kernel/srrChannel.h"
#include "kernel/srrChannel_s.h"  // Shared kernel header, defines packing/unpacking
#include "string.h"


//---------------------------------------------------------------------------
// Channels
//---------------------------------------------------------------------------
int
channelCreate(unsigned iFlags)
{
  int iRetVal;
  int iSndSize(sizeof(SKernelMessageHeader) + sizeof(SChannelCreate));
  unsigned char * pSndData = new unsigned char[iSndSize];
  SKernelMessageHeader * pHeader = (SKernelMessageHeader *)(pSndData);
  SChannelCreate * pArgs = (SChannelCreate *)(pSndData + sizeof(SKernelMessageHeader));

  // Create header
  pHeader->iHeaderSize = sizeof(SKernelMessageHeader);
  pHeader->iVersion    = INTERFACE_VERSION(1,0);
  pHeader->iFunctionID = kfCHANNEL_CREATE;

  // Create arguments
  pArgs->iFlags        = iFlags;

  // Send message
  iRetVal = msgSend(scKERNEL, pSndData, iSndSize, 0, 0);

  delete[] pSndData;

  return iRetVal;
}

//---------------------------------------------------------------------------
int
channelDestroy(int iChannelID)
{
  int iRetVal;
  int iSndSize(sizeof(SKernelMessageHeader) + sizeof(SChannelDestroy));
  unsigned char * pSndData = new unsigned char[iSndSize];
  SKernelMessageHeader * pHeader = (SKernelMessageHeader *)(pSndData);
  SChannelDestroy * pArgs = (SChannelDestroy *)(pSndData + sizeof(SKernelMessageHeader));

  // Create header
  pHeader->iHeaderSize = sizeof(SKernelMessageHeader);
  pHeader->iVersion    = INTERFACE_VERSION(1,0);
  pHeader->iFunctionID = kfCHANNEL_DESTROY;

  // Create arguments
  pArgs->iChannelID    = iChannelID;

  // Send message
  iRetVal = msgSend(scKERNEL, pSndData, iSndSize, 0, 0);

  delete[] pSndData;

  return iRetVal;
}

//---------------------------------------------------------------------------
int
channelConnectAttach(uint32_t iNodeID, pid_t iProcessID, int iChannelID, int iFlags)
{
  int iRetVal;
  int iSndSize(sizeof(SKernelMessageHeader) + sizeof(SConnectAttach));
  unsigned char * pSndData = new unsigned char[iSndSize];
  SKernelMessageHeader * pHeader = (SKernelMessageHeader *)(pSndData);
  SConnectAttach * pArgs = (SConnectAttach *)(pSndData + sizeof(SKernelMessageHeader));

  // Create header
  pHeader->iHeaderSize = sizeof(SKernelMessageHeader);
  pHeader->iVersion    = INTERFACE_VERSION(1,0);
  pHeader->iFunctionID = kfCHANNEL_ATTACH;

  // Create arguments
  pArgs->iNodeID       = iNodeID;
  pArgs->iProcessID    = iProcessID;
  pArgs->iChannelID    = iChannelID;
  pArgs->iFlags        = iFlags;

  // Send message
  iRetVal = msgSend(scKERNEL, pSndData, iSndSize, 0, 0);

  delete[] pSndData;

  return iRetVal;
}

//---------------------------------------------------------------------------
int
channelConnectDetach(int iConnectionID)
{
  int iRetVal;
  int iSndSize(sizeof(SKernelMessageHeader) + sizeof(SConnectDetach));
  unsigned char * pSndData = new unsigned char[iSndSize];
  SKernelMessageHeader * pHeader = (SKernelMessageHeader *)(pSndData);
  SConnectDetach * pArgs = (SConnectDetach *)(pSndData + sizeof(SKernelMessageHeader));

  // Create header
  pHeader->iHeaderSize = sizeof(SKernelMessageHeader);
  pHeader->iVersion    = INTERFACE_VERSION(1,0);
  pHeader->iFunctionID = kfCHANNEL_DETACH;

  // Create arguments
  pArgs->iConnectionID = iConnectionID;

  // Send message
  iRetVal = msgSend(scKERNEL, pSndData, iSndSize, 0, 0);

  delete[] pSndData;

  return iRetVal;
}
/*
//---------------------------------------------------------------------------
// Named channels
//---------------------------------------------------------------------------
int
channelCreateNamed(const char * sName, unsigned iFlags)
{
  int iRetVal;
  int iSndSize(sizeof(SKernelMessageHeader) + sizeof(SChannelCreateNamed) + strlen(sName) + 1);
  unsigned char * pSndData = new unsigned char[iSndSize];
  SKernelMessageHeader * pHeader = (SKernelMessageHeader *)(pSndData);
  SChannelCreateNamed * pArgs = (SChannelCreateNamed *)(pSndData + sizeof(SKernelMessageHeader));
  char * pStrings = (char *)(pSndData + sizeof(SKernelMessageHeader) + sizeof(SChannelCreateNamed));

  // Create header
  pHeader->iHeaderSize = sizeof(SKernelMessageHeader);
  pHeader->iVersion    = INTERFACE_VERSION(1,0);
  pHeader->iFunctionID = kfCHANNEL_CREATE_NAMED;

  // Create arguments
  pArgs->iNameIndex    = 0;
  pArgs->iFlags        = iFlags;

  // Fill data
  strcpy(pStrings, sName);

  // Send message
  iRetVal = msgSend(scKERNEL, pSndData, iSndSize, 0, 0);

  delete[] pSndData;

  return iRetVal;
}

//---------------------------------------------------------------------------
int
channelDestroyNamed(const char * sName)
{
  int iRetVal;
  int iSndSize(sizeof(SKernelMessageHeader) + sizeof(SChannelDestroyNamed) + strlen(sName) + 1);
  unsigned char * pSndData = new unsigned char[iSndSize];
  SKernelMessageHeader * pHeader = (SKernelMessageHeader *)(pSndData);
  SChannelDestroyNamed * pArgs = (SChannelDestroyNamed *)(pSndData + sizeof(SKernelMessageHeader));
  char * pStrings = (char *)(pSndData + sizeof(SKernelMessageHeader) + sizeof(SChannelDestroyNamed));

  // Create header
  pHeader->iHeaderSize = sizeof(SKernelMessageHeader);
  pHeader->iVersion    = INTERFACE_VERSION(1,0);
  pHeader->iFunctionID = kfCHANNEL_DESTROY_NAMED;

  // Create arguments
  pArgs->iNameIndex    = 0;

  // Fill data
  strcpy(pStrings, sName);

  // Send message
  iRetVal = msgSend(scKERNEL, pSndData, iSndSize, 0, 0);

  delete[] pSndData;

  return iRetVal;
}

//---------------------------------------------------------------------------
int
channelConnectAttachNamed(const char * sName, int iFlags)
{
  int iRetVal;
  int iSndSize(sizeof(SKernelMessageHeader) + sizeof(SConnectAttachNamed) + strlen(sName) + 1);
  unsigned char * pSndData = new unsigned char[iSndSize];
  SKernelMessageHeader * pHeader = (SKernelMessageHeader *)(pSndData);
  SConnectAttachNamed * pArgs = (SConnectAttachNamed *)(pSndData + sizeof(SKernelMessageHeader));
  char * pStrings = (char *)(pSndData + sizeof(SKernelMessageHeader) + sizeof(SConnectAttachNamed));

  // Create header
  pHeader->iHeaderSize = sizeof(SKernelMessageHeader);
  pHeader->iVersion    = INTERFACE_VERSION(1,0);
  pHeader->iFunctionID = kfCHANNEL_ATTACH_NAMED;

  // Create arguments
  pArgs->iNameIndex    = 0;
  pArgs->iFlags        = iFlags;

  // Fill data
  strcpy(pStrings, sName);

  // Send message
  iRetVal = msgSend(scKERNEL, pSndData, iSndSize, 0, 0);

  delete[] pSndData;

  return iRetVal;
}

//---------------------------------------------------------------------------
int
channelConnectDetachNamed(const char * sName)
{
  int iRetVal;
  int iSndSize(sizeof(SKernelMessageHeader) + sizeof(SConnectDetachNamed) + strlen(sName) + 1);
  unsigned char * pSndData = new unsigned char[iSndSize];
  SKernelMessageHeader * pHeader = (SKernelMessageHeader *)(pSndData);
  SConnectDetachNamed * pArgs = (SConnectDetachNamed *)(pSndData + sizeof(SKernelMessageHeader));
  char * pStrings = (char *)(pSndData + sizeof(SKernelMessageHeader) + sizeof(SConnectDetachNamed));

  // Create header
  pHeader->iHeaderSize = sizeof(SKernelMessageHeader);
  pHeader->iVersion    = INTERFACE_VERSION(1,0);
  pHeader->iFunctionID = kfCHANNEL_DETACH_NAMED;

  // Create arguments
  pArgs->iNameIndex    = 0;

  // Fill data
  strcpy(pStrings, sName);

  // Send message
  iRetVal = msgSend(scKERNEL, pSndData, iSndSize, 0, 0);

  delete[] pSndData;

  return iRetVal;
}
*/
