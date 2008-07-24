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

#include <cctype>
#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/MessageLoader.h>

#include "WsmConstants.h"
#include "WsmReader.h"
#include "WsmWriter.h"
#include "WsmResponseEncoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

WsmResponseEncoder::WsmResponseEncoder()
{
}

WsmResponseEncoder::~WsmResponseEncoder()
{
}

void WsmResponseEncoder::sendResponse(
    WsmResponse* response,
    const String& action,
    Buffer* bodygiven,
    Buffer* extraHeaders)
{
    PEG_METHOD_ENTER(TRC_WSMSERVER, "WsmResponseEncoder::sendResponse");
    PEG_TRACE((TRC_WSMSERVER, Tracer::LEVEL3,
        "WsmResponseEncoder::sendResponse(): action = %s",
        (const char*)action.getCString()));

    if (!response)
    {
        PEG_METHOD_EXIT();
        return;
    }

    Uint32 queueId = response->getQueueId();
    Boolean httpCloseConnect = response->getHttpCloseConnect();

    PEG_TRACE((TRC_WSMSERVER, Tracer::LEVEL4,
        "WsmResponseEncoder::sendResponse()- "
            "response->getHttpCloseConnect() returned %d",
        httpCloseConnect));

    MessageQueue* queue = MessageQueue::lookup(queueId);
    if (!queue)
    {
        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "ERROR: non-existent queueId = %u, response not sent.", queueId));
        PEG_METHOD_EXIT();
        return;
    }
    PEGASUS_ASSERT(dynamic_cast<HTTPConnection*>(queue) != 0);

    HttpMethod httpMethod = response->getHttpMethod();
    String messageId = response->getMessageId();
    String relatesTo = response->getRelatesTo();
    Buffer message;

    // Note: the language is ALWAYS passed empty to the xml formatters because
    // it is HTTPConnection that needs to make the decision of whether to add
    // the languages to the HTTP message.
    ContentLanguageList contentLanguage;

    Uint32 httpHeaderSize = 0;
    Buffer bodylocal, headerslocal;
    Buffer& body = bodygiven ? *bodygiven : bodylocal;
    Buffer& headers = extraHeaders ? *extraHeaders : headerslocal;

    if (response->getType() == SOAP_FAULT)
    {
        message = WsmWriter::formatSoapFault(
            ((SoapFaultResponse*) response)->getFault(),
            messageId,
            relatesTo,
            httpMethod,
            httpHeaderSize);
    }
    else if (response->getType() == WSM_FAULT)
    {
        message = WsmWriter::formatWsmFault(
            ((WsmFaultResponse*) response)->getFault(),
            messageId,
            relatesTo,
            httpMethod,
            httpHeaderSize);
    }
    else
    {
        // else non-error condition
        try
        {
            message = WsmWriter::formatWsmRspMessage(
                action,
                messageId,
                relatesTo,
                httpMethod,
                contentLanguage,
                body,
                headers,
                httpHeaderSize);
        }
        catch (PEGASUS_STD(bad_alloc)&)
        {
            WsmFault fault(WsmFault::wsman_InternalError,
                MessageLoaderParms(
                    "WsmServer.WsmResponseEncoder.OUT_OF_MEMORY",
                    "A System error has occurred. Please retry the "
                        "WS-Management operation at a later time."));
            WsmFaultResponse outofmem(relatesTo, queueId, httpMethod,
                httpCloseConnect, fault);

            // try again with new error and no body
            body.clear();
            sendResponse(&outofmem);
            PEG_METHOD_EXIT();
            return;
        }
    }

    // If MaxEnvelopeSize is not set, it's never been specified 
    // in the request
    if (response->getMaxEnvelopeSize() &&
        message.size() - httpHeaderSize > response->getMaxEnvelopeSize())
    {
        // try again with new error and no body
        body.clear();

        if (response->getType() == WSM_FAULT ||
            response->getType() == SOAP_FAULT)
        {
            WsmFault fault(WsmFault::wsman_EncodingLimit,
                MessageLoaderParms(
                    "WsmServer.WsmResponseEncoder.FAULT_MAX_ENV_SIZE_EXCEEDED",
                    "Fault response could not be encoded within requested "
                    "envelope size limits."),
                WSMAN_FAULTDETAIL_MAXENVELOPESIZE);
            WsmFaultResponse faultResponse(relatesTo, queueId, httpMethod,
                httpCloseConnect, fault);

            sendResponse(&faultResponse);
        }
        else
        {
            // DSP0226 R6.2-2:  If the mustUnderstand attribute is set to
            // "true", the service shall comply with the request.  If the
            // response would exceed the maximum size, the service should
            // return a wsman:EncodingLimit fault.  Because a service might
            // execute the operation prior to knowing the response size, the
            // service should undo any effects of the operation before
            // issuing the fault.  If the operation cannot be reversed (such
            // as a destructive wxf:Put or wxf:Delete, or a wxf:Create), the
            // service shall indicate that the operation succeeded in the
            // wsman:EncodingLimit fault with the following detail code:
            //     http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/
            //         UnreportableSuccess

            WsmFault fault(WsmFault::wsman_EncodingLimit,
                MessageLoaderParms(
                    "WsmServer.WsmResponseEncoder.UNREPORTABLE_SUCCESS",
                    "Success response could not be encoded within "
                    "requested envelope size limits."),
                WSMAN_FAULTDETAIL_UNREPORTABLESUCCESS);
            WsmFaultResponse faultResponse(relatesTo, queueId, httpMethod,
                httpCloseConnect, fault);

            sendResponse(&faultResponse);
        }

        PEG_METHOD_EXIT();
        return;
    }

    // Note: WS-Management responses are never sent in chunks, so there is no
    // need to check dynamic_cast<HTTPConnection*>(queue)->isChunkRequested().
    // HTTPMessage::isComplete() defaults to true, and we leave it that way.

    AutoPtr<HTTPMessage> httpMessage(new HTTPMessage(message));

    if (response->getType() == SOAP_FAULT)
    {
        httpMessage->contentLanguages =
            ((SoapFaultResponse*) response)->getFault().getMessageLanguage();
    }
    else if (response->getType() == WSM_FAULT)
    {
        httpMessage->contentLanguages =
            ((WsmFaultResponse*) response)->getFault().getReasonLanguage();
    }
    else
    {
        httpMessage->contentLanguages = response->getContentLanguages();
    }

    httpMessage->setCloseConnect(httpCloseConnect);
    queue->enqueue(httpMessage.release());

    PEG_METHOD_EXIT();
}

