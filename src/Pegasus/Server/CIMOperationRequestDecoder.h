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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOperationRequestDecoder_h
#define Pegasus_CIMOperationRequestDecoder_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/HTTPMessage.h>

PEGASUS_NAMESPACE_BEGIN

class XmlParser;

/** This class decodes CIM operation requests and passes them down-stream.
*/
class CIMOperationRequestDecoder : public MessageQueue
{
public:

    CIMOperationRequestDecoder(
	MessageQueue* outputQueue,
	Uint32 returnQueueId);

    ~CIMOperationRequestDecoder();

    void sendResponse(
	Uint32 queueId, 
	Array<Sint8>& message);

    void sendError(
	Uint32 queueId, 
	const String& messageId,
	const String& methodName,
	CIMStatusCode code,
	const String& description);

    virtual void handleEnqueue();

    virtual const char* getQueueName() const;

    void handleHTTPMessage(HTTPMessage* httpMessage);

    void handleMethodCall(
	Uint32 queueId,
	Sint8* content);

    CIMCreateClassRequestMessage* decodeCreateClassRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMGetClassRequestMessage* decodeGetClassRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMModifyClassRequestMessage* decodeModifyClassRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMEnumerateClassNamesRequestMessage* decodeEnumerateClassNamesRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMEnumerateClassesRequestMessage* decodeEnumerateClassesRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMDeleteClassRequestMessage* decodeDeleteClassRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMCreateInstanceRequestMessage* decodeCreateInstanceRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMGetInstanceRequestMessage* decodeGetInstanceRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMModifyInstanceRequestMessage* decodeModifyInstanceRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMEnumerateInstanceNamesRequestMessage* decodeEnumerateInstanceNamesRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMEnumerateInstancesRequestMessage* decodeEnumerateInstancesRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMDeleteInstanceRequestMessage* decodeDeleteInstanceRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMSetQualifierRequestMessage* decodeSetQualifierRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMGetQualifierRequestMessage* decodeGetQualifierRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMEnumerateQualifiersRequestMessage* decodeEnumerateQualifiersRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMDeleteQualifierRequestMessage* decodeDeleteQualifierRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMReferenceNamesRequestMessage* decodeReferenceNamesRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMReferencesRequestMessage* decodeReferencesRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMAssociatorNamesRequestMessage* decodeAssociatorNamesRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMAssociatorsRequestMessage* decodeAssociatorsRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMGetPropertyRequestMessage* decodeGetPropertyRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMSetPropertyRequestMessage* decodeSetPropertyRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    CIMInvokeMethodRequestMessage* decodeInvokeMethodRequest(
        Uint32 queueId,
	XmlParser& parser, 
	const String& messageId,
	const CIMReference& reference,
	const String& cimMethodName);
        
    void sendMethodError(
	Uint32 queueId, 
	const String& messageId,
	const String& methodName,
	CIMStatusCode code,
	const String& description);

private:

    MessageQueue* _outputQueue;

    // Queue where responses should be enqueued.
    Uint32 _returnQueueId;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOperationRequestDecoder_h */
