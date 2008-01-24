#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include "inttypes.h"
#include "vector"


// -----------------------------------------------------------------------------
class IBlockDevice
{
public:
  virtual ~IBlockDevice(){}

  virtual int read (uint32_t startSector, uint32_t sectorCount,       void * data) = 0;
  virtual int write(uint32_t startSector, uint32_t sectorCount, const void * data) = 0;
};

// -----------------------------------------------------------------------------
class CPartition
 : public IBlockDevice
{
public:
  CPartition(IBlockDevice * blockDevice, uint32_t startSector, uint32_t sectorCount);
  virtual ~CPartition();

  virtual int read (uint32_t startSector, uint32_t sectorCount,       void * data);
  virtual int write(uint32_t startSector, uint32_t sectorCount, const void * data);

protected:
  IBlockDevice * pBlockDevice_;
  uint32_t iStartSector_;
  uint32_t iEndSector_;
  uint32_t iSectorCount_;
};

// -----------------------------------------------------------------------------
class IPartitionDriver
{
public:
  virtual ~IPartitionDriver(){}

  virtual bool init(IBlockDevice * device) = 0;
};

// -----------------------------------------------------------------------------
class IFileSystemDriver
{
public:
  virtual ~IFileSystemDriver(){}

  virtual bool init(IBlockDevice * device) = 0;
};

// -----------------------------------------------------------------------------
class IMountPoint
{
public:
  virtual ~IMountPoint(){}

  virtual int open(void * fileStruct, const char * path, int flags, int mode) = 0;
  virtual int close(int fd) = 0;
  virtual int write(int fd, const char * ptr, int len) = 0;
  virtual int read(int fd, char * ptr, int len) = 0;
/*
  virtual int seek(int fd, int pos, int dir) = 0;
  virtual int fstat(int fd, struct stat * st) = 0;
  virtual int stat(const char * file, struct stat * st) = 0;
  virtual int link(const char * existing, const char * newLink) = 0;
  virtual int unlink(const char * name) = 0;
  virtual int chdir(const char * name) = 0;

  virtual int rename(const char * oldName, const char * newName) = 0;
  virtual int mkdir(const char * path, int mode) = 0;

  virtual DIR_ITER * diropen(DIR_ITER * dirState, const char * path) = 0;
  virtual int dirreset(DIR_ITER * dirState) = 0;
  virtual int dirnext(DIR_ITER * dirState, char * filename, struct stat * filestat) = 0;
  virtual int dirclose(DIR_ITER * dirState) = 0;
*/
};

// -----------------------------------------------------------------------------
class CFileSystem
{
public:
  static void addBlockDevice(IBlockDevice * device);
  static void remBlockDevice(IBlockDevice * device);

  static void addPartitionDriver(IPartitionDriver * driver);
  static void remPartitionDriver(IPartitionDriver * driver);

  static void addFileSystemDriver(IFileSystemDriver * driver);
  static void remFileSystemDriver(IFileSystemDriver * driver);

  static void mount(IMountPoint * point, const char * location);
  static void unmount(IMountPoint * point);
  static void unmount(const char * location);

private:
  typedef std::vector<IBlockDevice *>       blockdevicelist_t;
  typedef std::vector<IPartitionDriver *>   partitiondriverlist_t;
  typedef std::vector<IFileSystemDriver *>  filesystemdriverlist_t;
  struct SMountPoint
  {
    char * location;
    IMountPoint * point;
  };
  typedef std::vector<SMountPoint *>        mountpointlist_t;

  static blockdevicelist_t       listBlockDevices_;
  static partitiondriverlist_t   listPartitionDrivers_;
  static filesystemdriverlist_t  listFileSystemDrivers_;
  static mountpointlist_t        listMountPoints_;
};


#endif
