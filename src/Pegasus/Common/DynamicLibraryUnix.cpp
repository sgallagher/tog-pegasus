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

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
#include <dlfcn.h>
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include <dll.h>
#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
#include <fcntl.h>
#include <qycmutiltyUtility.H>
#include <unistd.cleinc>
#include "qycmmsgclsMessage.H" // ycmMessage class
#include "OS400SystemState.h"  // OS400LoadDynamicLibrary, etc
#include "OS400ConvertChar.h"
#endif

#include <iostream>
PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

DynamicLibrary::DynamicLibrary(void) : _handle(0)
{
}

DynamicLibrary::DynamicLibrary(const DynamicLibrary & library) : _handle(0)
{
    if(library._handle != 0)
    {
        load();
    }
}

DynamicLibrary::DynamicLibrary(const String & path) : _fileName(path), _handle(0)
{
}

DynamicLibrary::~DynamicLibrary(void)
{
}

DynamicLibrary & DynamicLibrary::operator=(const DynamicLibrary & library)
{
    if(this == &library)
    {
        return(*this);
    }

    if(library._handle != 0)
    {
        // increment the library handle's reference count by loading the library again
        CString cstr = library._fileName.getCString();

        _handle = ::dlopen(cstr, RTLD_NOW);
    }

    return(*this);
}

bool DynamicLibrary::load(void)
{
    if(_handle == 0)
    {
        CString cstr = _fileName.getCString();

        cout << "trying to load " << cstr << endl;

        #if defined(PEGASUS_OS_LINUX) || defined(PEGASUS_OS_SOLARIS) || defined(PEGASUS_OS_AIX)
        _handle = ::dlopen(cstr, RTLD_NOW);
        #elif defined(PEGASUS_OS_TRU64)
        _handle = dlopen(cstr, RTLD_NOW);
        #elif defined(PEGASUS_OS_ZOS)
        _handle = dllload(cstr);
        #elif defined(PEGASUS_OS_OS400)
        _handle = OS400_LoadDynamicLibrary(cstr);
        #endif
    }

    if(_handle != 0)
    {
        return(true);
    }

    cout << "failure message: " << ::dlerror() << endl;
    return(false);
}

bool DynamicLibrary::unload(void)
{
    if(_handle != 0)
    {
        #if defined(PEGASUS_OS_LINUX) || defined(PEGASUS_OS_SOLARIS) || defined(PEGASUS_OS_AIX)
        dlclose(_handle);
        #elif defined(PEGASUS_OS_OS400)
        OS400_UnloadDynamicLibrary((int)_handle);
        #endif

        _handle = 0;
    }

    return(true);
}

String DynamicLibrary::getFileName(void) const
{
    return(_fileName);
}

DynamicLibrary::LIBRARY_HANDLE DynamicLibrary::getHandle(void) const
{
    return(_handle);
}

DynamicLibrary::LIBRARY_SYMBOL DynamicLibrary::getSymbol(const String & symbolName)
{
    LIBRARY_SYMBOL func = 0;

    if(_handle != 0)
    {
        CString cstr = symbolName.getCString();

        #if defined(PEGASUS_OS_LINUX) || defined(PEGASUS_OS_SOLARIS) || defined(PEGASUS_OS_AIX)
        func = (LIBRARY_SYMBOL)::dlsym(_handle, (const char *)cstr);
        #elif defined(PEGASUS_OS_ZOS)
        dllqueryfn((dllhandle *)libraryHandle, (char*)symbolName));
        #elif defined(PEGASUS_OS_OS400)
        OS400_LoadDynamicSymbol((int)libraryHandle, symbolName));
        #endif
    }

    return(func);
}

/*
String System::dynamicLoadError() {
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
