//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to 
// deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN 
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//=============================================================================
//
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%////////////////////////////////////////////////////////////////////////////

#include "OperatingSystem.h"

#include <sstream>
#include <iomanip>
#include <windows.h>


PEGASUS_NAMESPACE_BEGIN

OperatingSystem::OperatingSystem(void)
{
}

OperatingSystem::~OperatingSystem(void)
{
}

Boolean OperatingSystem::getCSName(String& csName)
{
//ATTN-SLC-P2-18-Apr-02: need to implement CSName key for Windows OS BZ#42

    // how get fully qualified machine name in Windows?
    return false;
}

Boolean OperatingSystem::getName(String& osName)
{
//ATTN-SLC-P2-18-Apr-02: need to implement Name key for Windows OS BZ#42

    // could set to some string based on OSType calculated below
    // see OS provider in generic directory.
    // would be nice to only do it once for both properties
    return false;
}

Boolean OperatingSystem::getCaption(String& caption)
{

   caption.assign("The current Operating System");

   return true;
}

Boolean OperatingSystem::getDescription(String& description)
{

    description.assign("This instance reflects the Operating System"
        " on which the CIMOM is executing (as distinguished from instances"
        " of other installed operating systems that could be run).");

   return true;
}

Boolean OperatingSystem::getInstallDate(CIMDateTime& installDate)
{
    return false;
}

/**
   getStatus method for Windows implementation of OS provider

   Would like to be able to return and actual status vs. just
   always Unknown, but didn't know how to differentiate between
   OK and Degraded (assuming they are the only values that make
   sense, since the CIMOM is up and running), but one could see
   an argument for including Stopping if the Shutdown or Reboot
   methods have been invoked. For now, always return "Unknown".
   */
Boolean OperatingSystem::getStatus(String& status)
{

// ATTN-SLC-P3-17-Apr-02: Get true Windows status (vs. Unknown) BZ#44

   status.assign("Unknown");

   return true;
}

Boolean OperatingSystem::getVersion(String& osVersion)
{
   OSVERSIONINFO ver;

   ::memset(&ver, 0, sizeof(ver));

   ver.dwOSVersionInfoSize = sizeof(ver);

   ::GetVersionEx(&ver);

   std::stringstream ss;

   ss << ver.dwMajorVersion << '.' << ver.dwMinorVersion << '.' << ver.dwBuildNumber;

   osVersion = ss.str().c_str();

   return(true);
}

Boolean OperatingSystem::getOSType(Uint16& osType)
{

   osType = OSTYPE::Unknown;

   OSVERSIONINFO ver;

   ::memset(&ver, 0, sizeof(ver));

   ver.dwOSVersionInfoSize = sizeof(ver);

   ::GetVersionEx(&ver);

   // values defined under the topic "Getting the System Version" in the Win32 platform SDK
   if((ver.dwPlatformId == VER_PLATFORM_WIN32_NT) && (ver.dwMajorVersion <= 4)) {
      osType = OSTYPE::WINNT;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_NT) && (ver.dwMajorVersion == 5) && (ver.dwMinorVersion == 0)) {
      osType = OSTYPE::Windows_2000;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_NT) && (ver.dwMajorVersion == 5) && (ver.dwMinorVersion == 1)) {
      osType = OSTYPE::Windows_2000;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (ver.dwMajorVersion == 4) && (ver.dwMinorVersion == 0)) {
      osType = OSTYPE::WIN95;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (ver.dwMajorVersion == 4) && (ver.dwMinorVersion == 10)) {
      osType = OSTYPE::WIN98;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (ver.dwMajorVersion == 4) && (ver.dwMinorVersion == 90)) {
      osType = OSTYPE::Windows_Me;
   }

   return(true);
}

Boolean OperatingSystem::getOtherTypeDescription(String& otherTypeDescription)
{
    return false;
}

Boolean OperatingSystem::getLastBootUpTime(CIMDateTime& lastBootUpTime)
{

   // May involve GetTickCount subtracted from local time and
   // formatted as CIMDateTime
   // DWORD dw = ::GetTickCount();

   return false;
}

