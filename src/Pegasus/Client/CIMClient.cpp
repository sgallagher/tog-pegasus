//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TimeValue.h>
#include "CIMOperationResponseDecoder.h"
#include "CIMOperationRequestEncoder.h"
#include "CIMClient.h"

#include <iostream>
#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
# include <winsock.h>
#else
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMClient::CIMClient(
    Monitor* monitor,
    HTTPConnector* httpConnector,
    Uint32 timeOutMilliseconds)
    : 
    _monitor(monitor), 
    _httpConnector(httpConnector),
    _timeOutMilliseconds(timeOutMilliseconds),
    _connected(false),
    _responseDecoder(0),
    _requestEncoder(0)
{
    //
    // Create client authenticator
    //
    _authenticator = new ClientAuthenticator();    
}

CIMClient::~CIMClient()
{
   delete _authenticator;
}

void CIMClient::handleEnqueue()
{

}

const char* CIMClient::getQueueName() const
{
    return "CIMClient";
}

void CIMClient::connect(const String& address)
{
    // If already connected, bail out!
    
    if (_connected)
	throw AlreadyConnected();
    
    // Create response decoder:
    
    _responseDecoder = new CIMOperationResponseDecoder(
        this, _requestEncoder, _authenticator);
    
    // Attempt to establish a connection:
    
    HTTPConnection* httpConnection;
    
    try
    {
	httpConnection = _httpConnector->connect(address, _responseDecoder);
    }
    catch (Exception& e)
    {
	delete _responseDecoder;
	throw e;
    }
    
    // Create request encoder:
    
    _requestEncoder = new CIMOperationRequestEncoder(
        httpConnection, _authenticator);

    _responseDecoder->setEncoderQueue(_requestEncoder);
    
    _connected = true;
}

void CIMClient::connectLocal(
    const String& portNumber,
    const String& userName)
{
    String address = String::EMPTY;
    char * port = (char *)malloc(32);

    if (portNumber.size())
    {
        port = portNumber.allocateCString();
    }
    else
    {
        Uint32 portNum = System::lookupPort(WBEM_SERVICE_NAME, WBEM_DEFAULT_PORT);
        sprintf(port, "%u", portNum);
    }

    //
    // Build address string using local host name and port number
    //
    address.append(Cat(_getLocalHostName(), ":", port));

    free(port);

    //
    // Set authentication information
    //
    if (userName.size())
    {
        _authenticator->setUserName(userName);
    }
    _authenticator->setAuthType(ClientAuthenticator::LOCAL);

    connect(address);
}

CIMClass CIMClient::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    String messageId = XmlWriter::getNextMessageId();
    
    // encode request
    Message* request = new CIMGetClassRequestMessage(
	messageId,
	nameSpace,
	className,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_GET_CLASS_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);

    CIMGetClassResponseMessage* response = 
        (CIMGetClassResponseMessage*)message;
    
    Destroyer<CIMGetClassResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->cimClass);
}

CIMInstance CIMClient::getInstance(
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    String messageId = XmlWriter::getNextMessageId();
    
    // encode request
    Message* request = new CIMGetInstanceRequestMessage(
	messageId,
	nameSpace,
	instanceName,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_GET_INSTANCE_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMGetInstanceResponseMessage* response = 
        (CIMGetInstanceResponseMessage*)message;
    
    Destroyer<CIMGetInstanceResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->cimInstance);
}

void CIMClient::deleteClass(
    const String& nameSpace,
    const String& className)
{
    String messageId = XmlWriter::getNextMessageId();
    
    // encode request
    Message* request = new CIMDeleteClassRequestMessage(
	messageId,
	nameSpace,
	className,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_DELETE_CLASS_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMDeleteClassResponseMessage* response = 
        (CIMDeleteClassResponseMessage*)message;
    
    Destroyer<CIMDeleteClassResponseMessage> destroyer(response);
    
    _checkError(response);
}

void CIMClient::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMDeleteInstanceRequestMessage(
	messageId,
	nameSpace,
	instanceName,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_DELETE_INSTANCE_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMDeleteInstanceResponseMessage* response = 
        (CIMDeleteInstanceResponseMessage*)message;
    
    Destroyer<CIMDeleteInstanceResponseMessage> destroyer(response);
    
    _checkError(response);
}

void CIMClient::createClass(
    const String& nameSpace,
    const CIMClass& newClass)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMCreateClassRequestMessage(
	messageId,
	nameSpace,
	newClass,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_CREATE_CLASS_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMCreateClassResponseMessage* response = 
        (CIMCreateClassResponseMessage*)message;
    
    Destroyer<CIMCreateClassResponseMessage> destroyer(response);
    
    _checkError(response);
}

CIMReference CIMClient::createInstance(
    const String& nameSpace,
    const CIMInstance& newInstance)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMCreateInstanceRequestMessage(
	messageId,
	nameSpace,
	newInstance,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_CREATE_INSTANCE_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMCreateInstanceResponseMessage* response = 
        (CIMCreateInstanceResponseMessage*)message;
    
    Destroyer<CIMCreateInstanceResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->instanceName);
}

