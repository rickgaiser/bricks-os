#include "kernel/fileSystem.h"
#include "string.h"


CFileSystem::blockdevicelist_t       CFileSystem::listBlockDevices_;
CFileSystem::partitiondriverlist_t   CFileSystem::listPartitionDrivers_;
CFileSystem::filesystemdriverlist_t  CFileSystem::listFileSystemDrivers_;
CFileSystem::mountpointlist_t        CFileSystem::listMountPoints_;


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CPartition::CPartition(IBlockDevice * blockDevice, uint32_t startSector, uint32_t sectorCount)
 : pBlockDevice_(blockDevice)
 , iStartSector_(startSector)
 , iEndSector_  (startSector + sectorCount - 1)
 , iSectorCount_(sectorCount)
{
}

// -----------------------------------------------------------------------------
CPartition::~CPartition()
{
}

// -----------------------------------------------------------------------------
int
CPartition::read(uint32_t startSector, uint32_t sectorCount, void * data)
{
  if((startSector + iStartSector_ + sectorCount) <= iEndSector_)
    return pBlockDevice_->read(startSector + iStartSector_, sectorCount, data);
  else
    return -1;
}

// -----------------------------------------------------------------------------
int
CPartition::write(uint32_t startSector, uint32_t sectorCount, const void * data)
{
  if((startSector + iStartSector_ + sectorCount) <= iEndSector_)
    return pBlockDevice_->write(startSector + iStartSector_, sectorCount, data);
  else
    return -1;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
CFileSystem::addBlockDevice(IBlockDevice * device)
{
  // Check if device is already in the list
  // ...

  // Add device to the list
  listBlockDevices_.push_back(device);

  // Check for partitions on the block device
  for(unsigned int i(0); i < listPartitionDrivers_.size(); i++)
    if(listPartitionDrivers_[i]->init(device) == true)
      return;

  // Check for file systems on the block device
  for(unsigned int i(0); i < listFileSystemDrivers_.size(); i++)
    if(listFileSystemDrivers_[i]->init(device) == true)
      return;
}

// -----------------------------------------------------------------------------
void
CFileSystem::remBlockDevice(IBlockDevice * device)
{
}

// -----------------------------------------------------------------------------
void
CFileSystem::addPartitionDriver(IPartitionDriver * driver)
{
  // Check if driver is already in the list
  // ...

  // Add driver to the list
  listPartitionDrivers_.push_back(driver);

  // Check for unused block devices
  // ...
}

// -----------------------------------------------------------------------------
void
CFileSystem::remPartitionDriver(IPartitionDriver * driver)
{
}

// -----------------------------------------------------------------------------
void
CFileSystem::addFileSystemDriver(IFileSystemDriver * driver)
{
  // Check if driver is already in the list
  // ...

  // Add driver to the list
  listFileSystemDrivers_.push_back(driver);

  // Check for unused block devices
  // ...
}

// -----------------------------------------------------------------------------
void
CFileSystem::remFileSystemDriver(IFileSystemDriver * driver)
{
}

// -----------------------------------------------------------------------------
void
CFileSystem::mount(IMountPoint * point, const char * location)
{
  // Check if already mounted
  // ...

  CFileSystem::SMountPoint * pMP = new CFileSystem::SMountPoint;
  pMP->location = new char[strlen(location)];
  strcpy(pMP->location, location);
  pMP->point = point;

  // Add mount point to the list
  listMountPoints_.push_back(pMP);
}

// -----------------------------------------------------------------------------
void
CFileSystem::unmount(IMountPoint * point)
{
}

// -----------------------------------------------------------------------------
void
CFileSystem::unmount(const char * location)
{
}