Boolean OperatingSystem::getLocalDateTime(CIMDateTime& localDateTime)
{
   SYSTEMTIME time;
   Sint16 currentTimeZone;

   ::memset(&time, 0, sizeof(time));

   ::GetLocalTime(&time);

   std::stringstream ss;

   ss << std::setfill('0');
   ss << std::setw(4) << time.wYear;
   ss << std::setw(2) << time.wMonth;
   ss << std::setw(2) << time.wDay;
   ss << std::setw(2) << time.wHour;
   ss << std::setw(2) << time.wMinute;
   ss << std::setw(2) << time.wSecond;
   ss << ".";
   ss << std::setw(6) << time.wMilliseconds * 1000;
   if (getCurrentTimeZone(currentTimeZone))
   {
      ss << (currentTimeZone < 0 ? "-" : "+");
   }
   ss << std::setw(3) << ::abs(currentTimeZone);

   localDateTime = ss.str().c_str();

   return(true);
}

Boolean OperatingSystem::getCurrentTimeZone(Sint16& currentTimeZone)
{

   currentTimeZone = 0;

   TIME_ZONE_INFORMATION timezone;

   ::memset(&timezone, 0, sizeof(timezone));

   switch(::GetTimeZoneInformation(&timezone)) {
   case TIME_ZONE_ID_UNKNOWN:
      currentTimeZone = timezone.Bias;
   case TIME_ZONE_ID_STANDARD:
      currentTimeZone = timezone.Bias + timezone.StandardBias;
   case TIME_ZONE_ID_DAYLIGHT:
      currentTimeZone = timezone.Bias + timezone.DaylightBias;
   default:
      break;
   }

   // the bias used to calculate the time zone is a factor that is used to determine the
   // UTC time from the local time. to get the UTC offset from the local time, use the inverse.
   if(currentTimeZone != 0) {
      currentTimeZone *= -1;
   }

   return(true);
}

Boolean OperatingSystem::getNumberOfLicensedUsers(Uint32& numberOfLicensedUsers)
{
    return false;
}

Boolean OperatingSystem::getNumberOfUsers(Uint32& numberOfUsers)
{
    return false; 
}

Boolean OperatingSystem::getNumberOfProcesses(Uint32& numberOfProcesses)
{
    return false;
}

Boolean OperatingSystem::getMaxNumberOfProcesses(Uint32& mMaxProcesses)
{
    return false; 
}

Boolean OperatingSystem::getTotalSwapSpaceSize(Uint64& mTotalSwapSpaceSize)
{
    return false;
}

Boolean OperatingSystem::getTotalVirtualMemorySize(Uint64& total)
{
   total = 0;

   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   total = mem.dwTotalVirtual / 1024;

   return(total);
}

Boolean OperatingSystem::getFreeVirtualMemory(Uint64& freeVirtualMemory)
{

   freeVirtualMemory = 0;

   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   freeVirtualMemory = mem.dwAvailVirtual / 1024;

   return(true);

}

Boolean OperatingSystem::getFreePhysicalMemory(Uint64& total)
{

   total = 0;

   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   total = mem.dwAvailPhys / 1024;

   return(true);
}

Boolean OperatingSystem::getTotalVisibleMemorySize(Uint64& memory)
{

   memory = 0;

   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   memory = mem.dwAvailVirtual / 1024;

   return(memory);

}

Boolean OperatingSystem::getSizeStoredInPagingFiles(Uint64& total)
{

   total = 0;

   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   total = mem.dwTotalPageFile / 1024;

   return(true);
}

Boolean OperatingSystem::getFreeSpaceInPagingFiles(
                                              Uint64& freeSpaceInPagingFiles)
{

   freeSpaceInPagingFiles = 0;

   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   freeSpaceInPagingFiles = mem.dwAvailPageFile / 1024;

   return(true);
}

Boolean OperatingSystem::getMaxProcessMemorySize(Uint64& maxProcessMemorySize)
{
   maxProcessMemorySize = 0;

   SYSTEM_INFO sys;

   ::memset(&sys, 0, sizeof(sys));

   ::GetSystemInfo(&sys);

   maxProcessMemorySize = (DWORD(sys.lpMaximumApplicationAddress) - DWORD(sys.lpMinimumApplicationAddress)) / 1024;

   return(true);
}

Boolean OperatingSystem::getDistributed(Boolean& distributed)
{
    return false;
}

Boolean OperatingSystem::getSystemUpTime(Uint64& mUpTime)
{
    return false;
}

Boolean OperatingSystem::getOperatingSystemCapability(String& scapability)
{
    return false;
}

Uint32 OperatingSystem::Reboot()
{
   return false;
}

Uint32 OperatingSystem::Shutdown()
{
   return false;
}

PEGASUS_NAMESPACE_END

