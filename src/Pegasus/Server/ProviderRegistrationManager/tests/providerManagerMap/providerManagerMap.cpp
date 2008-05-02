//%2008////////////////////////////////////////////////////////////////////////
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
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; Novell, Inc.; The Open Group.
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
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderManagerMap.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static Boolean verbose;

int main(int argc, char** argv)
{
    verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;

    const char* _pegHome = getenv ("PEGASUS_HOME"); 
    if (_pegHome == NULL)
    {
        PEGASUS_STD (cout) << argv[0] << " +++++ tests failed: "
            << "PEGASUS_HOME environment variable must be set"
                           << PEGASUS_STD (endl);
        return -1; 
    }
    ConfigManager::setPegasusHome(_pegHome); 

    try
    {
        String name; 
        String ver; 

        name = "C++Default"; 
        ver = "2.1.0";
        PEGASUS_TEST_ASSERT(
            ProviderManagerMap::instance().isValidProvMgrIfc(name, ver));
        ver = "2.6.0"; 
        PEGASUS_TEST_ASSERT(
            ProviderManagerMap::instance().isValidProvMgrIfc(name, ver));
        name = "Junk"; 
        PEGASUS_TEST_ASSERT(
            !ProviderManagerMap::instance().isValidProvMgrIfc(name, ver));
#ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
        name = "CMPI"; 
        ver = "2.0.0"; 
        PEGASUS_TEST_ASSERT(
            ProviderManagerMap::instance().isValidProvMgrIfc(name, ver));
        ver = "1.2.3"; 
        PEGASUS_TEST_ASSERT(
            !ProviderManagerMap::instance().isValidProvMgrIfc(name, ver));
#endif
#ifdef PEGASUS_ENABLE_JMPI_PROVIDER_MANAGER
        name = "JMPI"; 
        ver = "2.2.0"; 
        PEGASUS_TEST_ASSERT(
            ProviderManagerMap::instance().isValidProvMgrIfc(name, ver));
#endif
    }
    catch(Exception& e)
    {
    PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
    PEGASUS_STD (cout) << argv[0] << " +++++ tests failed"
                       << PEGASUS_STD (endl);
    exit(-1);
    }


    PEGASUS_STD(cout) << argv[0] << " +++++ passed all tests" 
                      << PEGASUS_STD(endl);

    return 0;
}

