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

#include <cctype>
#include <cstdio>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/Logger.h>
#include "CIMExportResponseEncoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportResponseEncoder::CIMExportResponseEncoder()
{

}

CIMExportResponseEncoder::~CIMExportResponseEncoder()
{

}

void CIMExportResponseEncoder::sendResponse(
    Uint32 queueId, 
    Array<Sint8>& message)
{
    MessageQueue* queue = MessageQueue::lookup(queueId);

    if (queue)
    {
	HTTPMessage* httpMessage = new HTTPMessage(message);
	queue->enqueue(httpMessage);
    }
}

void CIMExportResponseEncoder::sendError(
    Uint32 queueId, 
    const String& messageId,
    const String& cimMethodName,
    CIMStatusCode code,
    const String& description) 
{
    ArrayDestroyer<char> tmp1(cimMethodName.allocateCString());
    ArrayDestroyer<char> tmp2(description.allocateCString());

    Array<Sint8> message = XmlWriter::formatMethodResponseHeader(
	XmlWriter::formatMessageElement(
	    messageId,
	    XmlWriter::formatSimpleRspElement(
		XmlWriter::formatIMethodResponseElement(
		    tmp1.getPointer(),
		    XmlWriter::formatErrorElement(code, tmp2.getPointer())))));
    
    sendResponse(queueId, message);
}

void CIMExportResponseEncoder::sendError(
    CIMResponseMessage* response,
    const String& cimMethodName)
{
    Uint32 queueId = response->queueIds.top();
    response->queueIds.pop();

    sendError(
	queueId,
	response->messageId, 
	cimMethodName, 
	response->errorCode, 
	response->errorDescription);
}

void CIMExportResponseEncoder::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
	return;

    switch (message->getType())
    {
	case CIM_GET_CLASS_RESPONSE_MESSAGE:
	    encodeGetClassResponse(
		(CIMGetClassResponseMessage*)message);
	    break;
    }

    delete message;
}

const char* CIMExportResponseEncoder::getQueueName() const
{
    return "CIMExportResponseEncoder";
}

void CIMExportResponseEncoder::encodeGetClassResponse(
    CIMGetClassResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "GetClass");
	return;
    }

    Array<Sint8> body;
    response->cimClass.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetClass", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

PEGASUS_NAMESPACE_END
