#ifndef SHARED_SRRCHANNEL_H
#define SHARED_SRRCHANNEL_H


#include "sys/types.h"
#include "inttypes.h"


// -----------------------------------------------------------------------------
// Header to be included in every kernel message
// size: 12 bytes
struct SKernelMessageHeader
{
  // Size of this header (so the header can be extended in the future)
  uint32_t iHeaderSize;
  // Version of message being sent (allows backward compatibility in the future)
  uint32_t iVersion;
  // ID of function to be called
  uint32_t iFunctionID;
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
// MACRO for interface version
// Message version format: 8.24, major.minor
//   Major: interface version (interface changes MUST have a new major version)
//   Minor: interface additions (add new functionality to the interface)
// Note:
//   Try only to add functionality to the interface, try to prevent changing
//   it. If the interface has to change, try to be backward compatible.
#define INTERFACE_VERSION(major, minor) ((major<<24)+minor)

// -----------------------------------------------------------------------------
// Functions defined for the kernel connection
enum EKernelFunctionID
{
   kfERROR = 0  // Error detection for function ID == 0

   // Channels
  ,kfCHANNEL_CREATE
  ,kfCHANNEL_DESTROY
  ,kfCHANNEL_ATTACH
  ,kfCHANNEL_DETACH

  // Named channels
  ,kfCHANNEL_CREATE_NAMED
  ,kfCHANNEL_DESTROY_NAMED
  ,kfCHANNEL_ATTACH_NAMED
  ,kfCHANNEL_DETACH_NAMED

  // Files
  ,kfFILE_OPEN
  ,kfFILE_CLOSE
  ,kfFILE_READ
  ,kfFILE_WRITE
  ,kfFILE_LSEEK
  ,kfFILE_IOCTL
  ,kfFILE_FCNTL
};

// -----------------------------------------------------------------------------
// Structures defined for kernel messages
// Channel structs
struct SChannelCreate
{
  unsigned iFlags;
} __attribute__ ((__packed__));

struct SChannelDestroy
{
  unsigned iChannelID;
} __attribute__ ((__packed__));

struct SConnectAttach
{
  uint32_t iNodeID;
  pid_t iProcessID;
  int iChannelID;
  int iFlags;
} __attribute__ ((__packed__));

struct SConnectDetach
{
  int iConnectionID;
} __attribute__ ((__packed__));

// Named channel structs
struct SChannelCreateNamed
{
  unsigned short iNameIndex;
  unsigned iFlags;
} __attribute__ ((__packed__));

struct SChannelDestroyNamed
{
  unsigned short iNameIndex;
} __attribute__ ((__packed__));

struct SConnectAttachNamed
{
  unsigned short iNameIndex;
  int iFlags;
} __attribute__ ((__packed__));

struct SConnectDetachNamed
{
  unsigned short iNameIndex;
} __attribute__ ((__packed__));

// File structs
struct SFileOpen
{
  unsigned short iNameIndex;
  int iOFlags;
} __attribute__ ((__packed__));

struct SFileClose
{
  int iFD;
} __attribute__ ((__packed__));

struct SFileRead
{
  int iFD;
  size_t iSize;
  off_t iOffset;
} __attribute__ ((__packed__));

struct SFileWrite
{
  int iFD;
  size_t iSize;
  off_t iOffset;
} __attribute__ ((__packed__));

struct SFileLseek
{
  int iFD;
  off_t iOffset;
  int iWhence;
} __attribute__ ((__packed__));

struct SFileIoctl
{
  int iFD;
  int iRequest;
} __attribute__ ((__packed__));

struct SFileFcntl
{
  int iFD;
  int iCMD;
} __attribute__ ((__packed__));


#endif
