#include "kernel/debug.h"
#include "kernel/fatDriver.h"


//#define LE16(x)  (((x<<8)&0xff00)|((x>>8)&0x00ff))
//#define LE32(x)  (((x<<24)&0xff000000)|((x<<8)&0x00ff0000)|((x>>8)&0x0000ff00)|((x>>24)&0x000000ff))
#define LE16(x)  (x)
#define LE32(x)  (x)


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

struct SBPB16
{
  uint8_t  BS_DrvNum;
  uint8_t  BS_Reserved1;
  uint8_t  BS_BootSig;
  uint32_t BS_VolID;
  char     BS_VolLab[11];
  char     BS_FilSysType[8];
} __attribute__ ((__packed__));

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
// -----------------------------------------------------------------------------
CFATDriver::CFATDriver()
{
}

// -----------------------------------------------------------------------------
CFATDriver::~CFATDriver()
{
}

// -----------------------------------------------------------------------------
bool
CFATDriver::init(IBlockDevice * device)
{
  uint8_t * data = new uint8_t[512];
  bool bRetVal(false);

  //printk("CFATDriver::init\n");

  if(device->read(0, 1, data) == 0)
  {
    SBPB   * pBPB   = (SBPB   *)(&data[0]);
    SBPB16 * pBPB16 = (SBPB16 *)(&data[36]);
    SBPB32 * pBPB32 = (SBPB32 *)(&data[36]);

    // Validate boot signature
    if((data[0x1FE] == 0x55) && (data[0x1FF] == 0xAA))
    {
      // Validate "FAT" string
      if(((pBPB16->BS_FilSysType[0] == 'F') && (pBPB16->BS_FilSysType[1] == 'A') && (pBPB16->BS_FilSysType[2] == 'T')) ||
         ((pBPB32->BS_FilSysType[0] == 'F') && (pBPB32->BS_FilSysType[1] == 'A') && (pBPB32->BS_FilSysType[2] == 'T')))
      {
        uint32_t iClusterCount;
        uint32_t iRootDirSectors;
        uint32_t iDataSectors;        uint32_t iFATSz;
        uint32_t iTotSec;

        iRootDirSectors = ((pBPB->BPB_RootEntCnt * 32) + (pBPB->BPB_BytsPerSec - 1)) / pBPB->BPB_BytsPerSec;
        if(pBPB->BPB_FATSz16 != 0)
          iFATSz = pBPB->BPB_FATSz16;
        else
          iFATSz = LE32(pBPB32->BPB_FATSz32);
        if(pBPB->BPB_TotSec16 != 0)
          iTotSec = pBPB->BPB_TotSec16;
        else          iTotSec = pBPB->BPB_TotSec32;
        iDataSectors = iTotSec - (pBPB->BPB_RsvdSecCnt + (pBPB->BPB_NumFATs * iFATSz) + iRootDirSectors);
        iClusterCount = iDataSectors / pBPB->BPB_SecPerClus;

        if(iClusterCount < 4085)        {
          pBPB16->BS_VolLab[10] = 0;
          printk("CFATDriver::init: valid FAT12 volume found (%s)\n", pBPB16->BS_VolLab);
        }
        else if(iClusterCount < 65525)        {
          pBPB16->BS_VolLab[10] = 0;
          printk("CFATDriver::init: valid FAT16 volume found (%s)\n", pBPB16->BS_VolLab);
        }
        else        {
          pBPB32->BS_VolLab[10] = 0;
          printk("CFATDriver::init: valid FAT32 volume found (%s)\n", pBPB32->BS_VolLab);
        }

        // Mount to the filesystem
        // FIXME: Where do we mount the volume?
        CFileSystem::mount(new CFATMountPoint(device), "drive0");
        bRetVal = true;
      }
      //else
      //  printk(" - ERROR: \"FAT\" string not found\n");
    }
    //else
    //  printk(" - ERROR: Invalid boot signature\n");
  }
  //else
  //  printk(" - ERROR: Unable to read sector\n");

  delete data;

  return bRetVal;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CFATMountPoint::CFATMountPoint(IBlockDevice * device)
 : device_(device)
{
}

// -----------------------------------------------------------------------------
CFATMountPoint::~CFATMountPoint()
{
}

// -----------------------------------------------------------------------------
int
CFATMountPoint::open(void * fileStruct, const char * path, int flags, int mode)
{
  printk("CFATMountPoint::open(%s)\n", path);

  return 0;
}

// -----------------------------------------------------------------------------
int
CFATMountPoint::close(int fd)
{
  printk("CFATMountPoint::close\n");

  return 0;
}

// -----------------------------------------------------------------------------
int
CFATMountPoint::write(int fd, const char * ptr, int len)
{
  printk("CFATMountPoint::write\n");

  return 0;
}

// -----------------------------------------------------------------------------
int
CFATMountPoint::read(int fd, char * ptr, int len)
{
  printk("CFATMountPoint::read\n");

  return 0;
}
