//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DynamicLibrary_h
#define Pegasus_DynamicLibrary_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

#include <Pegasus/Common/Linkage.h>

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#include <windows.h>
#endif

PEGASUS_NAMESPACE_BEGIN

/*
// This is an opaque type which is used to represent dynamic library
// handles returned by the System::loadDynamicLibrary() method and
// accepted by the System::loadDynamicProcedure() method.
typedef struct DynamicLibraryHandle_* DynamicLibraryHandle;

// This is an opaque type which is returned by System::loadDynamicSymbol().
// Values of this type may be casted to the appropriate target type.
#if !defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) && !defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
typedef struct DynamicSymbolHandle_* DynamicSymbolHandle;
#else
extern "C" {typedef int (* DynamicSymbolHandle)(void);}
#endif
*/

class PEGASUS_COMMON_LINKAGE DynamicLibrary
{
public:
    #if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
    typedef HMODULE LIBRARY_HANDLE;
    typedef int LIBRARY_SYMBOL;
    #elif defined(PEGASUS_PLATFORM_IX86_LINUX_GNU)
    typedef void * LIBRARY_HANDLE;
    typedef void * LIBRARY_SYMBOL;
    #elif defined(PLATFORM_HPUX_PARISC_ACC) || defined(PLATFORM_HPUX_IA64_ACC)
    typedef void * LIBRARY_HANDLE;
    #endif

public:
    DynamicLibrary(void);
    DynamicLibrary(const DynamicLibrary & library);
    explicit DynamicLibrary(const String & fileName);
    virtual ~DynamicLibrary(void);

    DynamicLibrary & operator=(const DynamicLibrary & library);

    virtual Boolean load(void);
    virtual Boolean unload(void);

    String getFileName(void) const;
    LIBRARY_HANDLE getHandle(void) const;
    LIBRARY_SYMBOL getSymbol(const String & symbolName);

    //static String dynamicLoadError(void);

private:
    String _fileName;
    LIBRARY_HANDLE _handle;

};

PEGASUS_NAMESPACE_END

#endif
