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
// Author: Yi Zhou (yi_zhou@hp.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>
#include <Pegasus/Provider/OperationFlag.h>

#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMPropertyList.h>

#include "PG_TestPropertyTypes.h"

PEGASUS_NAMESPACE_BEGIN

extern "C" PEGASUS_EXPORT CIMBaseProvider* PegasusCreateProvider(const String & classname)
{
	if (String::equalNoCase(classname, "PG_TestPropertyTypes"))
	{
		return(new PG_TestPropertyTypes());
	}
	return (0);
}

PG_TestPropertyTypes::PG_TestPropertyTypes(void)
{
}

PG_TestPropertyTypes::~PG_TestPropertyTypes(void)
{
}

void PG_TestPropertyTypes::initialize(CIMOMHandle& cimom)
{
	// save cimom handle
	_cimom = cimom;

	// create default instances
	CIMInstance instance1("PG_TestPropertyTypes");

	instance1.addProperty(CIMProperty("CreationClassName", 
			String("PG_TestPropertyTypes")));   // key
	instance1.addProperty(CIMProperty("InstanceId", Uint64(1))); //key
	instance1.addProperty(CIMProperty("PropertyString", 
				String("PG_TestPropertyTypes_Instance1")));
	instance1.addProperty(CIMProperty("PropertyUint8", Uint8(120)));
	instance1.addProperty(CIMProperty("PropertyUint16", Uint16(1600)));
	instance1.addProperty(CIMProperty("PropertyUint32", Uint32(3200)));
	instance1.addProperty(CIMProperty("PropertyUint64", Uint64(6400)));
	instance1.addProperty(CIMProperty("PropertySint8", Sint8(-120)));
	instance1.addProperty(CIMProperty("PropertySint16", Sint16(-1600)));
	instance1.addProperty(CIMProperty("PropertySint32", Sint32(-3200)));
	instance1.addProperty(CIMProperty("PropertySint64", Sint64(-6400)));
	instance1.addProperty(CIMProperty("PropertyBoolean", Boolean(1)));
	instance1.addProperty(CIMProperty("PropertyReal32", Real32(1.32)));
	instance1.addProperty(CIMProperty("PropertyReal64", Real64(1.64)));
	instance1.addProperty(CIMProperty("PropertyDatetime",
			      CIMDateTime("20010515104354.000000:000")));

	_instances.append(instance1);

	CIMInstance instance2("PG_TestPropertyTypes");

	instance2.addProperty(CIMProperty("CreationClassName", 
			String("PG_TestPropertyTypes")));   // key
	instance2.addProperty(CIMProperty("InstanceId", Uint64(2))); //key
	instance2.addProperty(CIMProperty("PropertyString", 
				String("PG_TestPropertyTypes_Instance2")));

	instance2.addProperty(CIMProperty("PropertyUint8", Uint8(122)));
	instance2.addProperty(CIMProperty("PropertyUint16", Uint16(1602)));
	instance2.addProperty(CIMProperty("PropertyUint32", Uint32(3202)));
	instance2.addProperty(CIMProperty("PropertyUint64", Uint64(6402)));
	instance2.addProperty(CIMProperty("PropertySint8", Sint8(-122)));
	instance2.addProperty(CIMProperty("PropertySint16", Sint16(-1602)));
	instance2.addProperty(CIMProperty("PropertySint32", Sint32(-3202)));
	instance2.addProperty(CIMProperty("PropertySint64", Sint64(-6402)));
	instance2.addProperty(CIMProperty("PropertyBoolean", Boolean(0)));
	instance2.addProperty(CIMProperty("PropertyReal32", Real32(1.322)));
	instance2.addProperty(CIMProperty("PropertyReal64", Real64(1.642)));
	instance2.addProperty(CIMProperty("PropertyDatetime",
			      CIMDateTime("20010515104354.000000:000")));

	_instances.append(instance2);

}

void PG_TestPropertyTypes::terminate(void)
{
}

void PG_TestPropertyTypes::getInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	// synchronously get references
	Array<CIMReference> references = _enumerateInstanceNames(context, instanceReference);

	// ensure the InstanceId key is valid
	Array<KeyBinding> keys = instanceReference.getKeyBindings();
	Uint32 i;
	for (i=0; i<keys.size() && keys[i].getName() != "InstanceId"; i++);

	if (i==keys.size())
	{
		throw CIMException(CIM_ERR_INVALID_PARAMETER);
	}

	// ensure the Namespace is valid
	if (instanceReference.getNameSpace() != "test/static")
	{
		throw CIMException(CIM_ERR_INVALID_NAMESPACE);
	}

	// ensure the class existing in the specified namespace
	if (instanceReference.getClassName() != "PG_TestPropertyTypes")
	{
		throw CIMException(CIM_ERR_INVALID_CLASS);
	}

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

