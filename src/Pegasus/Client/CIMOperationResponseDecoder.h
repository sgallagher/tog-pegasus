//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOperationResponseDecoder_h
#define Pegasus_CIMOperationResponseDecoder_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Client/ClientAuthenticator.h>
#include <Pegasus/Client/CIMClientException.h>
#include <Pegasus/Client/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class XmlParser;

/**
    This message is sent from the response decoder to the CIMClient, indicating
    an error in issuing a CIM request.
*/
class ClientExceptionMessage : public Message
{
public:
    ClientExceptionMessage(Exception* clientException_)
        :
        Message(CLIENT_EXCEPTION_MESSAGE),
        clientException(clientException_)
    {
    }

    Exception* clientException;
};


/** This class receives HTTP messages and decodes them into CIM Operation 
    Responses messages which it places on its output queue.
*/
class PEGASUS_CLIENT_LINKAGE CIMOperationResponseDecoder : public MessageQueue
{
public:

    /** Constuctor.
	@param outputQueue queue to receive decoded HTTP messages.
    */
   CIMOperationResponseDecoder(
        MessageQueue* outputQueue,
        MessageQueue* encoderQueue,
        ClientAuthenticator* authenticator);

    /** Destructor. */
    ~CIMOperationResponseDecoder();

    /** Initializes the encoder queue */
    void setEncoderQueue(MessageQueue* encoderQueue);

    /** This method is called when a message is enqueued on this queue. */
    virtual void handleEnqueue();

private:

    void _handleHTTPMessage(
	HTTPMessage* message);

    void _handleMethodResponse(
	char* content);

    CIMCreateClassResponseMessage* _decodeCreateClassResponse(
	XmlParser& parser, const String& messageId);

    CIMGetClassResponseMessage* _decodeGetClassResponse(
	XmlParser& parser, const String& messageId);

    CIMModifyClassResponseMessage* _decodeModifyClassResponse(
	XmlParser& parser, const String& messageId);

    CIMEnumerateClassNamesResponseMessage* _decodeEnumerateClassNamesResponse(
	XmlParser& parser, const String& messageId);

    CIMEnumerateClassesResponseMessage* _decodeEnumerateClassesResponse(
	XmlParser& parser, const String& messageId);

    CIMDeleteClassResponseMessage* _decodeDeleteClassResponse(
	XmlParser& parser, const String& messageId);

    CIMCreateInstanceResponseMessage* _decodeCreateInstanceResponse(
	XmlParser& parser, const String& messageId);

    CIMGetInstanceResponseMessage* _decodeGetInstanceResponse(
	XmlParser& parser, const String& messageId);

    CIMModifyInstanceResponseMessage* _decodeModifyInstanceResponse(
	XmlParser& parser, const String& messageId);

    CIMEnumerateInstanceNamesResponseMessage* _decodeEnumerateInstanceNamesResponse(
	XmlParser& parser, const String& messageId);

    CIMEnumerateInstancesResponseMessage* _decodeEnumerateInstancesResponse(
	XmlParser& parser, const String& messageId);

    CIMDeleteInstanceResponseMessage* _decodeDeleteInstanceResponse(
	XmlParser& parser, const String& messageId);

    CIMGetPropertyResponseMessage* _decodeGetPropertyResponse(
	XmlParser& parser, const String& messageId);

    CIMSetPropertyResponseMessage* _decodeSetPropertyResponse(
	XmlParser& parser, const String& messageId);

    CIMSetQualifierResponseMessage* _decodeSetQualifierResponse(
	XmlParser& parser, const String& messageId);

    CIMGetQualifierResponseMessage* _decodeGetQualifierResponse(
	XmlParser& parser, const String& messageId);

    CIMEnumerateQualifiersResponseMessage* _decodeEnumerateQualifiersResponse(
	XmlParser& parser, const String& messageId);

    CIMDeleteQualifierResponseMessage* _decodeDeleteQualifierResponse(
	XmlParser& parser, const String& messageId);

    CIMReferenceNamesResponseMessage* _decodeReferenceNamesResponse(
	XmlParser& parser, const String& messageId);

    CIMReferencesResponseMessage* _decodeReferencesResponse(
	XmlParser& parser, const String& messageId);

    CIMAssociatorNamesResponseMessage* _decodeAssociatorNamesResponse(
	XmlParser& parser, const String& messageId);

    CIMAssociatorsResponseMessage* _decodeAssociatorsResponse(
	XmlParser& parser, const String& messageId);

    CIMExecQueryResponseMessage* _decodeExecQueryResponse(
	XmlParser& parser, const String& messageId);

    CIMInvokeMethodResponseMessage* _decodeInvokeMethodResponse(
	XmlParser& parser, const String& messageId, const String& methodName);

    MessageQueue*        _outputQueue;

    MessageQueue*        _encoderQueue;

    ClientAuthenticator* _authenticator;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOperationResponseDecoder_h */
