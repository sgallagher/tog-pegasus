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
// Modified By: Markus Mueller (sedgewick_de@yahoo.de)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMBaseProviderFacade.h"
#include <Pegasus/Provider/OperationFlag.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>

#include <Pegasus/Common/Destroyer.h>

PEGASUS_NAMESPACE_BEGIN

CIMBaseProviderFacade::CIMBaseProviderFacade(CIMBaseProvider * provider) : _provider(provider)
{
}

CIMBaseProviderFacade::~CIMBaseProviderFacade(void)	
{
}

void CIMBaseProviderFacade::initialize(CIMOMHandle & cimom)
{
	// initialize the logical provider
	CIMBaseProvider * provider = dynamic_cast<CIMBaseProvider *>(_provider);
	
	if(provider != 0)
	{
		provider->initialize(cimom);
	}
}

void CIMBaseProviderFacade::terminate(void)
{
	// test for the appropriate interface
	CIMBaseProvider * provider = dynamic_cast<CIMBaseProvider *>(_provider);

	if(provider != 0)
	{
		provider->terminate();
	}
}

void CIMBaseProviderFacade::getInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	// test for the appropriate interface
	CIMInstanceProvider * provider = dynamic_cast<CIMInstanceProvider *>(_provider);

	if(provider == 0)
	{
		throw NotSupported("CIMInstanceProvider::*");
	}

	// forward request
	provider->getInstance(context, instanceReference, flags, propertyList, handler);

	// try enumerateInstances if not supported
}

void CIMBaseProviderFacade::enumerateInstances(
	const OperationContext & context,
	const CIMReference & classReference,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	// test for the appropriate interface
	CIMInstanceProvider * provider = dynamic_cast<CIMInstanceProvider *>(_provider);

	if(provider == 0)
	{
		throw NotSupported("CIMInstanceProvider::*");
	}

	// forward request
	provider->enumerateInstances(context, classReference, flags, propertyList, handler);

	// try enumerateInstanceNames and getInstance if not supported
}

void CIMBaseProviderFacade::enumerateInstanceNames(
	const OperationContext & context,
	const CIMReference & classReference,
	ResponseHandler<CIMReference> & handler)
{
	// test for the appropriate interface
	CIMInstanceProvider * provider = dynamic_cast<CIMInstanceProvider *>(_provider);

	if(provider == 0)
	{
		throw NotSupported("CIMInstanceProvider::*");
	}

	// forward request
	provider->enumerateInstanceNames(context, classReference, handler);
	
	// try enumerateInstances if not supported
}

void CIMBaseProviderFacade::modifyInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const CIMInstance & instanceObject,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	// test for the appropriate interface
	CIMInstanceProvider * provider = dynamic_cast<CIMInstanceProvider *>(_provider);

	if(provider == 0)
	{
		throw NotSupported("CIMInstanceProvider::*");
	}

	// forward request
	provider->modifyInstance(context, instanceReference, instanceObject, flags, propertyList, handler);
}

void CIMBaseProviderFacade::createInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const CIMInstance & instanceObject,
	ResponseHandler<CIMReference> & handler)
{
	// test for the appropriate interface
	CIMInstanceProvider * provider = dynamic_cast<CIMInstanceProvider *>(_provider);

	if(provider == 0)
	{
		throw NotSupported("CIMInstanceProvider::*");
	}

	// forward request
	provider->createInstance(context, instanceReference, instanceObject, handler);
}

void CIMBaseProviderFacade::deleteInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	ResponseHandler<CIMInstance> & handler)
{
	// test for the appropriate interface
	CIMInstanceProvider * provider = dynamic_cast<CIMInstanceProvider *>(_provider);

	if(provider == 0)
	{
		throw NotSupported("CIMInstanceProvider::*");
	}

	// forward request
	provider->deleteInstance(context, instanceReference, handler);
}

void CIMBaseProviderFacade::getClass(
	const OperationContext & context,
	const CIMReference & classReference,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMClass> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::getClass");
}

void CIMBaseProviderFacade::enumerateClasses(
	const OperationContext & context,
	const CIMReference & classReference,
	const Uint32 flags,
	ResponseHandler<CIMClass> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::enumerateClasses");
}

void CIMBaseProviderFacade::enumerateClassNames(
	const OperationContext & context,
	const CIMReference & classReference,
	const Uint32 flags,
	ResponseHandler<CIMReference> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::enumerateClassNames");
}

void CIMBaseProviderFacade::modifyClass(
	const OperationContext & context,
	const CIMReference & classReference,
	const CIMClass & classObject,
	ResponseHandler<CIMClass> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::modifyClass");
}

