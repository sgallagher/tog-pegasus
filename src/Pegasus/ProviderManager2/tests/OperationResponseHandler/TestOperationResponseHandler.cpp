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
// Author: Chip Vincent
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/ProviderManager2/OperationResponseHandler.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const char * verbose = 0;

// test null object checks
void Test1(void)
{
    if(verbose)
    {
        cout << "Test1()" << endl;
    }

    try
    {
        // create dummy request and response messages
        CIMGetInstanceRequestMessage request(
            String::EMPTY,
            CIMNamespaceName(),
            CIMObjectPath("dummy"),
            false,
            false,
            false,
            CIMPropertyList(),
            QueueIdStack());

        CIMGetInstanceResponseMessage response(
            String::EMPTY,
            CIMException(),
            QueueIdStack(),
            CIMInstance());

        GetInstanceResponseHandler handler(&request, &response);

        handler.processing();

        CIMInstance cimInstance;

        handler.deliver(cimInstance);

        handler.complete();

        throw Exception("Failed to detect null object in CIMGetInstanceResponseHandler::deliver().");
    }
    catch(CIMException &)
    {
        // do nothing expected
    }

    try
    {
        // create dummy request and response messages
        CIMEnumerateInstancesRequestMessage request(
            String::EMPTY,
            CIMNamespaceName(),
            CIMName("dummy"),
            false,
            false,
            false,
            false,
            CIMPropertyList(),
            QueueIdStack());

        CIMEnumerateInstancesResponseMessage response(
            String::EMPTY,
            CIMException(),
            QueueIdStack(),
            Array<CIMInstance>());

        EnumerateInstancesResponseHandler handler(&request, &response);

        handler.processing();

        CIMInstance cimInstance;

        handler.deliver(cimInstance);

        handler.complete();

        throw Exception("Failed to detect null object in CIMEnumerateInstancesResponseHandler::deliver().");
    }
    catch(CIMException &)
    {
        // do nothing expected
    }

    try
    {
        // create dummy request and response messages
        CIMEnumerateInstanceNamesRequestMessage request(
            String::EMPTY,
            CIMNamespaceName(),
            CIMName("dummy"),
            QueueIdStack());

        CIMEnumerateInstanceNamesResponseMessage response(
            String::EMPTY,
            CIMException(),
            QueueIdStack(),
            Array<CIMObjectPath>());

        EnumerateInstanceNamesResponseHandler handler(&request, &response);

        handler.processing();

        CIMObjectPath cimObjectPath;

        handler.deliver(cimObjectPath);

        handler.complete();

        throw Exception("Failed to detect null object in CIMEnumerateInstanceNamesResponseHandler::deliver().");
    }
    catch(CIMException &)
    {
        // do nothing expected
    }

    try
    {
        // create dummy request and response messages
        CIMCreateInstanceRequestMessage request(
            String::EMPTY,
            CIMNamespaceName(),
            CIMInstance("dummy"),
            QueueIdStack());

        CIMCreateInstanceResponseMessage response(
            String::EMPTY,
            CIMException(),
            QueueIdStack(),
            CIMObjectPath());

        CreateInstanceResponseHandler handler(&request, &response);

        handler.processing();

        CIMObjectPath cimObjectPath;

        handler.deliver(cimObjectPath);

        handler.complete();

        throw Exception("Failed to detect null object in CIMCreateInstanceResponseHandler::deliver().");
    }
    catch(CIMException &)
    {
    }
}

// test too many or too few objects delivered
void Test2(void)
{
    if(verbose)
    {
        cout << "Test2()" << endl;
    }

    try
    {
        // create dummy request and response messages
        CIMGetInstanceRequestMessage request(
            String::EMPTY,
            CIMNamespaceName(),
            CIMObjectPath("dummy"),
            false,
            false,
            false,
            CIMPropertyList(),
            QueueIdStack());

        CIMGetInstanceResponseMessage response(
            String::EMPTY,
            CIMException(),
            QueueIdStack(),
            CIMInstance());

        GetInstanceResponseHandler handler(&request, &response);

        handler.processing();

        handler.complete();

        throw Exception("Failed to detect too few objects in CIMGetInstanceResponseHandler::complete().");
    }
    catch(CIMException &)
    {
        // do nothing expected
    }

    try
    {
        // create dummy request and response messages
        CIMGetInstanceRequestMessage request(
            String::EMPTY,
            CIMNamespaceName(),
            CIMObjectPath("dummy"),
            false,
            false,
            false,
            CIMPropertyList(),
            QueueIdStack());

        CIMGetInstanceResponseMessage response(
            String::EMPTY,
            CIMException(),
            QueueIdStack(),
            CIMInstance());

        GetInstanceResponseHandler handler(&request, &response);

        handler.processing();

        CIMInstance cimInstance1("dummy");

        handler.deliver(cimInstance1);

        CIMInstance cimInstance2("dummy");

        handler.deliver(cimInstance2);

        handler.complete();

        throw Exception("Failed to detect too many objects in CIMGetInstanceResponseHandler::deliver().");
    }
    catch(CIMException &)
    {
        // do nothing expected
    }

    try
    {
        // create dummy request and response messages
        CIMCreateInstanceRequestMessage request(
            String::EMPTY,
            CIMNamespaceName(),
            CIMInstance("dummy"),
            QueueIdStack());

        CIMCreateInstanceResponseMessage response(
            String::EMPTY,
            CIMException(),
            QueueIdStack(),
            CIMObjectPath());

        CreateInstanceResponseHandler handler(&request, &response);

        handler.processing();

        handler.complete();

        throw Exception("Failed to detect too few objects in CIMCreateInstanceResponseHandler::complete().");
    }
    catch(CIMException &)
    {
    }

    try
    {
        // create dummy request and response messages
        CIMCreateInstanceRequestMessage request(
            String::EMPTY,
            CIMNamespaceName(),
            CIMInstance("dummy"),
            QueueIdStack());

        CIMCreateInstanceResponseMessage response(
            String::EMPTY,
            CIMException(),
            QueueIdStack(),
            CIMObjectPath());

        CreateInstanceResponseHandler handler(&request, &response);

        handler.processing();

        CIMObjectPath cimObjectPath1("dummy");

        handler.deliver(cimObjectPath1);

        CIMObjectPath cimObjectPath2("dummy");

        handler.deliver(cimObjectPath2);

        handler.complete();

        throw Exception("Failed to detect too many objects in CIMGetInstanceResponseHandler::deliver().");
    }
    catch(CIMException &)
    {
        // do nothing expected
    }
}

int main(int argc, char** argv)
{
    const char * verbose = getenv("PEGASUS_TEST_VERBOSE");

    try
    {
        Test1();
        Test2();
    }
    catch(CIMException & e)
    {
        cout << "CIMException: " << e.getCode() << " " << e.getMessage() << endl;

        return(-1);
    }
    catch(Exception & e)
    {
        cout << "Exception: " << e.getMessage() << endl;

        return(-1);
    }
    catch(...)
    {
        cout << "unknown exception" << endl;

        return(-1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return(0);
}
