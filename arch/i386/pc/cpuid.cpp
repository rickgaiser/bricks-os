#include "cpuid.h"
#include "hal.h"
#include "string.h"
#include "iostream"


namespace CPU
{


enum EVendorID
{
    VID_UNKNOWN
  , VID_INTEL
  , VID_UMC
  , VID_AMD
  , VID_CYRIX
  , VID_NEXTGEN
  , VID_CENTAUR
  , VID_RISE
  , VID_TRANSMETA
};

// CPUID, eax=0
uint32_t iCPUIDMax = 0;
char sVendorID[13] = {0};
EVendorID eVendorID;
// CPUID, eax=1
uint32_t iCPUSignature = 0;
uint32_t iCPUType = 0;
uint32_t iCPUFamily = 0;
uint32_t iCPUModel = 0;
uint32_t iCPUSteppingID = 0;
uint32_t iCPUInfo = 0;
uint32_t iBrandID = 0;
uint32_t iCPUFeatures2 = 0;
uint32_t iCPUFeatures1 = 0;
// CPUID, eax=0x80000001, 0x80000002, 0x80000003
uint32_t iCPUIDExMax = 0;
char brandString[48] = {0};
char * pBrandString = brandString;  // Points to first USED (non ' ') character in brandString
static const char * type[] =
{
    "Original OEM Processor"
  , "Intel OverDrive Processor"
  , "Dual Processor"
  , "Intel Reserved"
};


// -----------------------------------------------------------------------------
bool
hasCPUID()
{
  unsigned int f1, f2;

  __asm__("pushfl\n\t"
          "pushfl\n\t"
          "popl %0\n\t"
          "movl %0,%1\n\t"
          "xorl %2,%0\n\t"
          "pushl %0\n\t"
          "popfl\n\t"
          "pushfl\n\t"
          "popl %0\n\t"
          "popfl"
          : "=&r" (f1), "=&r" (f2)
          : "ir" (0x00200000));

  return ((f1^f2) & 0x00200000) != 0;
}

// -----------------------------------------------------------------------------
void
init()
{
  uint32_t iDummy;
  
  std::cout<<"Processor:"<<std::endl;

  // Check if CPUID is supported
  if(hasCPUID() == true)
  {
    // CPUID is supported, must be a late 486 or newer
    // Get vendorID and max supported CPUID functions
    sVendorID[12] = 0;
    cpuid(0, &iCPUIDMax, (uint32_t *)&sVendorID[0], (uint32_t *)&sVendorID[8], (uint32_t *)&sVendorID[4]);
    std::cout<<" - Vendor ID: "<<sVendorID<<std::endl;
    if(strcmp(sVendorID, "GenuineIntel") == 0)
      eVendorID = VID_INTEL;
    else if((strcmp(sVendorID, "AuthenticAMD") == 0) || (strcmp(sVendorID, "AMD ISBETTER" == 0)))
      eVendorID = VID_AMD;
    //else if(strcmp(sVendorID, "CyrixInstead") == 0)
    //  eVendorID = VID_CYRIX;
    //else if(strcmp(sVendorID, "CentaurHauls") == 0)
    //  eVendorID = VID_CENTAUR;
    //else if(strcmp(sVendorID, "GenuineTMx86") == 0)
    //  eVendorID = VID_TRANSMETA;
    //else if(strcmp(sVendorID, "NexGenDriven") == 0)
    //  eVendorID = VID_NEXTGEN;
    //else if(strcmp(sVendorID, "UMC UMC UMC ") == 0)
    //  eVendorID = VID_UMC;
    //else if(strcmp(sVendorID, "RiseRiseRise") == 0)
    //  eVendorID = VID_RISE;
    else
      eVendorID = VID_UNKNOWN;

    if(iCPUIDMax >= 1)
    {
      uint32_t iCPUExFamily;
      uint32_t iCPUExModel;
      
      // Get cpu info and features
      cpuid(1, &iCPUSignature, &iCPUInfo, &iCPUFeatures2, &iCPUFeatures1);
      // Parse cpu info
      iCPUExFamily   = (iCPUSignature & 0x0ff00000) >> 20;
      iCPUExModel    = (iCPUSignature & 0x000f0000) >> 16;
      iCPUType       = (iCPUSignature & 0x00003000) >> 12;
      iCPUFamily     = (iCPUSignature & 0x00000f00) >>  8;
      iCPUModel      = (iCPUSignature & 0x000000f0) >>  4;
      iCPUSteppingID = (iCPUSignature & 0x0000000f);
      // 8bit brand ID: NOTE: AMD Also has a 12bit brand ID
      iBrandID       = (iCPUInfo & 0x000000ff);
      
      // Only Intel defines extended model info for family 6
      if((eVendorID == VID_INTEL) || (iCPUFamily == 0x06))
      {
        iCPUModel += (iCPUExModel << 4);
      }
      else if(iCPUFamily == 0x0f)
      {
        iCPUModel += (iCPUExModel << 4);
        iCPUFamily += iCPUExFamily;
      }
      
      // Only Intel defines a cpu type
      if(eVendorID == VID_INTEL)
        std::cout<<" - Type:      "<<type[iCPUType]<<std::endl;
      std::cout<<" - Family:    "<<iCPUFamily<<std::endl;
      std::cout<<" - Model:     "<<iCPUModel<<std::endl;
      std::cout<<" - Stepping:  "<<iCPUSteppingID<<std::endl;
    }
    
    // Extended CPUID
    cpuid(0x80000000, &iCPUIDExMax, &iDummy, &iDummy, &iDummy);

    // If 8bit brand ID == 0:
    //  - Intel: Legacy processor (no support for brand ID)
    //  - AMD: Use 12bit brand ID (if both 0: engineering sample)
    if((iBrandID == 0) && (eVendorID == VID_AMD) && (iCPUIDExMax >= 0x80000001))
      cpuid(0x80000001, &iDummy, &iBrandID, &iDummy, &iDummy);

    // Check if Brand String is supported
    if(iCPUIDExMax >= 0x80000004)
    {
      // Display Brand string
      cpuid(0x80000002, (uint32_t *)&brandString[ 0], (uint32_t *)&brandString[ 4], (uint32_t *)&brandString[ 8], (uint32_t *)&brandString[12]);
      cpuid(0x80000003, (uint32_t *)&brandString[16], (uint32_t *)&brandString[20], (uint32_t *)&brandString[24], (uint32_t *)&brandString[28]);
      cpuid(0x80000004, (uint32_t *)&brandString[32], (uint32_t *)&brandString[36], (uint32_t *)&brandString[40], (uint32_t *)&brandString[44]);
      
      pBrandString = brandString;
      while(*pBrandString == ' ')
        pBrandString++;
      
      if(pBrandString[0] != 0)
        std::cout<<" - Brand:     "<<pBrandString<<std::endl;
    }
    else if(iBrandID != 0)
    {
      // Display Brand ID
      std::cout<<" - Brand:     "<<iBrandID<<std::endl;
    }
  }
  else
  {
    // CPUID not supported, must be a 386 or an early 486
    std::cout<<" - 386/486"<<std::endl;
  }
}


};

