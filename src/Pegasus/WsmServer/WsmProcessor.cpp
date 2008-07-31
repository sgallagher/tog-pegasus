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

#include <cctype>
#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Common/AutoPtr.h>
#include "WsmConstants.h"
#include "WsmProcessor.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

Uint64 WsmProcessor::_currentEnumContext = 0;

WsmProcessor::WsmProcessor(
    MessageQueueService* cimOperationProcessorQueue,
    CIMRepository* repository)
    : MessageQueueService(PEGASUS_QUEUENAME_WSMPROCESSOR),
      _wsmResponseEncoder(),
      _wsmRequestDecoder(this),
      _cimOperationProcessorQueue(cimOperationProcessorQueue),
      _repository(repository),
      _wsmToCimRequestMapper(repository)
{
}

WsmProcessor::~WsmProcessor()
{
}

void WsmProcessor::handleEnqueue(Message* message)
{
    if (!message)
    {
        return;
    }

    PEGASUS_ASSERT(dynamic_cast<CIMResponseMessage*>(message) != 0);
    handleResponse(dynamic_cast<CIMResponseMessage*>(message));
}

void WsmProcessor::handleEnqueue()
{
    Message* message = dequeue();
    handleEnqueue(message);
}

void WsmProcessor::handleRequest(WsmRequest* wsmRequest)
{
    PEG_METHOD_ENTER(TRC_WSMSERVER, "WsmProcessor::handleRequest()");

    // Process requests by type.  For now, only WS-Transfer operations are
    // implemented, and they all are handled by forwarding to the CIM Server.

    AutoPtr<WsmRequest> wsmRequestDestroyer(wsmRequest);

    try
    {
        CIMOperationRequestMessage* cimRequest =
            _wsmToCimRequestMapper.mapToCimRequest(wsmRequest);

        // Requests that do not have a CIM representation are mapped to NULL
        // and are meant to be handled by the WSM processor itself.
        if (cimRequest)
        {
            // Save the request until the response comes back.
            // Note that the CIM request has its own unique message ID.
            _requestTable.insert(cimRequest->messageId, wsmRequest);

            cimRequest->queueIds.push(getQueueId());
            _cimOperationProcessorQueue->enqueue(cimRequest);
        }
        else
        {
            switch (wsmRequest->getType())
            {
                case WS_ENUMERATION_PULL:
                    _handlePullRequest((WsenPullRequest*) wsmRequest);
                    break;

                case WS_ENUMERATION_RELEASE:
                    _handleReleaseRequest((WsenReleaseRequest*) wsmRequest);
                    break;

                default:
                    break;
            }
        }

        wsmRequestDestroyer.release();
    }
    catch (WsmFault& fault)
    {
        sendResponse(new WsmFaultResponse(wsmRequest, fault));
    }
    catch (CIMException& e)
    {
        sendResponse(new WsmFaultResponse(
            wsmRequest,
            _cimToWsmResponseMapper.mapCimExceptionToWsmFault(e)));
    }
    catch (Exception& e)
    {
        sendResponse(new WsmFaultResponse(
            wsmRequest,
            WsmFault(
                WsmFault::wsman_InternalError,
                e.getMessage(),
                e.getContentLanguages())));
    }
    catch (PEGASUS_STD(exception)& e)
    {
        sendResponse(new WsmFaultResponse(
            wsmRequest,
            WsmFault(WsmFault::wsman_InternalError, e.what())));
    }
    catch (...)
    {
        sendResponse(new WsmFaultResponse(
            wsmRequest,
            WsmFault(WsmFault::wsman_InternalError)));
    }

    // Note this requirement when Enumerate/Pull operations are supported:
    // DSP0226 R6.3-5: For operations that span multiple message sequences,
    // the wsman:Locale element is processed in the initial message only.
    // It should be ignored in subsequent messages because the first
    // message establishes the required locale. The service may issue a
    // fault if the wsman:Locale is present in subsequent messages and the
    // value is different from that used in the initiating request.

    PEG_METHOD_EXIT();
}

