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
// Author: Yi Zhou (yi_zhou@hp.com)
//
// Modified By: Chip Vincent (cvincent@us.ibm.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderRegistrationProvider.h"

#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>

PEGASUS_NAMESPACE_BEGIN

/**
   The name of the operational status property
*/
static const char _PROPERTY_OPERATIONALSTATUS [] = "OperationalStatus";

/**
   The name of the name property for PG_Provider class
*/
static const char _PROPERTY_PROVIDER_NAME [] = "Name";

/**
   The name of the Name property for PG_ProviderModule class
*/
static const char _PROPERTY_PROVIDERMODULE_NAME [] = "Name";

/**
   The name of the Vendor property for PG_ProviderModule class
*/
static const char _PROPERTY_VENDOR [] = "Vendor";

/**
   The name of the Version property for PG_ProviderModule class
*/
static const char _PROPERTY_VERSION [] = "Version";

/**
   The name of the interface type property for PG_ProviderModule class
*/
static const char _PROPERTY_INTERFACETYPE [] = "InterfaceType";

/**
   The name of the interface version property for PG_ProviderModule class
*/
static const char _PROPERTY_INTERFACEVERSION [] = "InterfaceVersion";

/**
   The name of the location property for PG_ProviderModule class
*/
static const char _PROPERTY_LOCATION [] = "Location";

/**
   The name of the CapabilityID property for provider capabilities class
*/
static const char _PROPERTY_CAPABILITYID [] = "CapabilityID";

/**
   The name of the provider module name  property for provider capabilities class
*/
static const char _PROPERTY_PROVIDERMODULENAME [] = "ProviderModuleName";

/**
   The name of the provider name  property for provider capabilities class
*/
static const char _PROPERTY_PROVIDERNAME [] = "ProviderName";

/**
   The name of the classname property for provider capabilities class
*/
static const char _PROPERTY_CLASSNAME [] = "ClassName";

/**
   The name of the Namespace property for provider capabilities class
*/
static const char _PROPERTY_NAMESPACES [] = "Namespaces";

/**
   The name of the provider type  property for provider capabilities class
*/
static const char _PROPERTY_PROVIDERTYPE [] = "ProviderType";

/**
   The name of the supported properties property for provider capabilities class
*/
static const char _PROPERTY_SUPPORTEDPROPERTIES [] = "SupportedProperties";

/**
   The name of the supported methods property for provider capabilities class
*/
static const char _PROPERTY_SUPPORTEDMETHODS [] = "SupportedMethods";

/**
   Registered instance provider type
*/
static const Uint16 _INSTANCE_PROVIDER    = 2;

/**
   Registered association provider type
*/
static const Uint16 _ASSOCIATION_PROVIDER    = 3;

/**
   Registered indication provider type
*/
static const Uint16 _INDICATION_PROVIDER    = 4;

/**
   Registered method provider type
*/
static const Uint16 _METHOD_PROVIDER    = 5;

/**
   stopping provider method
*/
static const char _STOP_PROVIDER[]     = "Stop";

/**
   starting provider method
*/
static const char _START_PROVIDER[]   = "Start";

/**
   Provider status
*/
static const Uint16 _MODULE_UNKNOWN     = 0;

static const Uint16 _MODULE_OK        = 2;

static const Uint16 _MODULE_STOPPED   = 10;

ProviderRegistrationProvider::ProviderRegistrationProvider(
    ProviderRegistrationManager * providerRegistrationManager)	
    :_id(peg_credential_types::PROVIDER)
{
    _providerRegistrationManager = providerRegistrationManager;

    _controller = &(ModuleController::get_client_handle(_id, &_client_handle));
    if(_client_handle == NULL)
      ThrowUninitializedHandle();
}

ProviderRegistrationProvider::~ProviderRegistrationProvider(void)	
{
    if (_providerRegistrationManager)
    {
	delete _providerRegistrationManager;
    }

    if (_client_handle)
    {
	delete _client_handle;
    }

}

void ProviderRegistrationProvider::initialize(CIMOMHandle & cimom)
{
    // This method should not be called because this is a control provider
    // and is not dynamically loaded through the provider manager
}

void ProviderRegistrationProvider::terminate(void)
{
}

