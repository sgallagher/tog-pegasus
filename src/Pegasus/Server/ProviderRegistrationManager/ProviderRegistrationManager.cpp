//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, Hewlett-Packard Company, 
// BMC Software, Tivoli Systems, IBM
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderRegistrationManager.h"

#include <Pegasus/Provider/SimpleResponseHandler.h>

#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Provider/OperationFlag.h>

#include "ProviderRegistrationTable.h"

#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_NAMESPACE_BEGIN

/**
   The name of the operational status property 
*/
static const char _PROPERTY_OPERATIONALSTATUS [] = "OperationalStatus";

/**
   The name of the provider module name  property for provider capabilities 
   class and PG_Provider class
*/
static const char _PROPERTY_PROVIDERMODULENAME [] = "ProviderModuleName";

/**
   The name of the Name property for PG_ProviderModule class
*/
static const char _PROPERTY_PROVIDERMODULE_NAME [] = "Name";

/**
   The name of the CapabilitiesID property for provider capabilities class
*/
static const char _PROPERTY_CAPABILITIESID [] = "CapabilityID";

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
   The name of the Name property for PG_Provider class
*/
static const char _PROPERTY_PROVIDER_NAME [] = "Name";

/**
   Registered instance provider 
*/
static const char INS_PROVIDER [] = "Instance";

/**
   Registered Association provider 
*/
static const char ASSO_PROVIDER [] = "Association";

/**
   Registered Indication provider 
*/
static const char IND_PROVIDER [] = "Indication";

/**
   Registered Method provider 
*/
static const char MET_PROVIDER [] = "Method";

/**
   Registered module
*/
static const char MODULE_KEY [] = "Module";

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
   Provider status
*/
static const Uint16 _PROVIDER_OK        = 2;

static const Uint16 _PROVIDER_STOPPED   = 10;

/**
    Hash table to store providerModule, provider, and providerCapabilities 
     instances.
*/
typedef HashTable<String,
    ProviderRegistrationTable*, EqualFunc<String>,HashFunc<String> > Table;

struct RegistrationTable
{
    Table table;
};

ProviderRegistrationManager::ProviderRegistrationManager(CIMRepository* repository)	
    : _repository(repository)
{
    _registrationTable = new RegistrationTable;

    // 
    // get all registered providers from repository and add them to the table
    //
    _initialRegistrationTable();


}

ProviderRegistrationManager::~ProviderRegistrationManager(void)	
{
    if (_registrationTable)
    {
        delete _registrationTable;
    }
}

Boolean ProviderRegistrationManager::lookupInstanceProvider(
    const String & nameSpace, 
    const String & className,
    CIMInstance & provider,
    CIMInstance & providerModule)
{
    String providerName;
    String providerModuleName;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
	"ProviderRegistrationManager::lookupInstanceProvider");

    ProviderRegistrationTable* providerCapability = 0;
    ProviderRegistrationTable* _provider= 0;
    ProviderRegistrationTable* _providerModule = 0;
    
    //
    // create the key by using nameSpace, className, and providerType
    //
    String capabilityKey = _generateKey(nameSpace, className, INS_PROVIDER);
    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
		     "\nnameSpace = " + nameSpace + "; className = " +
			className +  "; capabilityKey = " + capabilityKey);

    try
    {
        // 
        // get provider capability instance from the table
        //
        if (!_registrationTable->table.lookup(capabilityKey, providerCapability))
        {
            PEG_METHOD_EXIT();
            throw CIMException(CIM_ERR_FAILED, "provider capability has not been registered yet");
        }

        Array<CIMInstance> instances = providerCapability->getInstances();

        Uint32 pos = instances[0].findProperty(_PROPERTY_PROVIDERNAME);

        if (pos == PEG_NOT_FOUND)
        {
            PEG_METHOD_EXIT();
    	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
        }

        //
        // get provider name
        //
        instances[0].getProperty(pos).getValue().get(providerName);

        //
        // get provider module name
        //
        Uint32 pos2 = instances[0].findProperty(_PROPERTY_PROVIDERMODULENAME);
        if (pos2 == PEG_NOT_FOUND)
        {
            PEG_METHOD_EXIT();
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
        }

        instances[0].getProperty(pos2).getValue().get(providerModuleName);

	//
	// create the key by using providerModuleName and providerName
	//
	String _providerKey = _generateKey(providerModuleName, providerName);

	//
	// create the key by using providerModuleName and MODULE_KEY
	//
	String _moduleKey = _generateKey(providerModuleName, MODULE_KEY);

        // 
        // get provider instance from the table 
        //
        if (!_registrationTable->table.lookup(_providerKey, _provider))
        {
            PEG_METHOD_EXIT();
	    throw CIMException(CIM_ERR_FAILED, "can not find the provider");
        }

        Array<CIMInstance> providerInstances = _provider->getInstances();
        provider = providerInstances[0];

        // 
        // get provider module instance from the table 
        //
        if (!_registrationTable->table.lookup(_moduleKey, _providerModule))
        {
            PEG_METHOD_EXIT();
	    throw CIMException(CIM_ERR_FAILED, "can not find the provider module");
        }

        Array<CIMInstance> providerModuleInstances = _providerModule->getInstances();
        providerModule = providerModuleInstances[0];
    
    }
    catch(CIMException & exception)
    {
	Tracer::traceCIMException(TRC_PROVIDERMANAGER, Tracer::LEVEL4, exception);
	PEG_METHOD_EXIT();
	return (false);
    }

    PEG_METHOD_EXIT();
    return (true);
}

Boolean ProviderRegistrationManager::lookupMethodProvider(
    const String & nameSpace, 
    const String & className, 
    const String & method, 
    CIMInstance & provider,
    CIMInstance & providerModule)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
	"ProviderRegistrationManager::lookupMethodProvider");

    String providerName;
    String providerModuleName;
    Array<CIMInstance> instances;
    CIMInstance instance;

    ProviderRegistrationTable* providerCapability = 0;
    ProviderRegistrationTable* _provider= 0;
    ProviderRegistrationTable* _providerModule = 0;

  try
  {
    //
    // check if the provider was registerted to support all methods
    // create the key by using nameSpace, className, allMethods, and providerType
    //
    String capabilityKey = _generateKey(nameSpace, className, "{}", MET_PROVIDER);

    if (_registrationTable->table.lookup(capabilityKey, providerCapability))
    {
        // provider was registered to support all the methods

	instances = providerCapability->getInstances();

 	//
	// get provider name
	//
	Uint32 pos = instances[0].findProperty(_PROPERTY_PROVIDERNAME);
    	if (pos == PEG_NOT_FOUND)
    	{
            PEG_METHOD_EXIT();
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	}

	instances[0].getProperty(pos).getValue().get(providerName);

	//
	// get provider module name
	//
	Uint32 pos2 = instances[0].findProperty(_PROPERTY_PROVIDERMODULENAME);
    	if (pos2 == PEG_NOT_FOUND)
    	{
            PEG_METHOD_EXIT();
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	}

    	instances[0].getProperty(pos2).getValue().get(providerModuleName);
    }
    else
    {
	//
	// provider was not registered to support all the methods
        // create the key by using nameSpace, className, method, and providerType
	//
    	capabilityKey = _generateKey(nameSpace, className, method, MET_PROVIDER);
        if (_registrationTable->table.lookup(capabilityKey, providerCapability))
	{
	    //
	    // provider was registerted to support the method
	    //
	    instances = providerCapability->getInstances();

	    //
	    // get provider name
	    //
	    Uint32 pos = instances[0].findProperty(_PROPERTY_PROVIDERNAME);
    	    if (pos == PEG_NOT_FOUND)
    	    {
                PEG_METHOD_EXIT();
	    	throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	    }

	    instances[0].getProperty(pos).getValue().get(providerName);

	    //
	    // get provider module name
	    //
	    Uint32 pos2 = instances[0].findProperty(_PROPERTY_PROVIDERMODULENAME);
    	    if (pos2 == PEG_NOT_FOUND)
    	    {
                PEG_METHOD_EXIT();
	    	throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	    }
    	    instances[0].getProperty(pos2).getValue().get(providerModuleName);
	}
	else
	{
            PEG_METHOD_EXIT();
            throw CIMException(CIM_ERR_FAILED, "provider has not been registered yet");
	}
	
    }

    //
    // create the key by using providerModuleName and providerName
    //
    String _providerKey = _generateKey(providerModuleName, providerName);

    //
    // create the key by using providerModuleName and MODULE_KEY
    //
    String _moduleKey = _generateKey(providerModuleName, MODULE_KEY);

    // 
    // get provider instance from the table 
    //
    if (!_registrationTable->table.lookup(_providerKey, _provider))
    {
        PEG_METHOD_EXIT();
	throw CIMException(CIM_ERR_FAILED, "can not find the provider");
    }

    Array<CIMInstance> providerInstances = _provider->getInstances();
    provider = providerInstances[0];

    // 
    // get provider module instance from the table 
    //
    if (!_registrationTable->table.lookup(_moduleKey, _providerModule))
    {
        PEG_METHOD_EXIT();
	throw CIMException(CIM_ERR_FAILED, "can not find the provider module");
    }
    
    Array<CIMInstance> providerModuleInstances = _providerModule->getInstances();
    providerModule = providerModuleInstances[0];
    
  }
  catch(CIMException & exception)
  {
        PEG_METHOD_EXIT();
	return (false);
  }

    PEG_METHOD_EXIT();
    return (true);
}

