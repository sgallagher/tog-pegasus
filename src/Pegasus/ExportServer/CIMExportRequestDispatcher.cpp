//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Provider/CIMOMHandle.h>

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
   _consumerTable.set(_dynamicReg, _staticConsumers, _persistence);
}

CIMExportRequestDispatcher::CIMExportRequestDispatcher()
   :
   Base("CIMExportRequestDispatcher", MessageQueue::getNextQueueId())
{

}

CIMExportRequestDispatcher::~CIMExportRequestDispatcher()
{

}

void CIMExportRequestDispatcher::_handle_async_request(AsyncRequest *req)
{
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
        return;
    }
    else
        Base::_handle_async_request(req);
}

void CIMExportRequestDispatcher::handleEnqueue(Message* message)
{

   if( ! message)
      return;

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

}


void CIMExportRequestDispatcher::handleEnqueue()
{
   Message *message = dequeue();
   if(message)
      handleEnqueue(message);

}


void CIMExportRequestDispatcher::_handleExportIndicationRequest(
    CIMExportIndicationRequestMessage* request)
{
    OperationContext context;

    CIMException cimException;

    if (request->indicationInstance.getClassName() ==
	"PG_IndicationConsumerRegistration")
    {
	CIMInstance instance = request->indicationInstance;
	if (instance.existsProperty("ConsumerId") &&
	    instance.existsProperty("Location") &&
	    instance.existsProperty("ActionType"))
	{
            String errorDescription;
	    CIMStatusCode errorCode = _consumerTable.registerConsumer(
		instance.getProperty(instance.findProperty("ConsumerId"))
		    .getValue().toString(),
		instance.getProperty(instance.findProperty("Location"))
		    .getValue().toString(),
		instance.getProperty(instance.findProperty("ActionType"))
		    .getValue().toString(),
		errorDescription);

            cimException = PEGASUS_CIM_EXCEPTION(errorCode, errorDescription);
	}
	else
	{
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                "Invalid Consumer registration data");
	}
    }
    else
    {
       Boolean caught_exception = false;
       
       CIMIndicationConsumer* consumer;
       try
       {
	  consumer = _lookupConsumer(request->url);
       }
       catch (Exception & )
       {
	  caught_exception = true;
	  cimException = 
	     PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
				   "No consumer is registered for this location.");
       }
       if( caught_exception == false) 
       {
	  if (consumer)
	  {
	     consumer->handleIndication(
		context,
		request->url,
		request->indicationInstance);
	  }
	  else
	  {
	     cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						  "No consumer is registered for this location.");
	  }
       }
    }

    CIMExportIndicationResponseMessage* response =
	new CIMExportIndicationResponseMessage(
	request->messageId,
	cimException,
	request->queueIds.copyAndPop());

    //
    //  Set response destination
    //
    PEG_TRACE_STRING(TRC_EXP_REQUEST_DISP, Tracer::LEVEL4, 
		     "CIMExportRequestDispatcher setting export indication response dest " +
		     ((MessageQueue::lookup(request->queueIds.top())) ? 
		      String( ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) ) : 
		      String("BAD queue name")));
		     
    
    response->dest = request->queueIds.top();

    _enqueueResponse(request, response);
}

// REVIEW: Why must consumer be dynamically loaded? It makes sense in
// the case in which they are provider (then let the provider manager do it).

CIMIndicationConsumer* CIMExportRequestDispatcher::_lookupConsumer(
    const String& url)
{
    CIMIndicationConsumer* consumer =
        _consumerTable.lookupConsumer(url);

    if (!consumer)
    {
	consumer = _consumerTable.loadConsumer(url);

// << Mon Apr 29 12:49:49 2002 mdd >>
//	if (!consumer)
//	    throw CIMException(CIM_ERR_FAILED);

	//ATTN: How will get this handle? Defining just to proceed further.
	//CIMOMHandle cimom;

	//consumer->initialize(cimom);
	if(consumer)
	   consumer->initialize();
    }

    return consumer;
}

PEGASUS_NAMESPACE_END
