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
// Modified By: Markus Mueller (sedgewick_de@yahoo.de)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderFacade.h"

#include <Pegasus/Provider/OperationFlag.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>

#include <Pegasus/Common/Destroyer.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
inline T * getInterface(CIMBaseProvider * provider)
{
    T * p = dynamic_cast<T *>(provider);

    if(p == 0)
    {
        throw CIMException(CIM_ERR_NOT_SUPPORTED, "Invalid provider interface.");
    }

    return(p);
}

ProviderFacade::ProviderFacade(CIMBaseProvider * provider) : _provider(provider)
{
}

ProviderFacade::~ProviderFacade(void)
{
}

void ProviderFacade::initialize(CIMOMHandle & cimom)
{
    _provider->initialize(cimom);
}

void ProviderFacade::terminate(void)
{
    _provider->terminate();
}

void ProviderFacade::getInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const Uint32 flags,
    const CIMPropertyList & propertyList,
    ResponseHandler<CIMInstance> & handler)
{
    CIMInstanceProvider * provider = getInterface<CIMInstanceProvider>(_provider);

    // forward request
    provider->getInstance(
        context,
        instanceReference,
        flags,
        propertyList,
        handler);

    // ATTN: how persistent should the facade be? that is, should the provider attempt
    // to resolve client requests when a provider does not explicity support an
    // operation? If so, how does a provider NOT support an operation? Is it valid for
    // a provider to NOT support individual gets or enumeratation of instances and/or
    // instance names? What is the minimum instance provider?
    //
    // Is it valid to assume providers support a particular operation by implementing it.

    /*
    // attempt CIMInstanceProvider::getInstance()
    try
    {
    // test for the appropriate interface
    CIMInstanceProvider * provider = dynamic_cast<CIMInstanceProvider *>(_provider);

    if(provider != 0)
    {
    // forward request
    provider->getInstance(context, instanceReference, flags, propertyList, handler);

    return;
    }
    }
    catch(...)
    {
    }

    // attempt CIMInstanceProvider::enumerateInstances()
    try
    {
    // test for the appropriate interface
    CIMInstanceProvider * provider = dynamic_cast<CIMInstanceProvider *>(_provider);

    if(provider != 0)
    {
    // forward request
    provider->enumerateInstances(context, instanceReference, flags, propertyList, handler);

    return;
    }
    }
    catch(...)
    {
    }

    // attempt CIMInstanceProvider::enumerateInstanceNames()
    try
    {
    }
    catch(...)
    {
    }
    */
}

void ProviderFacade::enumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const Uint32 flags,
    const CIMPropertyList & propertyList,
    ResponseHandler<CIMInstance> & handler)
{
    CIMInstanceProvider * provider = getInterface<CIMInstanceProvider>(_provider);

    // forward request
    provider->enumerateInstances(
        context,
        classReference,
        flags,
        propertyList,
        handler);

    // try enumerateInstanceNames and getInstance if not supported
}

void ProviderFacade::enumerateInstanceNames(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    ResponseHandler<CIMObjectPath> & handler)
{
    CIMInstanceProvider * provider = getInterface<CIMInstanceProvider>(_provider);

    // forward request
    provider->enumerateInstanceNames(
        context,
        classReference,
        handler);

    // try enumerateInstances if not supported
}

void ProviderFacade::modifyInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance & instanceObject,
    const Uint32 flags,
    const CIMPropertyList & propertyList,
    ResponseHandler<CIMInstance> & handler)
{
    CIMInstanceProvider * provider = getInterface<CIMInstanceProvider>(_provider);

    // forward request
    provider->modifyInstance(
        context,
        instanceReference,
        instanceObject,
        flags,
        propertyList,
        handler);
}

void ProviderFacade::createInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance & instanceObject,
    ResponseHandler<CIMObjectPath> & handler)
{
    CIMInstanceProvider * provider = getInterface<CIMInstanceProvider>(_provider);

    // forward request
    provider->createInstance(
        context,
        instanceReference,
        instanceObject,
        handler);
}

void ProviderFacade::deleteInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    ResponseHandler<CIMInstance> & handler)
{
    CIMInstanceProvider * provider = getInterface<CIMInstanceProvider>(_provider);

    // forward request
    provider->deleteInstance(
        context,
        instanceReference,
        handler);
}

void ProviderFacade::getClass(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const Uint32 flags,
    const CIMPropertyList & propertyList,
    ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED, "ProviderFacade::getClass");
}

void ProviderFacade::enumerateClasses(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const Uint32 flags,
    ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED, "ProviderFacade::enumerateClasses");
}

void ProviderFacade::enumerateClassNames(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const Uint32 flags,
    ResponseHandler<CIMObjectPath> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED, "ProviderFacade::enumerateClassNames");
}

void ProviderFacade::modifyClass(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const CIMClass & classObject,
    ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED, "ProviderFacade::modifyClass");
}

void ProviderFacade::createClass(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const CIMClass & classObject,
    ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED, "ProviderFacade::createClass");
}

void ProviderFacade::deleteClass(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED, "ProviderFacade::deleteClass");
}

void ProviderFacade::associators(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const String & associationClass,
    const String & resultClass,
    const String & role,
    const String & resultRole,
    const Uint32 flags,
    const CIMPropertyList & propertyList,
    ResponseHandler<CIMObject> & handler)
{
    CIMAssociationProvider * provider = getInterface<CIMAssociationProvider>(_provider);

    // forward request
    provider->associators(
        context,
        objectName,
        associationClass,
        resultClass,
        role,
        resultRole,
        flags,
        propertyList,
        handler);
}