void CIMClient::modifyClass(
    const String& nameSpace,
    const CIMClass& modifiedClass)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMModifyClassRequestMessage(
	messageId,
	nameSpace,
	modifiedClass,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_MODIFY_CLASS_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMModifyClassResponseMessage* response = 
        (CIMModifyClassResponseMessage*)message;
    
    Destroyer<CIMModifyClassResponseMessage> destroyer(response);
    
    _checkError(response);
}

void CIMClient::modifyInstance(
    const String& nameSpace,
    const CIMNamedInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMModifyInstanceRequestMessage(
	messageId,
	nameSpace,
	modifiedInstance,
	includeQualifiers,
	propertyList,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMModifyInstanceResponseMessage* response = 
        (CIMModifyInstanceResponseMessage*)message;
    
    Destroyer<CIMModifyInstanceResponseMessage> destroyer(response);
    
    _checkError(response);
}

Array<CIMClass> CIMClient::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMEnumerateClassesRequestMessage(
	messageId,
	nameSpace,
	className,
	deepInheritance,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE, messageId,
        _timeOutMilliseconds);
    
    CIMEnumerateClassesResponseMessage* response = 
        (CIMEnumerateClassesResponseMessage*)message;
    
    Destroyer<CIMEnumerateClassesResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->cimClasses);
}

Array<String> CIMClient::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMEnumerateClassNamesRequestMessage(
	messageId,
	nameSpace,
	className,
	deepInheritance,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE, messageId,
        _timeOutMilliseconds);
    
    CIMEnumerateClassNamesResponseMessage* response = 
        (CIMEnumerateClassNamesResponseMessage*)message;
    
    Destroyer<CIMEnumerateClassNamesResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->classNames);
}

Array<CIMNamedInstance> CIMClient::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMEnumerateInstancesRequestMessage(
	messageId,
	nameSpace,
	className,
	deepInheritance,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE, messageId,
        _timeOutMilliseconds);
    
    CIMEnumerateInstancesResponseMessage* response = 
        (CIMEnumerateInstancesResponseMessage*)message;
    
    Destroyer<CIMEnumerateInstancesResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->cimNamedInstances);
}

Array<CIMReference> CIMClient::enumerateInstanceNames(
    const String& nameSpace,
    const String& className)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMEnumerateInstanceNamesRequestMessage(
	messageId,
	nameSpace,
	className,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE, messageId,
        _timeOutMilliseconds);
    
    CIMEnumerateInstanceNamesResponseMessage* response = 
        (CIMEnumerateInstanceNamesResponseMessage*)message;
    
    Destroyer<CIMEnumerateInstanceNamesResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->instanceNames);
}

Array<CIMInstance> CIMClient::execQuery(
    const String& queryLanguage,
    const String& query)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMExecQueryRequestMessage(
	messageId,
	queryLanguage,
	query,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_EXEC_QUERY_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMExecQueryResponseMessage* response = 
        (CIMExecQueryResponseMessage*)message;
    
    Destroyer<CIMExecQueryResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->cimInstances);
}

Array<CIMObjectWithPath> CIMClient::associators(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMAssociatorsRequestMessage(
	messageId,
	nameSpace,
	objectName,
	assocClass,
	resultClass,
	role,
	resultRole,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_ASSOCIATORS_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMAssociatorsResponseMessage* response = 
        (CIMAssociatorsResponseMessage*)message;
    
    Destroyer<CIMAssociatorsResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->cimObjects);
}

Array<CIMReference> CIMClient::associatorNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMAssociatorNamesRequestMessage(
	messageId,
	nameSpace,
	objectName,
	assocClass,
	resultClass,
	role,
	resultRole,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMAssociatorNamesResponseMessage* response = 
        (CIMAssociatorNamesResponseMessage*)message;
    
    Destroyer<CIMAssociatorNamesResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->objectNames);
}

Array<CIMObjectWithPath> CIMClient::references(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMReferencesRequestMessage(
	messageId,
	nameSpace,
	objectName,
	resultClass,
	role,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_REFERENCES_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMReferencesResponseMessage* response = 
        (CIMReferencesResponseMessage*)message;
    
    Destroyer<CIMReferencesResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->cimObjects);
}

Array<CIMReference> CIMClient::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMReferenceNamesRequestMessage(
	messageId,
	nameSpace,
	objectName,
	resultClass,
	role,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_REFERENCE_NAMES_RESPONSE_MESSAGE, messageId,
        _timeOutMilliseconds);
    
    CIMReferenceNamesResponseMessage* response = 
        (CIMReferenceNamesResponseMessage*)message;
    
    Destroyer<CIMReferenceNamesResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->objectNames);
}

CIMValue CIMClient::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMGetPropertyRequestMessage(
	messageId,
	nameSpace,
	instanceName,
	propertyName,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_GET_PROPERTY_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMGetPropertyResponseMessage* response = 
        (CIMGetPropertyResponseMessage*)message;
    
    Destroyer<CIMGetPropertyResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->value);
}

