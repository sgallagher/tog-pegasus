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

#include "DynamicLibrary.h"

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
#include <dlfcn.h>
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX)
#include <dlfcn.h>
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include <dll.h>
#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
#include <fcntl.h>
#include <unistd.cleinc>
#include "OS400SystemState.h"  // OS400LoadDynamicLibrary, etc
#endif

PEGASUS_NAMESPACE_BEGIN

Boolean DynamicLibrary::load(void)
{
    // ensure the module is not already loaded
    PEGASUS_ASSERT(isLoaded() == false);

    CString cstr = _fileName.getCString();

    #if defined(PEGASUS_OS_LINUX) || defined(PEGASUS_OS_SOLARIS) || defined(PEGASUS_OS_AIX)
    _handle = dlopen(cstr, RTLD_NOW);
    #elif defined(PEGASUS_OS_TRU64)
    _handle = dlopen(cstr, RTLD_NOW);
    #elif defined(PEGASUS_OS_ZOS)
    _handle = dllload(cstr);
    #elif defined(PEGASUS_OS_OS400)
    _handle = OS400_LoadDynamicLibrary((const char *)cstr);
    #endif

    return(isLoaded());
}

Boolean DynamicLibrary::unload(void)
{
    // ensure the module is loaded
    PEGASUS_ASSERT(isLoaded() == true);

    #if defined(PEGASUS_OS_LINUX) || defined(PEGASUS_OS_SOLARIS) || defined(PEGASUS_OS_AIX)
    dlclose(_handle);
    #elif defined(PEGASUS_OS_OS400)
    OS400_UnloadDynamicLibrary(_handle);
    #endif

    _handle = 0;

    return(isLoaded());
}

Boolean DynamicLibrary::isLoaded(void) const
{
    return(_handle != 0);
}

DynamicLibrary::LIBRARY_SYMBOL DynamicLibrary::getSymbol(const String & symbolName)
{
    LIBRARY_SYMBOL func = 0;

    if(isLoaded())
    {
        CString cstr = symbolName.getCString();

        #if defined(PEGASUS_OS_LINUX) || defined(PEGASUS_OS_SOLARIS) || defined(PEGASUS_OS_AIX)
        func = (LIBRARY_SYMBOL)::dlsym(_handle, (const char *)cstr);
        #elif defined(PEGASUS_OS_ZOS)
        func = dllqueryfn((dllhandle *)_handle, (const char *)cstr));
        #elif defined(PEGASUS_OS_OS400)
        func = (LIBRARY_SYMBOL)OS400_LoadDynamicSymbol(_handle, (const char *)cstr));
        #endif
    }

    return(func);
}

/*
String System::dynamicLoadError(void)
{
    // ATTN: Is this safe in a multi-threaded process?  Should this string
    // be returned from loadDynamicLibrary?
    #ifdef PEGASUS_OS_HPUX
    // ATTN: If shl_load() returns NULL, this value should be strerror(errno)
    return String();
    #elif defined(PEGASUS_OS_ZOS)
    return String();
    #elif defined(PEGASUS_OS_OS400)
    return String(OS400_DynamicLoadError());
    #else
    String dlerr = dlerror();
    return dlerr;
    #endif
}
*/

PEGASUS_NAMESPACE_END