void ProviderFacade::associatorNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const String & associationClass,
    const String & resultClass,
    const String & role,
    const String & resultRole,
    ResponseHandler<CIMObjectPath> & handler)
{
    CIMAssociationProvider * provider = getInterface<CIMAssociationProvider>(_provider);

    // forward request
    provider->associatorNames(
        context,
        objectName,
        associationClass,
        resultClass,
        role,
        resultRole,
        handler);
}

void ProviderFacade::references(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const String & resultClass,
    const String & role,
    const Uint32 flags,
    const CIMPropertyList & propertyList,
    ResponseHandler<CIMObject> & handler)
{
    CIMAssociationProvider * provider = getInterface<CIMAssociationProvider>(_provider);

    // forward request
    provider->references(
        context,
        objectName,
        resultClass,
        role,
        flags,
        propertyList,
        handler);
}

void ProviderFacade::referenceNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const String & resultClass,
    const String & role,
    ResponseHandler<CIMObjectPath> & handler)
{
    CIMAssociationProvider * provider = getInterface<CIMAssociationProvider>(_provider);

    // forward request
    provider->referenceNames(
        context,
        objectName,
        resultClass,
        role,
        handler);
}

void ProviderFacade::getProperty(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const String & propertyName,
    ResponseHandler<CIMValue> & handler)
{
    // NOTE: CIMPropertyProvider interface not supported yet
    /*
    CIMPropertyProvider * provider = getInterface<CIMPropertyProvider>(_provider);

    // forward request
    provider->getProperty(
    context,
    objectName,
    propertyName,
    handler);
    */

    // NOTE: Use the CIMInstanceProvider interface until CIMPropertyProvider is supported
    handler.processing();

    Uint32 flags = OperationFlag::NONE;

    Array<String> propertyList;

    propertyList.append(propertyName);

    SimpleResponseHandler<CIMInstance> instanceHandler;

    getInstance(
        context,
        instanceReference,
        flags,
        propertyList,
        instanceHandler);

    if(instanceHandler.getObjects().size())
    {
        CIMInstance instance = instanceHandler.getObjects()[0];

        Uint32 pos = instance.findProperty(propertyName);

        if(pos != PEG_NOT_FOUND)
        {
            handler.deliver(instance.getProperty(pos).getValue());
        }
    }

    handler.complete();
}

void ProviderFacade::setProperty(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const String & propertyName,
    const CIMValue & newValue,
    ResponseHandler<CIMValue> & handler)
{
    // NOTE: CIMPropertyProvider interface not supported yet
    /*
    CIMPropertyProvider * provider = getInterface<CIMPropertyProvider>(_provider);

    // forward request
    provider->setProperty(
    context,
    objectName,
    propertyName,
    newValue,
    handler);
    */

    // NOTE: Use the CIMInstanceProvider interface until CIMPropertyProvider is supported
    handler.processing();

    CIMInstance instance(instanceReference.getClassName());

    instance.addProperty(CIMProperty(propertyName, newValue));

    Uint32 flags = OperationFlag::NONE;

    Array<String> propertyList;

    propertyList.append(propertyName);

    SimpleResponseHandler<CIMInstance> instanceHandler;

    modifyInstance(
        context,
        instanceReference,
        instance,
        flags,
        propertyList,
        instanceHandler);

    handler.complete();
}

void ProviderFacade::invokeMethod(
    const OperationContext & context,
    const CIMObjectPath & objectReference,
    const String & methodName,
    const Array<CIMParamValue> & inParameters,
    Array<CIMParamValue> & outParameters,
    ResponseHandler<CIMValue> & handler)
{
    CIMMethodProvider * provider = getInterface<CIMMethodProvider>(_provider);

    // forward request
    provider->invokeMethod(context, objectReference, methodName,
        inParameters, outParameters, handler);
}

void ProviderFacade::executeQuery(
    const OperationContext & context,
    const String & nameSpace,
    const String & queryLanguage,
    const String & query,
    ResponseHandler<CIMObject> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED, "CIMQueryProvider::*");
}

void ProviderFacade::enableIndications(ResponseHandler<CIMIndication> & handler)
{
    CIMIndicationProvider * provider = getInterface<CIMIndicationProvider>(_provider);

    // forward request
    provider->enableIndications(handler);
}

void ProviderFacade::disableIndications(void)
{
    CIMIndicationProvider * provider = getInterface<CIMIndicationProvider>(_provider);

    // forward request
    provider->disableIndications();
}

void ProviderFacade::createSubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
    CIMIndicationProvider * provider = getInterface<CIMIndicationProvider>(_provider);

    // forward request
    provider->createSubscription(
        context,
        subscriptionName,
        classNames,
        propertyList,
        repeatNotificationPolicy);
}

void ProviderFacade::modifySubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
    CIMIndicationProvider * provider = getInterface<CIMIndicationProvider>(_provider);

    // forward request
    provider->modifySubscription(
        context,
        subscriptionName,
        classNames,
        propertyList,
        repeatNotificationPolicy);
}

void ProviderFacade::deleteSubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames)
{
    CIMIndicationProvider * provider = getInterface<CIMIndicationProvider>(_provider);

    // forward request
    provider->deleteSubscription(
        context,
        subscriptionName,
        classNames);
}

PEGASUS_NAMESPACE_END