// ATTN-YZ-P1-20020301: Implement this interface
Boolean ProviderRegistrationManager::lookupAssociationProvider(
    const String & nameSpace, 
    const String & className,
    String& providerName, 
    String& location, 
    Uint16& status)
{
    return(1);
}

Boolean ProviderRegistrationManager::getIndicationProviders(
    const String & nameSpace, 
    const String & className,
    const CIMPropertyList & requiredPropertyList,
    Array<CIMInstance> & provider,
    Array<CIMInstance> & providerModule)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
	"ProviderRegistrationManager::getIndicationProviders");

    Array <CIMInstance> _providerInstances;
    Array <CIMInstance> _providerModuleInstances;

    String providerName;
    String providerModuleName;

    Array<String> requiredProperties;

    provider.clear();
    providerModule.clear();

    ProviderRegistrationTable* providerCapability = 0;
    ProviderRegistrationTable* _provider = 0;
    ProviderRegistrationTable* _providerModule = 0;
    
  try 
  {
    //
    // create the key by using nameSpace, className, and providerType
    //
    String capabilityKey = _generateKey(nameSpace, className, IND_PROVIDER);

    // 
    // get provider capability instances from the table
    //
    if (! _registrationTable->table.lookup(capabilityKey, providerCapability))
    {
        PEG_METHOD_EXIT();
        throw CIMException(CIM_ERR_FAILED, "provider capability has not been registered yet");
    }

    Array<CIMInstance> instances = providerCapability->getInstances();

    for (Uint32 i=0; i < instances.size(); i++)
    {
	Array <String> _supportedProperties;
	CIMValue value;

	//
	// get supported properties
	//
	Uint32 pos = instances[i].findProperty(_PROPERTY_SUPPORTEDPROPERTIES);
    	if (pos == PEG_NOT_FOUND)
    	{
            PEG_METHOD_EXIT();
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	}

	value = instances[i].getProperty(pos).getValue();
	if (!value.isNull())
	{
	    value.get(_supportedProperties);
	}

	//
	// get provider name
	//
	Uint32 pos2 = instances[i].findProperty(_PROPERTY_PROVIDERNAME);
    	if (pos2 == PEG_NOT_FOUND)
    	{
            PEG_METHOD_EXIT();
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	}
	instances[i].getProperty(pos2).getValue().get(providerName);

	//
	// get provider module name
	//
	Uint32 pos3 = instances[i].findProperty(_PROPERTY_PROVIDERMODULENAME);
    	if (pos3 == PEG_NOT_FOUND)
    	{
            PEG_METHOD_EXIT();
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	}
	instances[i].getProperty(pos3).getValue().get(providerModuleName);

	//
	// create the key by using providerModuleName and providerName
	//
	String _providerKey = _generateKey(providerModuleName, providerName);	

	//
	// create the key by using providerModuleName and MODULE_KEY
	//
	String _moduleKey = _generateKey(providerModuleName, MODULE_KEY);

	if (value.isNull())
	{
	    //
	    // provider supportes all the properties
	    // get provider instance from the table 
	    //
	    if (!_registrationTable->table.lookup(_providerKey, _provider))
    	    {
                PEG_METHOD_EXIT();
        	throw CIMException(CIM_ERR_FAILED, "can not find the provider");
    	    }
	    
	    _providerInstances = _provider->getInstances();

	    //
	    // if the instance of the PG_Provider was not in the list, add the
	    // instance to the list
	    if (!Contains (provider, _providerInstances[0]))
	    {
		provider.append(_providerInstances[0]);
	    }

	    //
	    // get provider module instance from the table
	    //
	    if (!_registrationTable->table.lookup(_moduleKey, _providerModule))
    	    {
                PEG_METHOD_EXIT();
        	throw CIMException(CIM_ERR_FAILED, "Can not find the provider module");
    	    }
	    
	    _providerModuleInstances = _providerModule->getInstances();

	    // if the instance of the provider module was not in the list, add the
	    // instance to the list
	    if (!Contains (providerModule, _providerModuleInstances[0]))
	    {
		providerModule.append(_providerModuleInstances[0]);
	    }
	}
	else
	{
	    if (!requiredPropertyList.isNull())
	    {
		// 
		// there is a list for the required properties
		//
		Boolean match = true; 
		requiredProperties = requiredPropertyList.getPropertyNameArray();

		// 
		// Compare supported properties with required properties
		//
		for (Uint32 j=0; j < requiredProperties.size() && match; j++)
		{
		    if (!Contains (_supportedProperties, requiredProperties[j]))
		    {
			match = false;
		    }
		}

		//
		// Required properties are supported
		//
		if (match)
		{
		    //
	    	    // get provider instance from the table by using 
		    // _providerKey to be key
	    	    //
	    	    if (!_registrationTable->table.lookup(_providerKey, _provider))
    	    	    {
                        PEG_METHOD_EXIT();
        		throw CIMException(CIM_ERR_FAILED, "Can not find the provider");
    	    	     }
	    
	    	     _providerInstances = _provider->getInstances();

		     //    
	    	     // if the instance of the PG_Provider was not in the list, add 
		     // the instance to the list 
		     //
		     if (!Contains (provider, _providerInstances[0]))
            	     {
                	provider.append(_providerInstances[0]);
            	     }

		     //
                     // get provider module instance from the table
            	     //
            	     if (!_registrationTable->table.lookup(_moduleKey, _providerModule))
            	     {
                         PEG_METHOD_EXIT();
                         throw CIMException(CIM_ERR_FAILED, "can not find the provider module");
            	     }

            	     _providerModuleInstances = _providerModule->getInstances();

		     //
            	     // if the instance of the provider module was not in the list, 
		     // add the instance to the list
		     //
            	     if (!Contains (providerModule, _providerModuleInstances[0]))
            	     {
                     	providerModule.append(_providerModuleInstances[0]);
            	     }
		}
	    }
	}
	
    }
  }
  catch (CIMException & exception)
  {
        PEG_METHOD_EXIT();
	return (false);
  }

    PEG_METHOD_EXIT();
    return (true);
}

// get registered provider
CIMInstance ProviderRegistrationManager::getInstance(
    const CIMReference & instanceReference)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance instance;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
		     "ProviderRegistrationManager::getInstance");

    CIMReference localReference("", "",
	instanceReference.getClassName(),
	instanceReference.getKeyBindings());

    _repository->read_lock();

    try 
    {
	instance = _repository->getInstance(
 	    			PEGASUS_NAMESPACENAME_INTEROP, localReference);
    }

    catch (CIMException & exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
	PEG_METHOD_EXIT();
	_repository->read_unlock();
	throw (exception);
    }
    catch (Exception & exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
	PEG_METHOD_EXIT();
	_repository->read_unlock();
	throw (exception);
    }

    _repository->read_unlock();

    PEG_METHOD_EXIT();
    return (instance);
}

// get all registered providers
Array<CIMNamedInstance> ProviderRegistrationManager::enumerateInstances(
    const CIMReference & ref)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance instance;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
	"ProviderRegistrationManager::enumerateInstances");

    Array<CIMNamedInstance> enumInstances;

    _repository->read_lock();

    try
    {
	enumInstances = _repository->enumerateInstances(
	    PEGASUS_NAMESPACENAME_INTEROP,
	    ref.getClassName());
    }
    catch (CIMException & exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
	PEG_METHOD_EXIT();
	_repository->read_unlock();
	throw (exception);
    }
    catch (Exception & exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
	PEG_METHOD_EXIT();
	_repository->read_unlock();
	throw (exception);
    }

    _repository->read_unlock();

    PEG_METHOD_EXIT();
    return (enumInstances);

}

