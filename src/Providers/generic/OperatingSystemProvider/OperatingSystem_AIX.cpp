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
// Author: Primo Sabatini (primos@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include "OperatingSystem.h"

// mbrasher: changed to use <strstream> since <sstream> does not
// exist on some linux platforms.
#include <strstream>

#include <iomanip>
#include <time.h>

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
   case AIX:
      name = "AIX";

      break;
   default:
      break;
   }

   return(name);
}

Uint16 OperatingSystem::GetOSType(void) const
{
   Uint16 type = Unknown;

   type = AIX;

   return(type);
}

String OperatingSystem::GetOtherTypeDescription(void) const
{
   return(String(""));
}

String OperatingSystem::GetVersion(void) const
{
   return (String(""));
}

// REVIEW: these method names should beging in lower case 
// (see coding standards).

CIMDateTime OperatingSystem::GetLastBootUpTime(void) const
{
   CIMDateTime lastDateTime;
   return lastDateTime;
}

CIMDateTime OperatingSystem::GetLocalDateTime(void) const
{
   CIMDateTime localDateTime;
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

   MaxNumberOfProcesses = 0x7fffffff;
   
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

   return(TotalVirtualMemorySize);
}

Uint64 OperatingSystem::GetFreeVirtualMemory(void) const
{
   Uint64 FreeVirtualMemory = 0;

   return(FreeVirtualMemory);
}

Uint64 OperatingSystem::GetFreePhysicalMemory(void) const
{
   Uint64 FreePhysicalMemory = 0;

   return(FreePhysicalMemory);
}

Uint64 OperatingSystem::GetTotalVisibleMemorySize(void) const
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


