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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
// 	   Al Stone <ahs3@fc.hp.com>
//
// Modified By: Al Stone <ahs3@fc.hp.com>
//              Bapu Patil (bapu_patil@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////
#ifndef _OPERATINGSYSTEM_H
#define _OPERATINGSYSTEM_H

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

class OperatingSystem
{
   public:

      enum OSTYPE {
           Unknown, Other, MACOS, ATTUNIX, DGUX, DECNT, Digital_Unix, 
           OpenVMS, HP_UX, AIX, MVS, OS400, OS2, JavaVM, MSDOS, WIN3x, 
           WIN95, WIN98, WINNT, WINCE, NCR3000, NetWare, OSF,
           DCOS, Reliant_UNIX, SCO_UnixWare, SCO_OpenServer, Sequent, 
           IRIX, Solaris, SunOS, U6000, ASERIES, TandemNSK, TandemNT, 
           BS2000, LINUX, Lynx, XENIX, VM_ESA, Interactive_UNIX,
           BSDUNIX, FreeBSD, NetBSD, GNU_Hurd, OS9, MACH_Kernel, 
           Inferno, QNX, EPOC, IxWorks, VxWorks, MiNT, BeOS, HP_MPE, 
           NextStep, PalmPilot, Rhapsody, Windows_2000, Dedicated,
           OS390, VSE, TPF, Windows_Me
           };

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
      Boolean getMaxProcsPerUser(Uint32& maxProcsPerUser);
      Boolean getSystemUpTime(Uint64& systemUpTime);
      Boolean getOperatingSystemCapability(String& operatingSystemCapability);
      
   protected:
      Uint64 _totalVM();
      Uint32 _reboot();
      Uint32 _shutdown();
};

#endif
