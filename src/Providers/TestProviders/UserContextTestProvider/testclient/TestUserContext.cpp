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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//      Chip Vincent (cvincent@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const CIMNamespaceName NAMESPACE = CIMNamespaceName("test/TestProvider");
static const char* alternateUserContext = "guest";

static char* verbose;
static String testUserContext;

void testUserContextRequestor()
{
printf("testUserContextRequestor()\n");
    try
    {
        CIMClient client;
        client.connectLocal();

        // Determine whether the CIM Server has authentication enabled

        CIMObjectPath authConfigInstName = CIMObjectPath(
            "PG_ConfigSetting.PropertyName=\"enableAuthentication\""); 
        CIMInstance authConfigInst =
            client.getInstance("root/PG_Internal", authConfigInstName);

        String authConfigValue;
        authConfigInst.getProperty(authConfigInst.findProperty("CurrentValue"))
            .getValue().get(authConfigValue);
        Boolean authenticationEnabled =
            String::equalNoCase(authConfigValue, "true");

        // Test a provider running in Requestor user context

        CIMObjectPath instName =
            CIMObjectPath("TST_UserContextRequestor.Id=1");
        CIMInstance cimInstance = client.getInstance(NAMESPACE, instName);

        String userContext;
        cimInstance.getProperty(cimInstance.findProperty("UserContext"))
            .getValue().get(userContext);

        if (verbose)
        {
            cout << "Requestor test: UserContext = " << userContext << endl;
        }

        if (authenticationEnabled)
        {
            PEGASUS_TEST_ASSERT(userContext == System::getEffectiveUserName());
        }
        else
        {
#ifndef PEGASUS_ENABLE_PRIVILEGE_SEPARATION
            // MEB: This test cannot pass with privilege separation enabled
            // since it expects to run as requestor, which in this case is
            // "root" whereas the server is "pegasus". But with authentication
            // enabled it will work fine.
            PEGASUS_TEST_ASSERT(userContext == testUserContext);
#endif
        }
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}

void testUserContextPrivileged()
{
printf("testUserContextPrivileged()\n");
    try
    {
        CIMClient client;
        client.connectLocal();

        // Test a provider running in Privileged user context

        CIMObjectPath instName =
            CIMObjectPath("TST_UserContextPrivileged.Id=1");

        // ATTN: use of the localOnly flag is deprecated, but nor reliably
        // applied by the CIMOM. An explicit parameter is required for now.
        CIMInstance cimInstance = 
            client.getInstance(NAMESPACE, instName, false);

        String userContext;
        cimInstance.getProperty(cimInstance.findProperty("UserContext"))
            .getValue().get(userContext);

        if (verbose)
        {
            cout << "Privileged test: UserContext = " << userContext << endl;
        }

        PEGASUS_TEST_ASSERT(userContext == testUserContext);
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}

void testUserContextDesignated()
{
printf("testUserContextDesignated()\n");
    try
    {
        if (!System::isSystemUser(alternateUserContext))
        {
            cout << " Skipping Designated UserContext test -- User \"" <<
                alternateUserContext <<
                "\" is not configured on this system." << endl;
            return;
        }

        CIMClient client;
        client.connectLocal();

        // Test a provider running in Designated user context

        CIMObjectPath instName =
            CIMObjectPath("TST_UserContextDesignated.Id=1");

        // ATTN: use of the localOnly flag is deprecated, but nor reliably
        // applied by the CIMOM. An explicit parameter is required for now.
        CIMInstance cimInstance = 
            client.getInstance(NAMESPACE, instName, false);

        String userContext;
        cimInstance.getProperty(cimInstance.findProperty("UserContext"))
            .getValue().get(userContext);

        if (verbose)
        {
            cout << "Designated test: UserContext = " << userContext << endl;
        }

        PEGASUS_TEST_ASSERT(userContext == String(alternateUserContext));
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}

void testUserContextCIMServer()
{
printf("testUserContextCIMServer()\n");
    try
    {
        CIMClient client;
        client.connectLocal();

        // Test a provider running in CIM Server user context

        CIMObjectPath instName =
            CIMObjectPath("TST_UserContextCIMServer.Id=1");

        // ATTN: use of the localOnly flag is deprecated, but nor reliably
        // applied by the CIMOM. An explicit parameter is required for now.
        CIMInstance cimInstance = 
            client.getInstance(NAMESPACE, instName, false);

        String userContext;
        cimInstance.getProperty(cimInstance.findProperty("UserContext"))
            .getValue().get(userContext);

        if (verbose)
        {
            cout << "CIMServer test: UserContext = " << userContext << endl;
        }

#ifdef PEGASUS_ENABLE_PRIVILEGE_SEPARATION

        // ATTN-MEB:
        //
        // We must skip this test for now since run-as-cim-server no longer 
        // means run-as-root, the way it did before. In this test, the 
        // following variables will end up being as follows:
        //
        // userContext=pegasus
        // testUserContext=root
        //
        // To make this test work correctly, we would have to obtain the 
        // serverUser.

        cout << " Skipping as-cimserver UserContext test when using privilege ";
        cout << "separation feature" << endl;

#else
        PEGASUS_TEST_ASSERT(userContext == testUserContext);

#endif /* PEGASUS_ENABLE_PRIVILEGE_SEPARATION */
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    // Perform a pre-test to ensure the Provider User Context feature is only
    // enabled when running in a privileged user context.

    if ((argc == 2) && !strcmp(argv[1], "pretest"))
    {
#ifndef PEGASUS_DISABLE_PROV_USERCTXT
        if (!System::isPrivilegedUser(System::getEffectiveUserName()))
        {
            cerr << "ERROR:  When running in a non-privileged user context, "
                "PEGASUS_DISABLE_PROV_USERCTXT must be set to 'true' in the "
                "build and test environment." << endl;
            return 1;
        }
#endif
        cout << argv[0] << " +++++ passed pretest" << endl;
        return 0;
    }

#ifndef PEGASUS_DISABLE_PROV_USERCTXT
    try
    {
        // Note: This test requires both the CIM Server and this client to
        // run in a privileged user context.  The CIM Server is assumed to
        // run in the same context as this client.
        testUserContext = System::getEffectiveUserName();
        if (!System::isPrivilegedUser(testUserContext))
        {
            cout << " Test skipped -- Must be run by a privileged user" << endl;
            return 0;
        }

#ifndef PEGASUS_DISABLE_PROV_USERCTXT_REQUESTOR
        testUserContextRequestor();
#endif

#ifndef PEGASUS_DISABLE_PROV_USERCTXT_PRIVILEGED
        testUserContextPrivileged();
#endif

#ifndef PEGASUS_DISABLE_PROV_USERCTXT_DESIGNATED
        testUserContextDesignated();
#endif

#ifndef PEGASUS_DISABLE_PROV_USERCTXT_CIMSERVER
        testUserContextCIMServer();
#endif

// The "guest" tests are disabled.  See Bug 3043.
#if 0
        // These tests must be run in a different user context
        if (!System::changeUserContext(alternateUserContext))
        {
            cout << " Skipping tests -- Could not run as user \"" <<
                alternateUserContext << "\"." << endl;
        }
        else
        {
#ifndef PEGASUS_DISABLE_PROV_USERCTXT_REQUESTOR
            testUserContextRequestor();
#endif

#ifndef PEGASUS_DISABLE_PROV_USERCTXT_PRIVILEGED
            testUserContextPrivileged();
#endif

#ifndef PEGASUS_DISABLE_PROV_USERCTXT_DESIGNATED
            testUserContextDesignated();
#endif

#ifndef PEGASUS_DISABLE_PROV_USERCTXT_CIMSERVER
            testUserContextCIMServer();
#endif
        }
#endif
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
#endif

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
