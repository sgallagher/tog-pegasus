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
// Modified By: Sean Keenan (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DynamicLibrary_h
#define Pegasus_DynamicLibrary_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>

#include <Pegasus/Common/Linkage.h>

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#include <windows.h>
#endif

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include <dll.h>
#endif

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE DynamicLibrary
{
public:
    #if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
      typedef HMODULE LIBRARY_HANDLE;
      typedef FARPROC LIBRARY_SYMBOL;
    #elif defined(PEGASUS_OS_LINUX) || defined(PEGASUS_OS_AIX) || defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_SOLARIS) || defined(PEGASUS_OS_DARWIN)
      typedef void * LIBRARY_HANDLE;
      typedef void * LIBRARY_SYMBOL;
    #elif defined(PEGASUS_OS_OS400)
      typedef int LIBRARY_HANDLE;
      typedef void * LIBRARY_SYMBOL;
    #elif defined(PEGASUS_OS_ZOS)
      typedef dllhandle * LIBRARY_HANDLE;
      typedef void * LIBRARY_SYMBOL;
    #elif defined(PEGASUS_OS_VMS)
      typedef void * LIBRARY_HANDLE;
      typedef void * LIBRARY_SYMBOL;
    #endif

public:
    DynamicLibrary(void);
    DynamicLibrary(const DynamicLibrary & library);
    explicit DynamicLibrary(const String & fileName);
    virtual ~DynamicLibrary(void);

    DynamicLibrary & operator=(const DynamicLibrary & library);

    virtual Boolean load(void);
    virtual Boolean unload(void);

    virtual Boolean isLoaded(void) const;

    String getFileName(void) const;
    LIBRARY_HANDLE getHandle(void) const;
    LIBRARY_SYMBOL getSymbol(const String & symbolName);

private:
    String _fileName;
    LIBRARY_HANDLE _handle;

};

PEGASUS_NAMESPACE_END

#endif
