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
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMBaseProviderHandle.h"

#include <Pegasus/Common/System.h>
#include <Pegasus/Provider/OperationFlag.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN

static Uint32 convertFlags(
	const Boolean localOnly,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const Boolean deepInheritance,
	const Boolean partialInstance)
{
	// convert flags
	Uint32 flags = OperationFlag::NONE;

	flags |= localOnly ? OperationFlag::LOCAL_ONLY : 0;
	flags |= includeQualifiers ? OperationFlag::INCLUDE_QUALIFIERS : 0;
	flags |= includeClassOrigin ? OperationFlag::INCLUDE_CLASS_ORIGIN : 0;
	flags |= deepInheritance ? OperationFlag::DEEP_INHERITANCE : 0;
	flags |= partialInstance ? OperationFlag::PARTIAL_INSTANCE : 0;

	return(flags);
}

static CIMReference buildReference(
	const String & host,
	const String & nameSpace,
	const String & className,
	const Array<KeyBinding> & keyBindings = Array<KeyBinding>())
{
	CIMReference reference;
	
	if(host.size() == 0)
	{
		// set default host name
		reference.setHost(System::getHostName());
	}
	else
	{
		reference.setHost(host);
	}
	
	if(nameSpace.size() == 0)
	{
		// set default namespace
		reference.setNameSpace("root/cimv2");
	}
	else
	{
		reference.setNameSpace(nameSpace);
	}
	
	reference.setClassName(className);
	reference.setKeyBindings(keyBindings);

	return(reference);
}

static CIMReference buildReference(
	const CIMReference & reference)
{
	return(buildReference(reference.getHost(), reference.getNameSpace(), reference.getClassName(), reference.getKeyBindings()));
}
	
CIMBaseProviderHandle::CIMBaseProviderHandle(CIMBaseProvider * provider) : _provider(0)
{
	_provider = new CIMBaseProviderFacade(provider);
}

CIMBaseProviderHandle::~CIMBaseProviderHandle(void)	
{
	if(_provider != 0)
	{
		delete _provider;
	}
}

void CIMBaseProviderHandle::initialize(CIMOMHandle & cimom)
{
	_provider->initialize(cimom);
}

void CIMBaseProviderHandle::terminate(void)
{
	_provider->terminate();
}

CIMClass CIMBaseProviderHandle::getClass(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
	// convert flags
	Uint32 flags = convertFlags(localOnly, includeQualifiers, includeClassOrigin, false, !propertyList.isNull());

	// build fully qualified reference
	CIMReference reference = buildReference("", nameSpace, className);

	SimpleResponseHandler<CIMClass> handler;

	// forward request
	_provider->getClass(context, reference, flags, propertyList.getPropertyNameArray(), handler);

	return(handler._objects[0]);
}

void CIMBaseProviderHandle::getClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMClass> CIMBaseProviderHandle::enumerateClasses(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
	// convert flags
	Uint32 flags = convertFlags(localOnly, includeQualifiers, includeClassOrigin, deepInheritance, false);

	// build fully qualified reference
	CIMReference reference = buildReference("", nameSpace, className);

	SimpleResponseHandler<CIMClass> handler;

	// forward request
	_provider->enumerateClasses(context, reference, flags, handler);

	return(handler._objects);
}

void CIMBaseProviderHandle::enumerateClassesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<String> CIMBaseProviderHandle::enumerateClassNames(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
	// convert flags
	Uint32 flags = convertFlags(false, false, false, deepInheritance, false);

	// build reference
	CIMReference reference = buildReference("", nameSpace, className);

	SimpleResponseHandler<CIMReference> handler;

	// forward request
	_provider->enumerateClassNames(context, reference, flags, handler);

	Array<CIMReference> references = handler._objects;

	// convert return value
	Array<String> names;

	for(Uint32 i = 0; i < references.size(); ++i)
	{
		names.append(references[i].getClassName());
	}

	return(names);
}

void CIMBaseProviderHandle::enumerateClassNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMBaseProviderHandle::createClass(
	const OperationContext & context,
    const String& nameSpace,
    const CIMClass& newClass)
{
	// build fully qualified reference
	CIMReference reference = buildReference("", nameSpace, newClass.getClassName());

	SimpleResponseHandler<CIMClass> handler;

	// forward request
	_provider->createClass(context, reference, newClass, handler);
}

