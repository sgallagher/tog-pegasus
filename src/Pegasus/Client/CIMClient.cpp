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
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/PegasusVersion.h>

#include "CIMOperationResponseDecoder.h"
#include "CIMOperationRequestEncoder.h"
#include "CIMClient.h"

#include <iostream>
#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
# include <windows.h>
#else
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMClient::CIMClient(Uint32 timeOutMilliseconds)
    : 
    MessageQueue(PEGASUS_QUEUENAME_CLIENT),
    _httpConnection(0),
    _timeOutMilliseconds(timeOutMilliseconds),
    _connected(false),
    _responseDecoder(0),
    _requestEncoder(0)
{
    //
    // Create Monitor and HTTPConnector
    //
    _monitor = new Monitor();
    _httpConnector = new HTTPConnector(_monitor);
}

CIMClient::~CIMClient()
{
   disconnect();
   delete _httpConnector;
   delete _monitor;
}

void CIMClient::handleEnqueue()
{

}

void CIMClient::_connect(
    const String& address,
    SSLContext* sslContext
) throw(CIMClientException)
{
    //
    // Create response decoder:
    //
    _responseDecoder = new CIMOperationResponseDecoder(
        this, _requestEncoder, &_authenticator);
    
    //
    // Attempt to establish a connection:
    //
    try
    {
	_httpConnection = _httpConnector->connect(address,
                                                  sslContext,
                                                  _responseDecoder);
    }
    catch (CannotCreateSocket& e)
    {
        delete _responseDecoder;
        throw CIMClientCannotCreateSocketException(e.getMessage());
    }
    catch (CannotConnect& e)
    {
        delete _responseDecoder;
        throw CIMClientCannotConnectException(e.getMessage());
    }
    catch (InvalidLocator& e)
    {
        delete _responseDecoder;
        throw CIMClientInvalidLocatorException(e.getMessage());
    }
    catch (UnexpectedFailure& e)
    {
        delete _responseDecoder;
        throw CIMClientConnectionException(e.getMessage());
    }
    
    //
    // Create request encoder:
    //
    _requestEncoder = new CIMOperationRequestEncoder(
        _httpConnection, &_authenticator);

    _responseDecoder->setEncoderQueue(_requestEncoder);

    _connected = true;
}

void CIMClient::connect(
    const String& address,
    SSLContext* sslContext,
    const String& userName,
    const String& password
) throw(CIMClientException)
{
    //
    // If already connected, bail out!
    //
    if (_connected)
	throw CIMClientAlreadyConnectedException();

    //
    // If the address is empty, reject it
    //
    if (address == String::EMPTY)
	throw CIMClientInvalidLocatorException(
            InvalidLocator(address).getMessage());

    //
    // Set authentication information
    //
    _authenticator.clearRequest(true);
    _authenticator.setAuthType(ClientAuthenticator::NONE);

    if (userName.size())
    {
        _authenticator.setUserName(userName);
    }

    if (password.size())
    {
        _authenticator.setPassword(password);
    }

    _connect(address, sslContext);
}


void CIMClient::connectLocal(SSLContext* sslContext) throw(CIMClientException)
{
    //
    // If already connected, bail out!
    //
    if (_connected)
	throw CIMClientAlreadyConnectedException();

    String address;

#ifndef PEGASUS_LOCAL_DOMAIN_SOCKET
    //
    // Look up the WBEM port number for the local system
    //
    Uint32 portNum = System::lookupPort(WBEM_SERVICE_NAME, WBEM_DEFAULT_PORT);
    char port[32];
    sprintf(port, "%u", portNum);

    //
    // Build address string using local host name and port number
    //
    address.append(_getLocalHostName());
    address.append(":");
    address.append(port);
#endif

    //
    // Set authentication type
    //
    _authenticator.clearRequest(true);
    _authenticator.setAuthType(ClientAuthenticator::LOCAL);

    _connect(address, sslContext);
}

void CIMClient::disconnect()
{
    if (_connected)
    {
        //
        // destroy response decoder
        //
        if (_responseDecoder)
        {
            delete _responseDecoder;
            _responseDecoder = 0;
        }

        //
        // Close the connection
        //
        _httpConnector->disconnect(_httpConnection);

        //
        // destroy request encoder
        //
        if (_requestEncoder)
        {
            delete _requestEncoder;
            _requestEncoder = 0;
        }

        _authenticator.clearRequest(true);

        _connected = false;
    }
}


CIMClass CIMClient::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMGetClassRequestMessage(
	String::EMPTY,
	nameSpace,
	className,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_GET_CLASS_RESPONSE_MESSAGE);

    CIMGetClassResponseMessage* response = 
        (CIMGetClassResponseMessage*)message;
    
    Destroyer<CIMGetClassResponseMessage> destroyer(response);
    
    return(response->cimClass);
}

