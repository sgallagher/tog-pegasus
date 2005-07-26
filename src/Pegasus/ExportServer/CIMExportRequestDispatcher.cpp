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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Vijay Eli, IBM (vijayeli@in.ibm.com)
//              John Alex, IBM (johnalex@us.ibm.com) - Bug#2290
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Server/CIMOperationRequestDispatcher.h>

#include "CIMExportRequestDispatcher.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportRequestDispatcher::CIMExportRequestDispatcher(
   Boolean dynamicReg, Boolean staticConsumers, Boolean persistence)
   : Base(PEGASUS_QUEUENAME_EXPORTREQDISPATCHER),
     _dynamicReg(dynamicReg),
     _staticConsumers(staticConsumers),
     _persistence(persistence)
{
   PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
      "CIMExportRequestDispatcher::CIMExportRequestDispatcher");
   PEG_METHOD_EXIT();
}

CIMExportRequestDispatcher::CIMExportRequestDispatcher()
   : Base(PEGASUS_QUEUENAME_EXPORTREQDISPATCHER),
     _dynamicReg( true ),
     _staticConsumers( false ),
     _persistence( false )
{
   PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
      "CIMExportRequestDispatcher::CIMExportRequestDispatcher");

   PEG_METHOD_EXIT();
}

CIMExportRequestDispatcher::~CIMExportRequestDispatcher()
{
   PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
      "CIMExportRequestDispatcher::~CIMExportRequestDispatcher");

   PEG_METHOD_EXIT();
}

void CIMExportRequestDispatcher::_handle_async_request(AsyncRequest *req)
{
   PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
      "CIMExportRequestDispatcher::_handle_async_request");

    PEGASUS_ASSERT(req != 0 && req->op != 0 );

    if ( req->getType() == async_messages::CIMSERVICE_STOP )
    {
        req->op->processing();
        handle_CimServiceStop(static_cast<CimServiceStop *>(req));
    }
    else if ( req->getType() == async_messages::ASYNC_LEGACY_OP_START )
    {
        req->op->processing();
        Message *legacy =
            (static_cast<AsyncLegacyOperationStart *>(req)->get_action());
        if (legacy->getType() == CIM_EXPORT_INDICATION_REQUEST_MESSAGE)
        {
            Message* legacy_response = _handleExportIndicationRequest(
                (CIMExportIndicationRequestMessage*) legacy);
            AsyncLegacyOperationResult *async_result =
                new AsyncLegacyOperationResult(
                    req->getKey(),
                    req->getRouting(),
                    req->op,
                    legacy_response);

            _complete_op_node(req->op, ASYNC_OPSTATE_COMPLETE, 0, 0);
            delete legacy;
        }
        else
        {
            Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "CIMExportRequestDispatcher::_handle_async_request got "
                    "unexpected legacy message type '%u'", legacy->getType());
	    _make_response(req, async_results::CIM_NAK);
            delete legacy;
        }
    }
    else
    {
        Base::_handle_async_request(req);
    }
    PEG_METHOD_EXIT();
}

// This callback method is currently unused.  ExportIndication messages
// are passed to the ProviderManager using SendWait rather than SendAsync
// so the responses can be routed correctly.
void CIMExportRequestDispatcher::_forwardRequestCallback(AsyncOpNode *op, 
							 MessageQueue *q, 
							 void *parm)
{
   PEGASUS_ASSERT(0);
#if 0
   CIMExportRequestDispatcher *service = 
      static_cast<CIMExportRequestDispatcher *>(q);

   AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
   AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());

   CIMResponseMessage *response;

   Uint32 msgType =  asyncReply->getType();

    if(msgType == async_messages::ASYNC_LEGACY_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage *>(
            (static_cast<AsyncLegacyOperationResult *>(asyncReply))->
                get_result());
    }
    else if(msgType == async_messages::ASYNC_MODULE_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage *>(
            (static_cast<AsyncModuleOperationResult *>(asyncReply))->
                get_result());
    }
    else
    {
        // Error
        Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "CIMExportRequestDispatcher::_forwardRequestCallback got "
                "unexpected message type '%u'", msgType);
    }

   PEGASUS_ASSERT(response != 0);
   // ensure that the destination queue is in response->dest
#ifdef PEGASUS_POINTER_64BIT  
   response->dest = (Uint64)parm;
#elif PEGASUS_PLATFORM_AIX_RS_IBMCXX
   // We cast to unsigned long
   // because sizeof(void *) == sizeof(unsigned long)
   response->dest = (unsigned long)parm;
#else
   response->dest = (Uint32)parm;
#endif

   if(parm != 0 )
        service->SendForget(response);
    else
        delete response;

    delete asyncRequest;
    delete asyncReply;
    op->release();
    service->return_op(op);
#endif
}

void CIMExportRequestDispatcher::handleEnqueue(Message* message)
{
   PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
      "CIMExportRequestDispatcher::handleEnqueue");

   if( ! message)
   {
      PEG_METHOD_EXIT();
      return;
   }

    switch (message->getType())
    {
	case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
        {
            CIMExportIndicationResponseMessage* response =
                _handleExportIndicationRequest(
                    (CIMExportIndicationRequestMessage*) message);

            Tracer::trace(
                TRC_HTTP,
                Tracer::LEVEL3,
                "_CIMExportRequestDispatcher::handleEnqueue(message) - message->getCloseConnect() returned %d",
                message->getCloseConnect());

            response->setCloseConnect(message->getCloseConnect());

            SendForget(response);
	    break;
        }

        default:
            Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "CIMExportRequestDispatcher::handleEnqueue got unexpected "
                    "message type '%u'", message->getType());
            break;
    }
    delete message;

    PEG_METHOD_EXIT();
}


void CIMExportRequestDispatcher::handleEnqueue()
{
   PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
      "CIMExportRequestDispatcher::handleEnqueue");

   Message *message = dequeue();
   if(message)
      handleEnqueue(message);

   PEG_METHOD_EXIT();
}


CIMExportIndicationResponseMessage*
CIMExportRequestDispatcher::_handleExportIndicationRequest(
    CIMExportIndicationRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
      "CIMExportRequestDispatcher::_handleExportIndicationRequest");

    OperationContext context;

    CIMException cimException;

    Array<Uint32> serviceIds;
    find_services(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    AsyncOpNode * op = this->get_op();

    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart(
	    get_next_xid(),
	    op,
	    serviceIds[0],
	    new CIMExportIndicationRequestMessage(*request),
	    this->getQueueId());

    asyncRequest->dest = serviceIds[0];

    //SendAsync(op,
    //          serviceIds[0],
    //          CIMExportRequestDispatcher::_forwardRequestCallback,
    //          this,
    //          (void *)request->queueIds.top());
    AsyncReply *asyncReply = SendWait(asyncRequest);

    CIMExportIndicationResponseMessage* response =
        reinterpret_cast<CIMExportIndicationResponseMessage *>(
            (static_cast<AsyncLegacyOperationResult *>(
                asyncReply))->get_result());
    response->dest = request->queueIds.top();
    response->synch_response(request);

    delete asyncReply;    // Recipient deletes request
    op->release();
    this->return_op(op);

    PEG_METHOD_EXIT();
    return response;
}

PEGASUS_NAMESPACE_END
