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

#ifndef Pegasus_ClientConnection_h
#define Pegasus_ClientConnection_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Protocol/Handler.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>

PEGASUS_NAMESPACE_BEGIN

class XmlParser;

class ClientConnection : public Handler, public MessageQueue
{
public:

    ClientConnection(MessageQueue* outputQueue);

    ~ClientConnection();

    virtual Boolean handleOpen(Channel* channel);

    virtual int handleMessage();

    int handleGetRequest();

    int handleMethodCall();

    virtual void handleEnqueue();

    void sendError(
	const String& messageId,
	const char* methodName,
	CIMStatusCode code,
	const char* description);

    void sendError(
	CIMResponseMessage* message,
	const char* methodName);

    void handleGetClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleGetClassResponse(
	CIMGetClassResponseMessage* response);

    void handleGetInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleGetInstanceResponse(
	CIMGetInstanceResponseMessage* response);

    void handleReferenceNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleReferenceNamesResponse(
	CIMReferenceNamesResponseMessage* response);

    void handleReferencesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleReferencesResponse(
	CIMReferencesResponseMessage* response);

    void handleAssociatorNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleAssociatorNamesResponse(
	CIMAssociatorNamesResponseMessage* response);

    void handleAssociatorsRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleAssociatorsResponse(
	CIMAssociatorsResponseMessage* response);

    void handleCreateInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleCreateInstanceResponse(
	CIMCreateInstanceResponseMessage* response);

    void handleEnumerateInstanceNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateInstanceNamesResponse(
	CIMEnumerateInstanceNamesResponseMessage* response);

    void handleDeleteQualifierRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleDeleteQualifierResponse(
	CIMDeleteQualifierResponseMessage* response);

    void handleGetQualifierRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleGetQualifierResponse(
	CIMGetQualifierResponseMessage* response);

    void handleSetQualifierRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleSetQualifierResponse(
	CIMSetQualifierResponseMessage* response);

    void handleEnumerateQualifiersRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateQualifiersResponse(
	CIMEnumerateQualifiersResponseMessage* response);

    void handleCreateClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleCreateClassResponse(
	CIMCreateClassResponseMessage* response);

    void handleModifyClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleModifyClassResponse(
	CIMModifyClassResponseMessage* response);

    void handleModifyInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleModifyInstanceResponse(
	CIMModifyInstanceResponseMessage* response);

    void handleEnumerateClassesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateClassesResponse(
	CIMEnumerateClassesResponseMessage* response);

    void handleEnumerateClassNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateClassNamesResponse(
	CIMEnumerateClassNamesResponseMessage* response);

    void handleDeleteClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleDeleteClassResponse(
	CIMDeleteClassResponseMessage* response);

    void handleDeleteInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleDeleteInstanceResponse(
	CIMDeleteInstanceResponseMessage* response);

    void handleEnumerateInstancesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateInstancesResponse(
	CIMEnumerateInstancesResponseMessage* response);

    void handleGetPropertyRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleSetPropertyRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    /** sendMessage sends the message and optionally
        generates a trace output.
	@param message - The message as an array<sint8>
    */     
    void sendMessage(Array<Sint8>& message);

private:

    MessageQueue* _outputQueue;
    Channel* _channel;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ClientConnection_h */
