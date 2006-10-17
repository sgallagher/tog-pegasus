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
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/ExportClient/CIMExportClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/cimv2";

static void TestSendingToNonExistentConsumer()
{
    CIMException testException;
    Monitor* monitor = new Monitor;
    HTTPConnector* httpConnector = new HTTPConnector(monitor);

    CIMExportClient client(monitor, httpConnector);
    Uint32 port =
        System::lookupPort(WBEM_HTTP_SERVICE_NAME, WBEM_DEFAULT_HTTP_PORT);
    client.connect("localhost", port);

    CIMInstance indication(CIMName("My_IndicationClass"));
    indication.addProperty(CIMProperty(CIMName("DeviceName"), String("Disk")));
    indication.addProperty(CIMProperty(CIMName("DeviceId"), Uint32(1)));

    try
    {
        client.exportIndication("/CIMListener/NOT_A_CONSUMER", indication);
    }
    catch (CIMException& e)
    {
        testException = e;
    }

    PEGASUS_TEST_ASSERT(testException.getCode() == CIM_ERR_NOT_SUPPORTED);
}

static void TestExceptionHandling()
{
    Monitor* monitor = new Monitor;
    HTTPConnector* httpConnector = new HTTPConnector(monitor);
    CIMExportClient client(monitor, httpConnector);

    CIMInstance indication(CIMName("My_IndicationClass"));
    indication.addProperty(CIMProperty(CIMName("DeviceName"), String("Disk")));
    indication.addProperty(CIMProperty(CIMName("DeviceId"), Uint32(1)));

    /*
         Missing call to client.connect(...).  Verify NotConnectedException
         is returned.
    */
    Boolean exceptionCaught;
    exceptionCaught = false;
    try
    {
        client.exportIndication(
           "/CIMListener/Pegasus_IndicationStressTestConsumer", indication);
    }
    catch (const NotConnectedException&)
    {
        exceptionCaught = true;
    }
    PEGASUS_TEST_ASSERT(exceptionCaught);
}

static void TestTimeout()
{
    Monitor* monitor = new Monitor;
    HTTPConnector* httpConnector = new HTTPConnector(monitor);
    CIMExportClient client(monitor, httpConnector);

    PEGASUS_TEST_ASSERT(client.getTimeout() ==
        CIMExportClient::DEFAULT_TIMEOUT_MILLISECONDS);
    client.setTimeout(CIMExportClient::DEFAULT_TIMEOUT_MILLISECONDS+20000);
    PEGASUS_TEST_ASSERT(client.getTimeout() ==
        CIMExportClient::DEFAULT_TIMEOUT_MILLISECONDS+20000);
    client.setTimeout(CIMExportClient::DEFAULT_TIMEOUT_MILLISECONDS);
    PEGASUS_TEST_ASSERT(client.getTimeout() ==
        CIMExportClient::DEFAULT_TIMEOUT_MILLISECONDS);

    //
    //  Test setTimeout while connected
    //
    Uint32 port =
        System::lookupPort(WBEM_HTTP_SERVICE_NAME, WBEM_DEFAULT_HTTP_PORT);
    client.connect("localhost", port);
    PEGASUS_TEST_ASSERT(client.getTimeout() ==
        CIMExportClient::DEFAULT_TIMEOUT_MILLISECONDS);
    client.setTimeout(CIMExportClient::DEFAULT_TIMEOUT_MILLISECONDS+20000);
    PEGASUS_TEST_ASSERT(client.getTimeout() ==
        CIMExportClient::DEFAULT_TIMEOUT_MILLISECONDS+20000);
    client.setTimeout(CIMExportClient::DEFAULT_TIMEOUT_MILLISECONDS);
    PEGASUS_TEST_ASSERT(client.getTimeout() ==
        CIMExportClient::DEFAULT_TIMEOUT_MILLISECONDS);
}

