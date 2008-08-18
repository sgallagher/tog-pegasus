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
#include "WsmToCimRequestMapper.h"
#include "SoapResponse.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

WsmResponseEncoder::WsmResponseEncoder()
{
}

WsmResponseEncoder::~WsmResponseEncoder()
{
}

void WsmResponseEncoder::_sendResponse(SoapResponse* response)
{
    PEG_METHOD_ENTER(TRC_WSMSERVER, "WsmResponseEncoder::sendResponse");
    PEG_TRACE((TRC_WSMSERVER, Tracer::LEVEL3,
        "WsmResponseEncoder::sendResponse()"));

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

    Buffer message = response->getResponseContent();

    // Note: WS-Management responses are never sent in chunks, so there is no
    // need to check dynamic_cast<HTTPConnection*>(queue)->isChunkRequested().
    // HTTPMessage::isComplete() defaults to true, and we leave it that way.

    AutoPtr<HTTPMessage> httpMessage(new HTTPMessage(message));

    httpMessage->setCloseConnect(httpCloseConnect);
    queue->enqueue(httpMessage.release());

    PEG_METHOD_EXIT();
}

void WsmResponseEncoder::_sendUnreportableSuccess(WsmResponse* response)
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
    WsmFaultResponse faultResponse(
        response->getRelatesTo(), 
        response->getQueueId(), 
        response->getHttpMethod(),
        response->getHttpCloseConnect(), 
        fault);

    SoapResponse soapResponse(&faultResponse);
    _sendResponse(&soapResponse);
}

void WsmResponseEncoder::_sendEncodingLimitFault(WsmResponse* response)
{
    WsmFault fault(WsmFault::wsman_EncodingLimit,
        MessageLoaderParms(
            "WsmServer.WsmResponseEncoder.MAX_ENV_SIZE_EXCEEDED",
            "Response could not be encoded within requested "
            "envelope size limits."),
        WSMAN_FAULTDETAIL_MAXENVELOPESIZE);
    WsmFaultResponse faultResponse(
        response->getRelatesTo(), 
        response->getQueueId(), 
        response->getHttpMethod(),
        response->getHttpCloseConnect(), 
        fault);

    SoapResponse soapResponse(&faultResponse);
    _sendResponse(&soapResponse);
}

void WsmResponseEncoder::enqueue(WsmResponse* response)
{
    PEG_METHOD_ENTER(TRC_WSMSERVER, "WsmResponseEncoder::enqueue()");
    PEGASUS_ASSERT(response);

    PEG_TRACE((TRC_WSMSERVER, Tracer::LEVEL4,
        "WsmResponseEncoder::enqueue()- "
            "response->getHttpCloseConnect() returned %d",
        response->getHttpCloseConnect()));

    try 
    {
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
    }
    catch (PEGASUS_STD(bad_alloc)&)
    {
        WsmFault fault(WsmFault::wsman_InternalError,
            MessageLoaderParms(
                "WsmServer.WsmResponseEncoder.OUT_OF_MEMORY",
                "A System error has occurred. Please retry the "
                    "WS-Management operation at a later time."));
        WsmFaultResponse outofmem(
            response->getRelatesTo(), 
            response->getQueueId(), 
            response->getHttpMethod(),
            response->getHttpCloseConnect(), 
            fault);
        _encodeWsmFaultResponse(&outofmem);
    }

    PEG_METHOD_EXIT();
}

void WsmResponseEncoder::_encodeWxfGetResponse(WxfGetResponse* response)
{
    SoapResponse soapResponse(response);
    Buffer body;
    WsmWriter::appendInstanceElement(body, response->getInstance());
    if (soapResponse.appendBodyContent(body))
    {
        _sendResponse(&soapResponse);
    }
    else
    {
        _sendUnreportableSuccess(response);
    }
}