void PG_TestPropertyTypes::enumerateInstances(
	const OperationContext & context,
	const CIMReference & ref,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{

	// ensure the Namespace is valid
	if (ref.getNameSpace() != "test/static")
	{
		throw CIMException(CIM_ERR_INVALID_NAMESPACE);
	}

	// ensure the class existing in the specified namespace
	if (ref.getClassName() != "PG_TestPropertyTypes")
	{
		throw CIMException(CIM_ERR_INVALID_CLASS);
	}

	// begin processing the request
	handler.processing();

        // NOTE: It would be much more efficient to remember the instance names

	// get class definition from repository
	CIMClass cimclass = _cimom.getClass(context, ref.getNameSpace(), ref.getClassName(), false, true, true, CIMPropertyList());

	for(Uint32 i = 0; i < _instances.size(); i++)
	{
		handler.deliver( _instances[i]);
	}

	// complete processing the request
	handler.complete();
}

void PG_TestPropertyTypes::enumerateInstanceNames(
	const OperationContext & context,
	const CIMReference & classReference,
	ResponseHandler<CIMReference> & handler)
{

	// ensure the Namespace is valid
	if (classReference.getNameSpace() != "test/static")
	{
		throw CIMException(CIM_ERR_INVALID_NAMESPACE);
	}

	// ensure the class existing in the specified namespace
	if (classReference.getClassName() != "PG_TestPropertyTypes")
	{
		throw CIMException(CIM_ERR_INVALID_CLASS);
	}

	// begin processing the request
	handler.processing();

	// get class definition from repository
	CIMClass cimclass = _cimom.getClass(context, classReference.getNameSpace(), classReference.getClassName(), false, true, true, CIMPropertyList());

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

void PG_TestPropertyTypes::modifyInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const CIMInstance & instanceObject,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	// ATTN: This provider does not yet support partial modification
	if (flags & OperationFlag::PARTIAL_INSTANCE)
	{
	    throw CIMException(CIM_ERR_NOT_SUPPORTED);
	}

	// synchronously get references
	Array<CIMReference> references = _enumerateInstanceNames(context, instanceReference);

	// ensure the Namespace is valid
	if (instanceReference.getNameSpace() != "test/static")
	{
		throw CIMException(CIM_ERR_INVALID_NAMESPACE);
	}

	// ensure the class existing in the specified namespace
	if (instanceReference.getClassName() != "PG_TestPropertyTypes")
	{
		throw CIMException(CIM_ERR_INVALID_CLASS);
	}

        // ensure the property values are valid
        _testPropertyTypesValue(instanceObject);

	// ensure the request object exists
	if(Contains<CIMReference>(references, instanceReference) == false)
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	// begin processing the request
	handler.processing();

	Uint32 index = 0;

	// find instance index (corresponds to reference index)
	for(; (index < references.size()) && (instanceReference != references[index]); index++);

	// We do nothing here since we like to have static result	
	// complete processing the request
	handler.complete();

}

void PG_TestPropertyTypes::createInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const CIMInstance & instanceObject,
	ResponseHandler<CIMReference> & handler)
{
	// synchronously get references
	Array<CIMReference> references = _enumerateInstanceNames(context, instanceReference);

	// ensure the Namespace is valid

	if (instanceReference.getNameSpace() != "test/static")
	{
		throw CIMException(CIM_ERR_INVALID_NAMESPACE);
	}

	// ensure the class existing in the specified namespace
	if (instanceReference.getClassName() != "PG_TestPropertyTypes")
	{
		throw CIMException(CIM_ERR_INVALID_CLASS);
	}

	// ensure the InstanceId key is valid
        Uint32 propIndex = instanceObject.findProperty("InstanceId");
	if (propIndex == PEG_NOT_FOUND)
	{
            throw CIMException(CIM_ERR_INVALID_PARAMETER);
	}

//	Uint32 i;
//	for (i=0; i<keys.size() && keys[i].getName() != "InstanceId"; i++);

//	if (i==keys.size())
//	{
//		throw CIMException(CIM_ERR_INVALID_PARAMETER);
//	}

        // create the CIMReference to return
        KeyBinding kb1(instanceObject.getProperty(propIndex).getName(),
                       instanceObject.getProperty(propIndex).getValue().toString(),
                       KeyBinding::NUMERIC);
        KeyBinding kb2("CreationClassName", "PG_TestPropertyTypes",
                       KeyBinding::STRING);
        CIMReference returnReference(instanceReference);
	Array<KeyBinding> keys;
        keys.append(kb1);
        keys.append(kb2);
        returnReference.setKeyBindings(keys);

	// ensure the property values are valid
	_testPropertyTypesValue(instanceObject);

	// Determine if a property exists in the class
	if (!instanceObject.existsProperty("PropertyUint8"))
	{
		throw CIMException(CIM_ERR_INVALID_PARAMETER);
	}

	// ensure the requested object do not exist
	if(Contains<CIMReference>(references, instanceReference) == true)
	{
		throw CIMException(CIM_ERR_ALREADY_EXISTS);
	}

	// begin processing the request
	handler.processing();

	handler.deliver(returnReference);

	// complete processing request
	handler.complete();

}