static void testConnect()
{
    //
    //  Create CIMExportClient
    //
    Monitor* monitor = new Monitor;
    HTTPConnector* httpConnector = new HTTPConnector(monitor);
    CIMExportClient client(monitor, httpConnector);

    //
    //  Look up port number
    //
    Uint32 port =
        System::lookupPort(WBEM_HTTP_SERVICE_NAME, WBEM_DEFAULT_HTTP_PORT);

    //
    //  Test connect with empty string host
    //
    Boolean exceptionCaught = false;
    try
    {
        client.connect("", port);
    }
    catch(const Exception& e)
    {
        exceptionCaught = true;
    }
    PEGASUS_TEST_ASSERT(!exceptionCaught);

    //
    //  Test connect when already connected
    //
    Boolean alreadyConnectedCaught = false;
    try
    {
        client.connect("localhost", port);
    }
    catch(const AlreadyConnectedException&)
    {
        alreadyConnectedCaught = true;
    }
    PEGASUS_TEST_ASSERT(alreadyConnectedCaught);
    client.disconnect();
}

static void testSSLConnect()
{
    //
    //  Create CIMExportClient
    //
    Monitor* monitor = new Monitor;
    HTTPConnector* httpConnector = new HTTPConnector(monitor);
    CIMExportClient client(monitor, httpConnector);

    //
    //  Create SSLContext
    //
    const char* pegasusHome = getenv("PEGASUS_HOME");
    String trustStore = FileSystem::getAbsolutePath(pegasusHome,
        "client.pem");
    String certPath = FileSystem::getAbsolutePath(pegasusHome,
        "cert.pem");
    String keyPath = FileSystem::getAbsolutePath(pegasusHome,
        "file.pem");
    String randPath = String::EMPTY;
#ifdef PEGASUS_SSL_RANDOMFILE
    randPath = FileSystem::getAbsolutePath(pegasusHome,
        PEGASUS_SSLCLIENT_RANDOMFILE);
#endif

    SSLContext* sslContext = 0;
    sslContext = new SSLContext(trustStore, certPath, keyPath, 0, randPath);

    //
    //  Look up port number
    //
    Uint32 port =
        System::lookupPort(WBEM_HTTPS_SERVICE_NAME, WBEM_DEFAULT_HTTPS_PORT);

    //
    //  Test SSL connect with empty string host
    //
    Boolean exceptionCaught = false;
    try
    {
        client.connect("", port, *sslContext);
    }
    catch(const Exception& e)
    {
        exceptionCaught = true;
    }
    PEGASUS_TEST_ASSERT(!exceptionCaught);

    //
    //  Test SSL connect when already connected
    //
    Boolean alreadyConnectedCaught = false;
    try
    {
        client.connect("localhost", port, *sslContext);
    }
    catch(const AlreadyConnectedException&)
    {
        alreadyConnectedCaught = true;
    }
    PEGASUS_TEST_ASSERT(alreadyConnectedCaught);
    client.disconnect();

    //
    //  Test SSL connect with bad hostname to cause exception
    //
    Boolean invalidLocatorCaught = false;
    try
    {
        client.connect("invalid", port, *sslContext);
    }
    catch(const InvalidLocatorException& e)
    {
        invalidLocatorCaught = true;
    }
    PEGASUS_TEST_ASSERT(invalidLocatorCaught);
}

int main(int argc, char** argv)
{
    try
    {
        if ((argc == 2) && !strcmp(argv[1], "ssl"))
        {
#ifdef PEGASUS_HAS_SSL
            testSSLConnect();
#else
            cout << argv [0] <<
                ": Export Client SSL tests skipped because"
                " PEGASUS_HAS_SSL is not defined" << endl;
#endif
        }
        else if (argc == 1)
        {
            TestExceptionHandling();
            TestTimeout();
            TestSendingToNonExistentConsumer();
            testConnect();
        }
        else
        {
            cerr << "Error: unexpected test arguments" << endl;
            return 1;
        }
    }
    catch(const Exception& e)
    {
        cerr << "Exception: " << e.getMessage() << endl;
        return 1;
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