CIMInstance CIMClient::getInstance(
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMGetInstanceRequestMessage(
	String::EMPTY,
	nameSpace,
	instanceName,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_GET_INSTANCE_RESPONSE_MESSAGE);
    
    CIMGetInstanceResponseMessage* response = 
        (CIMGetInstanceResponseMessage*)message;
    
    Destroyer<CIMGetInstanceResponseMessage> destroyer(response);
    
    return(response->cimInstance);
}

void CIMClient::deleteClass(
    const String& nameSpace,
    const String& className
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMDeleteClassRequestMessage(
	String::EMPTY,
	nameSpace,
	className,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_DELETE_CLASS_RESPONSE_MESSAGE);
    
    CIMDeleteClassResponseMessage* response = 
        (CIMDeleteClassResponseMessage*)message;
    
    Destroyer<CIMDeleteClassResponseMessage> destroyer(response);
}

void CIMClient::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMDeleteInstanceRequestMessage(
	String::EMPTY,
	nameSpace,
	instanceName,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_DELETE_INSTANCE_RESPONSE_MESSAGE);
    
    CIMDeleteInstanceResponseMessage* response = 
        (CIMDeleteInstanceResponseMessage*)message;
    
    Destroyer<CIMDeleteInstanceResponseMessage> destroyer(response);
}

void CIMClient::createClass(
    const String& nameSpace,
    const CIMClass& newClass
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMCreateClassRequestMessage(
	String::EMPTY,
	nameSpace,
	newClass,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_CREATE_CLASS_RESPONSE_MESSAGE);
    
    CIMCreateClassResponseMessage* response = 
        (CIMCreateClassResponseMessage*)message;
    
    Destroyer<CIMCreateClassResponseMessage> destroyer(response);
}

CIMReference CIMClient::createInstance(
    const String& nameSpace,
    const CIMInstance& newInstance
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMCreateInstanceRequestMessage(
	String::EMPTY,
	nameSpace,
	newInstance,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_CREATE_INSTANCE_RESPONSE_MESSAGE);
    
    CIMCreateInstanceResponseMessage* response = 
        (CIMCreateInstanceResponseMessage*)message;
    
    Destroyer<CIMCreateInstanceResponseMessage> destroyer(response);
    
    return(response->instanceName);
}

void CIMClient::modifyClass(
    const String& nameSpace,
    const CIMClass& modifiedClass
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMModifyClassRequestMessage(
	String::EMPTY,
	nameSpace,
	modifiedClass,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_MODIFY_CLASS_RESPONSE_MESSAGE);
    
    CIMModifyClassResponseMessage* response = 
        (CIMModifyClassResponseMessage*)message;
    
    Destroyer<CIMModifyClassResponseMessage> destroyer(response);
}

void CIMClient::modifyInstance(
    const String& nameSpace,
    const CIMNamedInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMModifyInstanceRequestMessage(
	String::EMPTY,
	nameSpace,
	modifiedInstance,
	includeQualifiers,
	propertyList,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE);
    
    CIMModifyInstanceResponseMessage* response = 
        (CIMModifyInstanceResponseMessage*)message;
    
    Destroyer<CIMModifyInstanceResponseMessage> destroyer(response);
}

Array<CIMClass> CIMClient::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMEnumerateClassesRequestMessage(
	String::EMPTY,
	nameSpace,
	className,
	deepInheritance,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE);
    
    CIMEnumerateClassesResponseMessage* response = 
        (CIMEnumerateClassesResponseMessage*)message;
    
    Destroyer<CIMEnumerateClassesResponseMessage> destroyer(response);
    
    return(response->cimClasses);
}

Array<String> CIMClient::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMEnumerateClassNamesRequestMessage(
	String::EMPTY,
	nameSpace,
	className,
	deepInheritance,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE);
    
    CIMEnumerateClassNamesResponseMessage* response = 
        (CIMEnumerateClassNamesResponseMessage*)message;
    
    Destroyer<CIMEnumerateClassNamesResponseMessage> destroyer(response);
    
    return(response->classNames);
}

Array<CIMNamedInstance> CIMClient::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMEnumerateInstancesRequestMessage(
	String::EMPTY,
	nameSpace,
	className,
	deepInheritance,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE);
    
    CIMEnumerateInstancesResponseMessage* response = 
        (CIMEnumerateInstancesResponseMessage*)message;
    
    Destroyer<CIMEnumerateInstancesResponseMessage> destroyer(response);
    
    return(response->cimNamedInstances);
}

