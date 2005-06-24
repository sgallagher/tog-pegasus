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
//
// Author: Dong Xiang, EMC Corporation (xiang_dong@emc.com)
//
// Modified By:	Seema Gupta (gseema@in.ibm.com) for PEP135
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMListenerIndicationDispatcher.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Tracer.h>

#include <Pegasus/Listener/List.h>
#include <Pegasus/Consumer/CIMIndicationConsumer.h>
#include <Pegasus/Common/ContentLanguages.h>

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
// CIMListenerIndicationDispatchEvent
///////////////////////////////////////////////////////////////////////////////
class CIMListenerIndicationDispatchEvent
{
public:
	CIMListenerIndicationDispatchEvent(CIMIndicationConsumer* consumer,
                                           String url,
                                           CIMInstance instance,
                                           ContentLanguages contentLangs);
	~CIMListenerIndicationDispatchEvent();

	CIMIndicationConsumer* getConsumer() const;

	String getURL() const;
	CIMInstance getIndicationInstance() const;
        ContentLanguages getContentLanguages() const; 

private:
	CIMIndicationConsumer*	_consumer;
	String									_url;
	CIMInstance							_instance;
        ContentLanguages                    _contentLangs;
};

CIMListenerIndicationDispatchEvent::CIMListenerIndicationDispatchEvent(CIMIndicationConsumer* consumer,
                                                                       String url,
                                                                       CIMInstance instance,
                                                                       ContentLanguages contentLangs)
:_consumer(consumer),_url(url),_instance(instance), _contentLangs(contentLangs)
{
}
CIMListenerIndicationDispatchEvent::~CIMListenerIndicationDispatchEvent()
{
}
CIMIndicationConsumer* CIMListenerIndicationDispatchEvent::getConsumer() const
{
	return _consumer;
}
String CIMListenerIndicationDispatchEvent::getURL() const
{
	return _url;
}
CIMInstance CIMListenerIndicationDispatchEvent::getIndicationInstance() const
{
	return _instance;
}
ContentLanguages CIMListenerIndicationDispatchEvent::getContentLanguages() const
{
	return _contentLangs;
}

///////////////////////////////////////////////////////////////////////////////
// CIMListenerIndicationDispatcherRep
///////////////////////////////////////////////////////////////////////////////
class CIMListenerIndicationDispatcherRep
{
public:
	CIMListenerIndicationDispatcherRep();
	virtual ~CIMListenerIndicationDispatcherRep();
	
	Boolean addConsumer(CIMIndicationConsumer* consumer);
	Boolean removeConsumer(CIMIndicationConsumer* consumer);

	CIMExportIndicationResponseMessage* handleIndicationRequest(CIMExportIndicationRequestMessage* request);

	
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL deliver_routine(void *param);

private:
	void	deliverIndication(String url, CIMInstance instance, ContentLanguages contentLangs);

	ThreadPool* _thread_pool;
	PtrList*		_consumers;
};

static struct timeval deallocateWait = {15, 0};


CIMListenerIndicationDispatcherRep::CIMListenerIndicationDispatcherRep()
:_thread_pool(new ThreadPool(0, "ListenerIndicationDispatcher", 0, 0,
	deallocateWait))
,_consumers(new PtrList())
{
	      
}
CIMListenerIndicationDispatcherRep::~CIMListenerIndicationDispatcherRep()
{
	delete _thread_pool;
	delete _consumers;
}
	
Boolean CIMListenerIndicationDispatcherRep::addConsumer(CIMIndicationConsumer* consumer)
{
	_consumers->add(consumer);
	return true;
}
Boolean CIMListenerIndicationDispatcherRep::removeConsumer(CIMIndicationConsumer* consumer)
{
	_consumers->remove(consumer);
	return true;
}
CIMExportIndicationResponseMessage* CIMListenerIndicationDispatcherRep::handleIndicationRequest(CIMExportIndicationRequestMessage* request)
{
	PEG_METHOD_ENTER(TRC_SERVER,
		"CIMListenerIndicationDispatcherRep::handleIndicationRequest");

	CIMInstance instance = request->indicationInstance;
	String			url = request->destinationPath;
    ContentLanguages contentLangs =((ContentLanguageListContainer)request->operationContext.
			                                    get(ContentLanguageListContainer::NAME)).getLanguages();

	deliverIndication(url,instance,contentLangs);

	// compose a response message  
	CIMException cimException;

	CIMExportIndicationResponseMessage* response = new CIMExportIndicationResponseMessage(
		request->messageId,
		cimException,
		request->queueIds.copyAndPop());

	response->dest = request->queueIds.top();

	PEG_METHOD_EXIT();

	return response;
}

