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

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/CIMMessage.h>
#include "CIMOperationResponseDecoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMOperationResponseDecoder::CIMOperationResponseDecoder(
    MessageQueue* outputQueue) : _outputQueue(outputQueue)
{

}

CIMOperationResponseDecoder::~CIMOperationResponseDecoder()
{

}

void CIMOperationResponseDecoder::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
	return;

    switch (message->getType())
    {
	case HTTP_MESSAGE:
	{
	    HTTPMessage* httpMessage = (HTTPMessage*)message;
	    _handleHTTPMessage(httpMessage);
	    break;
	}

	default:
	    // ATTN: send this to the orphan queue!
	    break;
    }

    delete message;
}

const char* CIMOperationResponseDecoder::getQueueName() const
{
    return "CIMOperationResponseDecoder";
}

void CIMOperationResponseDecoder::_handleHTTPMessage(HTTPMessage* httpMessage)
{
    //
    // Parse the HTTP message:
    //

    String startLine;
    Array<HTTPHeader> headers;
    Sint8* content;
    Uint32 contentLength;

    httpMessage->parse(startLine, headers, content, contentLength);

    //
    // Search for "CIMOperation" header:
    //

    String cimOperation;

    if (!HTTPMessage::lookupHeader(
	headers, "*CIMOperation", cimOperation, true))
    {
	// ATTN: error discarded at this time!
	return;
    }

    //
    // Zero-terminate the message:
    //

    httpMessage->message.append('\0');

    //
    // If it is a method response, then dispatch it to be handled:
    //

    if (!String::equalNoCase(cimOperation, "MethodCall"))
    {
	// ATTN: error discarded at this time!
	return;
    }

    _handleMethodResponse(content);
}

void CIMOperationResponseDecoder::_handleMethodResponse(char* content)
{
    //
    // Create and initialize XML parser:
    //

    XmlParser parser((char*)content);
    XmlEntry entry;

    try
    {
	//
	// Process <?xml ... >
	//

	XmlReader::expectXmlDeclaration(parser, entry);

	//
	// Process <CIM ... >
	//

	XmlReader::testCimStartTag(parser);

	//
	// Expect <MESSAGE ... >
	//

	String messageId;
	const char* protocolVersion = 0;

	if (!XmlReader::getMessageStartTag(parser, messageId, protocolVersion))
	    throw XmlValidationError(
		parser.getLine(), "expected MESSAGE element");

	if (strcmp(protocolVersion, "1.0") != 0)
	{
	    // ATTN: protocol version being ignored at present!

	    return;
	}

	//
	// Expect <SIMPLERSP ... >
	//

	XmlReader::expectStartTag(parser, entry, "SIMPLERSP");

	//
	// Expect <IMETHODRESPONSE ... >
	//

	const char* iMethodResponseName = 0;

	if (!XmlReader::getIMethodResponseStartTag(parser, iMethodResponseName))
	{
	    // ATTN: error ignored for now!

	    return;
	}

	//
	// Dispatch the method:
	//

	if (EqualNoCase(iMethodResponseName, "GetClass"))
	    _decodeGetClassResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "GetInstance"))
	    _decodeGetInstanceResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "EnumerateClassNames"))
	    _decodeEnumerateClassNamesResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "References"))
	    _decodeReferencesResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "ReferenceNames"))
	    _decodeReferenceNamesResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "AssociatorNames"))
	    _decodeAssociatorNamesResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "Associators"))
	    _decodeAssociatorsResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "CreateInstance"))
	    _decodeCreateInstanceResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName,"EnumerateInstanceNames")==0)
	    _decodeEnumerateInstanceNamesResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "DeleteQualifier"))
	    _decodeDeleteQualifierResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "GetQualifier"))
	    _decodeGetQualifierResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "SetQualifier"))
	    _decodeSetQualifierResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "EnumerateQualifiers"))
	    _decodeEnumerateQualifiersResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "EnumerateClasses"))
	    _decodeEnumerateClassesResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "CreateClass"))
	    _decodeCreateClassResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "ModifyClass"))
	    _decodeModifyClassResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "ModifyInstance"))
	    _decodeModifyInstanceResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "DeleteClass"))
	    _decodeDeleteClassResponse(parser, messageId);
	else if (EqualNoCase(iMethodResponseName, "DeleteInstance"))
	    _decodeDeleteInstanceResponse(parser, messageId);

	//
	// Handle end tags:
	//

	XmlReader::expectEndTag(parser, "IMETHODRESPONSE");
	XmlReader::expectEndTag(parser, "SIMPLERSP");
	XmlReader::expectEndTag(parser, "MESSAGE");
	XmlReader::expectEndTag(parser, "CIM");
    }
    catch (Exception& x)
    {
	// ATTN: ignore the exception for now!

	PEGASUS_TRACE;
	cout << x.getMessage() << endl;
	return;
    }
}

