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

#ifndef Pegasus_RPM_h
#define Pegasus_RPM_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>

PEGASUS_NAMESPACE_BEGIN

//
// RPMData
//
class RPMData : ProviderData
{

public:
   virtual RPMData(void);
   virtual ~RPMData(void);

   virtual String GetCSCreationClassName(void) const;
   virtual String GetCSName(void) const;
   virtual String GetCreationClassName(void) const;
   virtual String GetName(void) const;
   virtual Uint16 GetOSType(void) const;
   virtual String GetOtherTypeDescription(void) const;
   virtual String GetVersion(void) const;
   virtual CIMDateTime GetLastBootUpTime(void) const;
   virtual CIMDateTime GetLocalDateTime(void) const;
   virtual CIMDateTime GetInstallDate(void) const;
   virtual Sint16 GetCurrentTimeZone(void) const;
   virtual Uint32 GetNumberOfLicensedUsers(void) const;
   virtual Uint32 GetNumberOfUsers(void) const;
   virtual Uint32 GetNumberOfProcesses(void) const;
   virtual Uint32 GetMaxNumberOfProcesses(void) const;
   virtual Uint64 GetTotalSwapSpaceSize(void) const;
   virtual Uint64 GetTotalVirtualMemorySize(void) const;
   virtual Uint64 GetFreeVirtualMemory(void) const;
   virtual Uint64 GetFreePhysicalMemory(void) const;
   virtual Uint64 GetTotalVisibleMemorySize(void) const;
   virtual Uint64 GetSizeStoredInPagingFiles(void) const;
   virtual Uint64 GetFreeSpaceInPagingFiles(void) const;
   virtual Uint64 GetMaxProcessMemorySize(void) const;
   virtual Boolean GetDistributed(void) const;

};

PEGASUS_NAMESPACE_END

#endif
