//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/Exception.h>
#include "CIMExportRequestEncoder.h"
#include "CIMExportResponseDecoder.h"
#include "CIMExportClient.h"

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportClient::CIMExportClient(
    Monitor* monitor,
    HTTPConnector* httpConnector,
    Uint32 timeOutMilliseconds)
    : 
    _connected(false),
    _monitor(monitor), 
    _httpConnector(httpConnector),
    _timeOutMilliseconds(timeOutMilliseconds),
    _requestEncoder(0),
    _responseDecoder(0)
{
    
}

CIMExportClient::~CIMExportClient()
{
    
}

void CIMExportClient::handleEnqueue()
{

}

const char* CIMExportClient::getQueueName() const
{
    return "CIMExportClient";
}

void CIMExportClient::connect(const String& address)
{
    // If already connected, bail out!
    
    if (_connected)
	throw AlreadyConnected();
    
    // Create response decoder:
    
    _responseDecoder = new CIMExportResponseDecoder(this);
    
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
    
    _requestEncoder = new CIMExportRequestEncoder(httpConnection);
    
    _connected = true;
}

CIMClass CIMExportClient::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
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
    
    _requestEncoder->enqueue(request);
    
    Message* message = _waitForResponse(
        CIM_GET_CLASS_RESPONSE_MESSAGE, messageId);

    CIMGetClassResponseMessage* response = 
        (CIMGetClassResponseMessage*)message;
    
    Destroyer<CIMGetClassResponseMessage> destroyer(response);
    
    _checkError(response);
    
    return(response->cimClass);
}

Message* CIMExportClient::_waitForResponse(
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

void CIMExportClient::_checkError(const CIMResponseMessage* responseMessage)
{
    if (responseMessage && (responseMessage->errorCode != CIM_ERR_SUCCESS))
    {
	throw CIMException(responseMessage->errorCode, 
	    __FILE__, __LINE__, responseMessage->errorDescription);
    }
}

PEGASUS_NAMESPACE_END
