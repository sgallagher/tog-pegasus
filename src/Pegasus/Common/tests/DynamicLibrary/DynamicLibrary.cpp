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
// Modified By:	Sean Keenan (sean.keenan@hp.com)
//             	Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/DynamicLibrary.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/PegasusAssert.h>

#include <iostream>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const String VALID_LIBRARY_NAME = "TestDynLib";
static const String INVALID_LIBRARY_NAME = "BADDynLib";

String getLibraryFileName(const String& libraryName)
{
#if defined(PEGASUS_OS_VMS)
    return String("/wbem_opt/wbem/providers/lib/lib") + fileName + ".exe";
#else
    return FileSystem::buildLibraryFileName(libraryName);
#endif
}

// load a valid module, export a symbol, call it, and unload module
void Test1()
{
    DynamicLibrary library(getLibraryFileName(VALID_LIBRARY_NAME));

    library.load();
    PEGASUS_TEST_ASSERT(library.isLoaded());

    Uint32 (* callme)(void) = (Uint32 (*)(void))library.getSymbol("callme");
    PEGASUS_TEST_ASSERT(callme);
    PEGASUS_TEST_ASSERT(callme() == 0xdeadbeef);

    library.unload();
    PEGASUS_TEST_ASSERT(!library.isLoaded());
}

// load valid module, assignment
void Test2()
{
    DynamicLibrary library(getLibraryFileName(VALID_LIBRARY_NAME));
    PEGASUS_TEST_ASSERT(!library.isLoaded());

    library.load();
    PEGASUS_TEST_ASSERT(library.isLoaded());

    {
        DynamicLibrary library2(library);
        PEGASUS_TEST_ASSERT(library2.isLoaded());
    }

    {
        DynamicLibrary library2;

        library2 = library;
        PEGASUS_TEST_ASSERT(library2.isLoaded());
    }

    library.unload();
    PEGASUS_TEST_ASSERT(!library.isLoaded());
}

// load an invalid module
void Test3()
{
    DynamicLibrary library(getLibraryFileName(INVALID_LIBRARY_NAME));
    PEGASUS_TEST_ASSERT(!library.isLoaded());

    library.load();
    PEGASUS_TEST_ASSERT(!library.isLoaded());
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