void WsmResponseEncoder::_encodeWxfPutResponse(WxfPutResponse* response)
{
    SoapResponse soapResponse(response);
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
            headers, 
            WsmNamespaces::WS_ADDRESSING, STRLIT("EndpointReference"));
        WsmWriter::appendEPRElement(headers, response->getEPR());
        WsmWriter::appendEndTag(
            headers, 
            WsmNamespaces::WS_ADDRESSING, STRLIT("EndpointReference"));
        WsmWriter::appendEndTag(
            headers, WsmNamespaces::WS_MAN, STRLIT("RequestedEPR"));
    }

    if (soapResponse.appendHeader(headers))
    {
        _sendResponse(&soapResponse);
    }
    else
    {
        _sendUnreportableSuccess(response);
    }
}

void WsmResponseEncoder::_encodeWxfCreateResponse(WxfCreateResponse* response)
{
    SoapResponse soapResponse(response);
    Buffer body;

    WsmWriter::appendStartTag(
        body, WsmNamespaces::WS_TRANSFER, STRLIT("ResourceCreated"));
    WsmWriter::appendEPRElement(body, response->getEPR());
    WsmWriter::appendEndTag(
        body, WsmNamespaces::WS_TRANSFER, STRLIT("ResourceCreated"));

    if (soapResponse.appendBodyContent(body))
    {
        _sendResponse(&soapResponse);
    }
    else
    {
        _sendUnreportableSuccess(response);
    }
}

void WsmResponseEncoder::_encodeWxfDeleteResponse(WxfDeleteResponse* response)
{
    SoapResponse soapResponse(response);
    _sendResponse(&soapResponse);
}

void WsmResponseEncoder::_encodeWsenEnumerateResponse(
    WsenEnumerateResponse* response)
{
    SoapResponse soapResponse(response);
    Buffer headers;

    if (response->requestedItemCount())
    {
        WsmWriter::appendStartTag(
            headers, WsmNamespaces::WS_MAN, STRLIT("TotalItemsCountEstimate"));
        WsmWriter::append(headers, response->getItemCount());
        WsmWriter::appendEndTag(
            headers, WsmNamespaces::WS_MAN, STRLIT("TotalItemsCountEstimate"));
    }

    if (!_encodeEnumerationData(
            soapResponse, 
            headers, 
            WS_ENUMERATION_ENUMERATE,
            response->getEnumerationContext(),
            response->isComplete(),
            response->getEnumerationData()))
    {
        _sendEncodingLimitFault(response);
        return;
    }

    _sendResponse(&soapResponse);
}

void WsmResponseEncoder::_encodeWsenPullResponse(WsenPullResponse* response)
{
    SoapResponse soapResponse(response);
    Buffer headers;

    if (!_encodeEnumerationData(
            soapResponse, 
            headers, 
            WS_ENUMERATION_PULL,
            response->getEnumerationContext(),
            response->isComplete(),
            response->getEnumerationData()))
    {
        _sendEncodingLimitFault(response);
        return;
    }

    _sendResponse(&soapResponse);
}

