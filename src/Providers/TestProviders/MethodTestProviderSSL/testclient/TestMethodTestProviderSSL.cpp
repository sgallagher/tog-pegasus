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

#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Constants.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const CIMNamespaceName NAMESPACE = CIMNamespaceName("test/TestProvider");

static Boolean verbose;

void callMethod(CIMClient* client, const CIMName& methodName)
{
    try
    {
        Array<CIMParamValue> inParams;
        Array<CIMParamValue> outParams;

        CIMObjectPath instName =
            CIMObjectPath("Test_MethodProviderSSLClass.Id=1");

        inParams.append(CIMParamValue("InParam1", Uint32(1)));
        inParams.append(CIMParamValue("InParam2", Uint32(2)));

        CIMValue returnValue = client->invokeMethod(
            NAMESPACE,
            instName,
            methodName,
            inParams,
            outParams);

        Uint32 rc;
        returnValue.get(rc);
        PEGASUS_TEST_ASSERT(rc == 10);

        PEGASUS_TEST_ASSERT(outParams.size() == 2);

        Uint32 outParam1 = 0;
        Uint32 outParam2 = 0;

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
                PEGASUS_TEST_ASSERT(0);
            }
        }
        PEGASUS_TEST_ASSERT(outParam1 == 21);
        PEGASUS_TEST_ASSERT(outParam2 == 32);
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}

/* Test for method invoke and response that handles ref parameter
   This functions sends two ref  in parameters and expects exactly the 
   same references back in the two out parameters.
*/
void callMethodRefParam(CIMClient* client, const CIMName& methodName)
{
    try
    {
        Array<CIMParamValue> inParams;
        Array<CIMParamValue> outParams;

        CIMObjectPath instName =
            CIMObjectPath("Test_MethodProviderSSLClass.Id=1");

        CIMObjectPath inParam1(
            String("//atp:77/root/cimv2:CIM_ComputerSystem."
                "last=\"Rafter\",first=\"Patrick\""));

        CIMObjectPath inParam2(
            String("//atp:77/root/cimv2:CIM_ComputerSystem."
                "last=\"Rafter\",first=\"Rafter\""));

        inParams.append(CIMParamValue("InParam1", inParam1));
        inParams.append(CIMParamValue("InParam2", inParam2));
        inParams.append(CIMParamValue("Recursed", Boolean(false)));

        CIMValue returnValue = client->invokeMethod(
            NAMESPACE,
            instName,
            methodName,
            inParams,
            outParams);

        Uint32 rc;
        returnValue.get(rc);
        PEGASUS_TEST_ASSERT(rc == 10);

        PEGASUS_TEST_ASSERT(outParams.size() == 3);

        CIMObjectPath outParam1;
        CIMObjectPath outParam2;

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
                PEGASUS_TEST_ASSERT(0);
            }
        }
        PEGASUS_TEST_ASSERT(outParam1 == inParam1);
        PEGASUS_TEST_ASSERT(outParam2 == inParam2);
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}