// get registered provider
void ProviderRegistrationProvider::getInstance(
    const OperationContext & context,
    const CIMReference & instanceReference,
    const Uint32 flags,
    const CIMPropertyList & propertyList,
    ResponseHandler<CIMInstance> & handler)
{

    if(!String::equalNoCase(instanceReference.getNameSpace(), 
      	                    PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED, instanceReference.getNameSpace());
    }

    // ensure the class existing in the specified namespace
    String className = instanceReference.getClassName();

    if(!String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDER) &&
       !String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw CIMException(CIM_ERR_NOT_SUPPORTED, className);
    }

    // begin processing the request
    handler.processing();

    CIMInstance instance;

    try
    {
	instance = _providerRegistrationManager->getInstance(instanceReference);
    }
    catch(CIMException& e)
    {
	throw(e);
    }

    handler.deliver(instance);

    // complete processing the request
    handler.complete();
}

// get all registered providers
void ProviderRegistrationProvider::enumerateInstances(
    const OperationContext & context,
    const CIMReference & classReference,
    const Uint32 flags,
    const CIMPropertyList & propertyList,
    ResponseHandler<CIMInstance> & handler)
{
    if(!String::equalNoCase(classReference.getNameSpace(), 
      	                    PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED, classReference.getNameSpace());
    }

    // ensure the class existing in the specified namespace
    String className = classReference.getClassName();

    if(!String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDER) &&
       !String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw CIMException(CIM_ERR_NOT_SUPPORTED, className);
    }

    // begin processing the request
    handler.processing();

    Array<CIMNamedInstance> enumInstances;

    try
    {
        enumInstances = _providerRegistrationManager->enumerateInstances(classReference);
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    // ATTN: remove when CIMNamedInstance removed.
    for(Uint32 i = 0, n = enumInstances.size(); i < n; i++)
    {
	handler.deliver(enumInstances[i].getInstance());
    }

    // complete processing the request
    handler.complete();
}

// get all registered provider names
void ProviderRegistrationProvider::enumerateInstanceNames(
    const OperationContext & context,
    const CIMReference & classReference,
    ResponseHandler<CIMReference> & handler)
{
    if(!String::equalNoCase(classReference.getNameSpace(), 
      	                    PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED, classReference.getNameSpace());
    }

    // ensure the class existing in the specified namespace
    String className = classReference.getClassName();

    if(!String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDER) &&
       !String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw CIMException(CIM_ERR_NOT_SUPPORTED, className);
    }

    // begin processing the request
    handler.processing();

    Array<CIMReference> enumInstanceNames;

    // get all instance names from repository
    try
    {
    	enumInstanceNames =
	    _providerRegistrationManager->enumerateInstanceNames(classReference);
    }
    catch(CIMException& e)
    {
        throw (e);
    }

    handler.deliver(enumInstanceNames);

    // complete processing the request
    handler.complete();
}

// change properties for the registered provider
// only support to change property of Namespaces, property of 
// SupportedProperties, and property of SupportedMethods
void ProviderRegistrationProvider::modifyInstance(
        const OperationContext & _context,
        const CIMReference & instanceReference,
        const CIMInstance & instanceObject,
        const Uint32 flags,
        const CIMPropertyList & propertyList,
        ResponseHandler<CIMInstance> & handler)
{
    if(!String::equalNoCase(instanceReference.getNameSpace(), 
      	                    PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED, instanceReference.getNameSpace());
    }

    //
    // only support to modify the instance of PG_ProviderCapabilities
    //
    if (!String::equalNoCase(instanceReference.getClassName(), 
			     PEGASUS_CLASSNAME_PROVIDERCAPABILITIES))
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED, instanceReference.getClassName());
    }

    //
    // only can modify the property of Namespaces, property of
    // SupportedProperties, and property of SupportedMethods
    //
    if (propertyList.isNull())
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED,
	    "Only can modify Namespaces, SupportedProperties, and SupportedMethods.");
    }

    Array<String> propertyArray = propertyList.getPropertyNameArray();
    for (Uint32 i=0; i<propertyArray.size(); i++)
    {
	if (!String::equalNoCase(propertyArray[i], _PROPERTY_NAMESPACES) &&
	    !String::equalNoCase(propertyArray[i], _PROPERTY_SUPPORTEDPROPERTIES) &&
	    !String::equalNoCase(propertyArray[i], _PROPERTY_SUPPORTEDMETHODS))
	{
	    throw CIMException (CIM_ERR_NOT_SUPPORTED, propertyArray[i]);
	}
    }

    // begin processing the request
    handler.processing();

    try
    {
        _providerRegistrationManager->modifyInstance(instanceReference, 
	    instanceObject, flags, propertyArray);
    }
    catch(CIMException& e)
    {
        throw (e);
    }

    // complete processing the request
    handler.complete();
}

