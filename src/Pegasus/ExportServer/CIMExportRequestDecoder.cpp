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
#include <Pegasus/Common/Logger.h>
#include "CIMExportRequestDecoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportRequestDecoder::CIMExportRequestDecoder(
    MessageQueue* outputQueue,
    Uint32 returnQueueId)
    : 
    _outputQueue(outputQueue),
    _returnQueueId(returnQueueId)
{

}

CIMExportRequestDecoder::~CIMExportRequestDecoder()
{

}

void CIMExportRequestDecoder::sendResponse(
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

void CIMExportRequestDecoder::sendError(
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

void CIMExportRequestDecoder::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
	return;

    switch (message->getType())
    {
	case HTTP_MESSAGE:
	    handleHTTPMessage((HTTPMessage*)message);
	    break;
    }

    delete message;
}

const char* CIMExportRequestDecoder::getQueueName() const
{
    return "CIMExportRequestDecoder";
}

//------------------------------------------------------------------------------
//
// From the HTTP/1.1 Specification (RFC 2626):
//
// Both types of message consist of a start-line, zero or more header fields 
// (also known as "headers"), an empty line (i.e., a line with nothing 
// preceding the CRLF) indicating the end of the header fields, and possibly 
// a message-body.
//
// Example CIM request:
//
//     M-POST /cimom HTTP/1.1 
//     HOST: www.erewhon.com 
//     Content-Type: application/xml; charset="utf-8" 
//     Content-Length: xxxx 
//     Man: http://www.dmtf.org/cim/operation ; ns=73 
//     73-CIMExport: MethodCall 
//     73-CIMMethod: EnumerateInstances 
//     73-CIMObject: root/cimv2 
// 
//------------------------------------------------------------------------------

void CIMExportRequestDecoder::handleHTTPMessage(HTTPMessage* httpMessage)
{
    // Save queueId:

    Uint32 queueId = httpMessage->queueId;

    // Parse the HTTP message:

    String startLine;
    Array<HTTPHeader> headers;
    Sint8* content;
    Uint32 contentLength;

    httpMessage->parse(startLine, headers, content, contentLength);

    // Parse the request line:

    String methodName;
    String requestUri;
    String httpVersion;

    HTTPMessage::parseRequestLine(
        startLine, methodName, requestUri, httpVersion);

    // Process M-POST and POST messages:


    if (methodName == "M-POST" || methodName == "POST")
    {
	// Search for "CIMOperation" header:

	String cimOperation;

	if (!HTTPMessage::lookupHeader(
	    headers, "*CIMOperation", cimOperation, true))
	{
	    // ATTN: error discarded at this time!
	    return;
	}

	// Zero-terminate the message:

	httpMessage->message.append('\0');

	// If it is a method call, then dispatch it to be handled:

	if (!String::equalNoCase(cimOperation, "MethodCall"))
	{
	    // ATTN: error discarded at this time!
	    return;
	}

	handleMethodCall(queueId, content);
    }
}


void CIMExportRequestDecoder::handleMethodCall(
    Uint32 queueId,
    Sint8* content)
{
    // Create a parser:

    XmlParser parser(content);
    XmlEntry entry;
    String messageId;
    const char* cimMethodName = "";

    try
    {
	// Expect <?xml ...>

	XmlReader::expectXmlDeclaration(parser, entry);

	// Expect <CIM ...>

	XmlReader::testCimStartTag(parser);

	// Expect <MESSAGE ...>

	const char* protocolVersion = 0;

	if (!XmlReader::getMessageStartTag(
	    parser, messageId, protocolVersion))
	{
	    throw XmlValidationError(
		parser.getLine(), "expected MESSAGE element");
	}

	if (strcmp(protocolVersion, "1.0") != 0)
	{
	    throw XmlSemanticError(parser.getLine(), 
		"Expected MESSAGE.PROTOCOLVERSION to be \"1.0\"");
	}
    }
    catch (Exception&)
    {
	// ATTN: no error can be sent back to the client yet since
	// errors require a message-id (which was in the process of
	// being obtained above).
	return;
    }

    try
    {
	// Expect <SIMPLEREQ ...>

	XmlReader::expectStartTag(parser, entry, "SIMPLEREQ");

	// Expect <IMETHODCALL ...>

	if (!XmlReader::getIMethodCallStartTag(parser, cimMethodName))
	{
	    throw XmlValidationError(parser.getLine(), 
		"expected IMETHODCALL element");
	}

	// Expect <LOCALNAMESPACEPATH ...>

	String nameSpace;

	if (!XmlReader::getLocalNameSpacePathElement(parser, nameSpace))
	{
	    throw XmlValidationError(parser.getLine(), 
		"expected LOCALNAMESPACEPATH element");
	}

	// Delegate to appropriate method to handle:

	if (CompareNoCase(cimMethodName, "GetClass") == 0)
	    decodeGetClassRequest(queueId, parser, messageId, nameSpace);
	else
	{
	    String description = "Unknown intrinsic method: ";
	    description += cimMethodName;

	    sendError(
		queueId, 
		messageId,
		cimMethodName,
		CIM_ERR_FAILED,
		description);

	    return;
	}

	// Expect </IMETHODCALL>

	XmlReader::expectEndTag(parser, "IMETHODCALL");

	// Expect </SIMPLEREQ>

	XmlReader::expectEndTag(parser, "SIMPLEREQ");

	// Expect </MESSAGE>

	XmlReader::expectEndTag(parser, "MESSAGE");

	// Expect </CIM>

	XmlReader::expectEndTag(parser, "CIM");
    }
    catch (Exception& e)
    {
	sendError(
	    queueId, 
	    messageId,
	    cimMethodName,
	    CIM_ERR_FAILED,
	    e.getMessage());
    }
}

void CIMExportRequestDecoder::decodeGetClassRequest(
    Uint32 queueId,
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // ATTN: handle property lists!

    String className;
    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className, true);
	else if (CompareNoCase(name, "LocalOnly") == 0)
	    XmlReader::getBooleanValueElement(parser, localOnly, true);
	else if (CompareNoCase(name, "IncludeQualifiers") == 0)
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
	    XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    Message* request = new CIMGetClassRequestMessage(
	messageId,
	nameSpace,
	className,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	Array<String>(),
	QueueIdStack(queueId, _returnQueueId));

    _outputQueue->enqueue(request);
}

PEGASUS_NAMESPACE_END