Array<CIMReference> CIMClient::enumerateInstanceNames(
    const String& nameSpace,
    const String& className
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMEnumerateInstanceNamesRequestMessage(
	String::EMPTY,
	nameSpace,
	className,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE);
    
    CIMEnumerateInstanceNamesResponseMessage* response = 
        (CIMEnumerateInstanceNamesResponseMessage*)message;
    
    Destroyer<CIMEnumerateInstanceNamesResponseMessage> destroyer(response);
    
    return(response->instanceNames);
}

Array<CIMObjectWithPath> CIMClient::execQuery(
    const String& nameSpace,
    const String& queryLanguage,
    const String& query
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMExecQueryRequestMessage(
	String::EMPTY,
	nameSpace,
	queryLanguage,
	query,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_EXEC_QUERY_RESPONSE_MESSAGE);
    
    CIMExecQueryResponseMessage* response = 
        (CIMExecQueryResponseMessage*)message;
    
    Destroyer<CIMExecQueryResponseMessage> destroyer(response);
    
    return(response->cimObjects);
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
    const CIMPropertyList& propertyList
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMAssociatorsRequestMessage(
	String::EMPTY,
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
    
    Message* message = _doRequest(request, CIM_ASSOCIATORS_RESPONSE_MESSAGE);
    
    CIMAssociatorsResponseMessage* response = 
        (CIMAssociatorsResponseMessage*)message;
    
    Destroyer<CIMAssociatorsResponseMessage> destroyer(response);
    
    return(response->cimObjects);
}

Array<CIMReference> CIMClient::associatorNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMAssociatorNamesRequestMessage(
	String::EMPTY,
	nameSpace,
	objectName,
	assocClass,
	resultClass,
	role,
	resultRole,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE);
    
    CIMAssociatorNamesResponseMessage* response = 
        (CIMAssociatorNamesResponseMessage*)message;
    
    Destroyer<CIMAssociatorNamesResponseMessage> destroyer(response);
    
    return(response->objectNames);
}

Array<CIMObjectWithPath> CIMClient::references(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMReferencesRequestMessage(
	String::EMPTY,
	nameSpace,
	objectName,
	resultClass,
	role,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_REFERENCES_RESPONSE_MESSAGE);
    
    CIMReferencesResponseMessage* response = 
        (CIMReferencesResponseMessage*)message;
    
    Destroyer<CIMReferencesResponseMessage> destroyer(response);
    
    return(response->cimObjects);
}

Array<CIMReference> CIMClient::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMReferenceNamesRequestMessage(
	String::EMPTY,
	nameSpace,
	objectName,
	resultClass,
	role,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_REFERENCE_NAMES_RESPONSE_MESSAGE);
    
    CIMReferenceNamesResponseMessage* response = 
        (CIMReferenceNamesResponseMessage*)message;
    
    Destroyer<CIMReferenceNamesResponseMessage> destroyer(response);
    
    return(response->objectNames);
}

CIMValue CIMClient::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMGetPropertyRequestMessage(
	String::EMPTY,
	nameSpace,
	instanceName,
	propertyName,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_GET_PROPERTY_RESPONSE_MESSAGE);
    
    CIMGetPropertyResponseMessage* response = 
        (CIMGetPropertyResponseMessage*)message;
    
    Destroyer<CIMGetPropertyResponseMessage> destroyer(response);
    
    return(response->value);
}

void CIMClient::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMSetPropertyRequestMessage(
	String::EMPTY,
	nameSpace,
	instanceName,
	propertyName,
	newValue,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_SET_PROPERTY_RESPONSE_MESSAGE);
    
    CIMSetPropertyResponseMessage* response = 
        (CIMSetPropertyResponseMessage*)message;
    
    Destroyer<CIMSetPropertyResponseMessage> destroyer(response);
}

CIMQualifierDecl CIMClient::getQualifier(
    const String& nameSpace,
    const String& qualifierName
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMGetQualifierRequestMessage(
	String::EMPTY,
	nameSpace,
	qualifierName,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_GET_QUALIFIER_RESPONSE_MESSAGE);
    
    CIMGetQualifierResponseMessage* response = 
        (CIMGetQualifierResponseMessage*)message;
    
    Destroyer<CIMGetQualifierResponseMessage> destroyer(response);
    
    return(response->cimQualifierDecl);
}

void CIMClient::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDeclaration
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMSetQualifierRequestMessage(
	String::EMPTY,
	nameSpace,
	qualifierDeclaration,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_SET_QUALIFIER_RESPONSE_MESSAGE);
    
    CIMSetQualifierResponseMessage* response = 
        (CIMSetQualifierResponseMessage*)message;
    
    Destroyer<CIMSetQualifierResponseMessage> destroyer(response);
}