void WsmResponseEncoder::enqueue(WsmResponse* response)
{
    PEG_METHOD_ENTER(TRC_WSMSERVER, "WsmResponseEncoder::enqueue()");
    PEGASUS_ASSERT(response);

    PEG_TRACE((TRC_WSMSERVER, Tracer::LEVEL4,
        "WsmResponseEncoder::enqueue()- "
            "response->getHttpCloseConnect() returned %d",
        response->getHttpCloseConnect()));

    switch (response->getType())
    {
        case WS_TRANSFER_GET:
            _encodeWxfGetResponse((WxfGetResponse*) response);
            break;

        case WS_TRANSFER_PUT:
            _encodeWxfPutResponse((WxfPutResponse*) response);
            break;

        case WS_TRANSFER_CREATE:
            _encodeWxfCreateResponse((WxfCreateResponse*) response);
            break;

        case WS_TRANSFER_DELETE:
            _encodeWxfDeleteResponse((WxfDeleteResponse*) response);
            break;

        case WS_ENUMERATION_ENUMERATE:
            _encodeWsenEnumerateResponse((WsenEnumerateResponse*) response);
            break;

        case WS_ENUMERATION_PULL:
            _encodeWsenPullResponse((WsenPullResponse*) response);
            break;

        case WS_ENUMERATION_RELEASE:
            _encodeWsenReleaseResponse((WsenReleaseResponse*) response);
            break;

        case WSM_FAULT:
            _encodeWsmFaultResponse((WsmFaultResponse*) response);
            break;

        case SOAP_FAULT:
            _encodeSoapFaultResponse((SoapFaultResponse*) response);
            break;

        default:
            // Unexpected message type
            PEGASUS_ASSERT(0);
            break;
    }

    PEG_METHOD_EXIT();
}

void WsmResponseEncoder::_encodeWxfGetResponse(WxfGetResponse* response)
{
    Buffer body;
    WsmWriter::appendInstanceElement(body, response->getInstance());
    sendResponse(response, WSM_ACTION_GET_RESPONSE, &body);
}

void WsmResponseEncoder::_encodeWxfPutResponse(WxfPutResponse* response)
{
    Buffer body;
    Buffer headers;

    // DSP0226 R6.5-1:  A service receiving a message that contains the
    // wsman:RequestEPR header block should return a response that contains
    // a wsman:RequestedEPR header block.  This block contains the most recent
    // EPR of the resource being accessed or a status code if the service
    // cannot determine or return the EPR.  This EPR reflects any identity
    // changes that may have occurred as a result of the current operation, as
    // set forth in the following behavior.  The header block in the
    // corresponding response message has the following format:
    //     <wsman:RequestedEPR...>
    //       [ <wsa:EndpointReference>
    //          wsa:EndpointReferenceType
    //       </wsa:EndpointReference> |
    //       <wsman:EPRInvalid/> |
    //       <wsman:EPRUnknown/> ]
    //     </wsman:RequestedEPR>
    if (response->getRequestedEPR())
    {
        WsmWriter::appendStartTag(
            headers, WsmNamespaces::WS_MAN, STRLIT("RequestedEPR"));
        WsmWriter::appendStartTag(
            headers, WsmNamespaces::WS_ADDRESSING, STRLIT("EndpointReference"));
        WsmWriter::appendEPRElement(headers, response->getEPR());
        WsmWriter::appendEndTag(
            headers, WsmNamespaces::WS_ADDRESSING, STRLIT("EndpointReference"));
        WsmWriter::appendEndTag(
            headers, WsmNamespaces::WS_MAN, STRLIT("RequestedEPR"));
    }
    sendResponse(response, WSM_ACTION_PUT_RESPONSE, &body, &headers);
}

