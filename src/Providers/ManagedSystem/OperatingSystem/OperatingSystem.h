#ifndef _OPERATINGSYSTEM_H
#define _OPERATINGSYSTEM_H
//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//==========================================================================
//
// Author: Christopher Neufeld <neufeld@linuxcare.com>
// 	   Al Stone <ahs3@fc.hp.com>
//
// Modified By: Al Stone <ahs3@fc.hp.com>
//
//%/////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>

PEGASUS_NAMESPACE_BEGIN

class OperatingSystem
{
   public:
      OperatingSystem(void);
      ~OperatingSystem(void);
      Boolean getCSName(String& csName);
      Boolean getName(String& name);
      Boolean getCaption(String& caption);
      Boolean getDescription(String& description);
      Boolean getInstallDate(CIMDateTime& installDate);
      Boolean getStatus(String& status);
      Boolean getOSType(Uint16& osType);
      Boolean getOtherTypeDescription(String& otherTypeDescription);
      Boolean getVersion(String& osVersion);
      Boolean getLastBootUpTime(CIMDateTime& lastBootUpTime);
      Boolean getLocalDateTime(CIMDateTime& localDateTime);
      Boolean getCurrentTimeZone(Sint16& currentTimeZone);
      Boolean getNumberOfLicensedUsers(Uint32& numberOfLicensedUsers);
      Boolean getNumberOfUsers(Uint32& numberOfUsers);
      Boolean getNumberOfProcesses(Uint32& numberOfProcesses);
      Boolean getMaxNumberOfProcesses(Uint32& maxNumberOfProcesses);
      Boolean getTotalSwapSpaceSize(Uint64& totalSwapSpaceSize);
      Boolean getTotalVirtualMemorySize(Uint64& totalVirtualMemorySize);
      Boolean getFreeVirtualMemory(Uint64& freeVirtualMemory);
      Boolean getFreePhysicalMemory(Uint64& freePhysicalMemory);
      Boolean getTotalVisibleMemorySize(Uint64& totalVisibleMemorySize);
      Boolean getSizeStoredInPagingFiles(Uint64& sizeStoredInPagingFiles);
      Boolean getFreeSpaceInPagingFiles(Uint64& freeSpaceInPagingFiles);
      Boolean getMaxProcessMemorySize(Uint64& maxProcessMemorySize);
      Boolean getDistributed(Boolean& distributed);
      Boolean getSystemUpTime(Uint64& systemUpTime);
      Boolean getOperatingSystemCapability(String& operatingSystemCapability);
      Uint32 Reboot();
      Uint32 Shutdown();
      
   protected:
};

PEGASUS_NAMESPACE_END

#endif