void PG_TestPropertyTypes::deleteInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	ResponseHandler<CIMInstance> & handler)
{
	// synchronously get references
	Array<CIMReference> references = _enumerateInstanceNames(context, instanceReference);

	// ensure the Namespace is valid
	if (instanceReference.getNameSpace() != "test/static")
	{
		throw CIMException(CIM_ERR_INVALID_NAMESPACE);
	}

	// ensure the class existing in the specified namespace
	if (instanceReference.getClassName() != "PG_TestPropertyTypes")
	{
		throw CIMException(CIM_ERR_INVALID_CLASS);
	}

	// ensure the requested object exists
	if(Contains<CIMReference>(references, instanceReference) == false)
	{
		throw CIMException(CIM_ERR_NOT_FOUND);
	}

	// begin processing the request
	handler.processing();

	Uint32 index = 0;
	
	for(; (index < references.size()) && (instanceReference != references[index]); index++);

	// we do not remove instance 
	// complete processing the request
	handler.complete();

}

void PG_TestPropertyTypes::getProperty(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const String & propertyName,
	ResponseHandler<CIMValue> & handler)
{
	throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void PG_TestPropertyTypes::setProperty(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const String & propertyName,
	const CIMValue & newValue,
	ResponseHandler<CIMValue> & handler)
{
	throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> PG_TestPropertyTypes::_enumerateInstanceNames(
	const OperationContext & context,
	const CIMReference & classReference)
{
	SimpleResponseHandler<CIMReference> handler;

	enumerateInstanceNames(context, classReference, handler);

	return(handler._objects);
}

// private method: _testPropertyTypesValue
void PG_TestPropertyTypes::_testPropertyTypesValue(
			  const CIMInstance & instanceObject)
{
	Uint32 PropertyCount = instanceObject.getPropertyCount();

	Uint32 j;
	for (j=0; j<PropertyCount; j++)
	{
	  const CIMConstProperty & property = instanceObject.getProperty(j);
	  const String & propertyName = property.getName();
	  const CIMValue& propertyValue = property.getValue();

	  CIMType type = propertyValue.getType();
	
	  switch (type)
	  {
	     case CIMType::UINT8:
	    	Uint8 propertyValueUint8;
		propertyValue.get(propertyValueUint8);	
		if (propertyValueUint8 >= 255) 
		{
		   throw CIMException(CIM_ERR_INVALID_PARAMETER);
		} 
		break;

	     case CIMType::UINT16:
	    	Uint16 propertyValueUint16;
		propertyValue.get(propertyValueUint16);	
		if (propertyValueUint16 >= 10000) 
		{
		   throw CIMException(CIM_ERR_INVALID_PARAMETER);
		} 
		break;

	     case CIMType::UINT32:
	    	Uint32 propertyValueUint32;
		propertyValue.get(propertyValueUint32);	
		if (propertyValueUint32 >= 10000000) 
		{
		   throw CIMException(CIM_ERR_INVALID_PARAMETER);
		} 
		break;

	     case CIMType::UINT64:
	    	Uint64 propertyValueUint64;
		propertyValue.get(propertyValueUint64);	
		if (propertyValueUint64 >= 1000000000) 
		{
		   throw CIMException(CIM_ERR_INVALID_PARAMETER);
		} 
		break;

	     case CIMType::SINT8:
	    	Sint8 propertyValueSint8;
		propertyValue.get(propertyValueSint8);	
		if (propertyValueSint8 <= -120) 
		{
		   throw CIMException(CIM_ERR_INVALID_PARAMETER);
		} 
		break;

	     case CIMType::SINT16:
	    	Sint16 propertyValueSint16;
		propertyValue.get(propertyValueSint16);	
		if (propertyValueSint16 < -10000) 
		{
		   throw CIMException(CIM_ERR_INVALID_PARAMETER);
		} 
		break;

	     case CIMType::SINT32:
	    	Sint32 propertyValueSint32;
		propertyValue.get(propertyValueSint32);	
		if (propertyValueSint32 <= -10000000) 
		{
		   throw CIMException(CIM_ERR_INVALID_PARAMETER);
		} 
		break;

	     case CIMType::SINT64:
	    	Sint64 propertyValueSint64;
		propertyValue.get(propertyValueSint64);	
		if (propertyValueSint64 <= -1000000000) 
		{
		   throw CIMException(CIM_ERR_INVALID_PARAMETER);
		} 
		break;

	     case CIMType::REAL32:
	    	Real32 propertyValueReal32;
		propertyValue.get(propertyValueReal32);	
		if (propertyValueReal32 >= 10000000.32) 
		{
		   throw CIMException(CIM_ERR_INVALID_PARAMETER);
		} 
		break;

	     case CIMType::REAL64:
	    	Real64 propertyValueReal64;
		propertyValue.get(propertyValueReal64);	
		if (propertyValueReal64 >= 1000000000.64) 
		{
		   throw CIMException(CIM_ERR_INVALID_PARAMETER);
		} 
		break;

	     default:
		;
	  }
	} // end for loop
}

PEGASUS_NAMESPACE_END
