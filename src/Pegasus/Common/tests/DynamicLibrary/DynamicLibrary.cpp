//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By:	Sean Keenan (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/DynamicLibrary.h>

PEGASUS_USING_PEGASUS;

#include <iostream>

PEGASUS_USING_STD;

#if defined(PEGASUS_OS_TYPE_WINDOWS)
static const String VALID_FILE_NAME = "DynLib.dll";
static const String INVALID_FILE_NAME = "BADDynLib.dll";
#elif defined(PEGASUS_OS_DARWIN)
static const String VALID_FILE_NAME = "libDynLib.dylib";
static const String INVALID_FILE_NAME = "libBADDynLib.dylib";
#elif defined(PEGASUS_OS_VMS)
static const String VALID_FILE_NAME = "/wbem_opt/wbem/providers/lib/libTestDynLib.exe";
static const String INVALID_FILE_NAME = "/wbem_opt/wbem/providers/lib/libBADTestDynLib.exe";
#else
static const String VALID_FILE_NAME = "libDynLib.so";
static const String INVALID_FILE_NAME = "libBADDynLib.so";
#endif

// load a valid module, export a symbol, call it, and unload module
void Test1(void)
{
    DynamicLibrary library(VALID_FILE_NAME);

    library.load();

    if(!library.isLoaded())
    {
        cout << "failed to load " << library.getFileName() << endl;

        throw 0;
    }

    Uint32 (* callme)(void) = (Uint32 (*)(void))library.getSymbol("callme");

    if(callme == 0)
    {
        library.unload();

        cout << "failed to export callme() from " << library.getFileName() << endl;

        throw 0;
    }

    cout << "callme() returned << " << hex << callme() << endl;

    library.unload();

    if(library.isLoaded())
    {
        cout << "failed to unload " << library.getFileName() << endl;

        throw 0;
    }
}

// load valid module, assignment
void Test2(void)
{
    DynamicLibrary library(VALID_FILE_NAME);

    library.load();

    {
        DynamicLibrary library2(library);

        if(library2.isLoaded() != library.isLoaded())
        {
            cout << "failed to preserve module state in copy " << library.getFileName() << endl;
        }
    }

    {
        DynamicLibrary library2;

        library2 = library;

        if(library2.isLoaded() != library.isLoaded())
        {
            cout << "failed to preserve module state in assignment " << library.getFileName() << endl;
        }
    }

    library.unload();
}

// load an invalid module
void Test3(void)
{
    DynamicLibrary library(INVALID_FILE_NAME);

    library.load();

    if(library.isLoaded())
    {
        library.unload();

        cout << "failed by loading " << library.getFileName() << endl;

        throw 0;
    }
}

int main(int argc, char** argv)
{
    const char * verbose = getenv("PEGASUS_TEST_VERBOSE");

    Test1();
    Test2();
    Test3();

    cout << argv[0] << " +++++ passed all tests" << endl;

    return(0);
}
