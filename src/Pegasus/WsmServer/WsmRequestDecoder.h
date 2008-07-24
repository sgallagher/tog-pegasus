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
//=============================================================================
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WsmRequestDecoder_h
#define Pegasus_WsmRequestDecoder_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/WsmServer/WsmRequest.h>

PEGASUS_NAMESPACE_BEGIN

class WsmReader;
class WsmProcessor;

/** This class decodes WS-Man operation requests and passes them down-stream.
 */
class WsmRequestDecoder : public MessageQueueService
{
public:

    WsmRequestDecoder(WsmProcessor* wsmProcessor);

    ~WsmRequestDecoder();

    void sendResponse(
        Uint32 queueId,
        Buffer& message,
        Boolean httpCloseConnect = false);

    void sendHttpError(
        Uint32 queueId,
        const String& status,
        const String& cimError = String::EMPTY,
        const String& messageBody = String::EMPTY,
        Boolean httpCloseConnect = false);

    virtual void handleEnqueue(Message *);

    virtual void handleEnqueue();

    void handleHTTPMessage(HTTPMessage* httpMessage);

    void handleWsmMessage(
        Uint32 queueId,
        HttpMethod httpMethod,
        char* content,
        Uint32 contentLength,
        String& soapAction,
        const String& authType,
        const String& userName,
        const String& ipAddress,
        const AcceptLanguageList& httpAcceptLanguages,
        const ContentLanguageList& httpContentLanguages,
        Boolean httpCloseConnect);

    // Sets the flag to indicate whether or not the CIMServer is shutting down.
    void setServerTerminating(Boolean flag) { _serverTerminating = flag; }

private:

    void _checkRequiredHeader(
        const char* headerName,
        Boolean headerSpecified);

    WxfGetRequest* _decodeWSTransferGet(
        WsmReader& wsmReader,
        const String& messageId,
        const WsmEndpointReference& epr);

    WxfPutRequest* _decodeWSTransferPut(
        WsmReader& wsmReader,
        const String& messageId,
        const WsmEndpointReference& epr);

    WxfCreateRequest* _decodeWSTransferCreate(
        WsmReader& wsmReader,
        const String& messageId,
        const WsmEndpointReference& epr);

    WxfDeleteRequest* _decodeWSTransferDelete(
        WsmReader& wsmReader,
        const String& messageId,
        const WsmEndpointReference& epr);

    WsenEnumerateRequest* _decodeWSEnumerationEnumerate(
        WsmReader& wsmReader,
        const String& messageId,
        const WsmEndpointReference& epr,
        Boolean requestItemCount);

    WsenPullRequest* _decodeWSEnumerationPull(
        WsmReader& wsmReader,
        const String& messageId,
        const WsmEndpointReference& epr,
        Boolean requestItemCount);

    WsenReleaseRequest* _decodeWSEnumerationRelease(
        WsmReader& wsmReader,
        const String& messageId,
        const WsmEndpointReference& epr);

    // The queue to which to forward the decoded WsmRequests.
    WsmProcessor* _wsmProcessor;

    // Flag to indicate whether or not the CIMServer is shutting down.
    Boolean _serverTerminating;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WsmRequestDecoder_h */