void CIMClient::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMSetPropertyRequestMessage(
	messageId,
	nameSpace,
	instanceName,
	propertyName,
	newValue,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_SET_PROPERTY_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMSetPropertyResponseMessage* response = 
        (CIMSetPropertyResponseMessage*)message;
    
    Destroyer<CIMSetPropertyResponseMessage> destroyer(response);
    
    _checkError(response);
}

CIMQualifierDecl CIMClient::getQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMGetQualifierRequestMessage(
	messageId,
	nameSpace,
	qualifierName,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_GET_QUALIFIER_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMGetQualifierResponseMessage* response = 
        (CIMGetQualifierResponseMessage*)message;
    
    Destroyer<CIMGetQualifierResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->cimQualifierDecl);
}

void CIMClient::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDeclaration)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMSetQualifierRequestMessage(
	messageId,
	nameSpace,
	qualifierDeclaration,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_SET_QUALIFIER_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMSetQualifierResponseMessage* response = 
        (CIMSetQualifierResponseMessage*)message;
    
    Destroyer<CIMSetQualifierResponseMessage> destroyer(response);
    
    _checkError(response);
}

void CIMClient::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMDeleteQualifierRequestMessage(
	messageId,
	nameSpace,
	qualifierName,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMDeleteQualifierResponseMessage* response = 
        (CIMDeleteQualifierResponseMessage*)message;
    
    Destroyer<CIMDeleteQualifierResponseMessage> destroyer(response);
    
    _checkError(response);
}

Array<CIMQualifierDecl> CIMClient::enumerateQualifiers(
    const String& nameSpace)
{
    String messageId = XmlWriter::getNextMessageId();
    
    Message* request = new CIMEnumerateQualifiersRequestMessage(
	messageId,
	nameSpace,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE, messageId,
        _timeOutMilliseconds);
    
    CIMEnumerateQualifiersResponseMessage* response = 
        (CIMEnumerateQualifiersResponseMessage*)message;
    
    Destroyer<CIMEnumerateQualifiersResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->qualifierDeclarations);
}

CIMValue CIMClient::invokeMethod(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters)
{
    String messageId = XmlWriter::getNextMessageId();

    // ATTN-RK-P2-20020301: Does it make sense to have a nameSpace parameter
    // when the namespace should already be included in the instanceName?
    // ATTN-RK-P3-20020301: Do we need to make sure the caller didn't specify
    // a host name in the instanceName?

    Message* request = new CIMInvokeMethodRequestMessage(
	messageId,
	nameSpace,
	instanceName,
	methodName,
	inParameters,
	QueueIdStack());
    
    _authenticator->clearRequest();

    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_INVOKE_METHOD_RESPONSE_MESSAGE, messageId, _timeOutMilliseconds);
    
    CIMInvokeMethodResponseMessage* response = 
        (CIMInvokeMethodResponseMessage*)message;
    
    Destroyer<CIMInvokeMethodResponseMessage> destroyer(response);
    
    _checkError(response);
    
    outParameters = response->outParameters;
    
    return(response->retValue);
}

Message* CIMClient::_waitForResponse(
    const Uint32 messageType,
    const String& messageId,
    const Uint32 timeOutMilliseconds)
{
    if (!_connected)
	throw NotConnected();
    
    long rem = long(timeOutMilliseconds);

    for (;;)
    {
	//
	// Wait until the timeout expires or an event occurs:
	//

	TimeValue start = TimeValue::getCurrentTime();
	_monitor->run(rem);
	TimeValue stop = TimeValue::getCurrentTime();

	//
	// Check to see if incoming queue has a message of the appropriate
	// type with the given message id:
	//

	Message* message = findByType(messageType);

	if (message)
	{
	    CIMResponseMessage* responseMessage = (CIMResponseMessage*)message;

	    if (responseMessage->messageId == messageId)
	    {
		remove(responseMessage);
		return responseMessage;
	    }
	}

	// 
	// Terminate loop if timed out:
	//

	long diff = stop.toMilliseconds() - start.toMilliseconds();

	if (diff >= rem)
	    break;

	rem -= diff;
    }

    //
    // Throw timed out exception:
    //

    throw TimedOut();
}

void CIMClient::_checkError(const CIMResponseMessage* responseMessage)
{
    if (responseMessage && (responseMessage->errorCode != CIM_ERR_SUCCESS))
    {
	throw CIMException(responseMessage->errorCode, 
	    __FILE__, __LINE__, responseMessage->errorDescription);
    }
}

String CIMClient::_getLocalHostName()
{
    static String hostname;

    if (!hostname.size())
    {
#ifdef PEGASUS_ACCEPT_ONLY_LOCAL_CONNECTIONS
        struct sockaddr_in address;

        memset(&address, 0, sizeof(address));

        address.sin_addr.s_addr = INADDR_LOOPBACK;
        hostname.assign(inet_ntoa(address.sin_addr));
#else
        hostname.assign(System::getHostName());
#endif
    }

    return hostname;
}

PEGASUS_NAMESPACE_END