// get all registered provider names
Array<CIMReference> ProviderRegistrationManager::enumerateInstanceNames(
    const CIMReference & ref)
{

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance instance;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
		     "ProviderRegistrationManager::enumerateInstanceNames");

    Array<CIMReference> enumInstanceNames;

    _repository->read_lock();

    try
    {
        // get all instance names from repository
        enumInstanceNames = _repository->enumerateInstanceNames(
	    PEGASUS_NAMESPACENAME_INTEROP,
	    ref.getClassName());
    }

    catch (CIMException & exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
	PEG_METHOD_EXIT();
	_repository->read_unlock();
	throw (exception);
    }
    catch (Exception & exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
	PEG_METHOD_EXIT();
	_repository->read_unlock();
	throw (exception);
    }

    _repository->read_unlock();

    PEG_METHOD_EXIT();
    return (enumInstanceNames);
}

// register a provider
CIMReference ProviderRegistrationManager::createInstance(
    const CIMReference & ref,
    const CIMInstance & instance)
{

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMReference cimRef;

    try
    {
	cimRef = _createInstance(ref, instance, OP_CREATE);
	return (cimRef);
    }
    catch (CIMException & exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
	throw (exception);
    }
    catch (Exception & exception)
    {
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage ();
	throw (exception);
    }
}

// Unregister a provider
void ProviderRegistrationManager::deleteInstance(
    const CIMReference & instanceReference)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
	_deleteInstance(instanceReference, OP_DELETE);
    }

    catch (CIMException & exception)
    {
	errorCode = exception.getCode ();
 	errorDescription = exception.getMessage ();
	throw (exception);
    }
    catch (Exception & exception)
    {
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage ();
	throw (exception);
    }

}

// modify a registered provider
void ProviderRegistrationManager::modifyInstance(
    const CIMReference & ref,
    const CIMInstance & cimInstance,
    const Uint32 flags,
    const Array<String> & propertyList)
{

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
                     "ProviderRegistrationManager::modifyInstance");

    CIMReference newInstanceRef("", "", ref.getClassName(), ref.getKeyBindings());

    CIMInstance givenInstance = cimInstance;
	
    _repository->read_lock();

    try
    {
	//
	// get the original instance 
	//
	CIMInstance origInstance = _repository->getInstance(
	    PEGASUS_NAMESPACENAME_INTEROP, newInstanceRef);

        _repository->read_unlock();

	//
	// creates the instance which replaces the original
	//
	CIMInstance instance = origInstance;

	//
	// loop through the propertyList replacing each property in the original
	// with the properties specified in the given instance
	//
	for (Uint32 i=0; i<propertyList.size(); i++)
	{
	    Uint32 origPos = instance.findProperty(propertyList[i]);
	    if (origPos != PEG_NOT_FOUND)
	    {
		//
		// Property set in the original
		//
		CIMProperty OrigProperty = instance.getProperty(origPos);

		//
		// get the given property value
		//
		Uint32 givenPos = givenInstance.findProperty(propertyList[i]);
		
		if (givenPos != PEG_NOT_FOUND)
		{
		    //
		    // Property set in both original and given
		    //
		    CIMProperty givenProperty;
		    givenProperty = givenInstance.getProperty(givenPos);

		    //
		    // copy over the property from the given to the original
		    //
		    if (flags & OperationFlag::INCLUDE_QUALIFIERS)
		    {
			//
			// Total property replacement
			//
			instance.removeProperty(origPos);
			instance.addProperty(givenProperty);
		    }
		    else
		    {
			//
			// Replace only the property value
			//
			OrigProperty.setValue(givenProperty.getValue());
			instance.removeProperty(origPos);
			instance.addProperty(OrigProperty);
		    }
		}
		else
		{
		    //
		    // Property set in original and not in given
		    // just remove the property (set to null)
		    //
		    instance.removeProperty(origPos);
		}
	    }
	    else
	    {
		//
		// Property not set in the original
		//

		//
		// Get the given property value
		//
		Uint32 givenPos = givenInstance.findProperty(propertyList[i]);

		if (givenPos != PEG_NOT_FOUND)
		{
		    //
		    // property set in given and not in original
		    //
		    CIMProperty givenProperty;
		    givenProperty = givenInstance.getProperty(givenPos);
		
		    //
		    // copy over the property from the given to the original
		    //
		    if (flags & OperationFlag::INCLUDE_QUALIFIERS)
		    {
			// Total property copy
			instance.addProperty(givenProperty);
		    }
		    else
		    {
			// copy only the property value
			CIMProperty newProperty(givenProperty.getName(),
						givenProperty.getValue(),
						givenProperty.getArraySize(),
						givenProperty.getReferenceClassName(),
						givenProperty.getClassOrigin(),
						givenProperty.getPropagated());
			instance.addProperty(newProperty);
		    }
		}
	    }
	}
	//
	// delete old instance
	// 
	_deleteInstance(ref, OP_MODIFY);

	//
	// create the new instance
	//
	CIMReference cimRef = _createInstance(ref, instance, OP_MODIFY);

	//
	// if only modify SupportedMethods, do not send notification to
	// subscription service
	//
	if (propertyList.size() == 1 &&
	    String::equalNoCase(propertyList[0], _PROPERTY_SUPPORTEDMETHODS))
	{
	    return;
	}

	//
	// get provider types
	// if the provider is indication provider, send notification
	// to subscription service
	Array<Uint16> providerTypes;
	instance.getProperty(instance.findProperty(
	    _PROPERTY_PROVIDERTYPE)).getValue().get(providerTypes);

	for (Uint32 k=0; k < providerTypes.size(); k++)
	{
	    if (providerTypes[k] == _INDICATION_PROVIDER)
	    {
		_sendModifyNotifyMessage(instance, origInstance);	
	    }
	}
    }
    catch (CIMException & exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
	PEG_METHOD_EXIT();
	_repository->read_unlock();
        throw (exception);
    }
    catch (Exception & exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
	PEG_METHOD_EXIT();
	_repository->read_unlock();
        throw (exception);
    }

	PEG_METHOD_EXIT();
}

Array<Uint16> ProviderRegistrationManager::getProviderModuleStatus(
    const String & providerModuleName)
{
    Array<Uint16> _providerModuleStatus;
    Array<CIMInstance> instances;

    //
    // create the key by using providerModuleName and MODULE_KEY
    //
    String _moduleKey = _generateKey(providerModuleName, MODULE_KEY);

    //
    // Find the entry whose key's value is same as _moduleKey
    // get provider module status from the value
    //
    ProviderRegistrationTable* _providerModule = 0;

    if (!_registrationTable->table.lookup(_moduleKey, _providerModule))
    {
    	throw (CIMException(CIM_ERR_FAILED, "Can not find the provider module."));
    }
    
    instances = _providerModule->getInstances();

    instances[0].getProperty(instances[0].findProperty
        (_PROPERTY_OPERATIONALSTATUS)).getValue().get(_providerModuleStatus);

    return (_providerModuleStatus);
}

Boolean ProviderRegistrationManager::setProviderModuleStatus(
    const String & providerModuleName,
    Array<Uint16> status)
{
    CIMInstance instance;
    CIMReference reference;
    Array<CIMNamedInstance> cimNamedInstances;
    String _providerModuleName;

    //
    // create the key by using providerModuleName and MODULE_KEY
    //
    String _moduleKey = _generateKey(providerModuleName, MODULE_KEY);

    _repository->write_lock();

    //
    // find the instance from repository
    //
    try
    {
	cimNamedInstances = _repository->enumerateInstances(
		PEGASUS_NAMESPACENAME_INTEROP,
		PEGASUS_CLASSNAME_PROVIDERMODULE);

	for(Uint32 i = 0, n=cimNamedInstances.size(); i < n; i++)
	{
	    instance = cimNamedInstances[i].getInstance();

	    //
	    // get provider module name
  	    // 
	    instance.getProperty(instance.findProperty
	    (_PROPERTY_PROVIDERMODULE_NAME)).getValue().get(_providerModuleName);
	    if (String::equalNoCase(providerModuleName, _providerModuleName))
	    {
		//
		// get CIMReference
		//
		reference = cimNamedInstances[i].getInstanceName();	

		CIMReference newInstancereference("", "",
		    reference.getClassName(),
		    reference.getKeyBindings());

		//
		// update repository
		//
		_repository->setProperty(PEGASUS_NAMESPACENAME_INTEROP, 
					 newInstancereference,
		    			 _PROPERTY_OPERATIONALSTATUS,
					 status);

		//
		// update the table
		//
		CIMInstance _instance = _repository->getInstance(
					  PEGASUS_NAMESPACENAME_INTEROP,
					  newInstancereference);

		//
		// remove old entry from table
		//
		_registrationTable->table.remove(_moduleKey);

		//
		// add the updated instance to the table
		//
		Array<CIMInstance> instances;
		instances.append(_instance);
		_addInstancesToTable(_moduleKey, instances);
		
		_repository->write_unlock();
		return (true);
	    }
	}
    }
    catch (CIMException& exception)
    {
	_repository->write_unlock();
	return (false);
    }
    catch (Exception& exception)
    {
	_repository->write_unlock();
	return (false);
    }
}

