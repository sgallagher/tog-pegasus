//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider2/CIMInstanceProvider.h>
#include <Pegasus/Provider2/SimpleResponseHandler.h>
#include <Pegasus/Provider2/CIMProviderStub.h>

#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>

#include "HelloWorldProvider.h"

PEGASUS_NAMESPACE_BEGIN

class IndicationThread : public Thread
{
public:
	IndicationThread(void);
	virtual ~IndicationThread(void);

	void start(const Uint32 frequency, const String & message);
	void stop(void);

	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL run(void *);

protected:
	Uint32 _frequency;
	String _message;

};

IndicationThread::IndicationThread(void) :
	Thread(run, 0, false)
{
}

IndicationThread::~IndicationThread(void)
{
}

void IndicationThread::start(const Uint32 frequency, const String & message)
{
	_frequency = frequency;
	_message = message;

	run(this);
}

void IndicationThread::stop(void)
{
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL IndicationThread::run(void * pv)
{
	IndicationThread * pThread = (IndicationThread *)pv;
	
	while(true)
	{
		// create indication
		CIMClass indication("Sample_HelloWorldIndication");

		// add message to indication
		//indication.addProperty(CIMProperty("Message", pThread->getMessage()));
		
		// add timestamp to indication
		//indication.addProperty(CIMProperty("TimeStamp", CIMDateTime()));

		// send indication
		//_handler->deliver(indication);
		
		//pThread->sleep(pThread->getFrequency());
	}
}

HelloWorldProvider::HelloWorldProvider(void)
{
}

HelloWorldProvider::~HelloWorldProvider(void)
{
}

void HelloWorldProvider::initialize(CIMOMHandle& cimom)
{
	// save cimom handle
	_cimom = cimom;

	// create default instances
	CIMInstance instance1("HelloWorld");

	instance1.addProperty(CIMProperty("Identifier", Uint8(1)));   // key
	instance1.addProperty(CIMProperty("Frequency", Uint16(5)));
	instance1.addProperty(CIMProperty("Message", String("Hello World")));

	_instances.append(instance1);

	CIMInstance instance2("HelloWorld");

	instance2.addProperty(CIMProperty("Identifier", Uint8(2)));   // key
	instance2.addProperty(CIMProperty("Frequency", Uint16(5)));
	instance2.addProperty(CIMProperty("Message", String("Hey Planet")));

	_instances.append(instance2);

	CIMInstance instance3("HelloWorld");

	instance3.addProperty(CIMProperty("Identifier", Uint8(3)));   // key
	instance3.addProperty(CIMProperty("Frequency", Uint16(5)));
	instance3.addProperty(CIMProperty("Message", String("Yo Earth")));

	_instances.append(instance3);
}

void HelloWorldProvider::terminate(void)
{
}

void HelloWorldProvider::getInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	// synchronously get references
	Array<CIMReference> references = _enumerateInstanceNames(context, instanceReference);

	// ensure the request object exists
	if(Contains<CIMReference>(references, instanceReference) == false)
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	// begin processing the request
	handler.processing();

	// instance index corresponds to reference index
	for(Uint32 i = 0; i < references.size(); i++)
	{
		if(instanceReference == references[i])
		{
			handler.deliver(_instances[i]);
		}
	}

	// complete processing the request
	handler.complete();
}

void HelloWorldProvider::enumerateInstances(
	const OperationContext & context,
	const CIMReference & ref,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	// begin processing the request
	handler.processing();

	handler.deliver(_instances);

	// complete processing the request
	handler.complete();
}

void HelloWorldProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMReference & classReference,
	ResponseHandler<CIMReference> & handler)
{
	// begin processing the request
	handler.processing();

	// get class definition from repository
	CIMClass cimclass = _cimom.getClass(classReference.getNameSpace(), classReference.getClassName());

	// convert instances to references;
	for(Uint32 i = 0; i < _instances.size(); i++)
	{
		CIMReference tempRef = _instances[i].getInstanceName(cimclass);

		// ensure references are fully qualified
		tempRef.setHost(classReference.getHost());
		tempRef.setNameSpace(classReference.getNameSpace());

		handler.deliver(tempRef);
	}

	// complete processing the request
	handler.complete();
}

void HelloWorldProvider::modifyInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const CIMInstance & instanceObject,
	ResponseHandler<CIMInstance> & handler)
{
	// synchronously get references
	Array<CIMReference> references = _enumerateInstanceNames(context, instanceReference);

	// ensure the request object exists
	if(Contains<CIMReference>(references, instanceReference) == false)
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	// begin processing the request
	handler.processing();

	Uint32 index = 0;

	// find instance index (corresponds to reference index)
	for(; (index < references.size()) && (instanceReference == references[index]); index++);

	// remove old instance and add new one
	_instances.remove(index);
	_instances.insert(index, instanceObject);
	
	// complete processing the request
	handler.complete();

	/*
	// stop the thread for the old instance
	_monitors[index].cancel();
	_monitors.remove(index);
	
	// start a new thread for the modified instance
	_monitors.append(IndicationThread());
	_monitors[index].run(0);	
	*/
}

void HelloWorldProvider::createInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const CIMInstance & instanceObject,
	ResponseHandler<CIMInstance> & handler)
{
	// synchronously get references
	Array<CIMReference> references = _enumerateInstanceNames(context, instanceReference);

	// ensure the requested object do not exist
	if(Contains<CIMReference>(references, instanceReference) == true)
	{
		throw CIMException(CIM_ERR_ALREADY_EXISTS);
	}

	// begin processing the request
	handler.processing();

	// add the new instance to the end of the list
	_instances.append(instanceObject);

	// complete processing request
	handler.complete();

	/*
	// add a new monitoring thread to the end of the list
	_monitors.append(IndicationThread());
	
	// start the last monitor
	_monitors[_monitors.size() - 1].run(0);
	*/
}

void HelloWorldProvider::deleteInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	ResponseHandler<CIMInstance> & handler)
{
	// synchronously get references
	Array<CIMReference> references = _enumerateInstanceNames(context, instanceReference);

	// ensure the requested object exists
	if(Contains<CIMReference>(references, instanceReference) == false)
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	// begin processing the request
	handler.processing();

	Uint32 index = 0;
	
	for(; (index < references.size()) && (instanceReference == references[index]); index++);

	_instances.remove(index);
	
	// complete processing the request
	handler.complete();

	/*
	// stop the thread for the old instance
	_monitors[index].cancel();
	_monitors.remove(index);
	*/
}

void HelloWorldProvider::provideIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	const CIMDateTime & minimumInterval,
	const CIMDateTime & maximumInterval,
	const Array<String> & propertyList,
	ResponseHandler<CIMIndication> & handler)
{
	handler.processing();
}

void HelloWorldProvider::updateIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	const CIMDateTime & minimumInterval,
	const CIMDateTime & maximumInterval,
	const Array<String> & propertyList,
	ResponseHandler<CIMIndication> & handler)
{
	handler.processing();
}

void HelloWorldProvider::cancelIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	ResponseHandler<CIMIndication> & handler)
{
	handler.complete();
}
	
void HelloWorldProvider::checkIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	const Array<String> & propertyList,
	ResponseHandler<CIMIndication> & handler)
{
	throw NotSupported("HelloWorldProvider::checkIndication");
}

Array<CIMReference> HelloWorldProvider::_enumerateInstanceNames(
	const OperationContext & context,
	const CIMReference & classReference)
{
	SimpleResponseHandler<CIMReference> handler;

	enumerateInstanceNames(context, classReference, handler);

	return(handler._objects);
}

PEGASUS_NAMESPACE_END
