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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Provider/CIMOMHandle.h>
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

    if ( req->getType() == async_messages::CIMSERVICE_STOP )
    {
        req->op->processing();
        handle_CimServiceStop(static_cast<CimServiceStop *>(req));
    }
    else if ( req->getType() == async_messages::ASYNC_LEGACY_OP_START )
    {
       try 
       {
        req->op->processing();
        Message *legacy = (static_cast<AsyncLegacyOperationStart *>(req)->get_action());
	handleEnqueue(legacy);
       }
       catch(Exception & )
       {
	  _make_response(req, async_results::CIM_NAK);
       }
        PEG_METHOD_EXIT();
        return;
    }
    else
    {
        Base::_handle_async_request(req);
    }
    PEG_METHOD_EXIT();
}

void CIMExportRequestDispatcher::_forwardRequestCallback(AsyncOpNode *op, 
							 MessageQueue *q, 
							 void *parm)
{
   CIMExportRequestDispatcher *service = 
      static_cast<CIMExportRequestDispatcher *>(q);

   AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
   AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());

   CIMResponseMessage *response;

   Uint32 msgType =  asyncReply->getType();

    if(msgType == async_messages::ASYNC_LEGACY_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage *>
            ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result())
;
    }
    else if(msgType == async_messages::ASYNC_MODULE_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage *>
            ((static_cast<AsyncModuleOperationResult *>(asyncReply))->get_result())
;
    }
    else
    {
        // Error
    }

   PEGASUS_ASSERT(response != 0);
   // ensure that the destination queue is in response->dest
#ifdef PEGASUS_ARCHITECTURE_IA64   
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
	    _handleExportIndicationRequest(
		(CIMExportIndicationRequestMessage*) message);
	    break;

        default:
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


void CIMExportRequestDispatcher::_handleExportIndicationRequest(
    CIMExportIndicationRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_EXP_REQUEST_DISP,
      "CIMExportRequestDispatcher::handleEnqueue");

    OperationContext context;

    CIMException cimException;

    Array<Uint32> serviceIds;
    find_services(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    CIMExportIndicationRequestMessage* request_copy =
	new CIMExportIndicationRequestMessage(*request);
 
    AsyncOpNode * op = this->get_op();

    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart(
	    get_next_xid(),
	    op,
	    serviceIds[0],
	    request_copy,
	    this->getQueueId());

    asyncRequest->dest = serviceIds[0];

    SendAsync(op,
	      serviceIds[0],
	      CIMExportRequestDispatcher::_forwardRequestCallback,
	      this,
	      (void *)request->queueIds.top());


    //
    //  Set response destination
    //
    PEG_TRACE_STRING(TRC_EXP_REQUEST_DISP, Tracer::LEVEL4, 
		     "CIMExportRequestDispatcher setting export indication response dest " +
		     ((MessageQueue::lookup(request->queueIds.top())) ? 
		      String( ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) ) : 
		      String("BAD queue name")));
    
    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