void CIMClient::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMDeleteQualifierRequestMessage(
	String::EMPTY,
	nameSpace,
	qualifierName,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE);
    
    CIMDeleteQualifierResponseMessage* response = 
        (CIMDeleteQualifierResponseMessage*)message;
    
    Destroyer<CIMDeleteQualifierResponseMessage> destroyer(response);
}

Array<CIMQualifierDecl> CIMClient::enumerateQualifiers(
    const String& nameSpace
) throw(CIMClientException)
{
    CIMRequestMessage* request = new CIMEnumerateQualifiersRequestMessage(
	String::EMPTY,
	nameSpace,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE);
    
    CIMEnumerateQualifiersResponseMessage* response = 
        (CIMEnumerateQualifiersResponseMessage*)message;
    
    Destroyer<CIMEnumerateQualifiersResponseMessage> destroyer(response);
    
    return(response->qualifierDeclarations);
}

CIMValue CIMClient::invokeMethod(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters
) throw(CIMClientException)
{
    // ATTN-RK-P2-20020301: Does it make sense to have a nameSpace parameter
    // when the namespace should already be included in the instanceName?
    // ATTN-RK-P3-20020301: Do we need to make sure the caller didn't specify
    // a host name in the instanceName?

    CIMRequestMessage* request = new CIMInvokeMethodRequestMessage(
	String::EMPTY,
	nameSpace,
	instanceName,
	methodName,
	inParameters,
	QueueIdStack());
    
    Message* message = _doRequest(request, CIM_INVOKE_METHOD_RESPONSE_MESSAGE);
    
    CIMInvokeMethodResponseMessage* response = 
        (CIMInvokeMethodResponseMessage*)message;
    
    Destroyer<CIMInvokeMethodResponseMessage> destroyer(response);
    
    outParameters = response->outParameters;
    
    return(response->retValue);
}

Message* CIMClient::_doRequest(
    CIMRequestMessage * request,
    const Uint32 expectedResponseMessageType
) throw(CIMClientException)
{
    if (!_connected)
    {
        delete request;
	throw CIMClientNotConnectedException();
    }
    
    String messageId = XmlWriter::getNextMessageId();
    const_cast<String &>(request->messageId) = messageId;

    _authenticator.clearRequest();

    // ATTN-RK-P2-20020416: We should probably clear out the queue first.
    PEGASUS_ASSERT(getCount() == 0);  // Shouldn't be any messages in our queue

    _requestEncoder->enqueue(request);

    Uint64 startMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
    Uint64 nowMilliseconds = startMilliseconds;
    Uint64 stopMilliseconds = nowMilliseconds + _timeOutMilliseconds;

    while (nowMilliseconds < stopMilliseconds)
    {
	//
	// Wait until the timeout expires or an event occurs:
	//

	_monitor->run(Uint32(stopMilliseconds - nowMilliseconds));

	//
	// Check to see if incoming queue has a message
	//

	Message* response = dequeue();

	if (response)
	{
            // Shouldn't be any more messages in our queue
            PEGASUS_ASSERT(getCount() == 0);

            if (response->getType() == CLIENT_EXCEPTION_MESSAGE)
            {
                CIMClientException* clientException =
                    ((ClientExceptionMessage*)response)->clientException;
                delete response;
                Destroyer<CIMClientException> d(clientException);
                throw *clientException;
            }
            else if (response->getType() == expectedResponseMessageType)
            {
                CIMResponseMessage* cimResponse = (CIMResponseMessage*)response;
                if (cimResponse->messageId != messageId)
                {
                    CIMClientResponseException responseException(
                        String("Mismatched response message ID:  Got \"") +
                        cimResponse->messageId + "\", expected \"" +
                        messageId + "\".");
                    delete response;
	            throw responseException;
                }
                if (cimResponse->cimException.getCode() != CIM_ERR_SUCCESS)
                {
                    CIMClientCIMException cimException(
                        cimResponse->cimException.getCode(),
                        cimResponse->cimException.getMessage());
                    delete response;
	            throw cimException;
                }
                return response;
            }
            else
            {
                CIMClientResponseException responseException(
                    "Mismatched response message type.");
                delete response;
	        throw responseException;
            }
	}

        nowMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
    }

    //
    // Throw timed out exception:
    //

    throw CIMClientTimeoutException();
}

String CIMClient::_getLocalHostName()
{
    static String hostname;

    if (!hostname.size())
    {
        hostname.assign(System::getHostName());
    }

    return hostname;
}

PEGASUS_NAMESPACE_END
