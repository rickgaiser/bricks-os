#ifndef FATDRIVER_H
#define FATDRIVER_H


#include "fileSystem.h"


// -----------------------------------------------------------------------------
struct SBPB
{
  uint8_t  BS_jmpBoot[3];
  char     BS_OEMName[8];
  uint16_t BPB_BytsPerSec;
  uint8_t  BPB_SecPerClus;
  uint16_t BPB_RsvdSecCnt;
  uint8_t  BPB_NumFATs;
  uint16_t BPB_RootEntCnt;
  uint16_t BPB_TotSec16;
  uint8_t  BPB_Media;
  uint16_t BPB_FATSz16;
  uint16_t BPB_SecPerTrk;
  uint16_t BPB_NumHeads;
  uint32_t BPB_HiddSec;
  uint32_t BPB_TotSec32;
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SBPB16
{
  uint8_t  BS_DrvNum;
  uint8_t  BS_Reserved1;
  uint8_t  BS_BootSig;
  uint32_t BS_VolID;
  char     BS_VolLab[11];
  char     BS_FilSysType[8];
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SBPB32
{
  uint32_t BPB_FATSz32;
  uint16_t BPB_ExtFlags;
  uint16_t BPB_FSVer;
  uint32_t BPB_RootClus;
  uint16_t BPB_FSInfo;
  uint16_t BPB_BkBootSec;
  uint8_t  BPB_Reserved[12];
  uint8_t  BS_DrvNum;
  uint8_t  BS_Reserved1;
  uint8_t  BS_BootSig;
  uint32_t BS_VolID;
  char     BS_VolLab[11];
  char     BS_FilSysType[8];
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
struct SFATEntry
{
  uint8_t  name[11];
  uint8_t  attr;
  uint8_t  reserved1;
  uint8_t  createTime10ms;
  uint16_t createTime;
  uint16_t createDate;
  uint16_t accessDate;
  uint16_t firstClusterHi;
  uint16_t writeTime;
  uint16_t writeDate;
  uint16_t firstClusterLo;
  uint32_t fileSize;
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
class CFATDriver
 : public IFileSystemDriver
{
public:
  CFATDriver();
  virtual ~CFATDriver();

  virtual bool init(IBlockDevice * device);
};

// -----------------------------------------------------------------------------
class CFATVolume
 : public IMountPoint
{
public:
  CFATVolume(IBlockDevice * device);
  virtual ~CFATVolume();

  virtual int open(void * fileStruct, const char * path, int flags, int mode);
  virtual int close(int fd);
  virtual int write(int fd, const char * ptr, int len);
  virtual int read(int fd, char * ptr, int len);

private:
  IBlockDevice * device_;

  uint8_t * pBootSector_;
  SBPB    * pBPB_;
  SBPB16  * pBPB16_;
  SBPB32  * pBPB32_;

  uint32_t iClusterCount_;
  uint32_t iRootDirSectors_;
  uint32_t iDataSectors_;
  uint32_t iFATSz_;
  uint32_t iTotSec_;
  uint32_t iFirstDataSector_;
  uint32_t iRootDirSec_;
};


#endif
