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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const CIMNamespaceName NAMESPACE = CIMNamespaceName("test/TestProvider");

static char* verbose;

void callMethod(const CIMName& methodName)
{
    try
    {
        CIMClient client;
        client.connectLocal();

        Array<CIMParamValue> inParams;
        Array<CIMParamValue> outParams;

        CIMObjectPath instName =
            CIMObjectPath("Test_MethodProviderClass.Id=1");

        inParams.append(CIMParamValue("InParam1", Uint32(1)));
        inParams.append(CIMParamValue("InParam2", Uint32(2)));

        CIMValue returnValue = client.invokeMethod(
            NAMESPACE,
            instName,
            methodName,
            inParams,
            outParams);

        Uint32 rc;
        returnValue.get(rc);
        assert(rc == 10);

        assert(outParams.size() == 2);

        Uint32 outParam1;
        Uint32 outParam2;

        for (Uint32 i = 0; i < 2; i++)
        {
            if (outParams[i].getParameterName() == "OutParam1")
            {
                outParams[i].getValue().get(outParam1);
            }
            else if (outParams[i].getParameterName() == "OutParam2")
            {
                outParams[i].getValue().get(outParam2);
            }
            else
            {
                assert(0);
            }
        }
        assert(outParam1 == 21);
        assert(outParam2 == 32);
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

    try
    {
        if (verbose)
        {
            cout << "Calling test2" << endl;
        }
        callMethod("test2");

        if (verbose)
        {
            cout << "Calling test1" << endl;
        }
        callMethod("test1");
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