// register a provider
void ProviderRegistrationProvider::createInstance(
    const OperationContext & context,
    const CIMReference & instanceReference,
    const CIMInstance & instanceObject,
    ResponseHandler<CIMReference> & handler)
{
    String className = instanceReference.getClassName();
    String nameSpace = instanceReference.getNameSpace();

    CIMReference returnReference;

    CIMInstance instance = instanceObject;

    if(!String::equalNoCase(nameSpace, PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED, nameSpace);
    }

    // ensure the class existing in the specified namespace
    if(!String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDER) &&
       !String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw CIMException(CIM_ERR_NOT_SUPPORTED, className);
    }

    //
    // Check all required properties are set
    //
    if(String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	//
	// Name, Version, InterfaceType, InterfaceVersion, and Location
	// properties must be set
	// OperationalStatus property needs to be set. If not, set to default
	//
	if (!instanceObject.existsProperty(_PROPERTY_PROVIDERMODULE_NAME))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing Name which is required property in PG_ProviderModule class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_VENDOR))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing Vendor which is required property in PG_ProviderModule class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_VERSION))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing Version which is required property in PG_ProviderModule class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_INTERFACETYPE))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing InterfaceType which is required property in PG_ProviderModule class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_INTERFACEVERSION))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing InterfaceVersion which is required property in PG_ProviderModule class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_LOCATION))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing Location which is required property in PG_ProviderModule class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_OPERATIONALSTATUS))
	{
	    Array<Uint16> _operationalStatus;
	    _operationalStatus.append(_MODULE_UNKNOWN);
	    instance.addProperty (CIMProperty
		(_PROPERTY_OPERATIONALSTATUS, _operationalStatus));
	}
    }
    else if(String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES))
    {
	//
	// ProviderModuleName, ProviderName, InstanceID, ClassName,
 	// Namespaces, and ProviderType properties must be set
	//

	if (!instanceObject.existsProperty(_PROPERTY_PROVIDERMODULENAME))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing ProviderModuleName which is required property in PG_ProviderCapabilities class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_PROVIDERNAME))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing ProviderName which is required property in PG_ProviderCapabilities class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_CAPABILITYID))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing CapabilityID which is required property in PG_ProviderCapabilities class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_CLASSNAME))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing ClassName which is required property in PG_ProviderCapabilities class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_NAMESPACES))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing Namespaces which is required property in PG_ProviderCapabilities class.");
	}

	if (!instanceObject.existsProperty(_PROPERTY_PROVIDERTYPE))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing ProviderType which is required property in PG_ProviderCapabilities class.");
	}
    }
    else // PEGASUS_CLASSNAME_PROVIDER
    {
	//
	// Name and ProviderModuleName properties must be set
	//
	if (!instanceObject.existsProperty(_PROPERTY_PROVIDER_NAME))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing Name which is required property in PG_Provider class.");
	}
	
	if (!instanceObject.existsProperty(_PROPERTY_PROVIDERMODULENAME))
	{
	    throw CIMException (CIM_ERR_FAILED, 
		"Missing ProviderModuleName which is required property in PG_Provider class.");
	}
    }

    // begin processing the request
    handler.processing();
	
    try
    {
    	returnReference =
	    _providerRegistrationManager->createInstance(instanceReference, instance);
    }
    catch(CIMException& e)
    {
        throw (e);
    }

    handler.deliver(returnReference);

    // complete processing request
    handler.complete();
}

