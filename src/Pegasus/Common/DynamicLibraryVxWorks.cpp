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
#include <dlfcn.h>

// The user of Pegasus is repsonsiable for building an array of the following
// structure and setting the pegasusSymbols variable below. Since
// VxWorks is statically linked, this static table is used to locate dynamic
// symbols by default.

struct PegasusSymbol
{
    char* libraryName;
    char* symbolName;
    void* symbolPtr;
};

PegasusSymbol* pegasusSymbolTable = 0;

PEGASUS_NAMESPACE_BEGIN

Boolean DynamicLibrary::_load()
{
    CString cstr(_fileName.getCString());
    const char* libraryName = (const char*)cstr;


    if (!pegasusSymbolTable)
        return false;

    for (PegasusSymbol* p = pegasusSymbolTable; p->libraryName; p++)
    {
        if (strcmp(p->libraryName, libraryName) == 0)
        {
            _handle = strdup(libraryName);
            return true;
        }
    }

    // Not found!
    return false;
}

void DynamicLibrary::_unload()
{
    char* libraryName = (char*)_handle;
    free(libraryName);
}

DynamicLibrary::DynamicSymbolHandle DynamicLibrary::getSymbol(
    const String& symbolName_)
{
    PEGASUS_ASSERT(isLoaded());
    CString cstr(symbolName_.getCString());
    const char* symbolName = (const char*)cstr;
    const char* libraryName = (const char*)_handle;

    if (!pegasusSymbolTable || !libraryName)
        return false;

    for (PegasusSymbol* p = pegasusSymbolTable; p->libraryName; p++)
    {
        if (strcmp(p->libraryName, libraryName) == 0 &&
            strcmp(p->symbolName, symbolName) == 0)
        {
            return p->symbolPtr;
        }
    }

    // Not found!
    return 0;
}

PEGASUS_NAMESPACE_END