void WsmProcessor::handleResponse(CIMResponseMessage* cimResponse)
{
    PEG_METHOD_ENTER(TRC_WSMSERVER, "WsmProcessor::handleResponse()");

    AutoPtr<CIMResponseMessage> cimResponseDestroyer(cimResponse);

    // Lookup the request this response corresponds to
    WsmRequest* wsmRequest;
    Boolean gotRequest =
        _requestTable.lookup(cimResponse->messageId, wsmRequest);
    PEGASUS_ASSERT(gotRequest);
    AutoPtr<WsmRequest> wsmRequestDestroyer(wsmRequest);
    _requestTable.remove(cimResponse->messageId);

    try
    {
        switch (wsmRequest->getType())
        {
            case WS_ENUMERATION_ENUMERATE:
                _handleEnumerateResponse(
                    cimResponse,
                    (WsenEnumerateRequest*) wsmRequest);
                break;

            default:
                _handleDefaultResponse(cimResponse, wsmRequest);
                break;
        }
    }
    catch (WsmFault& fault)
    {
        sendResponse(new WsmFaultResponse(wsmRequest, fault));
    }
    catch (CIMException& e)
    {
        sendResponse(new WsmFaultResponse(
            wsmRequest,
            _cimToWsmResponseMapper.mapCimExceptionToWsmFault(e)));
    }
    catch (Exception& e)
    {
        sendResponse(new WsmFaultResponse(
            wsmRequest,
            WsmFault(
                WsmFault::wsman_InternalError,
                e.getMessage(),
                e.getContentLanguages())));
    }
    catch (PEGASUS_STD(exception)& e)
    {
        sendResponse(new WsmFaultResponse(
            wsmRequest,
            WsmFault(WsmFault::wsman_InternalError, e.what())));
    }
    catch (...)
    {
        sendResponse(new WsmFaultResponse(
            wsmRequest,
            WsmFault(WsmFault::wsman_InternalError)));
    }

    PEG_METHOD_EXIT();
}

void WsmProcessor::sendResponse(WsmResponse* wsmResponse)
{
    PEG_METHOD_ENTER(TRC_WSMSERVER, "WsmProcessor::sendResponse()");

    _wsmResponseEncoder.enqueue(wsmResponse);
    delete wsmResponse;

    PEG_METHOD_EXIT();
}

Uint32 WsmProcessor::getWsmRequestDecoderQueueId()
{
    return _wsmRequestDecoder.getQueueId();
}

void WsmProcessor::_handleEnumerateResponse(
    CIMResponseMessage* cimResponse,
    WsenEnumerateRequest* wsmRequest)
{
    if (cimResponse->cimException.getCode() != CIM_ERR_SUCCESS)
    {
        _handleDefaultResponse(cimResponse, wsmRequest);
    }
    else
    {
        AutoMutex lock(_enumerationContextTableLock);
        Uint64 contextId = _currentEnumContext++;

        AutoPtr<WsenEnumerateResponse> wsmResponse(
            (WsenEnumerateResponse*) _cimToWsmResponseMapper.
                mapToWsmResponse(wsmRequest, cimResponse));

        // Create a new context
        _enumerationContextTable.insert(
            contextId,
            EnumerationContext(
                contextId,
                wsmRequest->enumerationMode,
                wsmRequest->expiration, 
                wsmRequest->epr,
                wsmResponse.get()));
        wsmResponse->setEnumerationContext(contextId);

        // Get the requsted chunk of results
        AutoPtr<WsenEnumerateResponse> splitResponse(
            _splitEnumerateResponse(wsmRequest, wsmResponse.get(), 
                wsmRequest->optimized ? wsmRequest->maxElements : 0));
        splitResponse->setEnumerationContext(contextId);

        // If no items are left in the orignal response, mark split 
        // response as complete
        if (wsmResponse->getSize() == 0)
        {
            splitResponse->setComplete();
        }

        _wsmResponseEncoder.enqueue(splitResponse.get());
        if (splitResponse->getSize() > 0)
        {
            // Add unprocessed items back to the context
            wsmResponse->merge(splitResponse.get());
        }

        // Remove the context if there are no instances left
        if (wsmResponse->getSize() == 0)
        {
            _enumerationContextTable.remove(contextId);
        }
        else
        {
            // If the context is not removed, the pointer to the response is
            // now owned by the context
            wsmResponse.release();
        }
    }
}