void ProviderRegistrationManager::_initialRegistrationTable()
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance instance;
    String providerModuleName;
    String providerName;
    String className;
    String capabilityKey;
    Array<String> namespaces;
    Array<Uint16> providerType;
    Array<String> supportedMethods;
    Array<CIMNamedInstance> cimNamedInstances;
  
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
		     "ProviderRegistrationManager::_initialRegistrationTable()");

    _repository->read_lock();

    try
    {
        //
        // get all instances of providerModule class
        //

	Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
		      "nameSpace = %s; className = %s", PEGASUS_NAMESPACENAME_INTEROP,
		     	PEGASUS_CLASSNAME_PROVIDERMODULE);	
        cimNamedInstances = _repository->enumerateInstances(
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_CLASSNAME_PROVIDERMODULE);

	Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
		        "PG_ProviderModule class has = %d instances",
		     	cimNamedInstances.size());	

    	for(Uint32 i = 0, n=cimNamedInstances.size(); i < n; i++)
    	{
	    Array<CIMInstance> instances;
            instance = cimNamedInstances[i].getInstance();

	    // get provider module name
            instance.getProperty(instance.findProperty
            (_PROPERTY_PROVIDERMODULE_NAME)).getValue().get(providerModuleName);
	
	    //
            // create the key by using providerModuleName and MODULE_KEY
            //
            String _moduleKey = _generateKey(providerModuleName, MODULE_KEY);

	    //
            // add the instance to the hash table by using _moduleKey
            //
            instances.append(instance);
            _addInstancesToTable(_moduleKey, instances);
    	}

        //
        // get all instances of provider class
        //
        cimNamedInstances = _repository->enumerateInstances(
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_CLASSNAME_PROVIDER);
	Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
		        "PG_Provider class has = %d instances",
		     	cimNamedInstances.size());	

    	for(Uint32 i = 0, n=cimNamedInstances.size(); i < n; i++)
    	{
	    Array<CIMInstance> instances;

            instance = cimNamedInstances[i].getInstance();

	    // get provider module name
            instance.getProperty(instance.findProperty
            (_PROPERTY_PROVIDERMODULENAME)).getValue().get(providerModuleName);
	
	    // get provider name
            instance.getProperty(instance.findProperty
            (_PROPERTY_PROVIDER_NAME)).getValue().get(providerName);

	    //
            // create the key by using providerModuleName and providerName
            //
            String _providerKey = _generateKey(providerModuleName, providerName);

	    //
            // add the instance to the hash table by using _providerKey
            //
            instances.append(instance);
            _addInstancesToTable(_providerKey, instances);
    	}

        //
        // get all instances of providerCapabilities class
        //
        cimNamedInstances = _repository->enumerateInstances(
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_CLASSNAME_PROVIDERCAPABILITIES);

    	for(Uint32 i = 0, n=cimNamedInstances.size(); i < n; i++)
    	{
            instance = cimNamedInstances[i].getInstance();

	    // get class name
            instance.getProperty(instance.findProperty
            (_PROPERTY_CLASSNAME)).getValue().get(className);

	    // get Namespace
            instance.getProperty(instance.findProperty
            (_PROPERTY_NAMESPACES)).getValue().get(namespaces);

	    // get provider type
            instance.getProperty(instance.findProperty
            (_PROPERTY_PROVIDERTYPE)).getValue().get(providerType);

	    for (Uint32 j=0; j < providerType.size(); j++)
	    {
		switch (providerType[j])
		{
		    case _INSTANCE_PROVIDER:
		    {
			for (Uint32 k=0; k < namespaces.size(); k++)
			{
	    		    Array<CIMInstance> instances;

			    //
			    // create a key by using namespace, className
			    // and providerType. Use this key to store the
			    // instance to the hash table
			    //
			    capabilityKey = _generateKey(namespaces[k], 
						className, INS_PROVIDER); 
			    instances.append(instance);
			    _addInstancesToTable(capabilityKey, instances);
			}
			break;
		    }

		    // ATTN-YZ-P1-20020301: Implement this provider
		    case _ASSOCIATION_PROVIDER:
		    {
			break;
		    }

		    case _INDICATION_PROVIDER:
		    {
			for (Uint32 k=0; k < namespaces.size(); k++)
			{
	    		    Array<CIMInstance> instances;

			    ProviderRegistrationTable* capabilities;
			
			    //
			    // create key by using namespace, className and
			    // providerType, store the instance to the table
			    //
			    capabilityKey = _generateKey(namespaces[k],
					className, IND_PROVIDER);

			    if (_registrationTable->table.lookup(capabilityKey, capabilities))
			    {
				// the class is already in the table
				instances = capabilities->getInstances();
				instances.append(instance);

				//
				// remove the entry from the table
				//
				_registrationTable->table.remove(capabilityKey);
			    }
			    else
			    {
				instances.append(instance);
			    }
			    
			    // Add the entry to the table
			    _addInstancesToTable(capabilityKey, instances);
			}	

			break;
		    }

		    case _METHOD_PROVIDER:
		    {
			//
			// get supported methods
			//
			Uint32 pos;
			pos = instance.findProperty(_PROPERTY_SUPPORTEDMETHODS);
			if (pos != PEG_NOT_FOUND)
			{
			    instance.getProperty(pos).getValue().get(supportedMethods);
			}
			else
			{
                            PEG_METHOD_EXIT();
			    throw CIMException(CIM_ERR_INVALID_PARAMETER);
			}

			Uint32 methodsCount = supportedMethods.size();
			
			for (Uint32 k=0; k < namespaces.size(); k++)
			{
			    // 
			    // create a key by using namespace, className, 
			    // method name and providerType
			    //

			    if (supportedMethods == EmptyStringArray())
			    {
	    		        Array<CIMInstance> instances;

			    	// The provider supports all the methods
			   	capabilityKey = _generateKey(namespaces[k],
					className, "{}", MET_PROVIDER);

				instances.append(instance);
				_addInstancesToTable(capabilityKey, instances);	 
			    }
			    else
			    {
				for (Uint32 n=0; n < methodsCount; n++)
				{
	    		            Array<CIMInstance> instances;

				    capabilityKey = _generateKey(namespaces[k],
					className, supportedMethods[n], MET_PROVIDER);
				    instances.append(instance);
				    _addInstancesToTable(capabilityKey, instances);
				}
			    }
			}

			break;
		    }

		    default:
			//
			//  Error condition: provider type not supported
			//
                        PEG_METHOD_EXIT();
			throw CIMException (CIM_ERR_NOT_SUPPORTED);
			break;
		}
	    }
    	}

    }

    catch (CIMException& exception)
    {
        errorCode = exception.getCode();
        errorDescription = exception.getMessage();
    }
    catch (Exception& exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage();
    }

    _repository->read_unlock();

    PEG_METHOD_EXIT();
}

