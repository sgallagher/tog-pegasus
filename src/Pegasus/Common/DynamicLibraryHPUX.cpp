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

#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/Pair.h>

#include <dl.h>

PEGASUS_NAMESPACE_BEGIN

// the HPUX dynamic library load and unload routines do not keep a reference count.
// this implies that if a library were loaded multiple times, a single unload will
// release the library from the process thereby potentially leaving dangling
// references behind. this is a tenative implementation that encapsulates this
// behavior from users of the DynamicLibrary object. the goal release the library
// only after an equal number of loads and unloads have occured.

Array<Pair<DynamicLibrary::LIBRARY_HANDLE, AtomicInt> > _references;

Uint32 _increment_handle(DynamicLibrary::LIBRARY_HANDLE handle)
{
    // scoped mutex

    // seek and increment
    for(Uint32 i = 0, n = _references.size(); i < n; i++)
    {
        if(handle == _references[i].first)
        {
            Uint32 n = (_references[i].second+=1).value();

            return(n);
        }
    }

    // not found, append and set at 1
    _references.append(Pair<DynamicLibrary::LIBRARY_HANDLE, AtomicInt>(handle, 1));

    return(1);
}

Uint32 _decrement_handle(DynamicLibrary::LIBRARY_HANDLE handle)
{
    // scoped mutex

    // seek and decrement
    for(Uint32 i = 0, n = _references.size(); i < n; i++)
    {
        if(handle == _references[i].first)
        {
            Uint32 n = (_references[i].second-=1).value();

            if(n == 0)
            {
                _references.remove(i);
            }

            return(n);
        }
    }

    // not found, must be 0
    return(0);
}

DynamicLibrary::DynamicLibrary(void) : _handle(0)
{
}

DynamicLibrary::DynamicLibrary(const DynamicLibrary & library) : _handle(0)
{
    if(library._handle != 0)
    {
        // increment the handle reference count if not 0
        _increment_handle(library._handle);

        _handle = library._handle;
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
        // increment the handle reference count if not 0
        _increment_handle(library._handle);

        _handle = library._handle;
    }

    return(*this);
}

String DynamicLibrary::getFileName(void) const
{
    return(_fileName);
}

bool DynamicLibrary::load(void)
{
    if(_handle == 0)
    {
        CString cstr = _fileName.getCString();

        // NOTE: if the module is already loaded, hopefully this call simply
        // returns a copy of the already loaded module. that way, it can be
        // used to maintain the reference count in this object. if this is
        // not the case, then the reference counting key should be changed
        // from the library handle to the module file (after is has been
        // resolved by the system is some unambiguos way).

        // NOTE: the implementation in System.cpp defines a variable for a compile
        // time option. this should be done with a comment or a compile time flag.
        // currently, a comment is used to determine which flags are passed to the
        // load call.

        //_handle = shl_load(cstr, BIND_IMMEDIATE | DYNAMIC_PATH | BIND_VERBOSE, 0L);
        _handle = shl_load(cstr, BIND_IMMEDIATE | DYNAMIC_PATH, 0L);
    }

    if(_handle != 0)
    {
        // increment the handle reference count if not 0
        _increment_handle(_handle);

        return(true);
    }

    return(false);
}

bool DynamicLibrary::unload(void)
{
    if(_handle != 0)
    {
        // release the library only if the handle reference count is 0
        if(_decrement_handle(_handle) == 0)
        {
            shl_unload(reinterpret_cast<shl_t>(_handle));
        }

        _handle = 0;
    }

    return(true);
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

        if(shl_findsym((shl_t *)&_handle, cstr, TYPE_UNDEFINED, &func) == 0)
        {
            return(func);
        }

        // NOTE: should the underscore be prepended by the caller or should
        // this be a compile time option?

        cstr = String(String("_") + symbolName).getCString();

        if(shl_findsym((shl_t *)_handle, cstr, TYPE_UNDEFINED, &func) == 0)
        {
            return(func);
        }
    }

    return(0);
}

PEGASUS_NAMESPACE_END