void CIMBaseProviderHandle::createClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMClass& newClass,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMBaseProviderHandle::modifyClass(
	const OperationContext & context,
    const String& nameSpace,
    const CIMClass& modifiedClass)
{
	// build fully qualified reference
	CIMReference reference = buildReference("", nameSpace, modifiedClass.getClassName());

	SimpleResponseHandler<CIMClass> handler;

	// forward request
	_provider->modifyClass(context, reference, modifiedClass, handler);
}

void CIMBaseProviderHandle::modifyClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMClass& modifiedClass,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMBaseProviderHandle::deleteClass(
	const OperationContext & context,
    const String& nameSpace,
    const String& className)
{
	// build fully qualified reference
	CIMReference reference = buildReference("", nameSpace, className);

	SimpleResponseHandler<CIMClass> handler;

	// forward request
	_provider->deleteClass(context, reference, handler);
}

void CIMBaseProviderHandle::deleteClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMInstance CIMBaseProviderHandle::getInstance(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
	// convert flags
	Uint32 flags = convertFlags(localOnly, includeQualifiers, includeClassOrigin, false, !propertyList.isNull());

	// build fully qualified reference
	CIMReference reference = buildReference("", nameSpace, instanceName.getClassName(), instanceName.getKeyBindings());

	SimpleResponseHandler<CIMInstance> handler;

	_provider->getInstance(context, reference, flags, propertyList.getPropertyNameArray(), handler);

	return(handler._objects[0]);
}

void CIMBaseProviderHandle::getInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMNamedInstance> CIMBaseProviderHandle::enumerateInstances(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    // convert flags
    Uint32 flags = convertFlags(localOnly, includeQualifiers, includeClassOrigin, deepInheritance, !propertyList.isNull());

    // build fully qualified reference
    CIMReference reference = buildReference("", nameSpace, className);

    SimpleResponseHandler<CIMNamedInstance> handler;

    // forward request
    _provider->enumerateInstances(context, reference, flags, propertyList.getPropertyNameArray(), handler);

    return(handler._objects);
}

void CIMBaseProviderHandle::enumerateInstancesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
	ResponseHandler<CIMNamedInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> CIMBaseProviderHandle::enumerateInstanceNames(
	const OperationContext & context,
    const String& nameSpace,
    const String& className)
{
	// build fully qualified reference
	CIMReference reference = buildReference("", nameSpace, className);

	SimpleResponseHandler<CIMReference> handler;

	// forward request
	_provider->enumerateInstanceNames(context, reference, handler);

	return(handler._objects);
}

void CIMBaseProviderHandle::enumerateInstanceNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMReference CIMBaseProviderHandle::createInstance(
	const OperationContext & context,
    const String& nameSpace,
    const CIMInstance& newInstance)
{
	const String& className = newInstance.getClassName();
	// build fully qualified reference
	// CIMReference reference = buildReference(newInstance.getInstanceName(_cimom.getClass(nameSpace, newInstance.getClassName())));
	CIMReference reference = buildReference("",nameSpace, className);

	SimpleResponseHandler<CIMReference> handler;

	_provider->createInstance(context, reference, newInstance, handler);

        if (handler._objects.size() == 1)
        {
	    return(handler._objects[0]);
        }
        else
        {
            // ATTN: Address this error condition
            return(reference);
        }
}

void CIMBaseProviderHandle::createInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMInstance& newInstance,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMBaseProviderHandle::modifyInstance(
	const OperationContext & context,
    const String& nameSpace,
    const CIMNamedInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
	// convert flags
	Uint32 flags = convertFlags(false, includeQualifiers, false, false, !propertyList.isNull());

	// build fully qualified reference
	// CIMReference reference; // = buildReference(modifiedInstance.getInstanceName(_cimom.getClass(nameSpace, modifiedInstance.getClassName())));
	CIMReference reference = buildReference(
	    "", nameSpace,
	    modifiedInstance.getInstanceName().getClassName(),
	    modifiedInstance.getInstanceName().getKeyBindings());

	SimpleResponseHandler<CIMInstance> handler;

	// forward request
	_provider->modifyInstance(context, reference, modifiedInstance.getInstance(), flags, propertyList.getPropertyNameArray(), handler);
}

void CIMBaseProviderHandle::modifyInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMNamedInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMBaseProviderHandle::deleteInstance(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName)
{
	// build fully qualified reference
	CIMReference reference = buildReference("", nameSpace, instanceName.getClassName(), instanceName.getKeyBindings());

	SimpleResponseHandler<CIMInstance> handler;

	// forward request
	_provider->deleteInstance(context, reference, handler);
}

