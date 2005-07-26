//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Willis White, IBM (whiwill@us.ibm.com)
//              John Alex, IBM (johnalex@us.ibm.com) - Bug#2290
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOperationResponseDecoder_h
#define Pegasus_CIMOperationResponseDecoder_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/ContentLanguages.h> // l10n
#include <Pegasus/Client/ClientAuthenticator.h>
#include <Pegasus/Client/CIMClientException.h>
#include <Pegasus/Client/Linkage.h>
#include "ClientPerfDataStore.h"


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
        ClientAuthenticator* authenticator,
        Uint32 showInput);

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
        char* content,
        const ContentLanguages& contentLanguages,
        Boolean reconnect);

    CIMCreateClassResponseMessage* _decodeCreateClassResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMGetClassResponseMessage* _decodeGetClassResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMModifyClassResponseMessage* _decodeModifyClassResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMEnumerateClassNamesResponseMessage* _decodeEnumerateClassNamesResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMEnumerateClassesResponseMessage* _decodeEnumerateClassesResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMDeleteClassResponseMessage* _decodeDeleteClassResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMCreateInstanceResponseMessage* _decodeCreateInstanceResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMGetInstanceResponseMessage* _decodeGetInstanceResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMModifyInstanceResponseMessage* _decodeModifyInstanceResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMEnumerateInstanceNamesResponseMessage* _decodeEnumerateInstanceNamesResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMEnumerateInstancesResponseMessage* _decodeEnumerateInstancesResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMDeleteInstanceResponseMessage* _decodeDeleteInstanceResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMGetPropertyResponseMessage* _decodeGetPropertyResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMSetPropertyResponseMessage* _decodeSetPropertyResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMSetQualifierResponseMessage* _decodeSetQualifierResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMGetQualifierResponseMessage* _decodeGetQualifierResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMEnumerateQualifiersResponseMessage* _decodeEnumerateQualifiersResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMDeleteQualifierResponseMessage* _decodeDeleteQualifierResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMReferenceNamesResponseMessage* _decodeReferenceNamesResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMReferencesResponseMessage* _decodeReferencesResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMAssociatorNamesResponseMessage* _decodeAssociatorNamesResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMAssociatorsResponseMessage* _decodeAssociatorsResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMExecQueryResponseMessage* _decodeExecQueryResponse(
        XmlParser& parser,
        const String& messageId,
        Boolean isEmptyImethodresponseTag);

    CIMInvokeMethodResponseMessage* _decodeInvokeMethodResponse(
        XmlParser& parser,
        const String& messageId,
        const String& methodName,
        Boolean isEmptyMethodresponseTag);

    MessageQueue*        _outputQueue;

    MessageQueue*        _encoderQueue;

    ClientAuthenticator* _authenticator;

    // Controls decoder client displays 1 = con, 2 = log display
    Uint32               _showInput;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOperationResponseDecoder_h */
