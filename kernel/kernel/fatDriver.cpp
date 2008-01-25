#include "kernel/debug.h"
#include "kernel/fatDriver.h"
#include "stddef.h"
#include "string.h"


//#define LE16(x)  (((x<<8)&0xff00)|((x>>8)&0x00ff))
//#define LE32(x)  (((x<<24)&0xff000000)|((x<<8)&0x00ff0000)|((x>>8)&0x0000ff00)|((x>>24)&0x000000ff))
#define LE16(x)  (x)
#define LE32(x)  (x)


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
    //SBPB   * pBPB   = (SBPB   *)(&data[0]);
    SBPB16 * pBPB16 = (SBPB16 *)(&data[36]);
    SBPB32 * pBPB32 = (SBPB32 *)(&data[36]);

    // Validate boot signature
    if((data[0x1FE] == 0x55) && (data[0x1FF] == 0xAA))
    {
      // Validate "FAT" string
      if(((pBPB16->BS_FilSysType[0] == 'F') && (pBPB16->BS_FilSysType[1] == 'A') && (pBPB16->BS_FilSysType[2] == 'T')) ||
         ((pBPB32->BS_FilSysType[0] == 'F') && (pBPB32->BS_FilSysType[1] == 'A') && (pBPB32->BS_FilSysType[2] == 'T')))
      {
        // Mount to the filesystem
        // FIXME: Where do we mount the volume?
        CFileSystem::mount(new CFATVolume(device), "drive0");
        bRetVal = true;
      }
    }
  }
  else
    printk("CFATDriver::init: ERROR: Unable to read boot sector\n");

  delete data;

  return bRetVal;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CFATVolume::CFATVolume(IBlockDevice * device)
 : device_(device)
{
  pBootSector_ = new uint8_t[512];

  if(device_->read(0, 1, pBootSector_) == 0)
  {
    pBPB_   = (SBPB   *)(&pBootSector_[0]);
    pBPB16_ = (SBPB16 *)(&pBootSector_[36]);
    pBPB32_ = (SBPB32 *)(&pBootSector_[36]);

    // Validate boot signature
    if((pBootSector_[0x1FE] == 0x55) && (pBootSector_[0x1FF] == 0xAA))
    {
      // Validate "FAT" string
      if(((pBPB16_->BS_FilSysType[0] == 'F') && (pBPB16_->BS_FilSysType[1] == 'A') && (pBPB16_->BS_FilSysType[2] == 'T')) ||
         ((pBPB32_->BS_FilSysType[0] == 'F') && (pBPB32_->BS_FilSysType[1] == 'A') && (pBPB32_->BS_FilSysType[2] == 'T')))
      {
        // Root directory sector count
        iRootDirSectors_ = ((pBPB_->BPB_RootEntCnt * 32) + (pBPB_->BPB_BytsPerSec - 1)) / pBPB_->BPB_BytsPerSec;
        // FAT size
        if(pBPB_->BPB_FATSz16 != 0)
          iFATSz_ = pBPB_->BPB_FATSz16;
        else
          iFATSz_ = pBPB32_->BPB_FATSz32;
        // First data sector
        iFirstDataSector_ = pBPB_->BPB_RsvdSecCnt + (pBPB_->BPB_NumFATs * iFATSz_) + iRootDirSectors_;
        // Total sectors
        if(pBPB_->BPB_TotSec16 != 0)
          iTotSec_ = pBPB_->BPB_TotSec16;
        else
          iTotSec_ = pBPB_->BPB_TotSec32;
        // Data sectors
        iDataSectors_ = iTotSec_ - (pBPB_->BPB_RsvdSecCnt + (pBPB_->BPB_NumFATs * iFATSz_) + iRootDirSectors_);
        // Total clusters
        iClusterCount_ = iDataSectors_ / pBPB_->BPB_SecPerClus;
        // Root directory start sector
        iRootDirSec_ = pBPB_->BPB_RsvdSecCnt + (pBPB_->BPB_NumFATs * pBPB_->BPB_FATSz16);

        char volumeName[12];
        volumeName[11] = 0;
        if(iClusterCount_ < 4085)
        {
          memcpy(volumeName, pBPB16_->BS_VolLab, 11);
          printk("CFATVolume::CFATVolume: FAT12 volume found:\n");
        }
        else if(iClusterCount_ < 65525)
        {
          memcpy(volumeName, pBPB16_->BS_VolLab, 11);
          printk("CFATVolume::CFATVolume: FAT16 volume found:\n");
        }
        else
        {
          memcpy(volumeName, pBPB32_->BS_VolLab, 11);
          printk("CFATVolume::CFATVolume: FAT32 volume found:\n");
        }
        printk(" - Name: \"%s\"\n", volumeName);
        printk(" - Size: %dMiB\n", (iTotSec_ * pBPB_->BPB_BytsPerSec) / (1024*1024));
      }
      else
        printk("CFATVolume::CFATVolume: ERROR: \"FAT\" string not found\n");
    }
    else
      printk("CFATVolume::CFATVolume: ERROR: Invalid boot signature\n");
  }
  else
    printk("CFATVolume::CFATVolume: ERROR: Unable to read sector\n");
}

// -----------------------------------------------------------------------------
CFATVolume::~CFATVolume()
{
  if(pBootSector_ != NULL)
    delete pBootSector_;
}

// -----------------------------------------------------------------------------
int
CFATVolume::open(const char * path, int flags, int mode)
{
  printk("CFATVolume::open(%s)\n", path);

  return -1;
}

// -----------------------------------------------------------------------------
int
CFATVolume::close(int fd)
{
  printk("CFATVolume::close\n");

  return -1;
}

// -----------------------------------------------------------------------------
int
CFATVolume::read(int fd, char * ptr, int len)
{
  printk("CFATVolume::read\n");

  return -1;
}

// -----------------------------------------------------------------------------
int
CFATVolume::write(int fd, const char * ptr, int len)
{
  printk("CFATVolume::write\n");

  return -1;
}

// -----------------------------------------------------------------------------
DIR_ITER *
CFATVolume::diropen(const char * path)
{
  printk("CFATVolume::diropen(%s)\n", path);

  /*
  // TEST: Print root directory
  printk("Root directory:\n", iRootDirSec_, iRootDirSectors_);
  SFATEntry * entry = (SFATEntry *)new uint8_t[512];
  if(device_->read(iRootDirSec_, 1, entry) == 0)
  {
    for(unsigned int i(0); i < (512/sizeof(SFATEntry)); i++)
    {
      // End of directory
      if(entry[i].name[0] == 0)
        break;

      // Not an empty entry
      if(entry[i].name[0] != 0xe5)
      {
        char entryName[12];
        entryName[11] = 0;
        memcpy(entryName, entry[i].name, 11);
        printk(" - %s\n", entryName);
      }
    }
  }
  else
    printk("CFATVolume::CFATVolume: ERROR: Unable to read root directory sector\n");
  delete entry;
  */

  return NULL;
}

// -----------------------------------------------------------------------------
int
CFATVolume::dirreset(DIR_ITER * dirState)
{
  printk("CFATVolume::dirreset\n");

  return -1;
}

// -----------------------------------------------------------------------------
int
CFATVolume::dirnext(DIR_ITER * dirState, char * filename, struct stat * filestat)
{
  printk("CFATVolume::dirnext\n");

  return -1;
}

// -----------------------------------------------------------------------------
int
CFATVolume::dirclose(DIR_ITER * dirState)
{
  printk("CFATVolume::dirclose\n");

  return -1;
}