void WsmResponseEncoder::_encodeWxfCreateResponse(WxfCreateResponse* response)
{
    Buffer body;
    WsmWriter::appendStartTag(
        body, WsmNamespaces::WS_TRANSFER, STRLIT("ResourceCreated"));
    WsmWriter::appendEPRElement(body, response->getEPR());
    WsmWriter::appendEndTag(
        body, WsmNamespaces::WS_TRANSFER, STRLIT("ResourceCreated"));
    sendResponse(response, WSM_ACTION_CREATE_RESPONSE, &body);
}

void WsmResponseEncoder::_encodeWxfDeleteResponse(WxfDeleteResponse* response)
{
    sendResponse(response, WSM_ACTION_DELETE_RESPONSE);
}

void WsmResponseEncoder::_encodeWsenEnumerateResponse(
    WsenEnumerateResponse* response)
{
    Buffer body, headers;

    WsmWriter::appendStartTag(
        body, WsmNamespaces::WS_ENUMERATION, STRLIT("EnumerateResponse"));

    _encodeEnumeratedItems(body, response->getEnumerationContext(),
        response->getInstances(), response->isComplete());

    WsmWriter::appendEndTag(
        body, WsmNamespaces::WS_ENUMERATION, STRLIT("EnumerateResponse"));

    if (response->requestedItemCount())
    {
        WsmWriter::appendStartTag(
            headers, WsmNamespaces::WS_MAN, STRLIT("TotalItemsCountEstimate"));
        WsmWriter::append(headers, response->getItemCount());
        WsmWriter::appendEndTag(
            headers, WsmNamespaces::WS_MAN, STRLIT("TotalItemsCountEstimate"));
        
        sendResponse(response, WSM_ACTION_ENUMERATE_RESPONSE, &body, &headers);
    }
    else
    {
        sendResponse(response, WSM_ACTION_ENUMERATE_RESPONSE, &body);
    }


}

void WsmResponseEncoder::_encodeWsenPullResponse(WsenPullResponse* response)
{
    Buffer body;

    WsmWriter::appendStartTag(
        body, WsmNamespaces::WS_ENUMERATION, STRLIT("PullResponse"));

    _encodeEnumeratedItems(body, response->getEnumerationContext(),
        response->getInstances(), response->isComplete());

    WsmWriter::appendEndTag(
        body, WsmNamespaces::WS_ENUMERATION, STRLIT("PullResponse"));

    sendResponse(response, WSM_ACTION_PULL_RESPONSE, &body);
}

void WsmResponseEncoder::_encodeEnumeratedItems(
    Buffer& body,
    Uint64 enumerationContext,
    Array<WsmInstance>& instances,
    Boolean isComplete)
{
    if (!isComplete)
    {
        WsmWriter::appendStartTag(
            body, WsmNamespaces::WS_ENUMERATION, STRLIT("EnumerationContext"));
        WsmWriter::append(body, enumerationContext);
        WsmWriter::appendEndTag(
            body, WsmNamespaces::WS_ENUMERATION, STRLIT("EnumerationContext"));
    }
    else
    {
        WsmWriter::appendEmptyTag(
            body, WsmNamespaces::WS_ENUMERATION, STRLIT("EnumerationContext"));
    }

    if (instances.size() > 0)
    {
        WsmWriter::appendStartTag(
            body, WsmNamespaces::WS_ENUMERATION, STRLIT("Items"));
        for (Uint32 i = 0; i < instances.size(); i++)
        {
            WsmWriter::appendInstanceElement(body, instances[i]);
        }
        WsmWriter::appendEndTag(
            body, WsmNamespaces::WS_ENUMERATION, STRLIT("Items"));
    }

    if (isComplete)
    {
        WsmWriter::appendEmptyTag(
            body, WsmNamespaces::WS_ENUMERATION, STRLIT("EndOfSequence"));
    }

}

void WsmResponseEncoder::_encodeWsenReleaseResponse(
    WsenReleaseResponse* response)
{
    sendResponse(response, WSM_ACTION_RELEASE_RESPONSE);
}

void WsmResponseEncoder::_encodeWsmFaultResponse(WsmFaultResponse* response)
{
    sendResponse(response);
}

void WsmResponseEncoder::_encodeSoapFaultResponse(SoapFaultResponse* response)
{
    sendResponse(response);
}

PEGASUS_NAMESPACE_END
