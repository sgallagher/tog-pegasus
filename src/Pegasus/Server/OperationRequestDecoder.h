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

#ifndef Pegasus_OperationRequestDecoder_h
#define Pegasus_OperationRequestDecoder_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/HTTPMessage.h>

PEGASUS_NAMESPACE_BEGIN

class XmlParser;

/** This class decodes CIM operation requests and places the decoded message
    on the output queue.
*/
class OperationRequestDecoder : public MessageQueue
{
public:

    OperationRequestDecoder(MessageQueue* outputQueue);

    ~OperationRequestDecoder();

    void sendResponse(Uint32 returnQueueId, Array<Sint8>& message);

    void sendError(
	Uint32 returnQueueId, 
	const String& messageId,
	const String& methodName,
	CIMStatusCode code,
	const String& description);

    virtual void handleEnqueue();

    void handleHTTPMessage(HTTPMessage* httpMessage);

    void OperationRequestDecoder::handleMethodCall(
	Uint32 returnQueueId,
	Sint8* content);

    ////////////////////////////////////////////////////////////////////////////
    //
    // Class-oriented methods:
    //
    ////////////////////////////////////////////////////////////////////////////

    void handleCreateClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleCreateClassResponseMessage(
	CIMCreateClassResponseMessage* response);

    void handleGetClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleGetClassResponseMessage(
	CIMGetClassResponseMessage* response);

    void handleModifyClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleModifyClassResponseMessage(
	CIMModifyClassResponseMessage* response);

    void handleEnumerateClassNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateClassNamesResponseMessage(
	CIMEnumerateClassNamesResponseMessage* response);

    void handleEnumerateClassesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateClassesResponseMessage(
	CIMEnumerateClassesResponseMessage* response);

    void handleDeleteClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleDeleteClassResponseMessage(
	CIMDeleteClassResponseMessage* response);

    ////////////////////////////////////////////////////////////////////////////
    //
    // Instance-oriented methods:
    //
    ////////////////////////////////////////////////////////////////////////////

    void handleCreateInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleCreateInstanceResponseMessage(
	CIMCreateInstanceResponseMessage* response);

    void handleGetInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleGetInstanceResponseMessage(
	CIMGetInstanceResponseMessage* response);

    void handleModifyInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleModifyInstanceResponseMessage(
	CIMModifyInstanceResponseMessage* response);

    void handleEnumerateInstanceNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateInstanceNamesResponseMessage(
	CIMEnumerateInstanceNamesResponseMessage* response);

    void handleEnumerateInstancesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateInstancesResponseMessage(
	CIMEnumerateInstancesResponseMessage* response);

    void handleDeleteInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleDeleteInstanceResponseMessage(
	CIMDeleteInstanceResponseMessage* response);

    ////////////////////////////////////////////////////////////////////////////
    //
    // Qualifier-oriented methods:
    //
    ////////////////////////////////////////////////////////////////////////////

    void handleSetQualifierRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleSetQualifierResponseMessage(
	CIMSetQualifierResponseMessage* response);

    void handleGetQualifierRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleGetQualifierResponseMessage(
	CIMGetQualifierResponseMessage* response);

    void handleDeleteQualifierRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleDeleteQualifierResponseMessage(
	CIMDeleteQualifierResponseMessage* response);

    void handleEnumerateQualifiersRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateQualifiersResponseMessage(
	CIMEnumerateQualifiersResponseMessage* response);

    ////////////////////////////////////////////////////////////////////////////
    //
    // Association-oriented methods:
    //
    ////////////////////////////////////////////////////////////////////////////

    void handleReferenceNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleReferenceNamesResponseMessage(
	CIMReferenceNamesResponseMessage* response);

    void handleReferencesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleReferencesResponseMessage(
	CIMReferencesResponseMessage* response);

    void handleAssociatorNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleAssociatorNamesResponseMessage(
	CIMAssociatorNamesResponseMessage* response);

    void handleAssociatorsRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleAssociatorsResponseMessage(
	CIMAssociatorsResponseMessage* response);

    ////////////////////////////////////////////////////////////////////////////
    //
    // Property-oriented methods:
    //
    ////////////////////////////////////////////////////////////////////////////

    void handleGetPropertyRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleSetPropertyRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

private:

    MessageQueue* _outputQueue;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_OperationRequestDecoder_h */
