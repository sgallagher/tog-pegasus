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

#include "PropertyProvider.h"

PEGASUS_NAMESPACE_BEGIN

PropertyProvider::PropertyProvider(void)
{
}

PropertyProvider::~PropertyProvider(void)
{
}

void PropertyProvider::initialize(CIMOMHandle & cimom)
{
	CIMReference reference1("Sample_PropertyProviderClass.Identifier=1");
	CIMProperty property1("Message", CIMValue("Hello World"));
	
	_properties.append(Pair<CIMReference, CIMProperty>(reference1, property1));
	
	CIMReference reference2("Sample_PropertyProviderClass.Identifier=2");
	CIMProperty property2("Message", CIMValue("Yo Planet"));
	
	_properties.append(Pair<CIMReference, CIMProperty>(reference1, property1));
}

void PropertyProvider::terminate(void)
{
}

void PropertyProvider::getProperty(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const String & propertyName,
	ResponseHandler<CIMValue> & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMReference localReference = CIMReference(
		String(),
		String(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());

	// begin processing the request
	handler.processing();
	
	for(Uint32 i = 0, n = _properties.size(); i < n; i++)
	{
		if(localReference == _properties[i].first)
		{
			// deliver the property value associated with the specified instance
			handler.deliver(_properties[i].second.getValue());

			// exit loop
			break;
		}
	}

	// complete processing the request
	handler.complete();
}

void PropertyProvider::setProperty(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const String & propertyName,
	const CIMValue & newValue,
	ResponseHandler<CIMValue> & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMReference localReference = CIMReference(
		String(),
		String(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());

	// begin processing the request
	handler.processing();
	
	for(Uint32 i = 0, n = _properties.size(); i < n; i++)
	{
		if(localReference == _properties[i].first)
		{
			// update the property value associated with the specified instance
			_properties[i].second.setValue(newValue);

			// deliver the property value associated with the specified instance
			handler.deliver(_properties[i].second.getValue());

			// exit loop
			break;
		}
	}

	// complete processing the request
	handler.complete();
}

PEGASUS_NAMESPACE_END
