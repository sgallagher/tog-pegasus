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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose = 0;

static const String NAMESPACE("test/TestProvider");
static const String Z_CLASSNAME("TST_InstanceZ");
static const String A_CLASSNAME("TST_InstanceA");
static const String B_CLASSNAME("TST_InstanceB");

// get repository instances (TST_InstanceZ)
void Test1(CIMClient& client)
{
    if(verbose)
    {
        cout << "Test1()" << endl;
    }

    Array<CIMInstance> zInstances =
        client.enumerateInstances(
            NAMESPACE,
            Z_CLASSNAME);

    for(Uint32 i = 0, n = zInstances.size(); i < n; i++)
    {
        if(verbose)
        {
            cout << zInstances[i].getPath().toString() << endl;
        }
    }
}

// compare repository instances (TST_InstanceZ) with TestProviderA instances (TST_InstanceA)
void Test2(CIMClient& client)
{
    if(verbose)
    {
        cout << "Test2()" << endl;
    }

    Array<CIMInstance> zInstances =
        client.enumerateInstances(
            NAMESPACE,
            Z_CLASSNAME);

    for(Uint32 i = 0, n = zInstances.size(); i < n; i++)
    {
        if(verbose)
        {
            cout << zInstances[i].getPath().toString() << endl;
        }
    }

    Array<CIMInstance> aInstances =
        client.enumerateInstances(
            NAMESPACE,
            A_CLASSNAME);

    for(Uint32 i = 0, n = aInstances.size(); i < n; i++)
    {
        if(verbose)
        {
            cout << aInstances[i].getPath().toString() << endl;
        }

        bool match = false;

        String aName = aInstances[i].getProperty(aInstances[i].findProperty("Name")).getValue().toString();
        CIMObjectPath aS = aInstances[i].getProperty(aInstances[i].findProperty("s")).getValue().toString();

        if(verbose)
        {
            cout << "aName = " << aName << endl;
            cout << "aS = " << aS.toString() << endl;
        }

        // search for a match (instances sets are not ordered).
        for(Uint32 j = 0, m = zInstances.size(); j < m; j++)
        {
            String zName = zInstances[i].getProperty(zInstances[i].findProperty("Name")).getValue().toString();
            CIMObjectPath zS = zInstances[i].getPath();

            if(verbose)
            {
                cout << "zName = " << zName << endl;
                cout << "zS = " << zS.toString() << endl;
            }

            if(String::equalNoCase(aName, zName) && (aS == zS))
            {
                match = true;

                break;
            }
        }

        if(!match)
        {
            throw Exception("could not find match");
        }
    }
}

// compare TestProviderA instances (TST_InstanceA) with TestProviderB instances (TST_InstanceB)
void Test3(CIMClient& client)
{
    if(verbose)
    {
        cout << "Test3()" << endl;
    }

    Array<CIMInstance> aInstances =
        client.enumerateInstances(
            NAMESPACE,
            A_CLASSNAME);

    for(Uint32 i = 0, n = aInstances.size(); i < n; i++)
    {
        if(verbose)
        {
            cout << aInstances[i].getPath().toString() << endl;
        }
    }

    Array<CIMInstance> bInstances =
        client.enumerateInstances(
            NAMESPACE,
            B_CLASSNAME);

    for(Uint32 i = 0, n = bInstances.size(); i < n; i++)
    {
        if(verbose)
        {
            cout << bInstances[i].getPath().toString() << endl;
        }

        bool match = false;

        String bName = bInstances[i].getProperty(bInstances[i].findProperty("Name")).getValue().toString();
        CIMObjectPath bS = bInstances[i].getProperty(bInstances[i].findProperty("s")).getValue().toString();

        if(verbose)
        {
            cout << "bName = " << bName << endl;
            cout << "bS = " << bS.toString() << endl;
        }

        // search for a match (instances sets are not ordered).
        for(Uint32 j = 0, m = aInstances.size(); j < m; j++)
        {
            String aName = aInstances[i].getProperty(aInstances[i].findProperty("Name")).getValue().toString();
            CIMObjectPath aS = aInstances[i].getPath();

            if(verbose)
            {
                cout << "aName = " << aName << endl;
                cout << "aS = " << aS.toString() << endl;
            }

            if(String::equalNoCase(bName, aName) && (bS == aS))
            {
                match = true;

                break;
            }
        }

        if(!match)
        {
            throw Exception("could not find match");
        }
    }
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    CIMClient client;

    try
    {
        client.connectLocal();

        Test1(client);
        Test2(client);
        Test3(client);
    }
    catch(const CIMException & e)
    {
        client.disconnect();

        cout << "CIMException: " << e.getCode() << " " << e.getMessage() << endl;

        return(1);
    }
    catch(const Exception & e)
    {
        client.disconnect();

        cout << "Exception: " << e.getMessage() << endl;

        return(1);
    }
    catch(...)
    {
        client.disconnect();

        cout << "unknown exception" << endl;

        return(1);
    }

    client.disconnect();

    cout << argv[0] << " +++++ passed all tests" << endl;

    return(0);
}
