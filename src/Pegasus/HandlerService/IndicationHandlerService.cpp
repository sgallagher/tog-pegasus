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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/PegasusVersion.h>

#include "IndicationHandlerService.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN

IndicationHandlerService::IndicationHandlerService(void)
   : Base(PEGASUS_QUEUENAME_INDHANDLERMANAGER)
{

}

IndicationHandlerService::IndicationHandlerService(CIMRepository* repository)
   : Base("IndicationHandlerService", MessageQueue::getNextQueueId()),
     _repository(repository)
{
}

void IndicationHandlerService::_handle_async_request(AsyncRequest *req)
{
   if ( req->getType() == async_messages::CIMSERVICE_STOP )
   {
      req->op->processing();
      handle_CimServiceStop(static_cast<CimServiceStop *>(req));
   }
   else if ( req->getType() == async_messages::ASYNC_LEGACY_OP_START )
   {
      req->op->processing();
      Message *legacy = (static_cast<AsyncLegacyOperationStart *>(req)->get_action());
      handleEnqueue(legacy);
      return;
   }
   else
      Base::_handle_async_request(req);
}

void IndicationHandlerService::handleEnqueue(Message* message)
{
   if( ! message )
      return;
   
   switch (message->getType())
   {
      case CIM_HANDLE_INDICATION_REQUEST_MESSAGE:
	 _handleIndication(message);
	 break;
   
      default:
	 break;
   }
   delete message;
}

void IndicationHandlerService::handleEnqueue()
{
   Message * message = dequeue();

   PEGASUS_ASSERT(message != 0);
   if( message )
      handleEnqueue(message);
}


void IndicationHandlerService::_handleIndicationCallBack(AsyncOpNode *op, 
							 MessageQueue *q, 
							 void *parm)
{
   IndicationHandlerService *service = 
      static_cast<IndicationHandlerService *>(q);

   AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
   AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());
   CIMRequestMessage *request = reinterpret_cast<CIMRequestMessage *>
      ((static_cast<AsyncLegacyOperationStart *>(asyncRequest))->get_action());
   CIMResponseMessage *response = reinterpret_cast<CIMResponseMessage *>
      ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
   PEGASUS_ASSERT(response != 0);
   // ensure that the destination queue is in response->dest
#ifdef PEGASUS_ARCHITECTURE_IA64   
   response->dest = (Uint64)parm;
#else
   response->dest = (Uint32)parm;
#endif
   service->SendForget(response);
   delete asyncRequest;
   delete asyncReply;
//   op->release();
//   service->return_op(op);
}


void IndicationHandlerService::_handleIndication(const Message* message)
{
   CIMHandleIndicationRequestMessage* request = 
      (CIMHandleIndicationRequestMessage*) message;

   CIMException cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_SUCCESS, String::EMPTY);

   String className = request->handlerInstance.getClassName();
    
   String nameSpace = request->nameSpace;

   CIMInstance indication = request->indicationInstance;
   CIMInstance handler = request->handlerInstance;

   Uint32 pos = PEG_NOT_FOUND;

   if (className == String("PG_IndicationHandlerCIMXML"))
       pos = handler.findProperty("destination");
   else if (className == String("PG_IndicationHandlerSNMPMapper"))
       pos = handler.findProperty("TrapDestination");

   if (pos == PEG_NOT_FOUND)
   {
       cimException = 
           PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String("Handler without destination"));
   }
   else
   {
       CIMProperty prop = handler.getProperty(pos);
       String destination = prop.getValue().toString();

       if (destination.size() == 0)
       {
           cimException = 
               PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String("invalid destination"));
       }
       else if ((className == "PG_IndicationHandlerCIMXML") &&
           (destination.subString(0, 9) == String("localhost")))
       {
          Array<Uint32> exportServer;

          find_services(PEGASUS_QUEUENAME_EXPORTREQDISPATCHER, 0, 0, &exportServer);

          // Listener is build with Cimom, so send message to ExportServer
	
          CIMExportIndicationRequestMessage* exportmessage =
	     new CIMExportIndicationRequestMessage(
	        "1234",
	        destination.subString(15), //taking localhost:5988 portion out from reg
	        indication,
	        QueueIdStack(exportServer[0], getQueueId()));
	
          AsyncOpNode* op = this->get_op();

          AsyncLegacyOperationStart *req =
	     new AsyncLegacyOperationStart(
	        get_next_xid(),
	        op,
	        exportServer[0],
	        exportmessage,
	        _queueId);

          PEG_TRACE_STRING(TRC_IND_HANDLE, Tracer::LEVEL4, 
		       "Indication handler forwarding message to " +
		       ((MessageQueue::lookup(exportServer[0])) ? 
			String( ((MessageQueue::lookup(exportServer[0]))->getQueueName()) ) : 
			String("BAD queue name")));
            
          SendAsync(op, 
		exportServer[0],
		IndicationHandlerService::_handleIndicationCallBack,
		this, 
		(void *)request->queueIds.top());

       }
       else
       {
          // generic handler. So load it and let it to do.
          CIMHandler* handlerLib = _lookupHandlerForClass(className);

          if (handlerLib)
          {
	     handlerLib->handleIndication(
	        handler,
	        indication,
	        nameSpace);
          }
          else
             cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String("Failed to load Handler"));
       }
   }

   CIMExportIndicationResponseMessage* response =
         new CIMExportIndicationResponseMessage(
            request->messageId,
            cimException,
            request->queueIds.copyAndPop());

    _enqueueResponse(request, response);
}

CIMHandler* IndicationHandlerService::_lookupHandlerForClass(
   const String& className)
{
   String handlerId;

   if (className == String("PG_IndicationHandlerCIMXML"))
       handlerId = String("CIMxmlIndicationHandler");
   else if (className == String("PG_IndicationHandlerSNMPMapper"))
       handlerId = String("snmpIndicationHandler");
   else
       return 0;

   CIMHandler* handler = _handlerTable.lookupHandler(handlerId);

   if (!handler)
   {
      handler = _handlerTable.loadHandler(handlerId);

      if (!handler)
	 throw CIMException(CIM_ERR_FAILED);

      handler->initialize(_repository);
   }

   return handler;
}

PEGASUS_NAMESPACE_END
