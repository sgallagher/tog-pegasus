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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//      Chip Vincent (cvincent@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
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
            assert(userContext == System::getEffectiveUserName());
        }
        else
        {
            assert(userContext == testUserContext);
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

        assert(userContext == testUserContext);
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}

void testUserContextDesignated()
{
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

        assert(userContext == String(alternateUserContext));
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}

void testUserContextCIMServer()
{
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

        assert(userContext == testUserContext);
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
