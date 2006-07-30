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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Sean Keenan, Hewlett-Packard Company <sean.keenan@hp.com>
//
//%/////////////////////////////////////////////////////////////////////////////

#include "DynamicLibrary.h"

#if defined(PEGASUS_OS_HPUX) || \
    defined(PEGASUS_OS_LINUX) || \
    defined(PEGASUS_OS_SOLARIS) || \
    defined(PEGASUS_OS_AIX) || \
    defined(PEGASUS_OS_TRU64) || \
    defined(PEGASUS_OS_DARWIN)
# include <dlfcn.h>
#elif defined(PEGASUS_OS_OS400)
# include <fcntl.h>
# include <unistd.cleinc>
# include "OS400SystemState.h"  // OS400LoadDynamicLibrary, etc
#elif defined(PEGASUS_ZOS_SECURITY)
# include <sys/stat.h>
# include "DynamicLibraryzOS_inline.h"
#elif defined(PEGASUS_OS_VMS)
// ATTN-VMS: Are all of these headers really needed?
# include <chfdef.h>
# include <dlfcn.h>
# include <unistd.h>
# include <errno.h>
# include <sys/types.h>
# include <cstdio>
# include <lib$routines.h>
# include <prvdef.h>
# include <descrip.h>
# include <stsdef.h>
# include <libdef.h>
# include <ssdef.h>
# include <starlet.h>
#endif

PEGASUS_NAMESPACE_BEGIN

Boolean DynamicLibrary::load(void)
{
    // ensure the module is not already loaded
    PEGASUS_ASSERT(isLoaded() == false);

    CString cstr = _fileName.getCString();

#if defined(PEGASUS_OS_HPUX) || \
    defined(PEGASUS_OS_LINUX) || \
    defined(PEGASUS_OS_SOLARIS) || \
    defined(PEGASUS_OS_AIX) || \
    defined(PEGASUS_OS_TRU64) || \
    defined(PEGASUS_OS_DARWIN) || \
    defined(PEGASUS_OS_VMS) 
    _handle = dlopen(cstr, RTLD_NOW);
#elif defined(PEGASUS_OS_ZOS)
# if defined(PEGASUS_ZOS_SECURITY)
    if (hasProgramControl(cstr))
        _handle = dllload(cstr);
    else 
        _handle = 0;
# else
        _handle = dllload(cstr);
# endif

# elif defined(PEGASUS_OS_OS400)
    _handle = OS400_LoadDynamicLibrary((const char *)cstr);
#endif

    return(isLoaded());
}

Boolean DynamicLibrary::unload(void)
{
    // ensure the module is loaded
    PEGASUS_ASSERT(isLoaded() == true);

#if defined(PEGASUS_OS_HPUX) || \
    defined(PEGASUS_OS_LINUX) || \
    defined(PEGASUS_OS_SOLARIS) || \
    defined(PEGASUS_OS_AIX) || \
    defined(PEGASUS_OS_TRU64) || \
    defined(PEGASUS_OS_DARWIN) || \
    defined(PEGASUS_OS_VMS)
    dlclose(_handle);
#elif defined(PEGASUS_OS_ZOS)
    dllfree(_handle);
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

DynamicLibrary::LIBRARY_SYMBOL DynamicLibrary::getSymbol(
    const String & symbolName)
{
    LIBRARY_SYMBOL func = 0;

    if (isLoaded())
    {
        CString cstr = symbolName.getCString();

#if defined(PEGASUS_OS_HPUX) || \
    defined(PEGASUS_OS_LINUX) || \
    defined(PEGASUS_OS_SOLARIS) || \
    defined(PEGASUS_OS_AIX) || \
    defined(PEGASUS_OS_TRU64) || \
    defined(PEGASUS_OS_DARWIN)
        func = (LIBRARY_SYMBOL)::dlsym(_handle, (const char*)cstr);
#elif defined(PEGASUS_OS_VMS)
        char* Errorout;
        // ATTN-VMS: why call dlsym() here? The result is unused.
        if ((func = (LIBRARY_SYMBOL)::dlsym(_handle, (const char*)cstr)) == 0)
            Errorout = dlerror();
#elif defined(PEGASUS_OS_ZOS)
        func = (LIBRARY_SYMBOL) dllqueryfn(_handle, (const char*)cstr);
#elif defined(PEGASUS_OS_OS400)
        func = (LIBRARY_SYMBOL)OS400_LoadDynamicSymbol(
            _handle, (const char*)cstr);
#endif
    }

    return func;
}

PEGASUS_NAMESPACE_END
