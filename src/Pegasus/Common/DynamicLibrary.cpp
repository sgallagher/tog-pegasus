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

#include <Pegasus/Common/PegasusVersion.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "DynamicLibraryWindows.cpp"
#elif defined(PEGASUS_OS_HPUX)
# include "DynamicLibraryHPUX.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "DynamicLibraryUnix.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_NAMESPACE_BEGIN

DynamicLibrary::DynamicLibrary(void) : _handle(0)
{
}

DynamicLibrary::DynamicLibrary(const DynamicLibrary & library) : _handle(0)
{
    _fileName = library._fileName;

    // load the module again, if necessary. this effectively increments the
    // operating system's reference count for the module.
    if(library._handle != 0)
    {
        load();
    }
}

DynamicLibrary::DynamicLibrary(const String & fileName) : _fileName(fileName), _handle(0)
{
}

DynamicLibrary::~DynamicLibrary(void)
{
    // unload the module, if necessary. this ensures 1) the module is released in the
    // event the caller did not explicity unload it, and 2) the operating system's
    // reference count is accurate.
    if(_handle != 0)
    {
        unload();
    }
}

DynamicLibrary & DynamicLibrary::operator=(const DynamicLibrary & library)
{
    if(this == &library)
    {
        return(*this);
    }

    _fileName = library._fileName;

    // load the module again, if necessary. this effectively increments the
    // operating system's reference count for the module.
    if(library._handle != 0)
    {
        load();
    }

    return(*this);
}

String DynamicLibrary::getFileName(void) const
{
    return(_fileName);
}

DynamicLibrary::LIBRARY_HANDLE DynamicLibrary::getHandle(void) const
{
    return(_handle);
}

PEGASUS_NAMESPACE_END
