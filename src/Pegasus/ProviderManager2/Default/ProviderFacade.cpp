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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Markus Mueller (sedgewick_de@yahoo.de)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Sushma Fernandes, Hewlett-Packard Company
//                  (sushma_fernandes@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderFacade.h"

#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/MessageLoader.h>

#include <Pegasus/ProviderManager2/SimpleResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN

class op_counter
{
public:
    op_counter(AtomicInt *counter)
    : _counter(counter)
    {
        (*_counter)++;
    }
    ~op_counter(void)
    {
        (*_counter)--;
    }
private:
    op_counter(void);
    AtomicInt *_counter;
};


template<class T>
inline T * getInterface(CIMProvider * provider)
{
    T * p = dynamic_cast<T *>(provider);

    if(p == 0)
    {
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "Invalid provider interface.");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED, MessageLoaderParms(
            "ProviderManager.ProviderFacade.INVALID_PROVIDER_INTERFACE",
            "Invalid provider interface."));
    }

    return(p);
}

ProviderFacade::ProviderFacade(CIMProvider * provider) : _provider(provider)
{
   _indications_enabled=false;
}

ProviderFacade::~ProviderFacade(void)
{
}

void ProviderFacade::initialize(CIMOMHandle & cimom)
{
    _provider->initialize(cimom);
}

#ifdef PEGASUS_PRESERVE_TRYTERMINATE
Boolean ProviderFacade::tryTerminate(void)
{
    return(_provider->tryTerminate());
}
#endif

void ProviderFacade::terminate(void)
{
    _provider->terminate();
}

void ProviderFacade::getInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{
    op_counter ops(&_current_operations);

    CIMInstanceProvider * provider = getInterface<CIMInstanceProvider>(_provider);

    // forward request
    provider->getInstance(
        context,
        instanceReference,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        handler);

}

void ProviderFacade::enumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{
    op_counter ops(&_current_operations);
    CIMInstanceProvider * provider = getInterface<CIMInstanceProvider>(_provider);

    // forward request
    provider->enumerateInstances(
        context,
        classReference,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        handler);

    // try enumerateInstanceNames and getInstance if not supported
}

void ProviderFacade::enumerateInstanceNames(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    ObjectPathResponseHandler & handler)
{
    op_counter ops(&_current_operations);
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
    const Boolean includeQualifiers,
    const CIMPropertyList & propertyList,
    ResponseHandler & handler)
{
    op_counter ops(&_current_operations);
    CIMInstanceProvider * provider = getInterface<CIMInstanceProvider>(_provider);

    // forward request
    provider->modifyInstance(
        context,
        instanceReference,
        instanceObject,
        includeQualifiers,
        propertyList,
        handler);
}

void ProviderFacade::createInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance & instanceObject,
    ObjectPathResponseHandler & handler)
{
    op_counter ops(&_current_operations);
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
    ResponseHandler & handler)
{
    op_counter ops(&_current_operations);
    CIMInstanceProvider * provider = getInterface<CIMInstanceProvider>(_provider);

    // forward request
    provider->deleteInstance(
        context,
        instanceReference,
        handler);
}

void ProviderFacade::execQuery(
    const OperationContext & context,
    const CIMObjectPath & nameSpaceAndClass,
    const QueryExpression & query,
    InstanceResponseHandler & handler)
{
    op_counter ops(&_current_operations);
    CIMInstanceQueryProvider *provider =
               getInterface<CIMInstanceQueryProvider>(_provider);
   // forward request
    provider->execQuery(
        context,
        nameSpaceAndClass,
        query,
        handler);
}


void ProviderFacade::associators(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & associationClass,
    const CIMName & resultClass,
    const String & role,
    const String & resultRole,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    ObjectResponseHandler & handler)
{
    op_counter ops(&_current_operations);
    CIMAssociationProvider * provider = getInterface<CIMAssociationProvider>(_provider);

    // forward request
    provider->associators(
        context,
        objectName,
        associationClass,
        resultClass,
        role,
        resultRole,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        handler);
}