void CIMBaseProviderFacade::createClass(
	const OperationContext & context,
	const CIMReference & classReference,
	const CIMClass & classObject,
	ResponseHandler<CIMClass> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::createClass");
}

void CIMBaseProviderFacade::deleteClass(
	const OperationContext & context,
	const CIMReference & classReference,
	ResponseHandler<CIMClass> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::deleteClass");
}

void CIMBaseProviderFacade::associators(
	const OperationContext & context,
	const CIMReference & objectName,
	const String & associationClass,
	const String & resultClass,
	const String & role,
	const String & resultRole,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMObject> & handler)
{
        // test for the appropriate interface
        CIMAssociationProvider * provider =
             dynamic_cast<CIMAssociationProvider *>(_provider);

        if(provider == 0)
        {
                throw NotSupported("CIMAssociationProvider::associators");
        }

        // forward request
        provider->associators(context, objectName, associationClass,
                              resultClass, role, resultRole, flags,
                              propertyList, handler);
}

void CIMBaseProviderFacade::associatorNames(
	const OperationContext & context,
	const CIMReference & objectName,
	const String & associationClass,
	const String & resultClass,
	const String & role,
	const String & resultRole,
	ResponseHandler<CIMReference> & handler)
{
        // test for the appropriate interface
        CIMAssociationProvider * provider =
             dynamic_cast<CIMAssociationProvider *>(_provider);

        if(provider == 0)
        {
                throw NotSupported("CIMAssociationProvider::associatorNames");
        }

        // forward request
        provider->associatorNames(context, objectName, associationClass,
                                  resultClass, role, resultRole, handler);
}

void CIMBaseProviderFacade::references(
	const OperationContext & context,
	const CIMReference & objectName,
	const String & resultClass,
	const String & role,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMObject> & handler)
{
        // test for the appropriate interface
        CIMAssociationProvider * provider =
             dynamic_cast<CIMAssociationProvider *>(_provider);

        if(provider == 0)
        {
                throw NotSupported("CIMAssociationProvider::references");
        }

        // forward request
        provider->references(context, objectName, resultClass,
                             role, flags, propertyList, handler);
}

void CIMBaseProviderFacade::referenceNames(
	const OperationContext & context,
	const CIMReference & objectName,
	const String & resultClass,
	const String & role,
	ResponseHandler<CIMReference> & handler)
{
        // test for the appropriate interface
        CIMAssociationProvider * provider =
             dynamic_cast<CIMAssociationProvider *>(_provider);

        if(provider == 0)
        {
                throw NotSupported("CIMAssociationProvider::referenceNames");
        }

        // forward request
        provider->referenceNames(context, objectName, resultClass,
                                 role, handler);
}

void CIMBaseProviderFacade::getProperty(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const String & propertyName,
	ResponseHandler<CIMValue> & handler)
{
    Uint32 flags = OperationFlag::PARTIAL_INSTANCE;    // LocalOnly = false
    Array<String> propertyList;
    propertyList.append(propertyName);
    SimpleResponseHandler<CIMInstance> instanceHandler;

    handler.processing();
    getInstance(
        context, instanceReference, flags, propertyList, instanceHandler);

    if (instanceHandler._objects.size())
    {
        CIMInstance instance = instanceHandler._objects[0];
        Uint32 pos = instance.findProperty(propertyName);
        if (pos == PEG_NOT_FOUND)
        {
            // ATTN: The provider should throw an exception if we gave them
            // a bad property name.  Can we assume the value is null?
            handler.deliver(CIMValue());
        }
        else
        {
            handler.deliver(instance.getProperty(pos).getValue());
        }
    }
    else
    {
        // ATTN: Shouldn't the provider really throw this?
        // This condition may indicate a faulty provider.
        throw CIMException(CIM_ERR_NOT_FOUND);
    }
    handler.complete();
}

void CIMBaseProviderFacade::setProperty(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const String & propertyName,
	const CIMValue & newValue,
	ResponseHandler<CIMValue> & handler)
{
    //
    // Create the instance to pass to modifyInstance()
    //
    CIMInstance instance(instanceReference.getClassName());
    // ATTN: Is this the correct construction for this property?
    instance.addProperty(CIMProperty(propertyName, newValue));

    //
    // Create the flags and propertyList to pass to modifyInstance()
    //
    Uint32 flags = OperationFlag::PARTIAL_INSTANCE;  // IncludeQualifiers false
    Array<String> propertyList;
    propertyList.append(propertyName);

    //
    // Create the ResponseHandler to pass to modifyInstance().
    // What is this for?  I don't know.
    //
    SimpleResponseHandler<CIMInstance> instanceHandler;

    //
    // Modify the instance to set the value of the given property
    //
    handler.processing();
    modifyInstance(
        context,
        instanceReference,
        instance,
        flags,
        propertyList,
        instanceHandler);
    handler.complete();
}