Boolean WsmResponseEncoder::_encodeEnumerationData(
    SoapResponse& soapResponse,
    Buffer& headers,
    WsmOperationType operation,
    Uint64 contextId,
    Boolean isComplete,
    WsenEnumerationData& data)
{
    Buffer bodyHeader, bodyTrailer;

    PEGASUS_ASSERT(operation == WS_ENUMERATION_ENUMERATE ||
        operation == WS_ENUMERATION_PULL);

    WsmWriter::appendStartTag(
        bodyHeader, WsmNamespaces::WS_ENUMERATION, 
        operation == WS_ENUMERATION_ENUMERATE ? 
            STRLIT("EnumerateResponse") : STRLIT("PullResponse"));

    if (!isComplete)
    {
        WsmWriter::appendStartTag(
            bodyHeader, WsmNamespaces::WS_ENUMERATION, 
            STRLIT("EnumerationContext"));
        WsmWriter::append(bodyHeader, contextId);
        WsmWriter::appendEndTag(
            bodyHeader, WsmNamespaces::WS_ENUMERATION, 
            STRLIT("EnumerationContext"));
    }
    else
    {
        WsmWriter::appendEmptyTag(
            bodyHeader, WsmNamespaces::WS_ENUMERATION, 
            STRLIT("EnumerationContext"));
    }

    if (data.getSize() > 0)
    {
        WsmWriter::appendStartTag(
            bodyHeader, WsmNamespaces::WS_ENUMERATION, STRLIT("Items"));
        WsmWriter::appendEndTag(
            bodyTrailer, WsmNamespaces::WS_ENUMERATION, STRLIT("Items"));
    }

    Uint32 eosPos = bodyTrailer.size();
    Uint32 eosSize = 0;
    if (isComplete)
    {
        WsmWriter::appendEmptyTag(
            bodyTrailer, 
            operation == WS_ENUMERATION_ENUMERATE ? 
                WsmNamespaces::WS_MAN : WsmNamespaces::WS_ENUMERATION, 
            STRLIT("EndOfSequence"));
        eosSize = bodyTrailer.size() - eosPos;
    }

    WsmWriter::appendEndTag(
        bodyTrailer, WsmNamespaces::WS_ENUMERATION, 
        operation == WS_ENUMERATION_ENUMERATE ? 
            STRLIT("EnumerateResponse") : STRLIT("PullResponse"));

    // Fault the request if it can't be encoded within the limits
    if (!soapResponse.appendHeader(headers) ||
        !soapResponse.appendBodyHeader(bodyHeader) ||
        !soapResponse.appendBodyTrailer(bodyTrailer))
    {
        return false;
    }

    // Now add the list of items
    Uint32 i;

    if (data.enumerationMode == WSEN_EM_OBJECT)
    {
        for (i = 0; i < data.instances.size(); i++)
        {
            Buffer body;

            if (data.polymorphismMode == WSMB_PM_EXCLUDE_SUBCLASS_PROPERTIES)
            {
                // The response does not contain the subclass properties, but
                // the class name is still that of the subclass. 
                // Replace it here.
                data.instances[i].setClassName(
                    WsmToCimRequestMapper::convertResourceUriToClassName(
                        data.classUri).getString());
            }

            WsmWriter::appendInstanceElement(body, data.instances[i]);
            if (!soapResponse.appendBodyContent(body))
            {
                break;
            }
        }
    }
    else if (data.enumerationMode == WSEN_EM_EPR)
    {
        for (i = 0; i < data.eprs.size(); i++)
        {
            Buffer body;
            WsmWriter::appendStartTag(
                body, 
                WsmNamespaces::WS_ADDRESSING, 
                STRLIT("EndpointReference"));
            WsmWriter::appendEPRElement(body, data.eprs[i]);
            WsmWriter::appendEndTag(
                body, 
                WsmNamespaces::WS_ADDRESSING, 
                STRLIT("EndpointReference"));
            if (!soapResponse.appendBodyContent(body))
            {
                break;
            }
        }
    }
    else
    {
        PEGASUS_ASSERT(0);
    }

    // If the list is not empty, but none of the items have been successfully
    // added to the soapResponse, fault the request because it cannot be
    // encoded within the specified limits.
    if (data.getSize() > 0 && i == 0)
    {
        return false;
    }

    // Remove the items we processed. The rest will be added back 
    // to the context
    if (i != 0)
    {
        data.remove(0, i);
    }

    // The request is complete but could not be encoded with MaxEnvelopeSize.
    // Clear EndOfSequence tag.
    if (isComplete && data.getSize() > 0)
    {
        soapResponse.getBodyTrailer().remove(eosPos, eosSize);
    }

    return true;
}

void WsmResponseEncoder::_encodeWsenReleaseResponse(
    WsenReleaseResponse* response)
{
    SoapResponse soapResponse(response);
    _sendResponse(&soapResponse);
}

void WsmResponseEncoder::_encodeWsmFaultResponse(WsmFaultResponse* response)
{
    SoapResponse soapResponse(response);
    _sendResponse(&soapResponse);
}

void WsmResponseEncoder::_encodeSoapFaultResponse(SoapFaultResponse* response)
{
    SoapResponse soapResponse(response);
    _sendResponse(&soapResponse);
}

PEGASUS_NAMESPACE_END
