//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//				Seema Gupta (gseema@in.ibm.com) for PEP135
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/PegasusVersion.h>

#include "IndicationHandlerService.h"

// l10n
#include <Pegasus/Common/MessageLoader.h>

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
        Message *legacy =
            (static_cast<AsyncLegacyOperationStart *>(req)->get_action());
        if (legacy->getType() == CIM_HANDLE_INDICATION_REQUEST_MESSAGE)
        {
            Message* legacy_response = _handleIndication(
                (CIMHandleIndicationRequestMessage*) legacy);
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
                "IndicationHandlerService::_handle_async_request got "
                    "unexpected legacy message type '%u'", legacy->getType());
            _make_response(req, async_results::CIM_NAK);
            delete legacy;
        }
    }
    else
    {
        Base::_handle_async_request(req);
    }
}

void IndicationHandlerService::handleEnqueue(Message* message)
{
    if (!message)
    {
        return;
    }

// l10n
    // Set the client's requested language into this service thread.
    // This will allow functions in this service to return messages
    // in the correct language.
    CIMMessage * msg = dynamic_cast<CIMMessage *>(message);
    if (msg != NULL)
    {
        if (msg->thread_changed())
        {
            AcceptLanguages *langs =  new AcceptLanguages(((AcceptLanguageListContainer)msg->operationContext.get
											(AcceptLanguageListContainer::NAME)).getLanguages());
            Thread::setLanguages(langs);
        }
    }
    else
    {
        Thread::clearLanguages();
    }

    switch (message->getType())
    {
        case CIM_HANDLE_INDICATION_REQUEST_MESSAGE:
        {
            CIMHandleIndicationResponseMessage* response = _handleIndication(
                (CIMHandleIndicationRequestMessage*) message);
            SendForget(response);
            break;
        }

        default:
            Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "IndicationHandlerService::handleEnqueue got unexpected "
                    "message type '%u'", message->getType());
            break;
    }

    delete message;
}

void IndicationHandlerService::handleEnqueue()
{
   Message * message = dequeue();

   PEGASUS_ASSERT(message != 0);
   if (message)
   {
       handleEnqueue(message);
   }
}

// This callback method is currently unused.  ExportIndication messages
// are passed to the CIMExportRequestDispatcher using SendWait rather than
// SendAsync so the responses can be routed correctly.
void IndicationHandlerService::_handleIndicationCallBack(AsyncOpNode *op,
                                                         MessageQueue *q,
                                                         void *parm)
{
   PEGASUS_ASSERT(0);
#if 0
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
#elif PEGASUS_PLATFORM_AIX_RS_IBMCXX
   // We cast to unsigned long
   // because sizeof(void *) == sizeof(unsigned long)
   response->dest = (unsigned long)parm;
#else
   response->dest = (Uint32)parm;
#endif
   service->SendForget(response);
   delete asyncRequest;
   delete asyncReply;
//   op->release();
//   service->return_op(op);
#endif
}


String IndicationHandlerService::_parseDestination(String dest)
{
    CString pCString = dest.getCString();
    char* p = const_cast<char*>((const char*) pCString);

    static char schemeType[] = "HTTP:";
    Boolean hasSchemeType = true;
    for(int i= 0; i<5; i++)
    {
        if(::toupper(p[i]) != schemeType[i])
        {
            hasSchemeType = false;
            break;
        }
    }
    if(hasSchemeType)
    {
        p += 5;
    }

    // See if there is a host name begins with "//":
    if (p[0] == '/' && p[1] == '/')
    {
        p += 2;
    }
    return String(p);
}