void CIMListenerIndicationDispatcherRep::deliverIndication(String url,
                                                           CIMInstance instance,
                                                           ContentLanguages contentLangs)
{
	// go thru all consumers and broadcast the result; should be run in seperate thread
	Iterator* it = _consumers->iterator();
	while(it->hasNext()==true)
	{
		CIMIndicationConsumer* consumer = static_cast<CIMIndicationConsumer*>(it->next());
		CIMListenerIndicationDispatchEvent* event = new CIMListenerIndicationDispatchEvent(
                                                                                     consumer,
                                                                                     url,
                                                                                     instance,
                                                                                     contentLangs);
		ThreadStatus rtn = _thread_pool->allocate_and_awaken(event,deliver_routine);
		
    		if (rtn != PEGASUS_THREAD_OK) 
    		{
	    	    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
				"Not enough threads to allocate a worker to deliver the event. ");
 
	    	    Tracer::trace(TRC_SERVER, Tracer::LEVEL2,
				"Could not allocate thread to deliver event. Instead using current thread.");
		    delete event;
		    throw Exception(MessageLoaderParms("Listener.CIMListenerIndicationDispatcher.CANNOT_ALLOCATE_THREAD",
					"Not enough threads to allocate a worker to deliver the event."));
    		}
	}
}
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL CIMListenerIndicationDispatcherRep::deliver_routine(void *param)
{
	CIMListenerIndicationDispatchEvent* event = static_cast<CIMListenerIndicationDispatchEvent*>(param);

	if(event!=NULL)
	{
		CIMIndicationConsumer* consumer = event->getConsumer();
		OperationContext context;
	        context.insert(ContentLanguageListContainer(event->getContentLanguages()));
		if(consumer)
		{
			consumer->consumeIndication(context,event->getURL(),event->getIndicationInstance());
		}

		delete event;
	}
		
	return (0);
}

///////////////////////////////////////////////////////////////////////////////
// CIMListenerIndicationDispatcher
///////////////////////////////////////////////////////////////////////////////
CIMListenerIndicationDispatcher::CIMListenerIndicationDispatcher()
:Base(PEGASUS_QUEUENAME_LISTENERINDICATIONDISPACTCHER)
,_rep(new CIMListenerIndicationDispatcherRep())
{
}
CIMListenerIndicationDispatcher::~CIMListenerIndicationDispatcher()
{
	if(_rep!=NULL)
		delete static_cast<CIMListenerIndicationDispatcherRep*>(_rep);

	_rep=NULL;
}

void CIMListenerIndicationDispatcher::handleEnqueue()
{
	PEG_METHOD_ENTER(TRC_SERVER, "CIMListenerIndicationDispatcher::handleEnqueue");
	
	Message *message = dequeue();
	if(message)
		handleEnqueue(message);
	
	PEG_METHOD_EXIT();
}

void CIMListenerIndicationDispatcher::handleEnqueue(Message* message)
{
	PEG_METHOD_ENTER(TRC_SERVER, "CIMListenerIndicationDispatcher::handleEnqueue");
	
	if(message!=NULL)
	{
		switch (message->getType())
    {
			case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
				{
					CIMExportIndicationRequestMessage* request = (CIMExportIndicationRequestMessage*)message;

					CIMExportIndicationResponseMessage* response = 
						static_cast<CIMListenerIndicationDispatcherRep*>(_rep)->handleIndicationRequest(request);

					_enqueueResponse(request, response);
				}
				break;
		default:
			break;
    }	
    delete message;
	}	
   
	PEG_METHOD_EXIT();
}
Boolean CIMListenerIndicationDispatcher::addConsumer(CIMIndicationConsumer* consumer)
{
	return static_cast<CIMListenerIndicationDispatcherRep*>(_rep)->addConsumer(consumer);
}
Boolean CIMListenerIndicationDispatcher::removeConsumer(CIMIndicationConsumer* consumer)
{
	return static_cast<CIMListenerIndicationDispatcherRep*>(_rep)->removeConsumer(consumer);
}

PEGASUS_NAMESPACE_END
