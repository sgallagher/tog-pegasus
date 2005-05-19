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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//				Seema Gupta (gseema@in.ibm.com) for PEP135
//              Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP101
//              Amit K Arora, IBM (amitarora@in.ibm.com) for Bug#2487
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/Constants.h>

#include "IndicationHandlerService.h"

// l10n
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/AutoPtr.h>
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
        AutoPtr<Message> legacy(static_cast<AsyncLegacyOperationStart *>(req)->get_action()); //PEP 101
        if (legacy->getType() == CIM_HANDLE_INDICATION_REQUEST_MESSAGE)
        {
            AutoPtr<Message> legacy_response(_handleIndication(
                (CIMHandleIndicationRequestMessage*) legacy.get())); //PEP 101
            legacy.release();
            AutoPtr<AsyncLegacyOperationResult> async_result(new AsyncLegacyOperationResult(
                    req->getKey(),
                    req->getRouting(),
                    req->op,
                    legacy_response.get())); //PEP 101
            legacy_response.release();
            async_result.release();
            _complete_op_node(req->op, ASYNC_OPSTATE_COMPLETE, 0, 0);
        }
        else
        {
            Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "IndicationHandlerService::_handle_async_request got "
                    "unexpected legacy message type '%u'", legacy->getType());
            _make_response(req, async_results::CIM_NAK);
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
    AutoPtr<CIMMessage>   msg(dynamic_cast<CIMMessage *>(message));
    if (msg.get() != NULL)
    {
        if (msg->thread_changed())
        {
            AutoPtr<AcceptLanguages> langs(new AcceptLanguages(((AcceptLanguageListContainer)msg->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages()));
            Thread::setLanguages(langs.get());
            langs.release();
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
            AutoPtr<CIMHandleIndicationResponseMessage> response(_handleIndication(
                (CIMHandleIndicationRequestMessage*) message));
            SendForget(response.get());
            response.release();
            break;
        }

        default:
            Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "IndicationHandlerService::handleEnqueue got unexpected "
                    "message type '%u'", message->getType());
            break;
    }

}

void IndicationHandlerService::handleEnqueue()
{
   AutoPtr<Message> message(dequeue());

   PEGASUS_ASSERT(message.get() != 0);
   if (message.get())
   {
       handleEnqueue(message.get());
       message.release();
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
   AutoPtr<IndicationHandlerService> service(static_cast<IndicationHandlerService *>(q));

   AutoPtr<AsyncRequest> asyncRequest(static_cast<AsyncRequest *>(op->get_request()));
   AutoPtr<AsyncReply> asyncReply(static_cast<AsyncReply *>(op->get_response()));
   AutoPtr<CIMRequestMessage> request(reinterpret_cast<CIMRequestMessage *>
      ((static_cast<AsyncLegacyOperationStart *>(asyncRequest))->get_action()));
   AutoPtr<CIMResponseMessage> response(reinterpret_cast<CIMResponseMessage *>
      ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result()));
   PEGASUS_ASSERT(response.get() != 0);
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
   service->SendForget(response.get());
   response.release();
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
        pos = handler.findProperty(PEGASUS_PROPERTYNAME_LSTNRDST_DESTINATION);

        if (pos == PEG_NOT_FOUND)
        {
            cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms("HandlerService.IndicationHandlerService."
                "CIMXML_HANDLER_WITHOUT_DESTINATION", "CIMXml Handler missing Destination property"));
        }
        else
        {
            CIMProperty prop = handler.getProperty(pos);
            String destination = prop.getValue().toString();

            if (destination.size() == 0)
            {
                cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                    MessageLoaderParms("HandlerService.IndicationHandlerService."
                    "INVALID_DESTINATION", "invalid destination"));
            }
