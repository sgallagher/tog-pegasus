//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

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

String OperatingSystem::GetCSCreationClassName(void) const
{
    // get from CIM_ComputerSystem.CreationClassName
    return(String());
}

String OperatingSystem::GetCSName(void) const
{
    // get from CIM_ComputerSystem.CreationClassName
    return(String());
}

String OperatingSystem::GetCreationClassName(void) const
{
    String CreationClassName("Pegasus_OperatingSystem");

    return(CreationClassName);
}

String OperatingSystem::GetName(void) const
{
    String name;

    switch(GetOSType())
    {
        case Unknown:
            name = "Unknown";
            break;

        case Other:
            name = "Other";
            break;

        case WIN95:
            name = "Microsoft Windows 95";
            break;

        case WIN98:
            name = "Microsoft Windows 98";
            break;

        case WINNT:
            name = "Microsoft Windows NT";
            break;

        case Windows_2000:
            name = "Microsoft Windows 2000";
            break;

        case Windows_Me:
            name = "Microsoft Windows Me";
            break;

        default:
            break;
    }

    return(name);
}

Uint16 OperatingSystem::GetOSType(void) const
{
    Uint16 type = Unknown;

    OSVERSIONINFO ver;

    ::memset(&ver, 0, sizeof(ver));

    ver.dwOSVersionInfoSize = sizeof(ver);

    ::GetVersionEx(&ver);

    // values defined under the topic "Getting the System Version" in the Win32 platform SDK
    if((ver.dwPlatformId == VER_PLATFORM_WIN32_NT) && (ver.dwMajorVersion <= 4)) {
        type = WINNT;
    }
    else if((ver.dwPlatformId == VER_PLATFORM_WIN32_NT) && (ver.dwMajorVersion == 5) && (ver.dwMinorVersion == 0)) {
        type = Windows_2000;
    }
    else if((ver.dwPlatformId == VER_PLATFORM_WIN32_NT) && (ver.dwMajorVersion == 5) && (ver.dwMinorVersion == 1)) {
        type = Windows_2000;
    }
    else if((ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (ver.dwMajorVersion == 4) && (ver.dwMinorVersion == 0)) {
        type = WIN95;
    }
    else if((ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (ver.dwMajorVersion == 4) && (ver.dwMinorVersion == 10)) {
        type = WIN98;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (ver.dwMajorVersion == 4) && (ver.dwMinorVersion == 90)) {
        type = Windows_Me;
    }

    return(type);
}

String OperatingSystem::GetOtherTypeDescription(void) const
{
    return(String());
}

String OperatingSystem::GetVersion(void) const
{
    String Version;

    OSVERSIONINFO ver;

    ::memset(&ver, 0, sizeof(ver));

    ver.dwOSVersionInfoSize = sizeof(ver);

    ::GetVersionEx(&ver);

    std::stringstream ss;

    ss << ver.dwMajorVersion << '.' << ver.dwMinorVersion << '.' << ver.dwBuildNumber;

    Version = ss.str().c_str();

    return(Version);
}

CIMDateTime OperatingSystem::GetLastBootUpTime(void) const
{
    CIMDateTime LastBootUpTime;

    DWORD dw = ::GetTickCount();

    return(LastBootUpTime);
}

CIMDateTime OperatingSystem::GetLocalDateTime(void) const
{
    CIMDateTime LocalDateTime;

    SYSTEMTIME time;

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
    ss << (GetCurrentTimeZone() < 0 ? "-" : "+");
    ss << std::setw(3) << ::abs(GetCurrentTimeZone());

    LocalDateTime = CIMDateTime (String (ss.str().c_str()));

    return(LocalDateTime);
}

CIMDateTime OperatingSystem::GetInstallDate(void) const
{
    CIMDateTime InstallDate;

    return(InstallDate);
}
Sint16 OperatingSystem::GetCurrentTimeZone(void) const
{
    Sint16 CurrentTimeZone = 0;

    TIME_ZONE_INFORMATION timezone;

    ::memset(&timezone, 0, sizeof(timezone));

    switch(::GetTimeZoneInformation(&timezone))
    {
        case TIME_ZONE_ID_UNKNOWN:
            CurrentTimeZone = static_cast<Sint16>(timezone.Bias);
            break;

        case TIME_ZONE_ID_STANDARD:
            CurrentTimeZone = static_cast<Sint16>(timezone.Bias + timezone.StandardBias);
            break;

        case TIME_ZONE_ID_DAYLIGHT:
            CurrentTimeZone = static_cast<Sint16>(timezone.Bias + timezone.DaylightBias);
            break;

        default:
            break;
    }

    // the bias used to calculate the time zone is a factor that is used to determine the
    // UTC time from the local time. to get the UTC offset from the local time, use the inverse.
    if(CurrentTimeZone != 0) {
        CurrentTimeZone *= -1;
    }

    return(CurrentTimeZone);
}

Uint32 OperatingSystem::GetNumberOfLicensedUsers(void) const
{
    return(0);
}

Uint32 OperatingSystem::GetNumberOfUsers(void) const
{
    return(0);
}

Uint32 OperatingSystem::GetNumberOfProcesses(void) const
{
    Uint32 NumberOfProcesses = 0;

    return(NumberOfProcesses);
}

Uint32 OperatingSystem::GetMaxNumberOfProcesses(void) const
{
    Uint32 MaxNumberOfProcesses = 0;

    //MaxNumberOfProcesses = 0xffffffff;

    return(MaxNumberOfProcesses);
}

Uint64 OperatingSystem::GetTotalSwapSpaceSize(void) const
{
    Uint64 TotalSwapSpaceSize = 0;

    return(TotalSwapSpaceSize);
}

Uint64 OperatingSystem::GetTotalVirtualMemorySize(void) const
{
    Uint64 TotalVirtualMemorySize = 0;

    MEMORYSTATUS mem;

    ::memset(&mem, 0, sizeof(mem));

    mem.dwLength = sizeof(mem);

    ::GlobalMemoryStatus(&mem);

    TotalVirtualMemorySize = mem.dwTotalVirtual / 1024;

    return(TotalVirtualMemorySize);
}

Uint64 OperatingSystem::GetFreeVirtualMemory(void) const
{
    Uint64 FreeVirtualMemory = 0;

    MEMORYSTATUS mem;

    ::memset(&mem, 0, sizeof(mem));

    mem.dwLength = sizeof(mem);

    ::GlobalMemoryStatus(&mem);

   FreeVirtualMemory = mem.dwAvailVirtual / 1024;

   return(FreeVirtualMemory);
}

Uint64 OperatingSystem::GetFreePhysicalMemory(void) const
{
    Uint64 FreePhysicalMemory = 0;

    MEMORYSTATUS mem;

    ::memset(&mem, 0, sizeof(mem));

    mem.dwLength = sizeof(mem);

    ::GlobalMemoryStatus(&mem);

    FreePhysicalMemory = mem.dwAvailPhys / 1024;

    return(FreePhysicalMemory);
}

Uint64 OperatingSystem::GetTotalVisibleMemorySize(void) const
{
    Uint64 TotalVisibleMemorySize = 0;

    MEMORYSTATUS mem;

    ::memset(&mem, 0, sizeof(mem));

    mem.dwLength = sizeof(mem);

    ::GlobalMemoryStatus(&mem);

    TotalVisibleMemorySize = mem.dwAvailVirtual / 1024;

    return(TotalVisibleMemorySize);
}

Uint64 OperatingSystem::GetSizeStoredInPagingFiles(void) const
{
    Uint64 SizeStoredInPagingFiles = 0;

    MEMORYSTATUS mem;

    ::memset(&mem, 0, sizeof(mem));

    mem.dwLength = sizeof(mem);

    ::GlobalMemoryStatus(&mem);

    SizeStoredInPagingFiles = mem.dwTotalPageFile / 1024;

    return(SizeStoredInPagingFiles);
}

Uint64 OperatingSystem::GetFreeSpaceInPagingFiles(void) const
{
    Uint64 FreeSpaceInPagingFiles = 0;

    MEMORYSTATUS mem;

    ::memset(&mem, 0, sizeof(mem));

    mem.dwLength = sizeof(mem);

    ::GlobalMemoryStatus(&mem);

    FreeSpaceInPagingFiles = mem.dwAvailPageFile / 1024;

    return(FreeSpaceInPagingFiles);
}

Uint64 OperatingSystem::GetMaxProcessMemorySize(void) const
{
    Uint64 MaxProcessMemorySize = 0;

    SYSTEM_INFO sys;

    ::memset(&sys, 0, sizeof(sys));

    ::GetSystemInfo(&sys);

    MaxProcessMemorySize =
                (reinterpret_cast<char *>(sys.lpMaximumApplicationAddress) -
                 reinterpret_cast<char *>(sys.lpMinimumApplicationAddress))
                                / 1024;

    return(MaxProcessMemorySize);
}

Boolean OperatingSystem::GetDistributed(void) const
{
    return(false);
}

PEGASUS_NAMESPACE_END