CIMHandleIndicationResponseMessage*
IndicationHandlerService::_handleIndication(
    CIMHandleIndicationRequestMessage* request)
{
    PEG_METHOD_ENTER (TRC_IND_HANDLE,
                      "IndicationHandlerService::_handleIndication");

    CIMException cimException =
        PEGASUS_CIM_EXCEPTION(CIM_ERR_SUCCESS, String::EMPTY);

    CIMName className = request->handlerInstance.getClassName();
    CIMNamespaceName nameSpace = request->nameSpace;

    CIMInstance indication = request->indicationInstance;
    CIMInstance handler = request->handlerInstance;

    Uint32 pos = PEG_NOT_FOUND;

    if (className.equal (PEGASUS_CLASSNAME_INDHANDLER_CIMXML) ||
        className.equal (PEGASUS_CLASSNAME_LSTNRDST_CIMXML))
    {
        pos = handler.findProperty(CIMName ("destination"));
    }
    else if (className.equal (PEGASUS_CLASSNAME_INDHANDLER_SNMP))
    {
        pos = handler.findProperty(CIMName ("TargetHost"));
    }

    if (pos == PEG_NOT_FOUND)
    {
        // l10n
        // cimException =
        //   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String("Handler without destination"));

        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("HandlerService.IndicationHandlerService."
                "HANDLER_WITHOUT_DESTINATION", "Handler without destination"));
    }
    else
    {
        CIMProperty prop = handler.getProperty(pos);
        String destination = prop.getValue().toString();

        //filter out http:// // ATTN: Do not enable yet.
        // destination = _parseDestination(destination);

        if (destination.size() == 0)
        {
            // l10n

            // cimException =
            //   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String("invalid destination"));

            cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms("HandlerService.IndicationHandlerService."
                    "INVALID_DESTINATION", "invalid destination"));
        }
        else if ((className.equal (PEGASUS_CLASSNAME_INDHANDLER_CIMXML) ||
                  className.equal (PEGASUS_CLASSNAME_LSTNRDST_CIMXML)) &&
//compared index 10 is not :
                 (destination.subString(0, 10) == String("localhost/")))
        {
            Array<Uint32> exportServer;

            find_services(PEGASUS_QUEUENAME_EXPORTREQDISPATCHER, 0, 0,
                          &exportServer);

            // Listener is build with Cimom, so send message to ExportServer

            CIMExportIndicationRequestMessage* exportmessage =
                new CIMExportIndicationRequestMessage(
                    XmlWriter::getNextMessageId(),
                    destination.subString(21), //taking localhost/CIMListener portion out from reg
                    indication,
                    QueueIdStack(exportServer[0], getQueueId()),
                    String::EMPTY,
                    String::EMPTY,
					((ContentLanguageListContainer)request->operationContext.get
								(ContentLanguageListContainer::NAME)).getLanguages());

			exportmessage->operationContext.set(request->operationContext.get(ContentLanguageListContainer::NAME)); 
            AsyncOpNode* op = this->get_op();

            AsyncLegacyOperationStart *asyncRequest =
                new AsyncLegacyOperationStart(
                    get_next_xid(),
                    op,
                    exportServer[0],
                    exportmessage,
                    _queueId);

            PEG_TRACE_STRING(TRC_IND_HANDLE, Tracer::LEVEL4,
               "Indication handler forwarding message to " +
               ((MessageQueue::lookup(exportServer[0])) ?
                   String( ((MessageQueue::lookup(exportServer[0]))->
                       getQueueName()) ) :
                   String("BAD queue name")));

            //SendAsync(op,
            //      exportServer[0],
            //      IndicationHandlerService::_handleIndicationCallBack,
            //      this,
            //      (void *)request->queueIds.top());
            AsyncReply *asyncReply = SendWait(asyncRequest);

            // Return the ExportIndication results in HandleIndication response
            CIMExportIndicationResponseMessage* exportResponse =
                reinterpret_cast<CIMExportIndicationResponseMessage *>(
                    (static_cast<AsyncLegacyOperationResult *>(
                        asyncReply))->get_result());
            cimException = exportResponse->cimException;

            delete exportResponse;
            delete asyncReply;
            op->release();
            this->return_op(op);
        }
        else
        {
            // generic handler. So load it and let it to do.
            CIMHandler* handlerLib = _lookupHandlerForClass(className);

            if (handlerLib)
            {
                try
                {
					ContentLanguages langs = ((ContentLanguageListContainer)request->operationContext.
												get(ContentLanguageListContainer::NAME)).getLanguages(); 
                    handlerLib->handleIndication(
                        request->operationContext,
                        handler,
                        indication,
                        nameSpace.getString(),
						langs);
                 }
                catch(CIMException& e)
                {
                    cimException =
                        PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
                }
            }
            else
            {
                // l10n

                // cimException =
                // PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String("Failed to load Handler"));

                cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                    MessageLoaderParms("HandlerService."
                        "IndicationHandlerService.FAILED_TO_LOAD",
                        "Failed to load Handler"));
            }
        }
    }

    CIMHandleIndicationResponseMessage* response =
        new CIMHandleIndicationResponseMessage(
            request->messageId,
            cimException,
            request->queueIds.copyAndPop());

    return response;
}

CIMHandler* IndicationHandlerService::_lookupHandlerForClass(
   const CIMName& className)
{
   String handlerId;

   if (className.equal (PEGASUS_CLASSNAME_INDHANDLER_CIMXML)
      || className.equal (PEGASUS_CLASSNAME_LSTNRDST_CIMXML)
      )
       handlerId = String("CIMxmlIndicationHandler");
   else if (className.equal (PEGASUS_CLASSNAME_INDHANDLER_SNMP))
       handlerId = String("snmpIndicationHandler");
   else
       return 0;

   CIMHandler* handler = _handlerTable.lookupHandler(handlerId);

   if (!handler)
   {
      handler = _handlerTable.loadHandler(handlerId);

      if (!handler)
          throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);

      handler->initialize(_repository);
   }

   return handler;
}

PEGASUS_NAMESPACE_END