void WsmProcessor::_handlePullRequest(WsenPullRequest* wsmRequest)
{
    EnumerationContext* enumContext;
    AutoMutex lock(_enumerationContextTableLock);

    if (_enumerationContextTable.lookupReference(
            wsmRequest->enumerationContext, enumContext))
    {
        // EPRs of the request and the enumeration context must match
        if (wsmRequest->epr != enumContext->epr)
        {
            throw WsmFault(
                WsmFault::wsa_MessageInformationHeaderRequired,
                MessageLoaderParms(
                    "WsmServer.WsmProcessor.INVALID_PULL_EPR",
                    "EPR of a Pull request does not match that of "
                    "an enumeration context."));
        }

        AutoPtr<WsenPullResponse> wsmResponse(_splitPullResponse(
            wsmRequest, enumContext->response, wsmRequest->maxElements));
        wsmResponse->setEnumerationContext(enumContext->contextId);
        if (enumContext->response->getSize() == 0)
        {
            wsmResponse->setComplete();
        }

        _wsmResponseEncoder.enqueue(wsmResponse.get());
        if (wsmResponse->getSize() > 0)
        {
            // Add unprocessed items back to the context
            enumContext->response->merge(wsmResponse.get());
        }

        // Remove the context if there are no instances left
        if (enumContext->response->getSize() == 0)
        {
            delete enumContext->response;
            _enumerationContextTable.remove(wsmRequest->enumerationContext);
        }
    }
    else
    {
        throw WsmFault(
            WsmFault::wsen_InvalidEnumerationContext,
            MessageLoaderParms(
                "WsmServer.WsmProcessor.INVALID_ENUMERATION_CONTEXT",
                "Enumeration context \"$0\" is not valid.",
                wsmRequest->enumerationContext));
    }
}

void WsmProcessor::_handleReleaseRequest(WsenReleaseRequest* wsmRequest)
{
    EnumerationContext enumContext;
    AutoMutex lock(_enumerationContextTableLock);
    if (_enumerationContextTable.lookup(
            wsmRequest->enumerationContext, enumContext))
    {
        // EPRs of the request and the enumeration context must match
        if (wsmRequest->epr != enumContext.epr)
        {
            throw WsmFault(
                WsmFault::wsa_MessageInformationHeaderRequired,
                MessageLoaderParms(
                    "WsmServer.WsmProcessor.INVALID_PULL_EPR",
                    "EPR of a Pull request does not match that of "
                    "an enumeration context."));
        }

        AutoPtr<WsenReleaseResponse> wsmResponse(new WsenReleaseResponse(
            wsmRequest, enumContext.response->getContentLanguages()));

        _enumerationContextTable.remove(wsmRequest->enumerationContext);

        _wsmResponseEncoder.enqueue(wsmResponse.get());
    }
    else
    {
        throw WsmFault(
            WsmFault::wsen_InvalidEnumerationContext,
            MessageLoaderParms(
                "WsmServer.WsmProcessor.INVALID_ENUMERATION_CONTEXT",
                "Enumeration context \"$0\" is not valid.",
                wsmRequest->enumerationContext));
    }
}

void WsmProcessor::_handleDefaultResponse(
    CIMResponseMessage* cimResponse, WsmRequest* wsmRequest)
{
    AutoPtr<WsmResponse> wsmResponse(
        _cimToWsmResponseMapper.mapToWsmResponse(wsmRequest, cimResponse));

    cimResponse->updateThreadLanguages();
    cimResponse->queueIds.pop();

    _wsmResponseEncoder.enqueue(wsmResponse.get());
}

WsenEnumerateResponse* WsmProcessor::_splitEnumerateResponse(
    WsenEnumerateRequest* request, WsenEnumerateResponse* response, Uint32 num)
{
    WsenEnumerationData splitData;
    response->getEnumerationData().split(splitData, num);

    return new WsenEnumerateResponse(splitData, response->getItemCount(),
        request, response->getContentLanguages());
}

WsenPullResponse* WsmProcessor::_splitPullResponse(
    WsenPullRequest* request, WsenEnumerateResponse* response, Uint32 num)
{
    WsenEnumerationData splitData;
    response->getEnumerationData().split(splitData, num);

    return new WsenPullResponse(splitData, request, 
        response->getContentLanguages());
}

PEGASUS_NAMESPACE_END