void CIMBaseProviderFacade::invokeMethod(
	const OperationContext & context,
	const CIMReference & objectReference,
	const String & methodName,
	const Array<CIMParamValue> & inParameters,
	Array<CIMParamValue> & outParameters,
	ResponseHandler<CIMValue> & handler)
{
        // test for the appropriate interface
        CIMMethodProvider * provider =
            dynamic_cast<CIMMethodProvider *>(_provider);

        if(provider == 0)
        {
                throw NotSupported("CIMMethodProvider::invokeMethod");
        }

        // forward request
        provider->invokeMethod(context, objectReference, methodName,
                               inParameters, outParameters, handler);
}

void CIMBaseProviderFacade::executeQuery(
	const OperationContext & context,
	const String & queryLanguage,
	const String & query,
	ResponseHandler<CIMObject> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::executeQuery");
}

void CIMBaseProviderFacade::provideIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	const CIMDateTime & minimumInterval,
	const CIMDateTime & maximumInterval,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::provideIndication");
}

void CIMBaseProviderFacade::updateIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	const CIMDateTime & minimumInterval,
	const CIMDateTime & maximumInterval,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::updateIndication");
}

void CIMBaseProviderFacade::cancelIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	ResponseHandler<CIMInstance> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::cancelIndication");
}

void CIMBaseProviderFacade::checkIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::checkIndication");
}

void CIMBaseProviderFacade::handleIndication(
	const OperationContext & context,
	const CIMInstance & indication,
	ResponseHandler<CIMInstance> & handler)
{
	throw NotSupported("CIMBaseProviderFacade::handleIndication");
}

void CIMBaseProviderFacade::enableIndication(
	const OperationContext & context,
	const String & nameSpace,
	const Array<String> & classNames,
	const String & providerName,
	const Array<String> & propertyList,
	const Uint16 repeatNotificationPolicy,
	const String & otherRepeatNotificationPolicy,
	const CIMDateTime & repeatNotificationInterval,
	const CIMDateTime & repeatNotificationGap,
	const Uint16 repeatNotificationCount,
	const String & condition,
	const String & queryLanguage,
	const CIMInstance & subscription,
	ResponseHandler<CIMInstance> & handler)
{
    	// Get appropriate interface
	CIMIndicationProvider * provider = 
	    dynamic_cast<CIMIndicationProvider *>(_provider);

	if(provider == 0)
	{
		throw NotSupported("CIMInstanceProvider::*");
	}

	// forward request
	provider->enableIndication(context, nameSpace, classNames,
	    providerName, propertyList, repeatNotificationPolicy, 
	    otherRepeatNotificationPolicy, repeatNotificationInterval, 
	    repeatNotificationGap, repeatNotificationCount, condition, 
	    queryLanguage, subscription, handler);
}


void CIMBaseProviderFacade::disableIndication(
	const OperationContext & context,
	const String & nameSpace,
	const Array<String> & classNames,
	const String & providerName,
	const CIMInstance & subscription,
	ResponseHandler<CIMInstance> & handler)
{
	// Get appropriate interface
	CIMIndicationProvider * provider = 
	    dynamic_cast<CIMIndicationProvider *>(_provider);

	if(provider == 0)
	{
		throw NotSupported("CIMInstanceProvider::*");
	}

	// forward request
	provider->disableIndication(context, nameSpace, classNames, 
	    providerName, subscription, handler);
}

void CIMBaseProviderFacade::modifyIndication(
	const OperationContext & context,
	const String & nameSpace,
	const Array<String> & classNames,
	const String & providerName,
	const Array<String> & propertyList,
	const Uint16 repeatNotificationPolicy,
	const String & otherRepeatNotificationPolicy,
	const CIMDateTime & repeatNotificationInterval,
	const CIMDateTime & repeatNotificationGap,
	const Uint16 repeatNotificationCount,
	const String & condition,
	const String & queryLanguage,
	const CIMInstance & subscription,
	ResponseHandler<CIMInstance> & handler)
{
	// Get appropriate interface
	CIMIndicationProvider * provider = 
	    dynamic_cast<CIMIndicationProvider *>(_provider);

	if(provider == 0)
	{
		throw NotSupported("CIMInstanceProvider::*");
	}

	// forward 
	provider->modifyIndication(context, nameSpace, 
	    classNames, providerName, propertyList, repeatNotificationPolicy, 
	    otherRepeatNotificationPolicy, repeatNotificationInterval, 
	    repeatNotificationGap, repeatNotificationCount, condition, 
	    queryLanguage, subscription, handler);
}

PEGASUS_NAMESPACE_END
