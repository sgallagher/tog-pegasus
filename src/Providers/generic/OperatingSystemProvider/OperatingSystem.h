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
// Modified By:
//         David Eger (dteger@us.ibm.com)
//         Bapu Patil (bapu_patil@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperatingSystem_h
#define Pegasus_OperatingSystem_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) && !defined(PEGASUS_OS_LSB)
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#include <sys/utsname.h>
#endif

#ifdef PEGASUS_PLATFORM_DARWIN_PPC_GNU
#include <sys/utsname.h>
#endif

PEGASUS_NAMESPACE_BEGIN

//
// OperatingSystem
//
class OperatingSystem
{
public:
   enum OSTYPE {
      Unknown, Other, MACOS, ATTUNIX, DGUX, DECNT, Digital_Unix, OpenVMS, HP_UX, AIX, MVS,
      OS400, OS2, JavaVM, MSDOS, WIN3x, WIN95, WIN98, WINNT, WINCE, NCR3000, NetWare, OSF,
      DCOS, Reliant_UNIX, SCO_UnixWare, SCO_OpenServer, Sequent, IRIX, Solaris, SunOS, U6000,
      ASERIES, TandemNSK, TandemNT, BS2000, LINUX, Lynx, XENIX, VM_ESA, Interactive_UNIX,
      BSDUNIX, FreeBSD, NetBSD, GNU_Hurd, OS9, MACH_Kernel, Inferno, QNX, EPOC, IxWorks,
      VxWorks, MiNT, BeOS, HP_MPE, NextStep, PalmPilot, Rhapsody, Windows_2000, Dedicated,
      OS390, VSE, TPF, Windows_Me
   };

public:
   OperatingSystem(void);
   virtual ~OperatingSystem(void);

   String GetCSCreationClassName(void) const;
   String GetCSName(void) const;
   String GetCreationClassName(void) const;
   String GetName(void) const;
   Uint16 GetOSType(void) const;
   String GetOtherTypeDescription(void) const;
   String GetVersion(void) const;
   CIMDateTime GetLastBootUpTime(void) const;
   CIMDateTime GetLocalDateTime(void) const;
   CIMDateTime GetInstallDate(void) const;
   Sint16 GetCurrentTimeZone(void) const;
   Uint32 GetNumberOfLicensedUsers(void) const;
   Uint32 GetNumberOfUsers(void) const;
   Uint32 GetNumberOfProcesses(void) const;
   Uint32 GetMaxNumberOfProcesses(void) const;
   Uint64 GetTotalSwapSpaceSize(void) const;
   Uint64 GetTotalVirtualMemorySize(void) const;
   Uint64 GetFreeVirtualMemory(void) const;
   Uint64 GetFreePhysicalMemory(void) const;
   Uint64 GetTotalVisibleMemorySize(void) const;
   Uint64 GetSizeStoredInPagingFiles(void) const;
   Uint64 GetFreeSpaceInPagingFiles(void) const;
   Uint64 GetMaxProcessMemorySize(void) const;
   Boolean GetDistributed(void) const;

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) && !defined(PEGASUS_OS_LSB)
private:
    struct utsname m_uts;
    struct sysinfo m_si;
#endif

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
private:
    struct utsname m_uts;
#endif

#ifdef PEGASUS_PLATFORM_DARWIN_PPC_GNU
private:
    struct utsname m_uts;
#endif


};

PEGASUS_NAMESPACE_END

#endif