//compared index 10 is not :
            else if (destination.subString(0, 10) == String("localhost/"))
            {
                Array<Uint32> exportServer;

                    find_services(PEGASUS_QUEUENAME_EXPORTREQDISPATCHER, 0, 0,
                          &exportServer);

                // Listener is build with Cimom, so send message to ExportServer

               AutoPtr<CIMExportIndicationRequestMessage> exportmessage( 
		    new CIMExportIndicationRequestMessage(
                    XmlWriter::getNextMessageId(),
                    destination.subString(21), //taking localhost/CIMListener portion out from reg
                    indication,
                    QueueIdStack(exportServer[0], getQueueId()),
                    String::EMPTY,
                    String::EMPTY));

                exportmessage->operationContext.set(
		    request->operationContext.get(
		    ContentLanguageListContainer::NAME)); 
                AutoPtr<AsyncOpNode> op( this->get_op());

                AutoPtr<AsyncLegacyOperationStart> asyncRequest(
		    new AsyncLegacyOperationStart(
                    get_next_xid(),
                    op.get(),
                    exportServer[0],
                    exportmessage.get(),
                    _queueId));
                exportmessage.release();

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
                AutoPtr<AsyncReply> asyncReply(SendWait(asyncRequest.get()));
	        asyncRequest.release();

                // Return the ExportIndication results in HandleIndication 
		//response
                AutoPtr<CIMExportIndicationResponseMessage> exportResponse(
		    reinterpret_cast<CIMExportIndicationResponseMessage *>(
                    (static_cast<AsyncLegacyOperationResult *>(
			asyncReply.get()))->get_result()));
                cimException = exportResponse->cimException;

                op->release();
                this->return_op(op.release());
            }
	    else
	    {
                _loadHandler(request, cimException);
	    }
        }
    }
    else if (className.equal (PEGASUS_CLASSNAME_INDHANDLER_SNMP))
    {
        pos = handler.findProperty(PEGASUS_PROPERTYNAME_LSTNRDST_TARGETHOST);

        if (pos == PEG_NOT_FOUND)
        {
            cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms("HandlerService.IndicationHandlerService."
                "SNMP_HANDLER_WITHOUT_TARGETHOST", "Snmp Handler missing Targethost property"));
        }
        else
        {
            CIMProperty prop = handler.getProperty(pos);
            String destination = prop.getValue().toString();

            if (destination.size() == 0)
            {
                cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms("HandlerService.IndicationHandlerService."
                    "INVALID_TARGETHOST", "invalid targethost"));
            }
	    else
            {
                _loadHandler(request, cimException);
            }
        }
    }
    else if ((className.equal (PEGASUS_CLASSNAME_LSTNRDST_SYSTEM_LOG)) ||
	     (className.equal (PEGASUS_CLASSNAME_LSTNRDST_EMAIL)))
    {
        _loadHandler(request, cimException);
    }

    CIMHandleIndicationResponseMessage* response =
        new CIMHandleIndicationResponseMessage(
            request->messageId,
            cimException,
            request->queueIds.copyAndPop());

    delete request;
    return response;
}

void IndicationHandlerService::_loadHandler(
    CIMHandleIndicationRequestMessage* request,
    CIMException & cimException)
{
    CIMName className = request->handlerInstance.getClassName();

    try
    {
        CIMHandler * handlerLib = _lookupHandlerForClass(className);

        if (handlerLib)
        {
	    ContentLanguages langs = 
                ((ContentLanguageListContainer)request->operationContext.
                get(ContentLanguageListContainer::NAME)).getLanguages();

            handlerLib->handleIndication(
                request->operationContext,
                request->nameSpace.getString(),
                request->indicationInstance,
                request->handlerInstance,
                request->subscriptionInstance,
                langs);
        }
        else
        {
            cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms("HandlerService."
                "IndicationHandlerService.FAILED_TO_LOAD",
                "Failed to load Handler"));
        }

    }
    catch (Exception& e)
    {
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Exception: Unknown");
    }
}

CIMHandler* IndicationHandlerService::_lookupHandlerForClass(
   const CIMName& className)
{
   String handlerId;

   if (className.equal (PEGASUS_CLASSNAME_INDHANDLER_CIMXML)
      || className.equal (PEGASUS_CLASSNAME_LSTNRDST_CIMXML)
      )
#ifdef PEGASUS_OS_OS400
   {
       handlerId = LIBRARY_NAME_CIMXML_INDICATION_HNDLR;
       handlerId.append("/CIMxmlIndicationHandler");
   }
#else
       handlerId = String("CIMxmlIndicationHandler");
#endif
   else if (className.equal (PEGASUS_CLASSNAME_INDHANDLER_SNMP))
#ifdef PEGASUS_OS_OS400
   {
       handlerId = LIBRARY_NAME_SNMP_INDICATION_HNDLR;
       handlerId.append("/snmpIndicationHandler");
   }
#else
       handlerId = String("snmpIndicationHandler");
#endif
   else if (className.equal (PEGASUS_CLASSNAME_LSTNRDST_SYSTEM_LOG))
       handlerId = String("SystemLogListenerDestination");
   else if (className.equal (PEGASUS_CLASSNAME_LSTNRDST_EMAIL))
       handlerId = String("EmailListenerDestination");
   else
       return 0;

   CIMHandler* handler = _handlerTable.getHandler(handlerId, _repository);

   return handler;
}

PEGASUS_NAMESPACE_END


