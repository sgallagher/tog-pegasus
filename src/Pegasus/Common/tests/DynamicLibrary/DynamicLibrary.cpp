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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/DynamicLibrary.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/PegasusAssert.h>

#include <iostream>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const String VALID_LIBRARY_NAME = "TestDynLib";
static const String INVALID_LIBRARY_NAME = "BADDynLib";

const char* verbose = 0;

String getLibraryFileName(const String& libraryName) { 
#if defined(PEGASUS_OS_VMS)
    String prefixDir;
# if defined(PEGASUS_USE_RELEASE_DIRS)
    prefixDir = String("/wbem_opt/wbem/providers/lib/");
# else
    const char* tmp = getenv("PEGASUS_HOME");
    if (tmp)
    {
        prefixDir = tmp + String("/bin/");
    }
# endif
    return prefixDir +
           FileSystem::buildLibraryFileName(libraryName) + ".exe"; 
# else
    return FileSystem::buildLibraryFileName(libraryName);
#endif
}

// load a valid module, export a symbol, call it, and unload module
void Test1()
{
    DynamicLibrary library(getLibraryFileName(VALID_LIBRARY_NAME));
    PEGASUS_TEST_ASSERT(library.getFileName() ==
        getLibraryFileName(VALID_LIBRARY_NAME));

    library.load();
    PEGASUS_TEST_ASSERT(library.isLoaded());

    Uint32 (* callme)(void) = (Uint32 (*)(void))library.getSymbol("callme");
    PEGASUS_TEST_ASSERT(callme);
    PEGASUS_TEST_ASSERT(callme() == 0xdeadbeef);

    Uint32 (* badfunc)(void) = (Uint32 (*)(void))library.getSymbol("badfunc");
    PEGASUS_TEST_ASSERT(badfunc == 0);

    library.unload();
    PEGASUS_TEST_ASSERT(!library.isLoaded());
}

// load valid module, assignment
void Test2()
{
    DynamicLibrary library(getLibraryFileName(VALID_LIBRARY_NAME));
    PEGASUS_TEST_ASSERT(!library.isLoaded());

    {
        DynamicLibrary library2(library);
        PEGASUS_TEST_ASSERT(!library2.isLoaded());
    }

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

    {
        DynamicLibrary library2(getLibraryFileName(VALID_LIBRARY_NAME));
        library2.load();
        PEGASUS_TEST_ASSERT(library2.isLoaded());
        library2 = library;
        PEGASUS_TEST_ASSERT(library2.isLoaded());
    }

    library = library;
    PEGASUS_TEST_ASSERT(library.isLoaded());

    library.unload();
    PEGASUS_TEST_ASSERT(!library.isLoaded());
}

// load an invalid module
void Test3()
{
    DynamicLibrary library(getLibraryFileName(INVALID_LIBRARY_NAME));
    PEGASUS_TEST_ASSERT(!library.isLoaded());

    Boolean loaded = library.load();
    PEGASUS_TEST_ASSERT(!loaded);
    PEGASUS_TEST_ASSERT(!library.isLoaded());

    String errorMessage = library.getLoadErrorMessage();
    String fileName = library.getFileName();
    if (verbose)
    {
        cout << "Invalid library name: " << fileName << endl;
        cout << "Invalid library load error: " << errorMessage << endl;
    }

    PEGASUS_TEST_ASSERT(fileName == getLibraryFileName(INVALID_LIBRARY_NAME));
}

// Test reference counting
void Test4()
{
    DynamicLibrary library(getLibraryFileName(VALID_LIBRARY_NAME));
    PEGASUS_TEST_ASSERT(!library.isLoaded());

    // Load the library
    Boolean loaded = library.load();
    PEGASUS_TEST_ASSERT(loaded);
    PEGASUS_TEST_ASSERT(library.isLoaded());

    // Load the library again
    loaded = library.load();
    PEGASUS_TEST_ASSERT(loaded);
    PEGASUS_TEST_ASSERT(library.isLoaded());

    // Unload the library
    library.unload();
    PEGASUS_TEST_ASSERT(library.isLoaded());

    // Unload the library again
    library.unload();
    PEGASUS_TEST_ASSERT(!library.isLoaded());

    // Load the library and then assign to a new instance
    library.load();
    PEGASUS_TEST_ASSERT(library.isLoaded());
    library.load();
    PEGASUS_TEST_ASSERT(library.isLoaded());
    library = DynamicLibrary(getLibraryFileName(VALID_LIBRARY_NAME));
    PEGASUS_TEST_ASSERT(!library.isLoaded());

    library.load();
    PEGASUS_TEST_ASSERT(library.isLoaded());
    library.load();
    PEGASUS_TEST_ASSERT(library.isLoaded());
    // Call the destructor while the library is loaded
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    Test1();
    Test2();
    Test3();
    Test4();

    cout << argv[0] << " +++++ passed all tests" << endl;

    return(0);
}
