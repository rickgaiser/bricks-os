#include "kernel/debug.h"
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
  //printk("CFileSystem::addBlockDevice\n");

  // Check if device is already in the list
  for(unsigned int i(0); i < listBlockDevices_.size(); i++)
    if(listBlockDevices_[i] == device)
      return;

  // Add device to the list
  listBlockDevices_.push_back(device);

  // Check for (extended) partitions on the block device
  for(unsigned int i(0); i < listPartitionDrivers_.size(); i++)
    if(listPartitionDrivers_[i]->init(device) == true)
      return;

  // Check for file systems on the block device
  for(unsigned int i(0); i < listFileSystemDrivers_.size(); i++)
    if(listFileSystemDrivers_[i]->init(device) == true)
      return;

  printk("CFileSystem::addBlockDevice: No handler for blockdevice\n");
}

// -----------------------------------------------------------------------------
void
CFileSystem::remBlockDevice(IBlockDevice * device)
{
  printk("CFileSystem::remBlockDevice\n");
}

// -----------------------------------------------------------------------------
void
CFileSystem::addPartitionDriver(IPartitionDriver * driver)
{
  //printk("CFileSystem::addPartitionDriver\n");

  // Check if driver is already in the list
  for(unsigned int i(0); i < listPartitionDrivers_.size(); i++)
    if(listPartitionDrivers_[i] == driver)
      return;

  // Add driver to the list
  listPartitionDrivers_.push_back(driver);

  // Check for unused block devices
  // ...
}

// -----------------------------------------------------------------------------
void
CFileSystem::remPartitionDriver(IPartitionDriver * driver)
{
  printk("CFileSystem::remPartitionDriver\n");
}

// -----------------------------------------------------------------------------
void
CFileSystem::addFileSystemDriver(IFileSystemDriver * driver)
{
  //printk("CFileSystem::addFileSystemDriver\n");

  // Check if driver is already in the list
  for(unsigned int i(0); i < listFileSystemDrivers_.size(); i++)
    if(listFileSystemDrivers_[i] == driver)
      return;

  // Add driver to the list
  listFileSystemDrivers_.push_back(driver);

  // Check for unused block devices
  // ...
}

// -----------------------------------------------------------------------------
void
CFileSystem::remFileSystemDriver(IFileSystemDriver * driver)
{
  printk("CFileSystem::remFileSystemDriver\n");
}

// -----------------------------------------------------------------------------
void
CFileSystem::mount(IMountPoint * point, const char * location)
{
  printk("CFileSystem::mount(%s)\n", location);

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
  printk("CFileSystem::unmount\n");
}

// -----------------------------------------------------------------------------
void
CFileSystem::unmount(const char * location)
{
  printk("CFileSystem::unmount\n");
}
