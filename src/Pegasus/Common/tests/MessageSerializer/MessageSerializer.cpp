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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Jenny Yu, Hewlett-Packard Company (jenny.yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////
 
  
#include <cstdlib>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMMessageSerializer.h>
#include <Pegasus/Common/CIMMessageDeserializer.h>

PEGASUS_USING_PEGASUS; 
PEGASUS_USING_STD;

const CIMNamespaceName TEST_NAMESPACE = CIMNamespaceName("/test/cimv2");
const CIMName MYCLASS = "MyClass";

static char * verbose;   
 
// 
// createInstances
//
Array<CIMObject> createInstances()
{
    Array<CIMObject>    cimObjects;
    String host = System::getHostName();

    CIMInstance instance1(MYCLASS);
    instance1.addProperty(CIMProperty("Name", "Carly"));
    instance1.addProperty(CIMProperty("Identifier", "111"));
    Array<CIMKeyBinding> keyBindings1;
    keyBindings1.append(CIMKeyBinding(CIMName("Name"), "Carly",
	                              CIMKeyBinding::STRING));
    CIMObjectPath path1(host, TEST_NAMESPACE, MYCLASS, keyBindings1);
    instance1.setPath(path1);
    cimObjects.append(instance1);

    CIMInstance instance2(MYCLASS);
    instance2.addProperty(CIMProperty("Name", "Greg"));
    instance2.addProperty(CIMProperty("Identifier", "222"));
    Array<CIMKeyBinding> keyBindings2;
    keyBindings2.append(CIMKeyBinding(CIMName("Name"), "Greg",
	                              CIMKeyBinding::STRING));
    CIMObjectPath path2(host, TEST_NAMESPACE, MYCLASS, keyBindings2);
    instance2.setPath(path2);
    cimObjects.append(instance2);

    return cimObjects;
}

// 
// verifyGetInstanceResponseMessage
//
void verifyGetInstanceResponseMessage(
    CIMGetInstanceResponseMessage * inMessage,
    CIMMessage * outMessage)
{
    CIMGetInstanceResponseMessage * outMessageRef =
        dynamic_cast<CIMGetInstanceResponseMessage*>(outMessage);
    PEGASUS_TEST_ASSERT(outMessageRef != 0);

    PEGASUS_TEST_ASSERT(inMessage->getType() == outMessage->getType());
    PEGASUS_TEST_ASSERT(inMessage->messageId == outMessageRef->messageId);
    PEGASUS_TEST_ASSERT(inMessage->cimException.getCode() ==
           outMessageRef->cimException.getCode());
    PEGASUS_TEST_ASSERT(inMessage->cimException.getMessage() ==
           outMessageRef->cimException.getMessage());
    PEGASUS_TEST_ASSERT(inMessage->queueIds.size() == outMessageRef->queueIds.size());
    if (!inMessage->queueIds.isEmpty())
    {
        PEGASUS_TEST_ASSERT(inMessage->queueIds.top() == outMessageRef->queueIds.top());
    }
    if (inMessage->cimInstance.isUninitialized())
    {
        PEGASUS_TEST_ASSERT(outMessageRef->cimInstance.isUninitialized());
    }
    else
    {
        PEGASUS_TEST_ASSERT(inMessage->cimInstance.identical(outMessageRef->cimInstance));
    }
}

// 
// verifyAssociatorsResponseMessage
//
void verifyAssociatorsResponseMessage(
    CIMAssociatorsResponseMessage * inMessage,
    CIMMessage * outMessage)
{
    CIMAssociatorsResponseMessage * outMessageRef =
        dynamic_cast<CIMAssociatorsResponseMessage*>(outMessage);
    PEGASUS_TEST_ASSERT(outMessageRef != 0);

    PEGASUS_TEST_ASSERT(inMessage->getType() == outMessage->getType());
    PEGASUS_TEST_ASSERT(inMessage->messageId == outMessageRef->messageId);
    PEGASUS_TEST_ASSERT(inMessage->cimException.getCode() ==
           outMessageRef->cimException.getCode());
    PEGASUS_TEST_ASSERT(inMessage->cimException.getMessage() ==
           outMessageRef->cimException.getMessage());
    PEGASUS_TEST_ASSERT(inMessage->queueIds.size() == outMessageRef->queueIds.size());
    if (!inMessage->queueIds.isEmpty())
    {
        PEGASUS_TEST_ASSERT(inMessage->queueIds.top() == outMessageRef->queueIds.top());
    }

    PEGASUS_TEST_ASSERT(inMessage->cimObjects.size() == outMessageRef->cimObjects.size());

    for (Uint32 i = 0, n = outMessageRef->cimObjects.size(); i < n; i++)
    {
        if (inMessage->cimObjects[i].isUninitialized())
        {
            PEGASUS_TEST_ASSERT(outMessageRef->cimObjects[i].isUninitialized());
        }
        else
        {
            PEGASUS_TEST_ASSERT(inMessage->cimObjects[i].identical(outMessageRef->cimObjects[i]));
        }
    }
}

// 
// serializeDeserializeMessage
//
CIMMessage* serializeDeserializeMessage(CIMMessage* inMessage)
{
    Buffer outBuffer;
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

// 
// testCIMGetInstanceRequestMessage
//
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
    PEGASUS_TEST_ASSERT(outMessageRef != 0);

    PEGASUS_TEST_ASSERT(inMessage->getType() == outMessage->getType());
    PEGASUS_TEST_ASSERT(inMessage->messageId == outMessageRef->messageId);
    PEGASUS_TEST_ASSERT(inMessage->queueIds.size() == outMessageRef->queueIds.size());
    if (!inMessage->queueIds.isEmpty())
    {
        PEGASUS_TEST_ASSERT(inMessage->queueIds.top() == outMessageRef->queueIds.top());
    }
    PEGASUS_TEST_ASSERT(inMessage->nameSpace == outMessageRef->nameSpace);
    PEGASUS_TEST_ASSERT(inMessage->className == outMessageRef->className);
    PEGASUS_TEST_ASSERT(inMessage->providerType == outMessageRef->providerType);
    PEGASUS_TEST_ASSERT(inMessage->authType == outMessageRef->authType);
    PEGASUS_TEST_ASSERT(inMessage->userName == outMessageRef->userName);
    PEGASUS_TEST_ASSERT(inMessage->instanceName == outMessageRef->instanceName);
    PEGASUS_TEST_ASSERT(inMessage->instanceName == outMessageRef->instanceName);
    PEGASUS_TEST_ASSERT(inMessage->localOnly == outMessageRef->localOnly);
    PEGASUS_TEST_ASSERT(inMessage->includeQualifiers ==
           outMessageRef->includeQualifiers);
    PEGASUS_TEST_ASSERT(inMessage->includeClassOrigin ==
           outMessageRef->includeClassOrigin);
    PEGASUS_TEST_ASSERT((inMessage->propertyList.isNull() &&
            outMessageRef->propertyList.isNull()) ||
           (!inMessage->propertyList.isNull() &&
            !outMessageRef->propertyList.isNull()));
    PEGASUS_TEST_ASSERT(inMessage->propertyList.getPropertyNameArray() ==
           outMessageRef->propertyList.getPropertyNameArray());
}

// 
// testCIMGetInstanceResponseMessage
//
void testCIMGetInstanceResponseMessage(const CIMInstance& cimInstance)
{
    AutoPtr<CIMGetInstanceResponseMessage> inMessage;
    AutoPtr<CIMMessage> outMessage;

    //
    // Test instance with fully qualified object path
    //
    inMessage.reset(new CIMGetInstanceResponseMessage(
        "123",                              // messageId
        CIMException(CIM_ERR_NOT_SUPPORTED,
            "Unsupported operation"),       // cimException
        QueueIdStack(),                     // queueIds
        cimInstance));                      // cimInstance

    outMessage.reset(
	serializeDeserializeMessage(inMessage.get()));

    // verify result
    verifyGetInstanceResponseMessage(inMessage.get(), outMessage.get());

    //
    // Test instance with local object path
    //
    CIMInstance myinst = cimInstance.clone();
    CIMObjectPath mypath = cimInstance.getPath();
    mypath.setHost(String::EMPTY);
    mypath.setNameSpace(CIMNamespaceName());
    myinst.setPath(mypath);

    inMessage.reset(new CIMGetInstanceResponseMessage(
        "123",                              // messageId
        CIMException(CIM_ERR_NOT_SUPPORTED,
            "Unsupported operation"),       // cimException
        QueueIdStack(),                     // queueIds
	myinst));                           // cimInstance

    outMessage.reset(
        serializeDeserializeMessage(inMessage.get()));

    // Verify result
    verifyGetInstanceResponseMessage(inMessage.get(), outMessage.get());

    //
    // Test uninitialized instance
    //
    inMessage.reset(new CIMGetInstanceResponseMessage(
        "123",                              // messageId
        CIMException(CIM_ERR_NOT_SUPPORTED,
            "Unsupported operation"),       // cimException
        QueueIdStack(),                     // queueIds
        CIMInstance()));                    // cimInstance

    outMessage.reset(
        serializeDeserializeMessage(inMessage.get()));

    // Verify result
    verifyGetInstanceResponseMessage(inMessage.get(), outMessage.get()); 
}

// 
// testCIMAssociatorsRequestMessage
//
void testCIMAssociatorsRequestMessage()
{
    QueueIdStack queueIds;
    queueIds.push(10);
    queueIds.push(5);

    AutoPtr<CIMAssociatorsRequestMessage> inMessage;
    inMessage.reset(new CIMAssociatorsRequestMessage(
        "TestMessageID",                    // messageId
        CIMNamespaceName("/test/cimv2"),    // nameSpace
        CIMObjectPath("MyClass.key=1"),     // objectName
	CIMName(),                          // assocClass
	CIMName(),                          // resultClass
	String(String::EMPTY),              // role
	String(String::EMPTY),              // resultRole
        true,                               // includeQualifiers
        true,                               // includeClassOrigin
        CIMPropertyList(),                  // propertyList
        queueIds,                           // queueIds
        String("TestAuthType"),             // authType
        String("TestUserName")));           // userName

    AutoPtr<CIMMessage> outMessage(
        serializeDeserializeMessage(inMessage.get()));

    CIMAssociatorsRequestMessage* outMessageRef;
    outMessageRef =
        dynamic_cast<CIMAssociatorsRequestMessage*>(outMessage.get());
    PEGASUS_TEST_ASSERT(outMessageRef != 0);

    PEGASUS_TEST_ASSERT(inMessage->getType() == outMessage->getType());
    PEGASUS_TEST_ASSERT(inMessage->messageId == outMessageRef->messageId);
    PEGASUS_TEST_ASSERT(inMessage->queueIds.size() == outMessageRef->queueIds.size());
    if (!inMessage->queueIds.isEmpty())
    {
        PEGASUS_TEST_ASSERT(inMessage->queueIds.top() == outMessageRef->queueIds.top());
    }
    PEGASUS_TEST_ASSERT(inMessage->nameSpace == outMessageRef->nameSpace);
    PEGASUS_TEST_ASSERT(inMessage->className == outMessageRef->className);
    PEGASUS_TEST_ASSERT(inMessage->providerType == outMessageRef->providerType);
    PEGASUS_TEST_ASSERT(inMessage->authType == outMessageRef->authType);
    PEGASUS_TEST_ASSERT(inMessage->userName == outMessageRef->userName);
    PEGASUS_TEST_ASSERT(inMessage->objectName == outMessageRef->objectName);
    PEGASUS_TEST_ASSERT(inMessage->assocClass == outMessageRef->assocClass);
    PEGASUS_TEST_ASSERT(inMessage->resultClass == outMessageRef->resultClass);
    PEGASUS_TEST_ASSERT(inMessage->role == outMessageRef->role);
    PEGASUS_TEST_ASSERT(inMessage->resultRole == outMessageRef->resultRole);
    PEGASUS_TEST_ASSERT(inMessage->includeQualifiers ==
           outMessageRef->includeQualifiers);
    PEGASUS_TEST_ASSERT(inMessage->includeClassOrigin ==
           outMessageRef->includeClassOrigin);
    PEGASUS_TEST_ASSERT((inMessage->propertyList.isNull() &&
            outMessageRef->propertyList.isNull()) ||
           (!inMessage->propertyList.isNull() &&
            !outMessageRef->propertyList.isNull()));
    PEGASUS_TEST_ASSERT(inMessage->propertyList.getPropertyNameArray() ==
           outMessageRef->propertyList.getPropertyNameArray());
}

// 
// testCIMAssociatorsResponseMessage
//
void testCIMAssociatorsResponseMessage(const Array<CIMObject>& cimObjects)
{
    AutoPtr<CIMAssociatorsResponseMessage> inMessage;
    AutoPtr<CIMMessage> outMessage;

    //
    // Test instances with fully qualified object path
    //
    inMessage.reset(new CIMAssociatorsResponseMessage(
        "124",                              // messageId
        CIMException(CIM_ERR_NOT_SUPPORTED,
            "Unsupported operation"),       // cimException
        QueueIdStack(),                     // queueIds
        cimObjects));                       // cimObject

    outMessage.reset(
	serializeDeserializeMessage(inMessage.get()));

    // Verify result
    verifyAssociatorsResponseMessage(inMessage.get(), outMessage.get());

    //
    // Test instances with local object path
    //
    Array <CIMObject> myObjects;
    CIMObjectPath mypath;
    for (Uint32 i = 0, n = cimObjects.size(); i < n; i++)
    {
	CIMInstance myinstance = CIMInstance(cimObjects[i].clone()); 
        mypath = myinstance.getPath();
        mypath.setHost(String::EMPTY);
        mypath.setNameSpace(CIMNamespaceName());
        myinstance.setPath(mypath);
        myObjects.append(myinstance);
    }

    inMessage.reset(new CIMAssociatorsResponseMessage(
        "124",                              // messageId
        CIMException(CIM_ERR_NOT_SUPPORTED,
            "Unsupported operation"),       // cimException
        QueueIdStack(),                     // queueIds
        myObjects));                        // cimObject

    outMessage.reset(
	serializeDeserializeMessage(inMessage.get()));

    // Verify result
    verifyAssociatorsResponseMessage(inMessage.get(), outMessage.get());

    //
    // Test uninitialized instances
    //
    myObjects.clear();
    CIMInstance inst1 = CIMInstance();
    CIMInstance inst2 = CIMInstance();
    myObjects.append(inst1);
    myObjects.append(inst2);

    inMessage.reset(new CIMAssociatorsResponseMessage(
        "124",                              // messageId
        CIMException(CIM_ERR_NOT_SUPPORTED,
            "Unsupported operation"),       // cimException
        QueueIdStack(),                     // queueIds
        myObjects));                        // cimObject

    outMessage.reset(
	serializeDeserializeMessage(inMessage.get()));

    // Verify result
    verifyAssociatorsResponseMessage(inMessage.get(), outMessage.get());
}

// 
// main
//
int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    Array<CIMObject>  cimObjects = createInstances();

    try
    {
        testCIMGetInstanceRequestMessage();
        testCIMGetInstanceResponseMessage(CIMInstance(cimObjects[0]));

        testCIMAssociatorsRequestMessage();
        testCIMAssociatorsResponseMessage(cimObjects);
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
