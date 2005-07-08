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
// Author: Dong Xiang, EMC Corporation (xiang_dong@emc.com)
//
// Modified By: Heather Sterling, IBM (hsterl@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "DynamicListenerIndicationDispatcher.h"
#include "DynamicConsumer.h"
#include "ConsumerManager.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Tracer.h>

#include <Pegasus/Listener/List.h>
#include <Pegasus/Consumer/CIMIndicationConsumer.h>
#include <Pegasus/Common/ContentLanguages.h>

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

#define PEGASUS_QUEUE_NAME "DynamicListenerIndicationDispatcher"


DynamicListenerIndicationDispatcher::DynamicListenerIndicationDispatcher(ConsumerManager* consumerManager) :
Base(PEGASUS_QUEUE_NAME),
_consumerManager(consumerManager)
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicListenerIndicationDispatcher::DynamicListenerIndicationDispatcher");

    PEG_METHOD_EXIT();
}


DynamicListenerIndicationDispatcher::~DynamicListenerIndicationDispatcher()
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicListenerIndicationDispatcher::~DynamicListenerIndicationDispatcher");

    PEG_METHOD_EXIT();
}

void DynamicListenerIndicationDispatcher::handleEnqueue()
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicListenerIndicationDispatcher::handleEnqueue");

    Message *message = dequeue();
    if (message)
        handleEnqueue(message);

    PEG_METHOD_EXIT();
}

void DynamicListenerIndicationDispatcher::handleEnqueue(Message* message)
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicListenerIndicationDispatcher::handleEnqueue");

    if (message!=NULL)
    {
        switch (message->getType())
        {
        case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
            {
                CIMExportIndicationRequestMessage* request = (CIMExportIndicationRequestMessage*)message;
                CIMException cimException;

                try
                {
                    _handleIndicationRequest(request);

                } catch (Exception& ex)
                {
                    PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL2, "Exception getting consumer: " + ex.getMessage());
                    cimException = CIMException(CIM_ERR_FAILED, ex.getMessage());

                    Logger::put(
                               Logger::ERROR_LOG,
                               "DynamicListenerIndicationDispatcher",
                               Logger::SEVERE,
                               "Exception getting consumer: $0", 
                               ex.getMessage());

                } catch (...)
                {
                    PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL2, "Exception getting consumer: Unknown");
                    cimException = CIMException(CIM_ERR_FAILED, "Unknown exception");

                    Logger::put(
                               Logger::ERROR_LOG,
                               "DynamicListenerIndicationDispatcher",
                               Logger::SEVERE,
                               "Unknown Exception getting consumer");
                }

                //At this point (barring one of the above exceptions), we can be reasonably sure that the 
                //indication will get delivered and processed. The request was well-formatted and we 
                //were able to locate and load the consumer.  Send an acknowledgement to the client
                //that we received the indication. 
                //We should not wait until the consumer reports ultimate success since that could take a long
                //time and would require us to store a bunch of status information.  Additionally, the wait
                //could cause a timeout exception on the client end.

                //ATTN: Why isn't the CIM exception getting appended to the response?  Go look in Message.h
                CIMExportIndicationResponseMessage* response = new CIMExportIndicationResponseMessage(
                                                                                                     request->messageId,
                                                                                                     cimException,
                                                                                                     request->queueIds.copyAndPop());

                response->dest = request->queueIds.top();
                _enqueueResponse(request, response);


            }
            break;
        default:
            {
                //unsupported message type
                //it should not get here; this error is caught in the request decoder
                PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL2, 
                                 "Unsupported msg type: " + String(MessageTypeToString(message->getType())));

                CIMRequestMessage* cimRequest = dynamic_cast<CIMRequestMessage*>(message);

                CIMResponseMessage* response = cimRequest->buildResponse();
                response->cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, 
                                                                 MessageLoaderParms("DynListener.DynamicListenerIndicationDispatcher.INVALID_MSG_TYPE", 
                                                                                    "Invalid message type"));

                _enqueueResponse (cimRequest, response);
            }
            break;
        }   
        delete message;
    }

    PEG_METHOD_EXIT();
}

void DynamicListenerIndicationDispatcher::_handleIndicationRequest(CIMExportIndicationRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicListenerIndicationDispatcher::handleIndicationRequest");

    OperationContext context = request->operationContext;
    String url = request->destinationPath;
    CIMInstance instance = request->indicationInstance;

    PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL4, "URL is " + url);

    Uint32 slash = url.find("/");
    if (slash == PEG_NOT_FOUND)
    {
        Logger::put(
                   Logger::ERROR_LOG,
                   "DynamicListenerIndicationDispatcher",
                   Logger::SEVERE,
                   "Invalid URL $0", 
                   url);

        MessageLoaderParms msgLoaderParms(
                                         "DynListener.DynamicListenerIndicationDispatcher.BAD_URL",
                                         "Invalid CIMXMLIndicationHandler destination: $0.",
                                         url);

        throw CIMException(CIM_ERR_FAILED, msgLoaderParms);
    }

    String consumerName = url.subString(slash+1);

    //check for a trailing slash, in the case that additional information is in the URL, i.e. /CIMListener/MyConsumer/9.44.169.132
    Uint32 trailingSlash = consumerName.find('/');
    if (trailingSlash != PEG_NOT_FOUND)
    {
        consumerName = consumerName.subString(0, trailingSlash);
        PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL2, "The consumer name with slash removed is!" + consumerName + "!");
    }
    
    //get consumer
    //this will throw an exception if it fails
    //gets deleted by the ConsumerManager
    DynamicConsumer* consumer = _consumerManager->getConsumer(consumerName);

    //deliver indication to consumer
    //gets deleted by the DynamicConsumer
    IndicationDispatchEvent* event = new IndicationDispatchEvent(request->operationContext,
                                                                 request->destinationPath,
                                                                 request->indicationInstance);

    //enqueue event
    consumer->enqueueEvent(event);

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
