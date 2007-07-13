//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include "DynamicLibrary.h"

#if defined(PEGASUS_OS_OS400)
# include <fcntl.h>
# include <unistd.cleinc>
# include "OS400SystemState.h"  // OS400LoadDynamicLibrary, etc
#else
# include <dlfcn.h>
#endif

#if defined(PEGASUS_ZOS_SECURITY)
# include <sys/stat.h>
# include "DynamicLibraryzOS_inline.h"
#endif

PEGASUS_NAMESPACE_BEGIN

Boolean DynamicLibrary::_load()
{
    CString cstr = _fileName.getCString();

#if defined(PEGASUS_ZOS_SECURITY)
    if (!hasProgramControl(cstr))
    {
        return false;
    }
#endif

#if defined(PEGASUS_OS_ZOS)
    _handle = dlopen(cstr, RTLD_LAZY | RTLD_GLOBAL);
#elif defined(PEGASUS_OS_OS400)
    _handle = OS400_LoadDynamicLibrary((const char *)cstr);
#elif defined(PEGASUS_OS_VMS)
    _handle = dlopen(cstr, RTLD_NOW);
#else
    _handle = dlopen(cstr, RTLD_NOW | RTLD_GLOBAL);
#endif

    if (_handle == 0)
    {
        // Record the load error message
#if defined(PEGASUS_OS_OS400)
        _loadErrorMessage = String(OS400_DynamicLoadError());
#else
        _loadErrorMessage = dlerror();
#endif
    }

    return isLoaded();
}

void DynamicLibrary::_unload()
{
#ifdef PEGASUS_OS_OS400
    OS400_UnloadDynamicLibrary(_handle);
#else
    dlclose(_handle);
#endif
}

DynamicLibrary::DynamicSymbolHandle DynamicLibrary::getSymbol(
    const String& symbolName)
{
    PEGASUS_ASSERT(isLoaded());

    CString cstr = symbolName.getCString();

#ifdef PEGASUS_OS_OS400
    DynamicSymbolHandle func = (DynamicSymbolHandle)
        OS400_LoadDynamicSymbol(_handle, (const char *)cstr);
#else
    DynamicSymbolHandle func =
        (DynamicSymbolHandle) dlsym(_handle, (const char *)cstr);
#endif

    return func;
}

PEGASUS_NAMESPACE_END