// Unregister a provider
void ProviderRegistrationProvider::deleteInstance(
    const OperationContext & context,
    const CIMReference & instanceReference,
    ResponseHandler<CIMInstance> & handler)
{
    if(!String::equalNoCase(instanceReference.getNameSpace(), 
      	                    PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED, instanceReference.getNameSpace());
    }

    String className = instanceReference.getClassName();

    // ensure the class existing in the specified namespace
    if(!String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDER) &&
       !String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw CIMException(CIM_ERR_NOT_SUPPORTED, className);
    }

    // begin processing the request
    handler.processing();

    try
    {
    	_providerRegistrationManager->deleteInstance(instanceReference);
    }
    catch(CIMException& e)
    {
        throw (e);
    }

    // complete processing the request
    handler.complete();
}

// Block a provider, unblock a provider, and stop a provider
void ProviderRegistrationProvider::invokeMethod(
    const OperationContext & context,
    const CIMReference & objectReference,
    const String & methodName,
    const Array<CIMParamValue> & inParameters,
    Array<CIMParamValue> & outParameters,
    ResponseHandler<CIMValue> & handler)
{
    if(!String::equalNoCase(objectReference.getNameSpace(), 
      	                    PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED, objectReference.getNameSpace());
    }

    String moduleName;
    Boolean moduleFound = false;

    // get module name from reference
    Array<KeyBinding> keys = objectReference.getKeyBindings();

    for(Uint32 i=0; i<keys.size() ; i++)
    {
	if(String::equalNoCase(keys[i].getName(), _PROPERTY_PROVIDERMODULE_NAME))
	{
	    moduleName = keys[i].getValue();
	    moduleFound = true;
	}
    }

    // if _PROPERTY_PROVIDERMODULE_NAME key not found
    if( !moduleFound)
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
		"key Name was not found");
    }

    //
    // get module status
    //
    Array<Uint16> _OperationalStatus = 
	_providerRegistrationManager->getProviderModuleStatus( moduleName);

    handler.processing();

    Sint16 ret_value;

    if(String::equalNoCase(methodName, _STOP_PROVIDER))
    {
	for (Uint32 i = 0; i<_OperationalStatus.size(); i++)
	{
	    // retValue equals 1 if module is already disabled
	    if (_OperationalStatus[i] == _MODULE_STOPPED)
	    {
		ret_value = 1;
		CIMValue retValue(ret_value);
		handler.deliver(retValue);
    		handler.complete();
		return;
	    }
	}

	CIMInstance mInstance;
	CIMInstance instance;
	Array<CIMInstance> instances;
	String _moduleName;

	// get module instance
	mInstance = _providerRegistrationManager->getInstance(objectReference);

	// get all provider instances which have same module name as moduleName
	CIMReference providerRef(objectReference.getHost(),
				 objectReference.getNameSpace(),
				 PEGASUS_CLASSNAME_PROVIDER,
				 objectReference.getKeyBindings());
	Array<CIMNamedInstance> namedInstances;
	namedInstances = _providerRegistrationManager->enumerateInstances(providerRef);
	for(Uint32 i = 0, n=namedInstances.size(); i < n; i++)
	{
	    instance = namedInstances[i].getInstance();
	    instance.getProperty(instance.findProperty
	    (_PROPERTY_PROVIDERMODULENAME)).getValue().get(_moduleName);	
	    if (String::equalNoCase(_moduleName, moduleName))
	    {
		instances.append(instance);
	    }
 
	}

        // 
        // get provider manager service
        //
        MessageQueueService * _service = _getProviderManagerService();

	// create CIMDisableModuleRequestMessage
	CIMDisableModuleRequestMessage * disable_req = 
	    new CIMDisableModuleRequestMessage(
		XmlWriter::getNextMessageId (),
		mInstance,
		instances,
		QueueIdStack(_service->getQueueId()));

  	Array<Uint16> _opStatus = 
	    _sendDisableMessageToProviderManager(disable_req);

	for (Uint32 i = 0; i<_opStatus.size(); i++)
	{
	    if (_opStatus[i] == _MODULE_STOPPED)
	    {
		// module was disabled successfully
		ret_value = 0;
		CIMValue retValue(ret_value);
		handler.deliver(retValue);
    		handler.complete();

	 	// send termination message to subscription service
		_sendTerminationMessageToSubscription(objectReference, moduleName);
		return;
	    }
	}

        // disable failed
	ret_value = -1;
	CIMValue retValue(ret_value);
	handler.deliver(retValue);
    	handler.complete();
	return;
    }
    else if(String::equalNoCase(methodName, _START_PROVIDER))
    {
	for (Uint32 i = 0; i<_OperationalStatus.size(); i++)
	{
	    // retValue equals 1 if module is already enabled
	    if (_OperationalStatus[i] == _MODULE_OK)
	    {
		ret_value = 1;
		CIMValue retValue(ret_value);
		handler.deliver(retValue);
    		handler.complete();
		return;
	    }
	}

        // 
        // get provider manager service
        //
        MessageQueueService * _service = _getProviderManagerService();

	// create CIMEnableModuleRequestMessage
	CIMEnableModuleRequestMessage * enable_req = 
	    new CIMEnableModuleRequestMessage(
		XmlWriter::getNextMessageId (),
		moduleName,
		QueueIdStack(_service->getQueueId()));

  	Array<Uint16> _opStatus;
        _opStatus = _sendEnableMessageToProviderManager(enable_req);

	for (Uint32 i = 0; i<_opStatus.size(); i++)
	{
	    if (_opStatus[i] == _MODULE_OK)
	    {
		// module was enabled successfully
		ret_value = 0;
		CIMValue retValue(ret_value);
		handler.deliver(retValue);
    		handler.complete();
		return;
	    }
	}

        // enable failed
	ret_value = -1;
	CIMValue retValue(ret_value);
	handler.deliver(retValue);
    	handler.complete();
	return;
    }
    else
    {
	throw CIMException(CIM_ERR_METHOD_NOT_AVAILABLE);
    }
}

