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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider2/CIMInstanceProvider.h>
#include <Pegasus/Provider2/SimpleResponseHandler.h>

#include "InstanceProvider.h"

PEGASUS_NAMESPACE_BEGIN

InstanceProvider::InstanceProvider(void)
{
}

InstanceProvider::~InstanceProvider(void)
{
}

void InstanceProvider::initialize(CIMOMHandle& cimom)
{
	// save cimom handle
	_cimom = cimom;

	// create default instances
	CIMInstance instance1("Sample_DummyInstance");

	instance1.addProperty(CIMProperty("Identifier", Uint8(1)));   // key
	instance1.addProperty(CIMProperty("Message", String("Hello World")));

	_instances.append(instance1);

	CIMInstance instance2("Sample_DummyInstance");

	instance2.addProperty(CIMProperty("Identifier", Uint8(2)));   // key
	instance2.addProperty(CIMProperty("Message", String("Hey Planet")));

	_instances.append(instance2);

	CIMInstance instance3("Sample_DummyInstance");

	instance3.addProperty(CIMProperty("Identifier", Uint8(3)));   // key
	instance3.addProperty(CIMProperty("Message", String("Yo Earth")));

	_instances.append(instance3);
}

void InstanceProvider::terminate(void)
{
}

void InstanceProvider::getInstance(
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
			// deliver requested instance
			handler.deliver(_instances[i]);

			break;
		}
	}

	// complete processing the request
	handler.complete();
}

void InstanceProvider::enumerateInstances(
	const OperationContext & context,
	const CIMReference & ref,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMNamedInstance> & handler)
{
	// begin processing the request
	handler.processing();

        // NOTE: It would be more efficient to remember the instance names

	// get class definition from repository
	CIMClass cimclass = _cimom.getClass(
		OperationContext(),
		ref.getNameSpace(),
		ref.getClassName(),
		false,
		false,
		false,
		Array<String>());

	// convert instances to references;
	for(Uint32 i = 0; i < _instances.size(); i++)
	{
		CIMReference tempRef = _instances[i].getInstanceName(cimclass);

		// ensure references are fully qualified
		tempRef.setHost(ref.getHost());
		tempRef.setNameSpace(ref.getNameSpace());

		// deliver named instance
		handler.deliver(CIMNamedInstance(tempRef, _instances[i]));
	}

	// complete processing the request
	handler.complete();
}

void InstanceProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMReference & classReference,
	ResponseHandler<CIMReference> & handler)
{
	// begin processing the request
	handler.processing();

	// get class definition from repository
	CIMClass cimclass = _cimom.getClass(
		OperationContext(),
		classReference.getNameSpace(),
		classReference.getClassName(),
		false,
		false,
		false,
		Array<String>());

	// convert instances to references;
	for(Uint32 i = 0; i < _instances.size(); i++)
	{
		CIMReference tempRef = _instances[i].getInstanceName(cimclass);

		// ensure references are fully qualified
		tempRef.setHost(classReference.getHost());
		tempRef.setNameSpace(classReference.getNameSpace());

		// deliver reference
		handler.deliver(tempRef);
	}

	// complete processing the request
	handler.complete();
}

void InstanceProvider::modifyInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const CIMInstance & instanceObject,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	throw NotImplemented("InstanceProvider::modifyInstance");
}

void InstanceProvider::createInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const CIMInstance & instanceObject,
	ResponseHandler<CIMReference> & handler)
{
	throw NotImplemented("InstanceProvider::createInstance");
}

void InstanceProvider::deleteInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	ResponseHandler<CIMInstance> & handler)
{
	throw NotImplemented("InstanceProvider::deleteInstance");
}

Array<CIMReference> InstanceProvider::_enumerateInstanceNames(
	const OperationContext & context,
	const CIMReference & classReference)
{
	SimpleResponseHandler<CIMReference> handler;

	enumerateInstanceNames(context, classReference, handler);

	return(handler._objects);
}

PEGASUS_NAMESPACE_END
