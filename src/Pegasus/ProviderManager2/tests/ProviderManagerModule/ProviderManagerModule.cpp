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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/FileSystem.h>

#include <Pegasus/ProviderManager2/ProviderManagerModule.h>

#include <iostream>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static String fileName;

void Test1(void)
{
    ProviderManagerModule module(fileName);

    module.load();

    if(!module.isLoaded())
    {
        cout << "failed to load " << module.getFileName() << endl;

        throw 0;
    }

    ProviderManager * p = module.getProviderManager("TEST");

    if(p == 0)
    {
        module.unload();

        cout << "failed to get provider manager from " << module.getFileName() << endl;

        throw 0;
    }

    delete p;

    module.unload();
}

void Test2(void)
{
    ProviderManagerModule module(fileName);

    module.load();

    {
        ProviderManagerModule module2(module);

        if(module2.isLoaded() != module.isLoaded())
        {
            cout << "failed to preserve module state in copy " << module.getFileName() << endl;
        }

        module2.unload();
    }

    module.unload();
}

// array behavior experiment
void Test3(void)
{
    Array<ProviderManagerModule> modules;

    for(Uint32 i = 0, n = 3; i < n; i++)
    {
        cout << "creating ProviderManagerModule object for " << fileName << endl;

        ProviderManagerModule module(fileName);

        modules.append(module);
    }

    for(Uint32 i = 0, n = modules.size(); i < n; i++)
    {
        cout << "loading ProviderManagerModule object for " << modules[i].getFileName() << endl;

        modules[i].load();
    }

    for(Uint32 i = 0, n = modules.size(); i < n; i++)
    {
        cout << "unloading ProviderManagerModule object for " << modules[i].getFileName() << endl;

        modules[i].unload();
    }

    while(modules.size() != 0)
    {
        cout << "removing (destroying) ProviderManagerModule object for " << modules[0].getFileName() << endl;

        modules.remove(0);
    }
}

int main(int argc, char** argv)
{
    const char * verbose = getenv("PEGASUS_TEST_VERBOSE");

    const String FILE_NAME =
        FileSystem::buildLibraryFileName("TestProviderManager");

    // Use "bin" directory for Windows, to be consistent with the default
    // providerDir value in Config/ProviderDirPropertyOwner.cpp.
#if defined (PEGASUS_PLATFORM_WIN32_IX86_MSVC)
    fileName=String(getenv("PEGASUS_HOME"))+String("/bin/")+FILE_NAME;
#elif defined (PEGASUS_OS_VMS)
    fileName= FILE_NAME;
#else
    fileName=String(getenv("PEGASUS_HOME"))+String("/lib/")+FILE_NAME;
#endif
    
    Test1();
    Test2();

    Test3();

    cout << argv[0] << " +++++ passed all tests" << endl;

    return(0);
}