void CIMBaseProviderHandle::deleteInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMInstance> CIMBaseProviderHandle::execQuery(
    const OperationContext & context,
    const String& queryLanguage,
    const String& query)
{
	SimpleResponseHandler<CIMObject> handler;

	_provider->executeQuery(context, queryLanguage, query, handler);

	// convert return objects
	Array<CIMInstance> cimInstances;

	for(Uint32 i = 0; i < handler._objects.size(); i++)
	{
	   CIMInstance tmp = CIMInstance(handler._objects[i]);

	   if (tmp)
	      cimInstances.append(tmp);
	}
	
	return(cimInstances);
}

void CIMBaseProviderHandle::execQueryAsync(
	const OperationContext & context,
    const String& queryLanguage,
    const String& query,
	ResponseHandler<CIMObject> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMObjectWithPath> CIMBaseProviderHandle::associators(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
	Uint32 flags = convertFlags(false, includeQualifiers, includeClassOrigin, false, !propertyList.isNull());

	// build fully qualified reference
	CIMReference reference = buildReference(objectName);

	SimpleResponseHandler<CIMObjectWithPath> handler;

	_provider->associators(context, reference, assocClass, resultClass, role, resultRole, flags, propertyList.getPropertyNameArray(), handler);

	return(handler._objects);
}

void CIMBaseProviderHandle::associatorsAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
	ResponseHandler<CIMObjectWithPath> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> CIMBaseProviderHandle::associatorNames(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{
	// build fully qualified reference
	CIMReference reference = buildReference(objectName);

	SimpleResponseHandler<CIMReference> handler;

	_provider->associatorNames(context, reference, assocClass, resultClass, role, resultRole, handler);

	return(handler._objects);
}

void CIMBaseProviderHandle::associatorNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMObjectWithPath> CIMBaseProviderHandle::references(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
	Uint32 flags = convertFlags(false, includeQualifiers, includeClassOrigin, false, !propertyList.isNull());

	// build fully qualified reference
	CIMReference reference = buildReference(objectName);

	SimpleResponseHandler <CIMObjectWithPath> handler;

	_provider->references(context, reference, resultClass, role, flags, propertyList.getPropertyNameArray(), handler);

	return(handler._objects);
}

void CIMBaseProviderHandle::referencesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    ResponseHandler<CIMObjectWithPath> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> CIMBaseProviderHandle::referenceNames(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{
	// build fully qualified reference
	CIMReference reference = buildReference(objectName);

	SimpleResponseHandler<CIMReference> handler;

	_provider->referenceNames(context, reference, resultClass, role, handler);

	return(handler._objects);
}

void CIMBaseProviderHandle::referenceNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMValue CIMBaseProviderHandle::getProperty(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName)
{
	// build fully qualified reference
	CIMReference reference = buildReference("", nameSpace, instanceName.getClassName(), instanceName.getKeyBindings());

	SimpleResponseHandler<CIMValue> handler;

	_provider->getProperty(context, reference, propertyName, handler);

	return(handler._objects[0]);
}

void CIMBaseProviderHandle::getPropertyAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
	ResponseHandler<CIMValue> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMBaseProviderHandle::setProperty(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{
	// build fully qualified reference
	CIMReference reference = buildReference("", nameSpace, instanceName.getClassName(), instanceName.getKeyBindings());

	SimpleResponseHandler<CIMValue> handler;

	_provider->setProperty(context, reference, propertyName, newValue, handler);
}

void CIMBaseProviderHandle::setPropertyAsync(
	const OperationContext & context,
    const String & nameSpace,
    const CIMReference & instanceName,
    const String & propertyName,
    const CIMValue & newValue,
	ResponseHandler<CIMValue> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMValue CIMBaseProviderHandle::invokeMethod(
	const OperationContext & context,
    const String & nameSpace,
    const CIMReference & instanceName,
    const String & methodName,
    const Array<CIMParamValue >& inParameters,
    Array<CIMParamValue> & outParameters)
{
	// build fully qualified reference
	CIMReference reference = buildReference(instanceName);

	SimpleResponseHandler<CIMValue> handler;

	// forward request
	_provider->invokeMethod(context, reference, methodName, inParameters, outParameters, handler);

	return(handler._objects[0]);
}

void CIMBaseProviderHandle::invokeMethodAsync(
	const OperationContext & context,
    const String & nameSpace,
    const CIMReference & instanceName,
    const String & methodName,
    const Array<CIMParamValue> & inParameters,
    Array<CIMParamValue> & outParameters,
	ResponseHandler<CIMValue> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

PEGASUS_NAMESPACE_END
