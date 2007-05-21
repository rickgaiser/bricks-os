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
class CFileSystem
{
public:
  static void addBlockDevice(IBlockDevice * device);
  static void remBlockDevice(IBlockDevice * device);

  static void addPartitionDriver(IPartitionDriver * driver);
  static void remPartitionDriver(IPartitionDriver * driver);

  static void addFileSystemDriver(IFileSystemDriver * driver);
  static void remFileSystemDriver(IFileSystemDriver * driver);

private:
  typedef std::vector<IBlockDevice *>       blockdevicelist_t;
  typedef std::vector<IPartitionDriver *>   partitiondriverlist_t;
  typedef std::vector<IFileSystemDriver *>  filesystemdriverlist_t;

  static blockdevicelist_t       listBlockDevices_;
  static partitiondriverlist_t   listPartitionDrivers_;
  static filesystemdriverlist_t  listFileSystemDrivers_;
};


#endif
