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
// Author: Robert Kieninger (kieningr@de.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include "OperatingSystem.h"

#include <strstream>

#include <iomanip>
#include <time.h>

PEGASUS_NAMESPACE_BEGIN

OperatingSystem::OperatingSystem(void)
{
    uname( &m_uts );    // Get the host computer info
//    sysinfo( &m_si );   // Get the os system info
}

OperatingSystem::~OperatingSystem(void)
{
}

String OperatingSystem::GetCSCreationClassName(void) const
{
   // get from CIM_ComputerSystem.CreationClassName
   return( String("foo") );
}

String OperatingSystem::GetCSName(void) const
{
   // get from CIM_ComputerSystem.CreationClassName
   return( String("") );
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
   case Unknown:
      name = "Unknown";

      break;
   case Other:
      name = "Other";

      break;
   case LINUX:
      name = "Linux";

      break;
   default:
      break;
   }

   return(name);
}

Uint16 OperatingSystem::GetOSType(void) const
{
   Uint16 type = Unknown;

   type = OS390;

   return(type);
}

String OperatingSystem::GetOtherTypeDescription(void) const
{
   return(String("z/OS"));
}

String OperatingSystem::GetVersion(void) const
{
   std::strstream ss;

   ss << m_uts.version;

   char* tmp = ss.str();
   String result(tmp);
   delete [] tmp;

   return result;
}

// REVIEW: these method names should beging in lower case
// (see coding standards).

CIMDateTime OperatingSystem::GetLastBootUpTime(void) const //TBD
{
/*   time_t boottime = time((time_t)0);// - m_si.uptime;
   struct tm tmBuffer;
   struct tm *lastboottime = localtime_r(&boottime, &tmBuffer);

   std::strstream ss;

   ss << std::setfill('0');
   ss << std::setw(4) << lastboottime->tm_year + 1900;
   ss << std::setw(2) << lastboottime->tm_mon + 1;
   ss << std::setw(2) << lastboottime->tm_mday;
   ss << std::setw(2) << lastboottime->tm_hour;
   ss << std::setw(2) << lastboottime->tm_min;
   ss << std::setw(2) << lastboottime->tm_sec;
   ss << '.';
   ss << std::setw(6) << 0;
   ss << (lastboottime->tm_gmtoff < 0 ? "-" : "+");
   ss << std::setw(3) << ::abs(lastboottime->tm_gmtoff / 60);

//   char* tmp = ss.str();
   CIMDateTime lastBootUpTime(ss.str());*/

   CIMDateTime lastBootUpTime;


//   delete [] tmp;
   return lastBootUpTime;
}

CIMDateTime OperatingSystem::GetLocalDateTime(void) const
{
/*   time_t currtime = time((time_t)0);
   struct tm tmBuffer;
   struct tm *loctime = localtime_r(&currtime, &tmBuffer);

   std::strstream ss;

   ss << std::setfill('0');
   ss << std::setw(4) << loctime->tm_year + 1900;
   ss << std::setw(2) << loctime->tm_mon + 1;
   ss << std::setw(2) << loctime->tm_mday;
   ss << std::setw(2) << loctime->tm_hour;
   ss << std::setw(2) << loctime->tm_min;
   ss << std::setw(2) << loctime->tm_sec;
   ss << '.';
   ss << std::setw(6) << 0;
   ss << (loctime->tm_gmtoff < 0 ? "-" : "+");
   ss << std::setw(3) << ::abs(loctime->tm_gmtoff / 60);

//   char* tmp = ss.str();
   CIMDateTime localDateTime(ss.str());*/
   CIMDateTime localDateTime;
//   delete [] tmp;
   return localDateTime;
}

CIMDateTime OperatingSystem::GetInstallDate(void) const
{
   CIMDateTime InstallDate;

   // REVIEW: all variable names must begin with lower case (see coding
   // standards).
   return(InstallDate);
}

Sint16 OperatingSystem::GetCurrentTimeZone(void) const
{
   Sint16 CurrentTimeZone = 0;

   //time_t currtime = time((time_t)0);
   //struct tm tmBuffer;
   //struct tm *loctime = localtime_r(&currtime, &tmBuffer);

   //CurrentTimeZone = loctime->tm_gmtoff / 60;   // gmtoff give offset from gmt

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

Uint32 OperatingSystem::GetNumberOfProcesses(void) const //TBD
{
   Uint32 NumberOfProcesses = 20;

   //NumberOfProcesses = m_si.procs;

   return(NumberOfProcesses);
}

Uint32 OperatingSystem::GetMaxNumberOfProcesses(void) const
{
   Uint32 MaxNumberOfProcesses = 0;

   MaxNumberOfProcesses = 0x7fffffff;

   return(MaxNumberOfProcesses);
}

Uint64 OperatingSystem::GetTotalSwapSpaceSize(void) const //TBD
{
   Uint64 TotalSwapSpaceSize = 0;

   //TotalSwapSpaceSize = m_si.totalswap / 1024;

   return(TotalSwapSpaceSize);
}

Uint64 OperatingSystem::GetTotalVirtualMemorySize(void) const  //TBD
{
   Uint64 TotalVirtualMemorySize = 0;

   return(TotalVirtualMemorySize);
}

Uint64 OperatingSystem::GetFreeVirtualMemory(void) const  //TBD
{
   Uint64 FreeVirtualMemory = 0;

   return(FreeVirtualMemory);
}

Uint64 OperatingSystem::GetFreePhysicalMemory(void) const  //TBD
{
   Uint64 FreePhysicalMemory = 0;

   //FreePhysicalMemory = m_si.totalram / 1024;

   return(FreePhysicalMemory);
}

Uint64 OperatingSystem::GetTotalVisibleMemorySize(void) const  //TBD
{
   Uint64 TotalVisibleMemorySize = 0;

   return(TotalVisibleMemorySize);
}

Uint64 OperatingSystem::GetSizeStoredInPagingFiles(void) const
{
   Uint64 SizeStoredInPagingFiles = 0;

   return(SizeStoredInPagingFiles);
}

Uint64 OperatingSystem::GetFreeSpaceInPagingFiles(void) const
{
   Uint64 FreeSpaceInPagingFiles = 0;

   return(FreeSpaceInPagingFiles);
}

Uint64 OperatingSystem::GetMaxProcessMemorySize(void) const
{
   Uint64 MaxProcessMemorySize = 0;

   return(MaxProcessMemorySize);
}

Boolean OperatingSystem::GetDistributed(void) const
{
   return(false);
}

PEGASUS_NAMESPACE_END

