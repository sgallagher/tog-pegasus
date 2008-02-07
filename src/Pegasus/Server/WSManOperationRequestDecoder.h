//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WSManOperationRequestDecoder_h
#define Pegasus_WSManOperationRequestDecoder_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_NAMESPACE_BEGIN

class SoapReader;

/** This class decodes WS-Man operation requests and passes them down-stream.
 */
class WSManOperationRequestDecoder : public MessageQueueService
{
public:
    typedef MessageQueueService Base;

    WSManOperationRequestDecoder(
        MessageQueueService* outputQueue,
        Uint32 returnQueueId);

   ~WSManOperationRequestDecoder();

   void sendResponse(
        Uint32 queueId,
        Buffer& message,
        Boolean closeConnect = false);

    void sendHttpError(
        Uint32 queueId,
        const String& status,
        const String& cimError = String::EMPTY,
        const String& messageBody = String::EMPTY,
        Boolean closeConnect = false);

    virtual void handleEnqueue(Message *);

    virtual void handleEnqueue();

    void handleHTTPMessage(HTTPMessage* httpMessage);

    void handleSoapMessage(
        Uint32 queueId,
        HttpMethod httpMethod,
        char* content,
        Uint32 contentLength,
        const String& soapAction,
        const String& authType,
        const String& userName,
        const String& ipAddress,
        const AcceptLanguageList& httpAcceptLanguages,
        const ContentLanguageList& httpContentLanguages,
        Boolean closeConnect);

    CIMOperationRequestMessage* decodeSoapEnvelope(
        SoapReader& soapReader,
        const String& soapAction,
        Uint32 queueId);

    CIMOperationRequestMessage* decodeSoapHeader(
        SoapReader& soapReader,
        const String& soapAction,
        Uint32 queueId);

    void decodeSoapBody(
        SoapReader& soapReader,
        Uint32 queueId,
        CIMOperationRequestMessage* request);

    // Sets the flag to indicate whether or not the CIMServer is shutting down.
    void setServerTerminating(Boolean flag) { _serverTerminating = flag; }

private:

    // Do not make _outputQueue an AutoPtr.
    MessageQueueService* _outputQueue;

    // Queue where responses should be enqueued.
    Uint32 _returnQueueId;

    // Flag to indicate whether or not the CIMServer is shutting down.
    Boolean _serverTerminating;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WSManOperationRequestDecoder_h */
