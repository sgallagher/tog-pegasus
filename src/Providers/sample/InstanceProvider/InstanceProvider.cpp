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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "InstanceProvider.h"

PEGASUS_NAMESPACE_BEGIN

InstanceProvider::InstanceProvider(void)
{
}

InstanceProvider::~InstanceProvider(void)
{
}

void InstanceProvider::initialize(CIMOMHandle & cimom)
{
	// create default instances
	CIMInstance instance1("Sample_InstanceProviderClass");
	CIMObjectPath reference1("Sample_InstanceProviderClass.Identifier=1");

	instance1.addProperty(CIMProperty("Identifier", Uint8(1)));   // key
	instance1.addProperty(CIMProperty("Message", String("Hello World")));

	_instances.append(instance1);
	_instanceNames.append(reference1);

	CIMInstance instance2("Sample_InstanceProviderClass");
	CIMObjectPath reference2("Sample_InstanceProviderClass.Identifier=2");

	instance2.addProperty(CIMProperty("Identifier", Uint8(2)));   // key
	instance2.addProperty(CIMProperty("Message", String("Yo Planet")));

	_instances.append(instance2);
	_instanceNames.append(reference2);

	CIMInstance instance3("Sample_InstanceProviderClass");
	CIMObjectPath reference3("Sample_InstanceProviderClass.Identifier=3");

	instance3.addProperty(CIMProperty("Identifier", Uint8(3)));   // key
	instance3.addProperty(CIMProperty("Message", String("Hey Earth")));

	_instances.append(instance3);
	_instanceNames.append(reference3);
}

void InstanceProvider::terminate(void)
{
}

void InstanceProvider::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const Uint32 flags,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());

	// begin processing the request
	handler.processing();

	// instance index corresponds to reference index
	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		if(localReference == _instanceNames[i])
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
	const CIMObjectPath & classReference,
	const Uint32 flags,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
	// begin processing the request
	handler.processing();

	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		// deliver instance
		handler.deliver(_instances[i]);
	}

	// complete processing the request
	handler.complete();
}

void InstanceProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	ObjectPathResponseHandler & handler)
{
	// begin processing the request
	handler.processing();

	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		// deliver reference
		handler.deliver(_instanceNames[i]);
	}

	// complete processing the request
	handler.complete();
}

void InstanceProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	const Uint32 flags,
	const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());
	
	// begin processing the request
	handler.processing();

	// instance index corresponds to reference index
	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		if(localReference == _instanceNames[i])
		{
			// overwrite existing instance
			_instances[i] = instanceObject;
			
			break;
		}
	}
	
	// complete processing the request
	handler.complete();
}

void InstanceProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());
	
	// instance index corresponds to reference index
	for(Uint32 i = 0, n = _instanceNames.size(); i < n; i++)
	{
		if(localReference == _instanceNames[i])
		{
			throw CIMObjectAlreadyExistsException(
                                  localReference.toString());
		}
	}
			
	// begin processing the request
	handler.processing();

	// add the new instance to the array
	_instances.append(instanceObject);
	_instanceNames.append(instanceReference);

	// deliver the new instance
	handler.deliver(_instanceNames[_instanceNames.size() - 1]);

	// complete processing the request
	handler.complete();
}

void InstanceProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());
	
	// begin processing the request
	handler.processing();

	// instance index corresponds to reference index
	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		if(localReference == _instanceNames[i])
		{
			// save the instance locally
			CIMInstance cimInstance(_instances[i]);

			// remove instance from the array
			_instances.remove(i);
			_instanceNames.remove(i);

			// exit loop
			break;
		}
	}
	
	// complete processing the request
	handler.complete();
}

PEGASUS_NAMESPACE_END