// register a provider
CIMReference ProviderRegistrationManager::_createInstance(
    const CIMReference & ref,
    const CIMInstance & instance,
    Operation flag)
{

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMReference cimRef;

    String _providerModule;
    String _providerName;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
		     "ProviderRegistrationManager::_createInstance");

    String className = ref.getClassName();

    _repository->write_lock();

    try
    {
	// 
	// register PG_ProviderModule class
	//
	if (String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERMODULE))
	{
    	    Array<CIMInstance> instances;

	    cimRef = _repository->createInstance(PEGASUS_NAMESPACENAME_INTEROP, instance); 

	    //
	    // get provider module name
	    //
	    instance.getProperty(instance.findProperty
		(_PROPERTY_PROVIDERMODULE_NAME)).getValue().get(_providerModule);
	    //
	    // Use provider module name to be a key, add the instance to
	    // the hash table 
	    //
	    instances.append(instance);
	    String _moduleKey = _generateKey(_providerModule, MODULE_KEY);
            _addInstancesToTable(_moduleKey, instances);

    	    _repository->write_unlock();

	    PEG_METHOD_EXIT();
	    return (cimRef);
	}	

	// 
	// register PG_Provider class
	//
	if (String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDER))
	{
    	    Array<CIMInstance> instances;

	    // get provider module name
	    instance.getProperty(instance.findProperty
		(_PROPERTY_PROVIDERMODULENAME)).getValue().get(_providerModule);
	    // get provider name
	    instance.getProperty(instance.findProperty
		(_PROPERTY_PROVIDER_NAME)).getValue().get(_providerName);

	    //
            // create the key by using _providerModule and _providerName
            //
            String _providerKey = _generateKey(_providerModule, _providerName);

	    //
            // create the key by using _providerModule and MODULE_KEY
            //
            String _moduleKey = _generateKey(_providerModule, MODULE_KEY);

	    //
	    // check if the PG_ProviderModule class was registered
	    //
	    if (_registrationTable->table.contains(_moduleKey))
	    {   
		//
		// the provider module class was registered
		//
	        cimRef = _repository->createInstance(PEGASUS_NAMESPACENAME_INTEROP, instance); 
	        //
	        // add the instance to the hash table 
	        //
		instances.append(instance);

	        _addInstancesToTable(_providerKey, instances);

		 _repository->write_unlock();

		PEG_METHOD_EXIT();
	        return (cimRef);
	    }
	    else
	    {
		//
		// the provider module class is not registered
		//
                PEG_METHOD_EXIT();
		throw CIMException(CIM_ERR_FAILED, "PG_ProviderModule class"
			" needs to be registered before register the PG_Provider class");
	    }
	}	

	//
	// register CIM_Capabilities class
	//
	if (String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES))
	{
	    Array<Uint16> _providerType;
	    Array<String> _namespaces;
	    Array<String> _supportedMethods;
	    Array<String> _supportedProperties;
	    String _className;
	    String _capabilityKey;

	    //
	    // get provider module name
	    //
	    instance.getProperty(instance.findProperty
		(_PROPERTY_PROVIDERMODULENAME)).getValue().get(_providerModule);
	    //
	    // get provider name
	    //
	    instance.getProperty(instance.findProperty
		(_PROPERTY_PROVIDERNAME)).getValue().get(_providerName);

	    //
            // create the key by using _providerModule and _providerName
            //
            String _providerKey = _generateKey(_providerModule, _providerName);

	    //
            // create the key by using _providerModule and MODULE_KEY
            //
            String _moduleKey = _generateKey(_providerModule, MODULE_KEY);

	    //
	    // check if the PG_Provider class was registered
	    //
	    if (_registrationTable->table.contains(_providerKey))
	    {
		//
		// the PG_Provider class was registered
		// get provider types
		//
	    	instance.getProperty(instance.findProperty
		    (_PROPERTY_PROVIDERTYPE)).getValue().get(_providerType);
		
		//
		// get namespaces
		//
	    	instance.getProperty(instance.findProperty
		    (_PROPERTY_NAMESPACES)).getValue().get(_namespaces);
		
		//
		// get classname
		//
	    	instance.getProperty(instance.findProperty
		    (_PROPERTY_CLASSNAME)).getValue().get(_className);
		
		for (Uint32 i=0; i < _providerType.size(); i++)
		{
		    switch (_providerType[i])
		    {
			case _INSTANCE_PROVIDER:
			{
			    for (Uint32 j=0; j < _namespaces.size(); j++)
			    {
    	    			Array<CIMInstance> instances;

				//
				// create a key by using namespace, className
				// and providerType
				//
			        _capabilityKey = _generateKey(_namespaces[j],
				     _className, INS_PROVIDER);
				if (_registrationTable->table.contains(_capabilityKey))
				{
				    // the instance was already registered
                                    PEG_METHOD_EXIT();
				    throw CIMException(CIM_ERR_ALREADY_EXISTS);
				}
				else
				{
				    // add the instance to the table
				    instances.append(instance);
				    _addInstancesToTable(_capabilityKey, instances);
				}
			    }

			    break;
			}

			case _ASSOCIATION_PROVIDER:
			{
			    // ATTN-YZ-P1-20020301: Need implement 
			    break;
			}

			case _INDICATION_PROVIDER:
			{
			    for (Uint32 j=0; j < _namespaces.size(); j++)
			    {
    	    			Array<CIMInstance> instances;

				ProviderRegistrationTable* providerCapabilities;
				//
				// create a key by using namespace, className
				// and providerType, store the instance to 
				// the table
				//
			        _capabilityKey = _generateKey(_namespaces[j],
				     _className, IND_PROVIDER);

				if (_registrationTable->table.lookup(_capabilityKey, providerCapabilities))
				{
				    //
				    // the class is already in the table
				    //
				    instances = providerCapabilities->getInstances();
				    instances.append(instance);
				    
				    //
	 			    // remove the entry from the table
				    //
				    _registrationTable->table.remove(_capabilityKey);
				}
				else
				{
				    instances.append(instance);
				}

				// update the entry
				_addInstancesToTable(_capabilityKey, instances);
			    }
			    
			    //
			    // Create CIMNotifyProviderRegistrationRequestMessage
			    // send the message to the subscription service
			    // if this function is called by createInstance
			    //
			    if (flag == OP_CREATE)
			    {
			        CIMInstance _providerInstance;
			    	CIMInstance _moduleInstance;

			      	Array<String> _oldNamespaces = EmptyStringArray();
			    	// 
			    	// get provider instance and module instance from the
			    	// registration table
			    	//
			    	_getInstances(_providerName, _providerModule, 
					  _providerInstance, _moduleInstance);

			    	CIMPropertyList _oldPropertyNames(EmptyStringArray());
			    	CIMPropertyList _newPropertyNames;

			    	//
			    	// get new property list from supported properties in the
			    	// providerCapability instance
			    	//
			    	_getPropertyNames(instance, _newPropertyNames);

				//
				// get indication server queueId
				//
				MessageQueueService * _service = _getIndicationService();

			    	CIMNotifyProviderRegistrationRequestMessage * notify_req =
			        new CIMNotifyProviderRegistrationRequestMessage(
				    XmlWriter::getNextMessageId (),
				    CIMNotifyProviderRegistrationRequestMessage::Operation(OP_CREATE),
				    _providerInstance,
				    _moduleInstance,	
				    _className,
				    _namespaces,
				    _oldNamespaces,
				    _newPropertyNames,
				    _oldPropertyNames,
				    QueueIdStack(_service->getQueueId()));

				_sendMessageToSubscription(notify_req);
			    }

			    break;
			}

			case _METHOD_PROVIDER:
			{
			    //
			    // get supportedMethods
			    //
			    Uint32 pos = instance.findProperty(_PROPERTY_SUPPORTEDMETHODS);
			    CIMValue value;
			    Uint32 methodsCount;

			    if (pos != PEG_NOT_FOUND)
			    {
				value = instance.getProperty(pos).getValue();
				if (!value.isNull())
				{
				    value.get(_supportedMethods);	
			    	    methodsCount = _supportedMethods.size();
				}
			    }
			    else
			    {
		                PEG_METHOD_EXIT();
				throw CIMException(CIM_ERR_INVALID_PARAMETER);
			    }

			    for (Uint32 j=0; j < _namespaces.size(); j++)
			    {
				//
				// create a key by using namespace, className,
				// method name and providerType, store the 
				// instance to the table 
				//

				if ( value.isNull() )
				{
				    //
				    // null value means all methods
				    //
    	    			    Array<CIMInstance> instances;

				    // Provider supports all methods
			            _capabilityKey = _generateKey(_namespaces[j],
				    _className, "{}", MET_PROVIDER);

				    if (_registrationTable->table.contains(_capabilityKey))
			            {
				        // the instance was already registered
                                        PEG_METHOD_EXIT();
				        throw CIMException(CIM_ERR_ALREADY_EXISTS);
			            }

				    // add the instance to the table
				    instances.append(instance);
				    _addInstancesToTable(_capabilityKey, instances);
				}
				else
				{
				    //
				    // there is a list for supported methods
				    //
				    for (Uint32 k = 0; k < methodsCount; k++)
				    {
    	    			        Array<CIMInstance> instances;

			                _capabilityKey = _generateKey(_namespaces[j],
				            _className, _supportedMethods[k], MET_PROVIDER);
					if (_registrationTable->table.contains(_capabilityKey))
					{
					    // the instance already registered
                                            PEG_METHOD_EXIT();
                                    	    throw CIMException(CIM_ERR_ALREADY_EXISTS);

					}
					else
					{
				    	    instances.append(instance);
				    	    _addInstancesToTable(_capabilityKey, instances);
					}
				    }
				}
			    }

			    break;
			}

			default:
            		    //
            		    //  Error condition: provider type not supported
            		    //
		            PEG_METHOD_EXIT();
            		    throw CIMException (CIM_ERR_NOT_SUPPORTED);
            		    break;
		    }
		}

	        cimRef = _repository->createInstance(PEGASUS_NAMESPACENAME_INTEROP, instance); 
		 _repository->write_unlock();

		PEG_METHOD_EXIT();
    		return (cimRef);
	    }
	    else
	    {
                // the provider class was not registered
//		PEG_METHOD_EXIT();
                throw CIMException(CIM_ERR_FAILED, "PG_Provider class needs "
                   "to be registered before register the Provider capabilities class");
	    }
	}

    }
    catch (CIMException & exception)
    {
	errorCode = exception.getCode ();
 	errorDescription = exception.getMessage ();
	_repository->write_unlock();
	PEG_METHOD_EXIT();
	throw (exception);
    }
    catch (Exception & exception)
    {
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage ();
	_repository->write_unlock();
	PEG_METHOD_EXIT();
	throw (exception);
    }