// get provider manager service
MessageQueueService * ProviderRegistrationProvider::_getProviderManagerService()
{
    MessageQueue * queue = MessageQueue::lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);
    MessageQueueService * _service = dynamic_cast<MessageQueueService *>(queue);

    return(_service);    
}

ProviderRegistrationProvider & ProviderRegistrationProvider::operator=(const ProviderRegistrationProvider & handle)
{
    if(this == &handle)
    {
        return(*this);
    }

    return(*this);
}

void ProviderRegistrationProvider::async_callback(Uint32 user_data,
    Message *reply,
    void *parm)
{
   callback_data *cb_data = reinterpret_cast<callback_data *>(parm);
   cb_data->reply = reply;
   cb_data->client_sem.signal();
}

Array<Uint16> ProviderRegistrationProvider::_sendDisableMessageToProviderManager(
        CIMDisableModuleRequestMessage * disable_req)
{
    MessageQueueService * _service = _getProviderManagerService();
    Uint32 _queueId = _service->getQueueId();

    callback_data *cb_data = new callback_data(this);

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart (
            _service->get_next_xid(),
            NULL,
            _queueId,
            disable_req,
            _queueId);

// ATTN-YZ-P2-05032002: Temporarily removed, until asyn_callback fixed
/*
    if( false  == _controller->ClientSendAsync(*_client_handle,
                                               0,
                                               _queueId,
                                               asyncRequest,
                                               ProviderRegistrationProvider::async_callback,
                                               (void *)cb_data) )
    {
       delete asyncRequest;
       delete cb_data;
       throw CIMException(CIM_ERR_NOT_FOUND);

    }

    cb_data->client_sem.wait();
    AsyncReply * asyncReply = static_cast<AsyncReply *>(cb_data->get_reply()) ;

    CIMDisableModuleResponseMessage * response =
	reinterpret_cast<CIMDisableModuleResponseMessage *>(
             (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
	CIMException e = response->cimException;

        delete asyncRequest;
        delete asyncReply;
        delete response;
        delete cb_data;

        throw (e);
    }

    Array<Uint16> operationalStatus = response->operationalStatus; 

    delete asyncRequest;
    delete asyncReply;
    delete response;
    delete cb_data;
*/

// ATTN-YZ-P2-05032002: Temporarily use ClientSendWait, until asyn_callback fixed

    AsyncReply * asyncReply = _controller->ClientSendWait(*_client_handle,
							  _queueId,
							  asyncRequest);
    CIMDisableModuleResponseMessage * response =
	reinterpret_cast<CIMDisableModuleResponseMessage *>(
             (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
	CIMException e = response->cimException;
        delete asyncRequest;
        delete asyncReply;
        delete response;
	throw (e);
    }

    Array<Uint16> operationalStatus = response->operationalStatus; 

    delete asyncRequest;
    delete asyncReply;
    delete response;
    
    return(operationalStatus);
}

Array<Uint16> ProviderRegistrationProvider::_sendEnableMessageToProviderManager(
        CIMEnableModuleRequestMessage * enable_req)
{
    MessageQueueService * _service = _getProviderManagerService();
    Uint32 _queueId = _service->getQueueId();

    callback_data *cb_data = new callback_data(this);

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart (
            _service->get_next_xid(),
            NULL,
            _queueId,
            enable_req,
            _queueId);

// ATTN-YZ-P2-05032002: Temporarily removed, until asyn_callback fixed
/*
    if( false  == _controller->ClientSendAsync(*_client_handle,
                                               0,
                                               _queueId,
                                               asyncRequest,
                                               ProviderRegistrationProvider::async_callback,
                                               (void *)cb_data) )
    {
       delete asyncRequest;
       delete cb_data;
       throw CIMException(CIM_ERR_NOT_FOUND);

    }

    cb_data->client_sem.wait();
    AsyncReply * asyncReply = static_cast<AsyncReply *>(cb_data->get_reply()) ;

    CIMEnableModuleResponseMessage * response =
	reinterpret_cast<CIMEnableModuleResponseMessage *>(
             (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
	CIMException e = response->cimException;
        delete asyncRequest;
        delete asyncReply;
        delete response;
        delete cb_data;
	throw (e);
    }

    Array<Uint16> operationalStatus = response->operationalStatus; 

    delete asyncRequest;
    delete asyncReply;
    delete response;
    delete cb_data;
*/

// ATTN-YZ-P2-05032002: Temporarily use ClientSendWait, until asyn_callback fixed

    AsyncReply * asyncReply = _controller->ClientSendWait(*_client_handle,
							  _queueId,
							  asyncRequest);
    CIMEnableModuleResponseMessage * response =
	reinterpret_cast<CIMEnableModuleResponseMessage *>(
             (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
	CIMException e = response->cimException;
        delete asyncRequest;
        delete asyncReply;
        delete response;
	throw (e);
    }

    Array<Uint16> operationalStatus = response->operationalStatus; 

    delete asyncRequest;
    delete asyncReply;
    delete response;
    
    return(operationalStatus);
}

// send termination message to subscription service
void ProviderRegistrationProvider::_sendTerminationMessageToSubscription(
    const CIMReference & ref, const String & moduleName)
{
    CIMInstance instance;
    String _moduleName;
    Array<CIMInstance> instances;

    CIMReference reference("", PEGASUS_NAMESPACENAME_INTEROP, 
	PEGASUS_CLASSNAME_PROVIDER, ref.getKeyBindings());
 
    // get all registered providers
    Array<CIMNamedInstance> enumInstances =
	_providerRegistrationManager->enumerateInstances(reference); 

    // find all the instances which have same module name as moduleName
    for (Uint32 i = 0, n=enumInstances.size(); i < n; i++)
    {
	instance = enumInstances[i].getInstance();

	//
        // get provider module name
        //
        instance.getProperty(instance.findProperty
	(_PROPERTY_PROVIDERMODULENAME)).getValue().get(_moduleName);

	if (String::equalNoCase(moduleName, _moduleName))
	{
	    instances.append(instance);
	}
    }

    //
    // get indication server queueId
    //
    MessageQueueService * _service = _getIndicationService();
    Uint32 _queueId = _service->getQueueId();

    CIMNotifyProviderTerminationRequestMessage * termination_req =
	new CIMNotifyProviderTerminationRequestMessage(
	    XmlWriter::getNextMessageId (),
	    instances,
	    QueueIdStack(_service->getQueueId()));

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart (
                _service->get_next_xid(),
                NULL,
                _queueId,
                termination_req,
                _queueId);

    if( false  == _controller->ClientSendForget(
                           *_client_handle,
                           _queueId,
                           asyncRequest))
    {
        delete asyncRequest;
        throw CIMException(CIM_ERR_NOT_FOUND);
    }
}

// get indication service
MessageQueueService * ProviderRegistrationProvider::_getIndicationService()
{
    MessageQueue * queue = MessageQueue::lookup(
	PEGASUS_QUEUENAME_INDICATIONSERVICE);

    MessageQueueService * _service =
	dynamic_cast<MessageQueueService *>(queue);
    return(_service);
}

PEGASUS_NAMESPACE_END
