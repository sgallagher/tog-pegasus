//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////
 
  
#include <cstdlib>
#include <cassert>
#include <Pegasus/Common/CIMMessageSerializer.h>
#include <Pegasus/Common/CIMMessageDeserializer.h>

PEGASUS_USING_PEGASUS; 
PEGASUS_USING_STD;

static char * verbose;   
 
CIMMessage* serializeDeserializeMessage(CIMMessage* inMessage)
{
    Array<char> outBuffer;
    CIMMessageSerializer::serialize(outBuffer, inMessage);
    outBuffer.append(0);

    char* inBuffer = new char[outBuffer.size()];
    memcpy(inBuffer, outBuffer.getData(), outBuffer.size());

    if (verbose)
    {
        cout << inBuffer << endl;
    }

    CIMMessage* outMessage;
    outMessage = CIMMessageDeserializer::deserialize(inBuffer);

    delete [] inBuffer;

    return outMessage;
}

void testCIMGetInstanceRequestMessage()
{
    QueueIdStack queueIds;
    queueIds.push(10);
    queueIds.push(4);

    AutoPtr<CIMGetInstanceRequestMessage> inMessage;
    inMessage.reset(new CIMGetInstanceRequestMessage(
        "TestMessageID",                    // messageId
        CIMNamespaceName("/test/cimv2"),    // nameSpace
        CIMObjectPath("MyClass.key=1"),     // instanceName
        false,                              // localOnly
        true,                               // includeQualifiers
        true,                               // includeClassOrigin
        CIMPropertyList(),                  // propertyList
        queueIds,                           // queueIds
        String("TestAuthType"),             // authType
        String("TestUserName")));           // userName

    AutoPtr<CIMMessage> outMessage(
        serializeDeserializeMessage(inMessage.get()));

    CIMGetInstanceRequestMessage* outMessageRef;
    outMessageRef =
        dynamic_cast<CIMGetInstanceRequestMessage*>(outMessage.get());
    assert(outMessageRef != 0);

    assert(inMessage->getType() == outMessage->getType());
    assert(inMessage->messageId == outMessageRef->messageId);
    assert(inMessage->queueIds.size() == outMessageRef->queueIds.size());
    if (!inMessage->queueIds.isEmpty())
    {
        assert(inMessage->queueIds.top() == outMessageRef->queueIds.top());
    }
    assert(inMessage->nameSpace == outMessageRef->nameSpace);
    assert(inMessage->className == outMessageRef->className);
    assert(inMessage->providerType == outMessageRef->providerType);
    assert(inMessage->authType == outMessageRef->authType);
    assert(inMessage->userName == outMessageRef->userName);
    assert(inMessage->instanceName == outMessageRef->instanceName);
    assert(inMessage->localOnly == outMessageRef->localOnly);
    assert(inMessage->includeQualifiers ==
           outMessageRef->includeQualifiers);
    assert(inMessage->includeClassOrigin ==
           outMessageRef->includeClassOrigin);
    assert((inMessage->propertyList.isNull() &&
            outMessageRef->propertyList.isNull()) ||
           (!inMessage->propertyList.isNull() &&
            !outMessageRef->propertyList.isNull()));
    assert(inMessage->propertyList.getPropertyNameArray() ==
           outMessageRef->propertyList.getPropertyNameArray());
}

void testCIMGetInstanceResponseMessage()
{
    AutoPtr<CIMGetInstanceResponseMessage> inMessage;
    inMessage.reset(new CIMGetInstanceResponseMessage(
        "123",                              // messageId
        CIMException(CIM_ERR_NOT_SUPPORTED,
            "Unsupported operation"),       // cimException
        QueueIdStack(),                     // queueIds
        CIMInstance()));                    // cimInstance

    AutoPtr<CIMMessage> outMessage(
        serializeDeserializeMessage(inMessage.get()));

    CIMGetInstanceResponseMessage* outMessageRef;
    outMessageRef =
        dynamic_cast<CIMGetInstanceResponseMessage*>(outMessage.get());
    assert(outMessageRef != 0);

    assert(inMessage->getType() == outMessage->getType());
    assert(inMessage->messageId == outMessageRef->messageId);
    assert(inMessage->cimException.getCode() ==
           outMessageRef->cimException.getCode());
    assert(inMessage->cimException.getMessage() ==
           outMessageRef->cimException.getMessage());
    assert(inMessage->queueIds.size() == outMessageRef->queueIds.size());
    if (!inMessage->queueIds.isEmpty())
    {
        assert(inMessage->queueIds.top() == outMessageRef->queueIds.top());
    }
    if (inMessage->cimInstance.isUninitialized())
    {
        assert(outMessageRef->cimInstance.isUninitialized());
    }
    else
    {
        assert(inMessage->cimInstance.identical(outMessageRef->cimInstance));
    }
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    try
    {
        testCIMGetInstanceRequestMessage();
        testCIMGetInstanceResponseMessage();
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