//    _repository->write_unlock();

    // Should never get here
    PEGASUS_ASSERT(0);
}

// Unregister a provider
void ProviderRegistrationManager::_deleteInstance(
    const CIMReference & instanceReference,
    Operation flag)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMReference cimRef;

    String className = instanceReference.getClassName();
    String providerName;

    CIMReference newInstancereference("", "",
	instanceReference.getClassName(),
	instanceReference.getKeyBindings());

    _repository->write_lock();

    try
    {
        CIMInstance instance = _repository->getInstance(
	    PEGASUS_NAMESPACENAME_INTEROP, newInstancereference);
	    
	//
	// unregister PG_ProviderCapability class
	//
        if(String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES))
        {
	    String deletedCapabilityID;
	    String deletedModule;
	    String deletedProvider;
	    Array<CIMInstance> instances;
	    Array<Uint16> providerType;

	    //
	    // delete the instance from repository
	    //
	    _repository->deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, newInstancereference);

	    //
	    // get the key ProviderModuleName
	    //
	    instance.getProperty(instance.findProperty(
		_PROPERTY_PROVIDERMODULENAME)).getValue().get(deletedModule);

	    //
	    // get the key ProviderName
	    //
	    instance.getProperty(instance.findProperty(
		_PROPERTY_PROVIDERNAME)).getValue().get(deletedProvider);

	    //
	    // get the key capabilityID
	    //
	    instance.getProperty(instance.findProperty(
		_PROPERTY_CAPABILITIESID)).getValue().get(deletedCapabilityID);
	    //
	    // remove all entries which have same ProviderModuleName, 
	    // ProviderName, and capabilityID from the 
	    // table; if the entry only has one instance, remove the entry;
   	    // otherwise, remove the instance. 
	    //
	    Table::Iterator k=_registrationTable->table.start();

	    for (Table::Iterator i=_registrationTable->table.start(); i; i=k)
	    {
		k++;
		instances = i.value()->getInstances();

		for (Sint32 j = 0; j < instances.size(); j++)
		{
	    	    String capabilityID;
		    String module;
		    String provider;
		    
		    Uint32 pos = instances[j].findProperty(_PROPERTY_PROVIDERMODULENAME);
		    Uint32 pos2 = instances[j].findProperty(_PROPERTY_PROVIDERNAME);
		    Uint32 pos3 = instances[j].findProperty(_PROPERTY_CAPABILITIESID);

		    if (pos != PEG_NOT_FOUND && pos2 != PEG_NOT_FOUND &&
			pos3 != PEG_NOT_FOUND)
		    {
		      // get provider module name
		      instances[j].getProperty(pos).getValue().get(module);

		      // get provider name
		      instances[j].getProperty(pos2).getValue().get(provider);

		      // get capabilityID
		      instances[j].getProperty(pos3).getValue().get(capabilityID);

		      if (String::equalNoCase(deletedModule, module) &&
		          String::equalNoCase(deletedProvider, provider) &&
		          String::equalNoCase(deletedCapabilityID, capabilityID))
		      {
		    	if (instances.size()== 1)
		    	{
		            _registrationTable->table.remove(i.key());
		    	}
		    	else
		    	{
		            instances.remove(j);
			    j = j - 1;
		    	}
		      }
		    }
		}

	    }

	    //
	    // get provider types
	    // if the provider is indication provider, send notification
	    // to subscription service
	    //
	    instance.getProperty(instance.findProperty(
		_PROPERTY_PROVIDERTYPE)).getValue().get(providerType);

	    for (Uint32 i=0; i < providerType.size(); i++)
	    {
		if (providerType[i] == _INDICATION_PROVIDER &&
		    flag == OP_DELETE)
		{
		    _sendDeleteNotifyMessage(instance);
		}
	    }
        }

	//
	// Unregister PG_Provider class
	// Note: Deleteting an instance of PG_Provider will cause the 
	// associated instances of PG_ProviderCapability to be deleted 
	//
        if(String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDER))
        {
	    CIMInstance capInstance;
	    CIMReference capReference;
	    String _providerName;
	    String _moduleName;
  	    Array<Uint16> providerType;

	    String deletedModuleName;
	    String deletedProviderName;

	    //
	    // get the provider module name
	    //
	    instance.getProperty(instance.findProperty(
		_PROPERTY_PROVIDERMODULENAME)).getValue().get(deletedModuleName);

	    //
	    // get the provider name
	    //
	    instance.getProperty(instance.findProperty(
		_PROPERTY_PROVIDER_NAME)).getValue().get(deletedProviderName);

	    //
	    // create the key by using deletedModuleName and deletedProviderName
            //
            String _deletedProviderKey = _generateKey(deletedModuleName,deletedProviderName);

	    //
	    // delete instance of PG_Provider from repository
	    //
	    _repository->deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, newInstancereference);

	    //
	    // delete associated instances of PG_ProviderCapability
	    //
	    Array<CIMNamedInstance> enumCapInstances;
	
	    enumCapInstances = _repository->enumerateInstances(
			PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES); 	

	    for (Uint32 i = 0, n = enumCapInstances.size(); i < n; i++)
	    {
		capInstance = enumCapInstances[i].getInstance();

		// 
		// get provider module name in the instance of provider capability
		//
		capInstance.getProperty(capInstance.findProperty(
		    _PROPERTY_PROVIDERMODULENAME)).getValue().get(_moduleName);

		// 
		// get provider name in the instance of provider capability
		//
		capInstance.getProperty(capInstance.findProperty(
		    _PROPERTY_PROVIDERNAME)).getValue().get(_providerName);

		if (String::equalNoCase(deletedModuleName, _moduleName) &&
		    String::equalNoCase(deletedProviderName, _providerName))
		{
		    //
		    // if provider name of instance of provider capability is
		    // same as deleted provider name and module name of instance
		    // of provider capability is same as deleted module name, delete 
		    // the instance of provider capability from repository
		    //
		    capReference = enumCapInstances[i].getInstanceName(); 

    		    CIMReference newCapReference("", "",
			capReference.getClassName(),
			capReference.getKeyBindings());

	    	    _repository->deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, newCapReference);

		    //
                    // get provider types
                    // if the provider is indication provider, send notification
                    // to subscription service
                    //
		    capInstance.getProperty(capInstance.findProperty(
			_PROPERTY_PROVIDERTYPE)).getValue().get(providerType);

		    for (Uint32 k=0; k < providerType.size(); k++)
		    {
			if (providerType[k] == _INDICATION_PROVIDER &&
			    flag == OP_DELETE)
			{
			    _sendDeleteNotifyMessage(capInstance);
			}
		    }
 
		}

	    }

	    //
	    // remove all entries which have same provider name and module name
	    // from the table; if the entry only has one instance, remove the entry;
   	    // otherwise, remove the instance. 
	    //
	    Table::Iterator k=_registrationTable->table.start();

	    for (Table::Iterator i=_registrationTable->table.start(); i; i=k)
	    {
		k++;
		Array<CIMInstance> instances;

		// 
		// remove all entries which their key's value is same as _deletedProviderKey 
		// from the table
		//
		if (String::equalNoCase(_deletedProviderKey, i.key()))
		{
		    _registrationTable->table.remove(i.key());
		}
		else
		{
		    instances = i.value()->getInstances();

		    for (Sint32 j = 0; j < instances.size(); j++)
		    {
	    	    	String _providerName;
	    	    	String _moduleName;
		    
			Uint32 pos = instances[j].findProperty(_PROPERTY_PROVIDERMODULENAME);
			Uint32 pos2 = instances[j].findProperty(_PROPERTY_PROVIDERNAME);

			if (pos != PEG_NOT_FOUND && pos2 != PEG_NOT_FOUND)
			{
			  instances[j].getProperty(pos).getValue().get(_moduleName);
			  instances[j].getProperty(pos2).getValue().get(_providerName);

			  if (String::equalNoCase(deletedModuleName, _moduleName) &&
			      String::equalNoCase(deletedProviderName, _providerName))
		    	  {
			    if (instances.size() == 1)
			    {
			        _registrationTable->table.remove(i.key());
			    }
			    else
			    {
			        instances.remove(j);
				j = j - 1;
			    }
		          }
			}
		    }
		}
	    }
	
	}

	//
	// unregister PG_ProviderModule class
	// Note: Deleteting an instance of PG_ProviderModule will cause the 
	// associated instances of PG_Provider, instances of PG_ProviderCapability 
	// to be deleted 
	//
        if(String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERMODULE))
        {
	    String deletedProviderModuleName;

	    //
	    // get the key provider module name
	    //
	    instance.getProperty(instance.findProperty(
		 _PROPERTY_PROVIDERMODULE_NAME)).getValue().get(deletedProviderModuleName);
	    //
            // create the key by using deletedProviderModuleName and MODULE_KEY
            //
            String _deletedModuleKey = _generateKey(deletedProviderModuleName, MODULE_KEY);


	    //
	    // delete instance of PG_ProviderModule from repository
	    //
	    _repository->deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, newInstancereference);

	    //
	    // delete associated instances of PG_Provider
	    //
	    Array<CIMNamedInstance> enumProviderInstances;
	
	    enumProviderInstances = _repository->enumerateInstances(
				PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER); 	

	    for (Uint32 i = 0, n = enumProviderInstances.size(); i < n; i++)
	    {
		CIMInstance providerInstance;
		CIMReference providerReference;
		String _providerModuleName;

		providerInstance = enumProviderInstances[i].getInstance();

		// 
		// get provider module name in the instance of PG_Provider 
		//
		providerInstance.getProperty(providerInstance.findProperty(
		    _PROPERTY_PROVIDERMODULENAME)).getValue().get(_providerModuleName);

		if (String::equalNoCase(deletedProviderModuleName, _providerModuleName))
		{
		    //
		    // if provider module name of instance of PG_Provider is
		    // same as deleted provider module name, delete the instance of
		    // PG_Provider from repository
		    //
		    providerReference = enumProviderInstances[i].getInstanceName(); 

    		    CIMReference newProviderReference("", "",
			providerReference.getClassName(),
			providerReference.getKeyBindings());

	    	    _repository->deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, newProviderReference);
		    
		}

	    }

	    //
	    // delete associated instances of PG_ProviderCapability
	    //
	    Array<CIMNamedInstance> enumCapInstances;
	
	    enumCapInstances = _repository->enumerateInstances(
				PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES); 	

	    for (Uint32 i = 0, n = enumCapInstances.size(); i < n; i++)
	    {
		CIMInstance capInstance;
		CIMReference capReference;
		String _providerModuleName;
		Array<Uint16> providerType;

		capInstance = enumCapInstances[i].getInstance();

		// 
		// get provider module name in the instance of provider capability
		//
		capInstance.getProperty(capInstance.findProperty(
		    _PROPERTY_PROVIDERMODULENAME)).getValue().get(_providerModuleName);

		if (String::equalNoCase(deletedProviderModuleName, _providerModuleName))
		{
		    //
		    // if provider module name of instance of provider capability is
		    // same as deleted provider module name, delete the instance of
		    // provider capability from repository
		    //
		    capReference = enumCapInstances[i].getInstanceName(); 

    		    CIMReference newCapReference("", "",
			capReference.getClassName(),
			capReference.getKeyBindings());

	    	    _repository->deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, newCapReference);
		    
		    //
                    // get provider types
                    // if the provider is indication provider, send notification
                    // to subscription service
                    //
		    capInstance.getProperty(capInstance.findProperty(
                        _PROPERTY_PROVIDERTYPE)).getValue().get(providerType);

                    for (Uint32 k=0; k < providerType.size(); k++)
                    {
                        if (providerType[k] == _INDICATION_PROVIDER &&
			    flag == OP_DELETE)
                        {
                            _sendDeleteNotifyMessage(capInstance);
                        }
                    }
		    
		}

	    }

	    //
	    // remove all entries which have same provider module name from the 
	    // table; if the entry only has one instance, remove the entry;
   	    // otherwise, remove the instance. 
	    //
	    Table::Iterator k=_registrationTable->table.start();
	    for (Table::Iterator i=_registrationTable->table.start(); i; i=k)
	    {
		k++;
		Array<CIMInstance> instances;

		// 
		// remove all entries which key's value is same as _deletedModuleKey 
		// from the table
		//
		if (String::equalNoCase(_deletedModuleKey, i.key()))
		{
		    _registrationTable->table.remove(i.key());
		}
		else
		{
		    //
		    // if the entry only has one instance and provider module name of 
		    // the instance is same as deleted provider module name, remove the
		    // entry; 
		    // if the entry has more than one instance and provider module name
		    // of the instance is same as deleted provider module name, remove
		    // the instance;
		    //
		    instances = i.value()->getInstances();

		    for (Uint32 j = 0; j < instances.size(); j++)
		    {
	    	        String _providerModuleName;
		    
		      Uint32 pos = instances[j].findProperty(_PROPERTY_PROVIDERMODULENAME);
		      if ( pos != PEGASUS_NOT_FOUND )
		      {
			instances[j].getProperty(pos).getValue().get(_providerModuleName);
			if (String::equalNoCase(deletedProviderModuleName, _providerModuleName))
		    	{
			    if (instances.size() == 1)
			    {
			        _registrationTable->table.remove(i.key());
			    }
			    else
			    {
			        instances.remove(j);
				j = j - 1;
			    }
		        }
		      }
		    }
		}
	    }
	}
     }

    catch (CIMException & exception)
    {
	errorCode = exception.getCode ();
 	errorDescription = exception.getMessage ();
	_repository->write_unlock();
	throw (exception);
    }
    catch (Exception & exception)
    {
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage ();
	_repository->write_unlock();
	throw (exception);
    }

    _repository->write_unlock();

}

