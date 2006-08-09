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

#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>

#include <dl.h>

PEGASUS_NAMESPACE_BEGIN

// the HPUX dynamic library load and unload routines do not keep a reference 
// count. this implies that if a library were loaded multiple times, a single 
// unload will release the library from the process thereby potentially leaving
// dangling references behind. this is a tenative implementation that 
// encapsulates this behavior from users of the DynamicLibrary object. the goal
// release the library only after an equal number of loads and unloads have 
// occured.

static Array<Pair<DynamicLibrary::LIBRARY_HANDLE, int> > _references;
static Mutex _mutex;

static Uint32 _increment_handle(DynamicLibrary::LIBRARY_HANDLE handle)
{
    AutoMutex autoMutex(_mutex);

    // seek and increment
    for (Uint32 i = 0, n = _references.size(); i < n; i++)
    {
        if (handle == _references[i].first)
        {
            Uint32 n = ++_references[i].second;
            return n;
        }
    }

    // not found, append and set at 1
    _references.append(Pair<DynamicLibrary::LIBRARY_HANDLE, int>(handle, 1));

    return 1;
}

static Uint32 _decrement_handle(DynamicLibrary::LIBRARY_HANDLE handle)
{
    AutoMutex autoMutex(_mutex);

    // seek and decrement
    for (Uint32 i = 0, n = _references.size(); i < n; i++)
    {
        if (handle == _references[i].first)
        {
            Uint32 n = --_references[i].second;

            if(n == 0)
            {
                _references.remove(i);
            }

            return n;
        }
    }

    // not found, must be 0
    return 0;
}

Boolean DynamicLibrary::_load()
{
    PEG_METHOD_ENTER(TRC_OS_ABSTRACTION, "DynamicLibrary::_load()");

    CString cstr = _fileName.getCString();

    Tracer::trace(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
        "Attempting to load library %s", (const char*)cstr);

    if (System::bindVerbose)
    {
        _handle =
            shl_load(cstr, BIND_IMMEDIATE | DYNAMIC_PATH | BIND_VERBOSE, 0L);
    }
    else
    {
        _handle = shl_load(cstr, BIND_IMMEDIATE | DYNAMIC_PATH, 0L);
    }

    Tracer::trace(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
        "After loading lib %s, error code is %d",
        (const char*)cstr, (_handle == (void *)0)?errno:0);

    if (_handle != 0)
    {
        // increment handle if valid
        _increment_handle(_handle);
    }
    else
    {
        // If shl_load() returns NULL, errno is set to indicate the error
        _loadErrorMessage = strerror(errno);
    }

    PEG_METHOD_EXIT();
    return(isLoaded());
}

void DynamicLibrary::_unload()
{
    // decrement handle is valid and release the library only if the handle 
    // reference count is 0
    if((_handle != 0) && (_decrement_handle(_handle) == 0))
    {
        shl_unload(reinterpret_cast<shl_t>(_handle));
    }
}

DynamicLibrary::LIBRARY_SYMBOL DynamicLibrary::getSymbol(
    const String & symbolName)
{
    PEGASUS_ASSERT(isLoaded());

    LIBRARY_SYMBOL func = 0;
    CString cstr = symbolName.getCString();

    if (shl_findsym((shl_t *)&_handle, cstr, TYPE_UNDEFINED, &func) == 0)
    {
        return func;
    }

    // NOTE: should the underscore be prepended by the caller or should
    // this be a compile time option?

    cstr = String(String("_") + symbolName).getCString();

    if (shl_findsym((shl_t *)_handle, cstr, TYPE_UNDEFINED, &func) == 0)
    {
        return func;
    }

    return 0;
}

PEGASUS_NAMESPACE_END