/* Test for method invoke and response that handles ref parameter arrays.
   This functions sends two ref array in parameters and expects exactly the 
   same reference arrays back in the two out parameters.
*/
void callMethodRefParamArray(CIMClient* client, const CIMName& methodName)
{
    try
    {
        Array<CIMParamValue> inParams;
        Array<CIMParamValue> outParams;

        CIMObjectPath instName =
            CIMObjectPath("Test_MethodProvideSSLClass.Id=1");

        CIMObjectPath inParamA1(
            String("//atp:77/root/cimv2:CIM_ComputerSystem."
                "last=\"Rafter\",first=\"Patrick\""));

        CIMObjectPath inParamA2(
            String("//atp:77/root/cimv2:CIM_ComputerSystem."
                "last=\"Rafter\",first=\"Rafter\""));

        CIMObjectPath inParamB1(
            String("//atp:77/root/cimv2:CIM_ComputerSystem."
                "last=\"Rafter\",first=\"Fred\""));

        CIMObjectPath inParamB2(
            String("//atp:77/root/cimv2:CIM_ComputerSystem."
                "last=\"Rafter\",first=\"John\""));

        Array<CIMObjectPath> InParamAArray;
        InParamAArray.append(inParamA1);
        InParamAArray.append(inParamA2);
        CIMValue inParam1(InParamAArray);

        Array<CIMObjectPath> InParamBArray;
        InParamBArray.append(inParamB1);
        InParamBArray.append(inParamB2);
        CIMValue inParam2(InParamBArray);

        inParams.append(CIMParamValue("InParam1", inParam1));
        inParams.append(CIMParamValue("InParam2", inParam2));

        CIMValue returnValue = client->invokeMethod(
            NAMESPACE,
            instName,
            methodName,
            inParams,
            outParams);

        Uint32 rc;
        returnValue.get(rc);
        PEGASUS_TEST_ASSERT(rc == 10);

        PEGASUS_TEST_ASSERT(outParams.size() == 2);

        Array<CIMObjectPath> outParam1;
        Array<CIMObjectPath> outParam2;

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
                PEGASUS_TEST_ASSERT(0);
            }
        }
        PEGASUS_TEST_ASSERT(outParam1 == inParam1);
        PEGASUS_TEST_ASSERT(outParam2 == inParam2);
    }
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }
}

/** This method handles the SSLCertificate verification part. */
static Boolean verifyCertificate(SSLCertificateInfo &certInfo)
{
    //
    // If server certificate was found in trust store and validated, then
    // return 'true' to accept the certificate, otherwise return 'false'.
    //
    if (certInfo.getResponseCode() == 1)
    {
cout<<"Here in true returning of verifyCertificate..."<<endl;
        return true;
    }
    else
    {
cout<<"Here in false returning of verifyCertificate..."<<endl;
        return false;
    }
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE") ? true : false;
    if ((argc < 3) || (argc > 6))
    {
        PEGASUS_STD(cout) << "Wrong number of arguments" << PEGASUS_STD(endl);
        exit(1);
    }
    
    String certpath = String::EMPTY;
    if (argv[1])
    {
        certpath = argv[1];
    }

    String keypath = String::EMPTY;
    if (argv[2])
    {
        keypath = argv[2];
    }
cout<<"Cert = "<<certpath<<"  Key = "<<keypath<<endl;

    const char* pegasusHome = getenv("PEGASUS_HOME");
    String randFile = String::EMPTY;
    if (argc >=4)
    {
        if (strcmp(argv[3],"CONFIG") == 0)
        {
            randFile = FileSystem::getAbsolutePath(
                pegasusHome, PEGASUS_SSLCLIENT_RANDOMFILE);
        }
        else if (argv[3])
        {
            randFile = argv[3];
        }
    }

    String userName = String::EMPTY;
    if (argc >=  5)
    {
        userName = argv[4];
    }

    String password = String::EMPTY;
    if (argc >=  6)
    {
        password = argv[5];
    }
   
    String trustStore = FileSystem::getAbsolutePath(pegasusHome,
        "client.pem");

    try
    {
        SSLContext *pCtx = NULL;
        if (certpath != String::EMPTY)
        {
           pCtx = new SSLContext(
               trustStore,
               certpath,
               keypath,
               verifyCertificate,
               randFile);
        }
        else
        {
           pCtx = new SSLContext(
               trustStore,
               verifyCertificate,
               randFile);
        }

        CIMClient* client = new CIMClient();
        client->connect("127.0.0.1", 5989, *pCtx, userName, password);
 
        if (verbose)
        {
            cout << "Calling test2" << endl;
        }
        callMethod(client, "Test2");

        if (verbose)
        {
            cout << "Calling test1" << endl;
        }
        callMethod(client, "Test1");

        if (verbose)
        {
            cout << "Calling test3" << endl;
        }
        callMethodRefParam(client, "Test3");
        if (verbose)
        {
            cout << "Calling test4" << endl;
        }
        callMethodRefParamArray(client, "Test4");
        if (verbose)
        {
            cout << "Calling getIdentity" << endl;
        }
        callMethod(client, "getIdentity");

    } 
    catch (Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