void ProviderRegistrationManager::_addInstancesToTable(
    const String & key,
    const Array<CIMInstance> & instances)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
		     "ProviderRegistrationManager::_addInstancesToTable");
    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "key = " + key);

    ProviderRegistrationTable* elementInfo = 0;

    try
    {
	elementInfo = new ProviderRegistrationTable(instances);
    }

    catch (Exception& e)
    {
	delete elementInfo;
	PEG_METHOD_EXIT();
        throw e;
    }
 
    if (!_registrationTable->table.insert(key,elementInfo))
    {
	Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                      "Exception:: Attempt to add duplicate entry to provider reistration hash table.");
	//ATTN-YZ-P3-20020301:Is this proper exception 
	PEG_METHOD_EXIT();
        throw CIMException(CIM_ERR_FAILED, "Can not insert element to the table ");
    }
    PEG_METHOD_EXIT();
}

String ProviderRegistrationManager::_generateKey(
    const String & name,
    const String & provider)
{
    String providerKey = name;
    providerKey.append(provider);

    providerKey.toLower();
    return (providerKey);
}

String ProviderRegistrationManager::_generateKey(
    const String & namespaceName,
    const String & className,
    const String & providerType)
{
    String providerKey = namespaceName;
    providerKey.append(className);
    providerKey.append(providerType);

    providerKey.toLower();
    return (providerKey);
}