void ProviderFacade::associatorNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & associationClass,
    const CIMName & resultClass,
    const String & role,
    const String & resultRole,
    ObjectPathResponseHandler & handler)
{
    op_counter ops(&_current_operations);
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
    const CIMName & resultClass,
    const String & role,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    ObjectResponseHandler & handler)
{
    op_counter ops(&_current_operations);
    CIMAssociationProvider * provider = getInterface<CIMAssociationProvider>(_provider);

    // forward request
    provider->references(
        context,
        objectName,
        resultClass,
        role,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        handler);
}

void ProviderFacade::referenceNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & resultClass,
    const String & role,
    ObjectPathResponseHandler & handler)
{
    op_counter ops(&_current_operations);
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
    const CIMName & propertyName,
    ValueResponseHandler & handler)
{
    op_counter ops(&_current_operations);

    // NOTE: Use the CIMInstanceProvider interface
    handler.processing();

    Array<CIMName> propertyList;

    propertyList.append(propertyName);

    SimpleInstanceResponseHandler instanceHandler;

    getInstance(
        context,
        instanceReference,
        false,  // includeQualifiers
        false,  // includeClassOrigin
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
        // Property not found. Return CIM_ERR_NO_SUCH_PROPERTY.
        else
        {
            handler.complete();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NO_SUCH_PROPERTY,
                propertyName.getString());
        }
    }

    handler.complete();
}

void ProviderFacade::setProperty(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMName & propertyName,
    const CIMValue & newValue,
    ResponseHandler & handler)
{
    op_counter ops(&_current_operations);

    // NOTE: Use the CIMInstanceProvider interface
    handler.processing();

    CIMInstance instance(instanceReference.getClassName());

    instance.addProperty(CIMProperty(propertyName, newValue));

    Array<CIMName> propertyList;

    propertyList.append(propertyName);

    SimpleInstanceResponseHandler instanceHandler;

    modifyInstance(
        context,
        instanceReference,
        instance,
        false,  // includeQualifiers
        propertyList,
        instanceHandler);

    handler.complete();
}

void ProviderFacade::invokeMethod(
    const OperationContext & context,
    const CIMObjectPath & objectReference,
    const CIMName & methodName,
    const Array<CIMParamValue> & inParameters,
    MethodResultResponseHandler & handler)
{
    op_counter ops(&_current_operations);
    CIMMethodProvider * provider = getInterface<CIMMethodProvider>(_provider);

    // forward request
    provider->invokeMethod(
        context,
        objectReference,
        methodName,
        inParameters,
        handler);
}

void ProviderFacade::enableIndications(IndicationResponseHandler & handler)
{
    _indications_enabled = true;
    // _current_operations++;
    op_counter ind_ops(&_current_operations);

    CIMIndicationProvider * provider = getInterface<CIMIndicationProvider>(_provider);

    // forward request
    provider->enableIndications(handler);
}

void ProviderFacade::disableIndications(void)
{
    CIMIndicationProvider * provider = getInterface<CIMIndicationProvider>(_provider);

    // forward request
    provider->disableIndications();
    _indications_enabled = false;
    // _current_operations--;
}

void ProviderFacade::createSubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
    op_counter ops(&_current_operations);
//    op_counter ind_ops(&_current_ind_operations);
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
    op_counter ops(&_current_operations);
//    op_counter ind_ops(&_current_ind_operations);
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
    op_counter ops(&_current_operations);

  //  op_counter ind_ops(&_current_ind_operations);

    CIMIndicationProvider * provider = getInterface<CIMIndicationProvider>(_provider);

    // forward request
    provider->deleteSubscription(
        context,
        subscriptionName,
        classNames);
}

// CIMIndicationConsumerProvider interface
void ProviderFacade::consumeIndication(
    const OperationContext & context,
    const String & destinationPath,
    const CIMInstance & indication)
{
    op_counter ops(&_current_operations);

    CIMIndicationConsumerProvider * provider = getInterface<CIMIndicationConsumerProvider>(_provider);

    provider->consumeIndication(
        context,
        destinationPath,
        indication);
}

PEGASUS_NAMESPACE_END
