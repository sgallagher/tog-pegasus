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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/CIMOMHandle.h>
#include <Pegasus/Repository/CIMRepository.h>

#include "CIMExportRequestDispatcher.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportRequestDispatcher::CIMExportRequestDispatcher(
    CIMRepository* repository)
    : _repository(repository)
{

}

CIMExportRequestDispatcher::~CIMExportRequestDispatcher()
{

}

const char* CIMExportRequestDispatcher::getQueueName() const
{
    return "CIMExportRequestDispatcher";
}

void CIMExportRequestDispatcher::handleEnqueue()
{
    Message* request = dequeue();

    if (!request)
	return;

    switch (request->getType())
    {
	case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
	    _handleExportIndicationRequest(
		(CIMExportIndicationRequestMessage*)request);
	    break;
    }

    delete request;
}

void CIMExportRequestDispatcher::_enqueueResponse(
    CIMRequestMessage* request,
    CIMResponseMessage* response)
{
    // Use the same key as used in the request:

    response->setKey(request->getKey());

    // Lookup the message queue:

    MessageQueue* queue = MessageQueue::lookup(request->queueIds.top());
    PEGASUS_ASSERT(queue != 0);

    // Enqueue the response:

    queue->enqueue(response);
}

void CIMExportRequestDispatcher::_handleExportIndicationRequest(
    CIMExportIndicationRequestMessage* request)
{
    // ATTN: This is just demo code

    OperationContext context;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    CIMIndicationConsumer* consumer = _lookupConsumer(request->url);
    
    if (consumer)
    	consumer->handleIndication(
	context,
	request->url, 
	request->indicationInstance);
    else
	throw CIMException(CIM_ERR_FAILED);

    CIMExportIndicationResponseMessage* response = 
	new CIMExportIndicationResponseMessage(
	request->messageId,
	errorCode,
	errorDescription,
	request->queueIds.copyAndPop());

    _enqueueResponse(request, response);
}

void CIMExportRequestDispatcher::handleIndication(
    CIMInstance& indicationHandlerInstance,
    CIMInstance& indicationInstance,
    String nameSpace)
{
    String className = indicationHandlerInstance.getClassName();
    CIMHandler* handler = _lookupHandlerForClass(nameSpace, className);
    
    if (handler)
    {
	handler->handleIndication(
	    indicationHandlerInstance,
	    indicationInstance,
	    nameSpace);
    }
    else
	throw CIMException(CIM_ERR_FAILED);
}

CIMHandler* CIMExportRequestDispatcher::_lookupHandlerForClass(
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

CIMIndicationConsumer* CIMExportRequestDispatcher::_lookupConsumer(const String& url)
{
    //ATTN: How to get NAMESPACE? Defining just to proceed further.
    String NAMESPACE = "root/cimv2";

    Array<CIMInstance> cInst;
    cInst = _repository->enumerateInstances(NAMESPACE,
        "PG_ConsumerRegistration");

    String consumerName;

    for (int i=0; (i < cInst.size()) && (consumerName.size() == 0); i++)
    {
        int urlPropertyPos;
        int consumerPropertyPos;
        String consumerUrl;

        urlPropertyPos = cInst[i].findProperty("url");

        // Ignore malformed consumer registration
        if (urlPropertyPos != PEG_NOT_FOUND)
        {
            try
            {
                cInst[i].getProperty(urlPropertyPos).getValue()
                    .get(consumerUrl);
                if (consumerUrl == url)
                {
                    consumerPropertyPos = cInst[i].findProperty("consumerName");

                    // Ignore malformed consumer registration
                    if (consumerPropertyPos != PEG_NOT_FOUND)
                    {
                        // TypeMismatch exception is caught in outer block
                        cInst[i].getProperty(consumerPropertyPos).getValue()
                            .get(consumerName);
                    }
                }
            }
            catch (TypeMismatch& e)
            {
                // Ignore malformed consumer registration
            }
        }
    }

    if (consumerName.size() == 0)
    {
        // ATTN: What to do if no consumers are registered for this URL?
        throw CIMException(CIM_ERR_FAILED);
    }

    CIMIndicationConsumer* consumer =
        _consumerTable.lookupConsumer(consumerName);

    if (!consumer)
    {
	consumer = _consumerTable.loadConsumer(consumerName);

	if (!consumer)
	    throw CIMException(CIM_ERR_FAILED);

	//ATTN: How will get this handle? Defining just to proceed further.
	CIMOMHandle cimom;

	consumer->initialize(cimom);
    }

    return consumer;
}

PEGASUS_NAMESPACE_END
