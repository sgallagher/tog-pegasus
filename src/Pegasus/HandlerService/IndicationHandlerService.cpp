//%///-*-c++-*-/////////////////////////////////////////////////////////////////
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/XmlWriter.h>

#include "IndicationHandlerService.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN

IndicationHandlerService::IndicationHandlerService(void)
        : Base("IndicationHandlerService", MessageQueue::getNextQueueId())
{

}

IndicationHandlerService::IndicationHandlerService(CIMRepository* repository)
   : Base("IndicationHandlerService", MessageQueue::getNextQueueId()),
     _repository(repository)
{
}

const char* IndicationHandlerService::getQueueName() const
{
    return "IndicationHandlerService";
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
        Message *legacy = (static_cast<AsyncLegacyOperationStart *>(req)->act);
        handleEnqueue(legacy);
        return;
    }
    else
	Base::_handle_async_request(req);
}

void IndicationHandlerService::handleEnqueue(Message* message)
{

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
    handleEnqueue(message);

    delete message;
}

void IndicationHandlerService::_handleIndication(const Message* message)
{
    CIMHandleIndicationRequestMessage* request = 
	(CIMHandleIndicationRequestMessage*) message;

    String className = request->handlerInstance.getClassName();
    
    String nameSpace = request->nameSpace;

    CIMInstance indication = request->indicationInstance;
    CIMInstance handler = request->handlerInstance;

    CIMExportIndicationResponseMessage* response;

    Uint32 pos = handler.findProperty("destination");
    if (pos == PEG_NOT_FOUND)
    {
	// malformed handler instance, no destination
	throw CIMException(CIM_ERR_FAILED);
    }

    CIMProperty prop = handler.getProperty(pos);
    String destination = prop.getValue().toString();
    
    if (destination.size() == 0)
	throw CIMException(CIM_ERR_FAILED);
 
    char* hostname = destination.allocateCString();
    char* p = strchr(hostname, ':');

    if (!p)
    {
	delete [] hostname;
    }

    *p++ = '\0';

    char* end = 0;
    int port = strtol(p, &end, 10);

    if (!end || *end != '\0')
    {
	delete [] hostname;
    }

    if ((className == "CIM_IndicationHandlerCIMXML") &&
	(destination.subString(0, 9) == String("localhost")))
    {
	Array<Uint32> exportServer;

	find_services(String("CIMExportRequestDispatcher"), 0, 0, &exportServer);
        
	// Listener is build with Cimom, so send message to ExportServer
	
	CIMExportIndicationRequestMessage* exportmessage =
	    new CIMExportIndicationRequestMessage(
		"1234",
		destination,
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
		//getQueueId());

	AsyncReply* reply = SendWait(req);

        _enqueueResponse(req, reply);

	delete req;
	delete exportmessage;
        delete reply;
    }
    else
    {
	// generic handler. So load it and let it to do.
	CIMHandler* handlerLib = _lookupHandlerForClass(nameSpace, className);

	if (handlerLib)
	{
	    handlerLib->handleIndication(
		handler,
		indication,
		nameSpace);
	}
	else
	    throw CIMException(CIM_ERR_FAILED);
    }

    delete request;
    return;
}

CIMHandler* IndicationHandlerService::_lookupHandlerForClass(
    const String& nameSpace,
    const String& className)
{
    //----------------------------------------------------------------------
    // Look up the class:
    //----------------------------------------------------------------------

    CIMClass cimClass = _repository->getClass(nameSpace, className);

    if (!cimClass)
        throw CIMException(CIM_ERR_INVALID_CLASS);

    //----------------------------------------------------------------------
    // Get the handler qualifier:
    //----------------------------------------------------------------------

    Uint32 pos = cimClass.findQualifier("Handler");

    if (pos == PEG_NOT_FOUND)
        return 0;

    CIMQualifier q = cimClass.getQualifier(pos);
    String handlerId;

    q.getValue().get(handlerId);

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
