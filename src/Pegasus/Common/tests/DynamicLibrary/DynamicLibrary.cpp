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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/DynamicLibrary.h>

PEGASUS_USING_PEGASUS;

#include <iostream>

PEGASUS_USING_STD;

int main(int argc, char** argv)
{
    const char * verbose = getenv("PEGASUS_TEST_VERBOSE");

    #if defined(PEGASUS_OS_WINDOWS)
    String fileName = "DynLib.dll";
    #else
    String fileName = "libDynLib.so";
    #endif

    DynamicLibrary library(fileName);

    if(library.load() == false)
    {
        cout << "failed to load " << library.getFileName() << endl;

        return(-1);
    }

    Uint32 (* callme)(void) = (Uint32 (*)(void))library.getSymbol("callme");

    if(callme == 0)
    {
        library.unload();

        cout << "failed to export callme() from " << library.getFileName() << endl;

        return(-1);
    }

    cout << "callme() returned << " << hex << callme() << endl;

    if(library.unload() == false)
    {
        cout << "failed to unload " << library.getFileName() << endl;

        return(-1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return(0);
}