void CIMOperationResponseDecoder::_decodeCreateClassResponse(
    XmlParser& parser, 
    const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMCreateClassResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));

    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMCreateClassResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeGetClassResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMGetClassResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    CIMClass()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMClass cimClass;

	if (!XmlReader::getClassElement(parser, cimClass))
	    throw XmlValidationError(parser.getLine(),"expected CLASS element");

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMGetClassResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    cimClass));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeModifyClassResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMModifyClassResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMModifyClassResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeEnumerateClassNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMEnumerateClassNamesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<String>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<String> classNames;
	String className;

	while (XmlReader::getClassNameElement(parser, className, false))
	    classNames.append(className);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMEnumerateClassNamesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    classNames));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeEnumerateClassesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMEnumerateClassesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMClass>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMClass> cimClasses;
	CIMClass cimClass;

	while (XmlReader::getClassElement(parser, cimClass))
	    cimClasses.append(cimClass);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMEnumerateClassesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    cimClasses));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeDeleteClassResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMDeleteClassResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMDeleteClassResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeCreateInstanceResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMCreateInstanceResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMCreateInstanceResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeGetInstanceResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMGetInstanceResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    CIMInstance()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMInstance cimInstance;

	if (!XmlReader::getInstanceElement(parser, cimInstance))
	{
	    throw XmlValidationError(
		parser.getLine(), "expected INSTANCE element");
	}

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMGetInstanceResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    cimInstance));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeModifyInstanceResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMModifyInstanceResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMModifyInstanceResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeEnumerateInstanceNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMEnumerateInstanceNamesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMReference>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMReference> instanceNames;
	String className;
	Array<KeyBinding> keyBindings;

	while (XmlReader::getInstanceNameElement(
	    parser, className, keyBindings))
	{
	    CIMReference r(
		String::EMPTY,
		String::EMPTY,
		className,
		keyBindings);
	    instanceNames.append(r);
	}

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMEnumerateInstanceNamesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    instanceNames));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeDeleteInstanceResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMDeleteInstanceResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMDeleteInstanceResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeSetQualifierResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMSetQualifierResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMSetQualifierResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeGetQualifierResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMGetQualifierResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    CIMQualifierDecl()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMQualifierDecl qualifierDecl;
	XmlReader::getQualifierDeclElement(parser, qualifierDecl);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMGetQualifierResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    qualifierDecl));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeEnumerateQualifiersResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMEnumerateQualifiersResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMQualifierDecl>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMQualifierDecl> qualifierDecls;
	CIMQualifierDecl qualifierDecl;

	while (XmlReader::getQualifierDeclElement(parser, qualifierDecl))
	    qualifierDecls.append(qualifierDecl);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMEnumerateQualifiersResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    qualifierDecls));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeDeleteQualifierResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMDeleteQualifierResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMDeleteQualifierResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

//MEB:

void CIMOperationResponseDecoder::_decodeReferenceNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMReferenceNamesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMReference>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMReference objectPath;
	Array<CIMReference> objectPaths;

	while (XmlReader::getObjectPathElement(parser, objectPath))
	    objectPaths.append(objectPath);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMReferenceNamesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    objectPaths));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeReferencesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMReferencesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMObjectWithPath>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMObjectWithPath> objectWithPathArray;
	CIMObjectWithPath tmp;

	while (XmlReader::getObjectWithPath(parser, tmp))
	    objectWithPathArray.append(tmp);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMReferencesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    objectWithPathArray));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeAssociatorNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMAssociatorNamesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMReference>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMReference objectPath;
	Array<CIMReference> objectPaths;

	while (XmlReader::getObjectPathElement(parser, objectPath))
	    objectPaths.append(objectPath);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMAssociatorNamesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    objectPaths));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

void CIMOperationResponseDecoder::_decodeAssociatorsResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMAssociatorsResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMObjectWithPath>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMObjectWithPath> objectWithPathArray;
	CIMObjectWithPath tmp;

	while (XmlReader::getObjectWithPath(parser, tmp))
	    objectWithPathArray.append(tmp);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_outputQueue->enqueue(new CIMAssociatorsResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    objectWithPathArray));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

PEGASUS_NAMESPACE_END
