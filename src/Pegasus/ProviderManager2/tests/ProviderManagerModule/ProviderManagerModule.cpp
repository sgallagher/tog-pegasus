//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

#include <Pegasus/ProviderManager2/ProviderManagerModule.h>

PEGASUS_USING_PEGASUS;

#include <iostream>

PEGASUS_USING_STD;

#if defined(PEGASUS_OS_TYPE_WINDOWS)
static const String FILE_NAME = "TestProviderManager.dll";
#else
static const String FILE_NAME = "libTestProviderManager.so";
#endif

void Test1(void)
{
    ProviderManagerModule module(FILE_NAME);

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

    module.unload();
}

void Test2(void)
{
    ProviderManagerModule module(FILE_NAME);

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

int main(int argc, char** argv)
{
    const char * verbose = getenv("PEGASUS_TEST_VERBOSE");

    Test1();
    Test2();

    cout << argv[0] << " +++++ passed all tests" << endl;

    return(0);
}
