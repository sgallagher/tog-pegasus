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
#include <Pegasus/Common/AutoPtr.h>
#include "WsmConstants.h"
#include "WsmProcessor.h"

PEGASUS_NAMESPACE_BEGIN

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

        // Save the request until the response comes back.
        // Note that the CIM request has its own unique message ID.
        _requestTable.insert(cimRequest->messageId, wsmRequest);
        wsmRequestDestroyer.release();

        cimRequest->queueIds.push(getQueueId());
        _cimOperationProcessorQueue->enqueue(cimRequest);
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
    _requestTable.remove(wsmRequest->messageId);

    try
    {
        AutoPtr<WsmResponse> wsmResponse(
            _cimToWsmResponseMapper.mapToWsmResponse(wsmRequest, cimResponse));

        cimResponse->updateThreadLanguages();
        cimResponse->queueIds.pop();
        _wsmResponseEncoder.enqueue(wsmResponse.get());
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

PEGASUS_NAMESPACE_END