String ProviderRegistrationManager::_generateKey(
    const String & namespaceName,
    const String & className,
    const String & supportedMethod,
    const String & providerType)
{

    String providerKey = namespaceName;
    providerKey.append(className);

    if (String::equal(supportedMethod, "{}"))
    {
        // Ihis method provider supports all methods
	providerKey.append("AllMethods");
    }
    else
    {
    	providerKey.append(supportedMethod);
    }

    providerKey.append(providerType);

    providerKey.toLower();
    return (providerKey);
}

//
// get provider instance and module instance from registration table 
// by using provider name or provider module name
//
void ProviderRegistrationManager::_getInstances(
    const String & providerName,
    const String & moduleName,
    CIMInstance & providerInstance,
    CIMInstance & moduleInstance)
{
    Array<CIMInstance> _providerInstances;
    Array<CIMInstance> _moduleInstances;
    ProviderRegistrationTable* _provider;
    ProviderRegistrationTable* _module;

    //
    // create the key by using moduleName and providerName
    //
    String _providerKey = _generateKey(moduleName, providerName);

    //
    // create the key by using moduleName and MODULE_KEY
    //
    String _moduleKey = _generateKey(moduleName, MODULE_KEY);

    //
    // get provider instance
    //
    if (_registrationTable->table.lookup(_providerKey, _provider))
    {
	_providerInstances = _provider->getInstances();
	providerInstance = _providerInstances[0];
    }

    //
    // get provider module instance
    //
    if (_registrationTable->table.lookup(_moduleKey, _module))
    {
	_moduleInstances = _module->getInstances();
	moduleInstance = _moduleInstances[0];
    }
}

//
// get property names from the supported properties in the
// provider capability instance
//
void ProviderRegistrationManager::_getPropertyNames(
    const CIMInstance & instance,
    CIMPropertyList propertyNames)
{
    Array<String> _supportedProperties;
    Uint32 pos;

    pos = instance.findProperty(_PROPERTY_SUPPORTEDPROPERTIES);

    if (pos == PEG_NOT_FOUND)
    {
	//
	// no properties
     	//
	CIMPropertyList _propertyList(EmptyStringArray());
	propertyNames = _propertyList;
    }
    else
    {
	CIMValue value = instance.getProperty(pos).getValue();
	if (value.isNull())
	{
	    //
	    // supports all properties
	    //
	    propertyNames.clear();
	}
	else
	{
	    value.get(_supportedProperties);
	    CIMPropertyList _propertyList(_supportedProperties);
	    propertyNames = _propertyList;
	}
    }
}

//
// send delete notify message to the subscription service when  
// the provider capability instance was deleted
//
void ProviderRegistrationManager::_sendDeleteNotifyMessage(
    const CIMInstance & instance)
{
    String _providerName;
    String _providerModule;
    String _className;
    Array<String> _namespaces;
    CIMInstance _providerInstance;
    CIMInstance _moduleInstance;
    
    //
    // get provider module name
    //
    instance.getProperty(instance.findProperty
        (_PROPERTY_PROVIDERMODULENAME)).getValue().get(_providerModule);

    //
    // get provider name
    //
    instance.getProperty(instance.findProperty
        (_PROPERTY_PROVIDERNAME)).getValue().get(_providerName);

    //
    // get namespaces
    //
    instance.getProperty(instance.findProperty
        (_PROPERTY_NAMESPACES)).getValue().get(_namespaces);

    //
    // get classname
    //
    instance.getProperty(instance.findProperty
        (_PROPERTY_CLASSNAME)).getValue().get(_className); 

    //
    // get provider instance and module instance from the
    // registration table
    //  
    _getInstances(_providerName, _providerModule,
		  _providerInstance, _moduleInstance);

    CIMPropertyList _newPropertyNames(EmptyStringArray());
    CIMPropertyList _oldPropertyNames;

    //
    // get old property list from supported properties in the
    // providerCapability instance
    //
    _getPropertyNames(instance, _oldPropertyNames);

    //
    // get indication server queueId
    // 
    MessageQueueService * _service = _getIndicationService();
    
    CIMNotifyProviderRegistrationRequestMessage * notify_req =
	new CIMNotifyProviderRegistrationRequestMessage(
	    XmlWriter::getNextMessageId (),
	    CIMNotifyProviderRegistrationRequestMessage::Operation(OP_DELETE),
	    _providerInstance,
	    _moduleInstance,
	    _className,
	    EmptyStringArray(),
	    _namespaces,
	    _newPropertyNames,
	    _oldPropertyNames,
	    QueueIdStack(_service->getQueueId()));

    _sendMessageToSubscription(notify_req);
}

//
// send modify notify message to the subscription service when  
// the provider capability instance was modified
//
void ProviderRegistrationManager::_sendModifyNotifyMessage(
    const CIMInstance & instance,
    const CIMInstance & origInstance)
{
    String _providerName;
    String _providerModule;
    String _className;
    Array<String> _newNamespaces;
    Array<String> _oldNamespaces;
    CIMInstance _providerInstance;
    CIMInstance _moduleInstance;
    
    //
    // get provider module name
    //
    instance.getProperty(instance.findProperty
        (_PROPERTY_PROVIDERMODULENAME)).getValue().get(_providerModule);

    //
    // get provider name
    //
    instance.getProperty(instance.findProperty
        (_PROPERTY_PROVIDERNAME)).getValue().get(_providerName);

    //
    // get new namespaces
    //
    instance.getProperty(instance.findProperty
        (_PROPERTY_NAMESPACES)).getValue().get(_newNamespaces);

    //
    // get old namespaces
    //
    origInstance.getProperty(origInstance.findProperty
        (_PROPERTY_NAMESPACES)).getValue().get(_oldNamespaces);

    //
    // get classname
    //
    instance.getProperty(instance.findProperty
        (_PROPERTY_CLASSNAME)).getValue().get(_className); 

    //
    // get provider instance and module instance from the
    // registration table
    //  
    _getInstances(_providerName, _providerModule,
		  _providerInstance, _moduleInstance);

    CIMPropertyList _newPropertyNames;
    CIMPropertyList _oldPropertyNames;

    //
    // get old property list from supported properties in the
    // providerCapability instance
    //
    _getPropertyNames(origInstance, _oldPropertyNames);

    //
    // get new property list from supported properties in the
    // providerCapability instance
    //
    _getPropertyNames(instance, _newPropertyNames);

    //
    // get indication server
    // 
    MessageQueueService * _service = _getIndicationService();

    CIMNotifyProviderRegistrationRequestMessage * notify_req =
	new CIMNotifyProviderRegistrationRequestMessage(
	    XmlWriter::getNextMessageId (),
	    CIMNotifyProviderRegistrationRequestMessage::Operation(OP_MODIFY),
	    _providerInstance,
	    _moduleInstance,
	    _className,
	    _newNamespaces,
	    _oldNamespaces,
	    _newPropertyNames,
	    _oldPropertyNames,
	    QueueIdStack(_service->getQueueId()));

    _sendMessageToSubscription(notify_req);
}

// get indication service
MessageQueueService * ProviderRegistrationManager::_getIndicationService()
{
    MessageQueue * queue = MessageQueue::lookup(PEGASUS_QUEUENAME_INDICATIONSERVICE);

    MessageQueueService * _service = dynamic_cast<MessageQueueService *>(queue);

    return(_service);
}

//
// send notify message to subscription service
//
void ProviderRegistrationManager::_sendMessageToSubscription(
    CIMNotifyProviderRegistrationRequestMessage * notify_req)
{
    pegasus_internal_identity _id = peg_credential_types::MODULE;
    ModuleController * _controller;
    ModuleController::client_handle *_client_handle;

    _controller = &(ModuleController::get_client_handle(_id, &_client_handle));
    if((_client_handle == NULL))
    {
	ThrowUnitializedHandle();
    }

    //
    // get indication server queueId
    //
    MessageQueueService * _service = _getIndicationService();
    Uint32 _queueId = _service->getQueueId();

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
    	new AsyncLegacyOperationStart (
		_service->get_next_xid(),
		NULL,
 		_queueId,	
		notify_req,
		_queueId);

    if( false  == _controller->ClientSendForget(
			   *_client_handle,
			   _queueId,
			   asyncRequest))
    {
    	delete asyncRequest;
    	throw CIMException(CIM_ERR_NOT_FOUND);
    }
				
    delete asyncRequest;
}

PEGASUS_NAMESPACE_END
