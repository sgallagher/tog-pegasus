//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//==============================================================================
//
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Alagaraja Ramasubramanian (alags_raj@in.ibm.com)
//
//%////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT  0x0500
#include <windows.h>    

#include "OperatingSystem.h"

#include <sstream>
#include <iomanip>
   


OperatingSystem::OperatingSystem(void)
{
}

OperatingSystem::~OperatingSystem(void)
{
}

Boolean OperatingSystem::getCSName(String& csName)
{
    // ALAGS : Fix for bug #42 : CSName info has been implementated
    char hostName[1024];
    LPTSTR lpBuffer = hostName;
    DWORD nSize = 1024;
    String hostNameStr;
    

    //First try to get FULLY qualified Name.
    if(!GetComputerNameEx(
           ComputerNamePhysicalDnsFullyQualified,
           lpBuffer,
           &nSize))
    {
        //Some problem in getting the FULLY qualified name.
        //So get the Hostname atleast.
        if(!GetComputerName(lpBuffer, &nSize))
        {
            //Even Hostname can not be retrieved
            hostNameStr.assign("Error while getting CSName");
        }
        else
        {
            hostNameStr.assign(hostName);
        }
    }
    else
    {
        hostNameStr.assign(hostName);
    }

    csName.assign(hostNameStr);
    return true;
}

Boolean OperatingSystem::getName(String& osName)
{
    // could set to some string based on OSType calculated below
    // see OS provider in generic directory.
    // would be nice to only do it once for both properties

    // ALAGS : Fix for Bug #42 : Cannot use info inside getOSType() method as it 
    // uses the OSVERSIONINFO structure. We need to use the OSVERSIONINFOEX 
    // structure for more information.

    OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;
    String versionName;

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
    {
        // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
            return false; //Could not get Name Info.
    }

    // Get the Base Windows Platform
    switch (osvi.dwPlatformId)
    {
        case VER_PLATFORM_WIN32_NT:

            if ( osvi.dwMajorVersion <= 4 )
                versionName.assign("Microsoft Windows NT");

            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
                versionName.assign("Microsoft Windows 2000");

            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
                versionName.assign("Microsoft Windows XP");

            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
                versionName.assign("Microsoft Windows Server 2003");

            break;

        case VER_PLATFORM_WIN32_WINDOWS:

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
            {
                versionName.assign("Microsoft Windows 95");
                if ( osvi.szCSDVersion[1] == 'C' || 
                     osvi.szCSDVersion[1] == 'B' )
                     versionName.append("OSR2");
            } 

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
            {
                versionName.assign("Microsoft Windows 98");
                if ( osvi.szCSDVersion[1] == 'A' )
                    versionName.append("SE" );
            } 

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
            {
                versionName.assign("Microsoft Windows Me");
            } 
            break;

        case VER_PLATFORM_WIN32s:

            versionName.assign("Microsoft Win32s");
            break;
    }

        
    // Get more information, if you can !
    if( bOsVersionInfoEx )
    {
        // Test for the workstation type.
        if ( osvi.wProductType == VER_NT_WORKSTATION )
        {
            if( osvi.dwMajorVersion == 4 )
                versionName.append( " Workstation 4.0" );
            else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
                versionName.append( " Home Edition" );
            else
                versionName.append( " Professional" );
        }
        // Test for the server type.
        else if ( osvi.wProductType == VER_NT_SERVER )
        {
            if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
            {
                if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                    versionName.append( " Datacenter Edition" );
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                    versionName.append( " Enterprise Edition" );
                else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
                    versionName.append( " Web Edition" );
                else
                    versionName.append( " Standard Edition" );
             }
             else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
             {
                 if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                     versionName.append( " Datacenter Server" );
                 else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                     versionName.append( " Advanced Server" );
                 else
                     versionName.append( " Server" );
             }

             else  // Windows NT 4.0 
             {
                 if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                     versionName.append ("Server 4.0, Enterprise Edition" );
                 else
                     versionName.append ( "Server 4.0" );
             }   
        } // EndIf for VER_NT_SERVER
    }

    osName.assign(versionName);
    return true;
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

   localDateTime = CIMDateTime (String (ss.str().c_str()));

   return(true);
}

Boolean OperatingSystem::getCurrentTimeZone(Sint16& currentTimeZone)
{

   currentTimeZone = 0;

   TIME_ZONE_INFORMATION timezone;

   ::memset(&timezone, 0, sizeof(timezone));

   switch(::GetTimeZoneInformation(&timezone)) {
   case TIME_ZONE_ID_UNKNOWN:
      currentTimeZone = (Sint16)timezone.Bias;
   case TIME_ZONE_ID_STANDARD:
      currentTimeZone = (Sint16)timezone.Bias + (Sint16)timezone.StandardBias;
   case TIME_ZONE_ID_DAYLIGHT:
      currentTimeZone = (Sint16)timezone.Bias + (Sint16)timezone.DaylightBias;
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

   if (total) 
      return true;
   else
      return false;
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

   if (memory)
      return true;
   else
      return false;
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

Boolean OperatingSystem::getMaxProcsPerUser(Uint32& maxProcsPerUser)
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

Uint32 OperatingSystem::_reboot()
{
   return false;
}

Uint32 OperatingSystem::_shutdown()
{
   return false;
}

