//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE

#include "OperatingSystem.h"

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
#include <sstream>
#include <iomanip>
#include <windows.h>
#endif

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

   switch(GetOSType()) {
   case OSTYPE::Unknown:
      name = "Unknown";

      break;
   case OSTYPE::Other:
      name = "Other";

      break;
   case OSTYPE::WIN95:
      name = "Microsoft Windows 95";

      break;
   case OSTYPE::WIN98:
      name = "Microsoft Windows 98";

      break;
   case OSTYPE::WINNT:
      name = "Microsoft Windows NT";

      break;
   case OSTYPE::Windows_2000:
      name = "Microsoft Windows 2000";

      break;
   case OSTYPE::Windows_Me:
      name = "Microsoft Windows Me";

      break;
   default:
      break;
   }

   return(name);
}

Uint16 OperatingSystem::GetOSType(void) const
{
   Uint16 type = OSTYPE::Unknown;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   OSVERSIONINFO ver;

   ::memset(&ver, 0, sizeof(ver));

   ver.dwOSVersionInfoSize = sizeof(ver);

   ::GetVersionEx(&ver);

   // values defined under the topic "Getting the System Version" in the Win32 platform SDK
   if((ver.dwPlatformId == VER_PLATFORM_WIN32_NT) && (ver.dwMajorVersion <= 4)) {
      type = OSTYPE::WINNT;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_NT) && (ver.dwMajorVersion == 5) && (ver.dwMinorVersion == 0)) {
      type = OSTYPE::Windows_2000;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_NT) && (ver.dwMajorVersion == 5) && (ver.dwMinorVersion == 1)) {
      type = OSTYPE::Windows_2000;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (ver.dwMajorVersion == 4) && (ver.dwMinorVersion == 0)) {
      type = OSTYPE::WIN95;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (ver.dwMajorVersion == 4) && (ver.dwMinorVersion == 10)) {
      type = OSTYPE::WIN98;
   }
   else if((ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (ver.dwMajorVersion == 4) && (ver.dwMinorVersion == 90)) {
      type = OSTYPE::Windows_Me;
   }
   #else
      type = OSTYPE::Other;
   #endif

   return(type);
}

String OperatingSystem::GetOtherTypeDescription(void) const
{
   return(String());
}

String OperatingSystem::GetVersion(void) const
{
   String Version;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   OSVERSIONINFO ver;

   ::memset(&ver, 0, sizeof(ver));

   ver.dwOSVersionInfoSize = sizeof(ver);

   ::GetVersionEx(&ver);

   std::stringstream ss;

   ss << ver.dwMajorVersion << '.' << ver.dwMinorVersion << '.' << ver.dwBuildNumber;

   Version = ss.str().c_str();
   #else
   #endif

   return(Version);
}

CIMDateTime OperatingSystem::GetLastBootUpTime(void) const
{
   CIMDateTime LastBootUpTime;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   DWORD dw = ::GetTickCount();
   #else
   #endif

   return(LastBootUpTime);
}

CIMDateTime OperatingSystem::GetLocalDateTime(void) const
{
   CIMDateTime LocalDateTime;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
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

   LocalDateTime = ss.str().c_str();
   #else
   #endif

   return(LocalDateTime);
}

CIMDateTime OperatingSystem::GetInstallDate(void) const
{
   CIMDateTime InstallDate;
   
   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   #else
   #endif
   
   return(InstallDate);
}
Sint16 OperatingSystem::GetCurrentTimeZone(void) const
{
   Sint16 CurrentTimeZone = 0;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   TIME_ZONE_INFORMATION timezone;

   ::memset(&timezone, 0, sizeof(timezone));

   switch(::GetTimeZoneInformation(&timezone)) {
   case TIME_ZONE_ID_UNKNOWN:
      CurrentTimeZone = timezone.Bias;
   case TIME_ZONE_ID_STANDARD:
      CurrentTimeZone = timezone.Bias + timezone.StandardBias;
   case TIME_ZONE_ID_DAYLIGHT:
      CurrentTimeZone = timezone.Bias + timezone.DaylightBias;
   default:
      break;
   }

   // the bias used to calculate the time zone is a factor that is used to determine the
   // UTC time from the local time. to get the UTC offset from the local time, use the inverse.
   if(CurrentTimeZone != 0) {
      CurrentTimeZone *= -1;
   }
   #else
   #endif

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

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   #else
   #endif

   return(NumberOfProcesses);
}

Uint32 OperatingSystem::GetMaxNumberOfProcesses(void) const
{
   Uint32 MaxNumberOfProcesses = 0;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   MaxNumberOfProcesses = 0xffffffff;
   #else
   #endif

   return(MaxNumberOfProcesses);
}

Uint64 OperatingSystem::GetTotalSwapSpaceSize(void) const
{
   Uint64 TotalSwapSpaceSize = 0;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   #else
   #endif

   return(TotalSwapSpaceSize);
}

Uint64 OperatingSystem::GetTotalVirtualMemorySize(void) const
{
   Uint64 TotalVirtualMemorySize = 0;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   TotalVirtualMemorySize = mem.dwTotalVirtual / 1024;
   #else
   #endif

   return(TotalVirtualMemorySize);
}

Uint64 OperatingSystem::GetFreeVirtualMemory(void) const
{
   Uint64 FreeVirtualMemory = 0;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   FreeVirtualMemory = mem.dwAvailVirtual / 1024;
   #else
   #endif

   return(FreeVirtualMemory);
}

Uint64 OperatingSystem::GetFreePhysicalMemory(void) const
{
   Uint64 FreePhysicalMemory = 0;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   FreePhysicalMemory = mem.dwAvailPhys / 1024;
   #else
   #endif

   return(FreePhysicalMemory);
}

Uint64 OperatingSystem::GetTotalVisibleMemorySize(void) const
{
   Uint64 TotalVisibleMemorySize = 0;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   TotalVisibleMemorySize = mem.dwAvailVirtual / 1024;
   #else
   #endif

   return(TotalVisibleMemorySize);
}

Uint64 OperatingSystem::GetSizeStoredInPagingFiles(void) const
{
   Uint64 SizeStoredInPagingFiles = 0;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   SizeStoredInPagingFiles = mem.dwTotalPageFile / 1024;
   #else
   #endif

   return(SizeStoredInPagingFiles);
}

Uint64 OperatingSystem::GetFreeSpaceInPagingFiles(void) const
{
   Uint64 FreeSpaceInPagingFiles = 0;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   MEMORYSTATUS mem;

   ::memset(&mem, 0, sizeof(mem));

   mem.dwLength = sizeof(mem);

   ::GlobalMemoryStatus(&mem);

   FreeSpaceInPagingFiles = mem.dwAvailPageFile / 1024;
   #else
   #endif

   return(FreeSpaceInPagingFiles);
}

Uint64 OperatingSystem::GetMaxProcessMemorySize(void) const
{
   Uint64 MaxProcessMemorySize = 0;

   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   SYSTEM_INFO sys;

   ::memset(&sys, 0, sizeof(sys));

   ::GetSystemInfo(&sys);

   MaxProcessMemorySize = (DWORD(sys.lpMaximumApplicationAddress) - DWORD(sys.lpMinimumApplicationAddress)) / 1024;
   #else
   #endif

   return(MaxProcessMemorySize);
}

Boolean OperatingSystem::GetDistributed(void) const
{
   #ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   return(false);
   #else
   #endif
}

PEGASUS_NAMESPACE_END
