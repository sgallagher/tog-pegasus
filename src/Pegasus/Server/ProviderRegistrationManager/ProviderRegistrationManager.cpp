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

#include "ProviderRegistrationTable.h"

#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_NAMESPACE_BEGIN

/**
   The name of the PG_Provider class 
*/
static const char _CLASS_PG_PROVIDER [] = "PG_Provider";

/**
   The name of the provider capabilities class 
*/
static const char _CLASS_PROVIDER_CAPABILITIES [] = "PG_ProviderCapabilities";

/**
   The name of the provider module class 
*/
static const char _CLASS_PROVIDER_MODULE [] = "PG_ProviderModule";

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
   The name of the namespace which is owned by CIMOM 
*/
static const char INTEROPNAMESPACE [] = "root/cimv2";

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
static const char _STOP_PROVIDER[]     = "StopService";

/**
   starting provider method
*/
static const char _START_PROVIDER[]   = "StartService";

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

    ProviderRegistrationTable* providerCapability = 0;
    ProviderRegistrationTable* _provider= 0;
    ProviderRegistrationTable* _providerModule = 0;
    
    //
    // create the key by using nameSpace, className, and providerType
    //
    String capabilityKey = _generateKey(nameSpace, className, INS_PROVIDER);

    try
    {
        // 
        // get provider capability instance from the table
        //
        if (!_registrationTable->table.lookup(capabilityKey, providerCapability))
        {
            throw CIMException(CIM_ERR_FAILED, "provider capability has not been registered yet");
        }

        Array<CIMInstance> instances = providerCapability->getInstances();

        Uint32 pos = instances[0].findProperty(_PROPERTY_PROVIDERNAME);

        if (pos == PEG_NOT_FOUND)
        {
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
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
        }

        instances[0].getProperty(pos2).getValue().get(providerModuleName);

        // 
        // get provider instance from the table by using providerName to be key
        //
        if (!_registrationTable->table.lookup(providerName, _provider))
        {
	    throw CIMException(CIM_ERR_FAILED, "can not find the provider");
        }

        Array<CIMInstance> providerInstances = _provider->getInstances();
        provider = providerInstances[0];

        // 
        // get provider module instance from the table by using providerModule
        // name to be key
        //
        if (!_registrationTable->table.lookup(providerModuleName, _providerModule))
        {
	    throw CIMException(CIM_ERR_FAILED, "can not find the provider module");
        }

    }
    catch(CIMException & exception)
    {
	return (false);
    }

    Array<CIMInstance> providerModuleInstances = _providerModule->getInstances();
    providerModule = providerModuleInstances[0];
    
    return (true);
}

Boolean ProviderRegistrationManager::lookupMethodProvider(
    const String & nameSpace, 
    const String & className, 
    const String & method, 
    CIMInstance & provider,
    CIMInstance & providerModule)
{
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
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	}

	instances[0].getProperty(pos).getValue().get(providerName);

	//
	// get provider module name
	//
	Uint32 pos2 = instances[0].findProperty(_PROPERTY_PROVIDERMODULENAME);
    	if (pos2 == PEG_NOT_FOUND)
    	{
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
	    	throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	    }

	    instances[0].getProperty(pos).getValue().get(providerName);

	    //
	    // get provider module name
	    //
	    Uint32 pos2 = instances[0].findProperty(_PROPERTY_PROVIDERMODULENAME);
    	    if (pos2 == PEG_NOT_FOUND)
    	    {
	    	throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	    }
    	    instances[0].getProperty(pos2).getValue().get(providerModuleName);
	}
	else
	{
            throw CIMException(CIM_ERR_FAILED, "provider has not been registered yet");
	}
	
    }

    // 
    // get provider instance from the table by using providerName to be key
    //
    if (!_registrationTable->table.lookup(providerName, _provider))
    {
	throw CIMException(CIM_ERR_FAILED, "can not find the provider");
    }

    Array<CIMInstance> providerInstances = _provider->getInstances();
    provider = providerInstances[0];

    // 
    // get provider module instance from the table by using providerModule
    // name to be key
    //
    if (!_registrationTable->table.lookup(providerModuleName, _providerModule))
    {
	throw CIMException(CIM_ERR_FAILED, "can not find the provider module");
    }
  }
  catch(CIMException & exception)
  {
	return (false);
  }

    Array<CIMInstance> providerModuleInstances = _providerModule->getInstances();
    providerModule = providerModuleInstances[0];
    
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
    const Array<String>& requiredProperties,
    Array<CIMInstance> & provider,
    Array<CIMInstance> & providerModule)
{
    Array <CIMInstance> _providerInstances;
    Array <CIMInstance> _providerModuleInstances;

    String providerName;
    String providerModuleName;

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
        throw CIMException(CIM_ERR_FAILED, "provider capability has not been registered yet");
    }

    Array<CIMInstance> instances = providerCapability->getInstances();

    for (Uint32 i=0; i < instances.size(); i++)
    {
	Array <String> _supportedProperties;
	//
	// get supported properties
	//
	Uint32 pos = instances[i].findProperty(_PROPERTY_SUPPORTEDPROPERTIES);
    	if (pos == PEG_NOT_FOUND)
    	{
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	}

	instances[i].getProperty(pos).getValue().get(_supportedProperties);

	//
	// get provider name
	//
	Uint32 pos2 = instances[i].findProperty(_PROPERTY_PROVIDERNAME);
    	if (pos2 == PEG_NOT_FOUND)
    	{
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	}
	instances[i].getProperty(pos2).getValue().get(providerName);

	//
	// get provider module name
	//
	Uint32 pos3 = instances[i].findProperty(_PROPERTY_PROVIDERMODULENAME);
    	if (pos3 == PEG_NOT_FOUND)
    	{
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
    	}
	instances[i].getProperty(pos3).getValue().get(providerModuleName);

	if (_supportedProperties == EmptyStringArray())
	{
	    //
	    // provider supportes all the properties
	    // get provider instance from the table by using providerName to be key
	    //
	    if (!_registrationTable->table.lookup(providerName, _provider))
    	    {
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
	    // get provider module instance from the table by using providerModule 
	    // Name to be key
	    //
	    if (!_registrationTable->table.lookup(providerModuleName, _providerModule))
    	    {
        	throw CIMException(CIM_ERR_FAILED, "can not find the provider module");
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
	    if (requiredProperties != EmptyStringArray())
	    {
		// 
		// there is a list for the required properties
		//
		Boolean match = true; 

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
		    // providerName to be key
	    	    //
	    	    if (!_registrationTable->table.lookup(providerName, _provider))
    	    	    {
        		throw CIMException(CIM_ERR_FAILED, "can not find the provider");
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
                     // get provider module instance from the table by using 
		     // providerModule Name to be key
            	     //
            	     if (!_registrationTable->table.lookup(providerModuleName, _providerModule))
            	     {
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
	return (false);
  }
    
    return (true);
}

// get registered provider
CIMInstance ProviderRegistrationManager::getInstance(
    const CIMReference & instanceReference)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance instance;

    CIMReference localReference("", "",
	instanceReference.getClassName(),
	instanceReference.getKeyBindings());

    try 
    {
	instance = _repository->getInstance(
 	    instanceReference.getNameSpace(), localReference);
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

    return (instance);
}

// get all registered providers
Array<CIMNamedInstance> ProviderRegistrationManager::enumerateInstances(
    const CIMReference & ref)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance instance;

    Array<CIMNamedInstance> enumInstances;

    try
    {
	enumInstances = _repository->enumerateInstances(
	    ref.getNameSpace(),
	    ref.getClassName());
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

    return (enumInstances);

}

// get all registered provider names
Array<CIMReference> ProviderRegistrationManager::enumerateInstanceNames(
    const CIMReference & ref)
{

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance instance;

    Array<CIMReference> enumInstanceNames;

    try
    {
        // get all instance names from repository
        enumInstanceNames = _repository->enumerateInstanceNames(
	    ref.getNameSpace(),
	    ref.getClassName());
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

    String _providerModule;
    String _providerName;

    String className = ref.getClassName();
    String nameSpace = ref.getNameSpace();

    try
    {
	// 
	// register PG_ProviderModule class
	//
	if (String::equalNoCase(className, _CLASS_PROVIDER_MODULE))
	{
    	    Array<CIMInstance> instances;

	    cimRef = _repository->createInstance(nameSpace, instance); 

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
	    _addInstancesToTable(_providerModule, instances);

	    return (cimRef);
	}	

	// 
	// register PG_Provider class
	//
	if (String::equalNoCase(className, _CLASS_PG_PROVIDER))
	{
    	    Array<CIMInstance> instances;

	    // get provider module name
	    instance.getProperty(instance.findProperty
		(_PROPERTY_PROVIDERMODULENAME)).getValue().get(_providerModule);
	    // get provider name
	    instance.getProperty(instance.findProperty
		(_PROPERTY_PROVIDER_NAME)).getValue().get(_providerName);

	    //
	    // check if the PG_ProviderModule class was registered
	    //
	    if (_registrationTable->table.contains(_providerModule))
	    {   
		//
		// the provider module class was registered
		//
	        cimRef = _repository->createInstance(nameSpace, instance); 
	        //
	        // Use provider name to be a key, add the instance to
	        // the hash table 
	        //
		instances.append(instance);

	        _addInstancesToTable(_providerName, instances);

	        return (cimRef);
	    }
	    else
	    {
		//
		// the provider module class is not registered
		//
		throw CIMException(CIM_ERR_FAILED, "PG_ProviderModule class"
			" needs to be registered before register the PG_Provider class");
	    }
	}	

	//
	// register CIM_Capabilities class
	//
	if (String::equalNoCase(className, _CLASS_PROVIDER_CAPABILITIES))
	{
	    Array<Uint16> _providerType;
	    Array<String> _namespaces;
	    Array<String> _supportedMethods;
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
	    // check if the PG_Provider class was registered
	    //
	    if (_registrationTable->table.contains(_providerName))
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
				// ATTN-YZ-P1-20020301: Implement send 
				// notification to subscription service
			    }
			    break;
			}

			case _METHOD_PROVIDER:
			{
			    //
			    // get supportedMethods
			    //
			    Uint32 pos = instance.findProperty(_PROPERTY_SUPPORTEDMETHODS);
			    if (pos != PEG_NOT_FOUND)
			    {
				instance.getProperty(pos).getValue().get(_supportedMethods);
			    }
			    else
			    {
				throw CIMException(CIM_ERR_INVALID_PARAMETER);
			    }

			    Uint32 methodsCount = _supportedMethods.size();
			    
			    for (Uint32 j=0; j < _namespaces.size(); j++)
			    {
				//
				// create a key by using namespace, className,
				// method name and providerType, store the 
				// instance to the table 
				//

				if (_supportedMethods == EmptyStringArray())
				{
    	    			    Array<CIMInstance> instances;

				    // Provider supports all methods
			            _capabilityKey = _generateKey(_namespaces[j],
				    _className, "{}", MET_PROVIDER);

				    if (_registrationTable->table.contains(_capabilityKey))
			            {
				        // the instance was already registered
				        throw CIMException(CIM_ERR_ALREADY_EXISTS);
			            }

				    // add the instance to the table
				    instances.append(instance);
				    _addInstancesToTable(_capabilityKey, instances);
				}
				else
				{
				    for (Uint32 k = 0; k < methodsCount; k++)
				    {
    	    			        Array<CIMInstance> instances;

			                _capabilityKey = _generateKey(_namespaces[j],
				            _className, _supportedMethods[k], MET_PROVIDER);
					if (_registrationTable->table.contains(_capabilityKey))
					{
					    // the instance already registered
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
            		    throw CIMException (CIM_ERR_NOT_SUPPORTED);
            		    break;
		    }
		}

	        cimRef = _repository->createInstance(nameSpace, instance); 
    		return (cimRef);
	    }
	    else
	    {
                // the provider class was not registered
                throw CIMException(CIM_ERR_FAILED, "PG_Provider class needs "
                   "to be registered before register the Provider capabilities class");
	    }
	}

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
    CIMReference cimRef;

    String className = instanceReference.getClassName();
    String nameSpace = instanceReference.getNameSpace();
    String providerName;

    CIMReference newInstancereference("", "",
	instanceReference.getClassName(),
	instanceReference.getKeyBindings());

    CIMInstance instance = _repository->getInstance(
	nameSpace, newInstancereference);
	    

    try
    {
	//
	// unregister PG_ProviderCapability class
	//
        if(String::equalNoCase(className, _CLASS_PROVIDER_CAPABILITIES))
        {
	    String deletedCapabilityID;
	    Array<CIMInstance> instances;
	    Uint32 instancesCount;
	    Array<Uint16> providerType;

	    //
	    // delete the instance from repository
	    //
	    _repository->deleteInstance(nameSpace, newInstancereference);

	    //
	    // get the key capabilityID
	    //
	    instance.getProperty(instance.findProperty(
		_PROPERTY_CAPABILITIESID)).getValue().get(deletedCapabilityID);
	    //
	    // remove all entries which have same capabilityID from the 
	    // table; if the entry only has one instance, remove the entry;
   	    // otherwise, remove the instance. 
	    //
	    for (Table::Iterator i=_registrationTable->table.start(); i; i++)
	    {
		instances = i.value()->getInstances();
		instancesCount = instances.size();

		for (Uint32 j = 0; j < instancesCount; j++)
		{
	    	    String capabilityID;
		    
		    Uint32 pos = instances[j].findProperty(_PROPERTY_CAPABILITIESID);
		    if ( pos != PEG_NOT_FOUND)
		    {
	    	    	instances[j].getProperty(pos).getValue().get(capabilityID);
		    	if (String::equal(deletedCapabilityID, capabilityID))
		    	{
			    if (instancesCount == 1)
			    {
			        _registrationTable->table.remove(i.key());
			    }
			    else
			    {
			        instances.remove(j);
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
		if (providerType[i] == _INDICATION_PROVIDER)
		{
		// ATTN-YZ-P1-20020308: Implement 
		// ATTN-YZ-P1-20020308: Implment 
/*
		// ATTN: This notification message need to rebuild
		// based on current design
	        String providername;
	        instance.getProperty(
		instance.findProperty("ProviderName")).getValue().get(providername);
	    String classname;
	    instance.getProperty(
		instance.findProperty("ClassName")).getValue().get(classname);
	    Array<String> oldPropertyNames = EmptyStringArray();
	    Array<String> newPropertyNames = EmptyStringArray();

		// send delete notification
		notifyProviderRegistration(providername,
			   classname,
			   newPropertyNames,
			   oldPropertyNames,
			   2);
*/

		}
	    }
        }

	//
	// Unregister PG_Provider class
	// Note: Deleteting an instance of PG_Provider will cause the 
	// associated instances of PG_ProviderCapability to be deleted 
	//
        if(String::equalNoCase(className, _CLASS_PG_PROVIDER))
        {
	    String deletedProviderName;

	    //
	    // get the key provider name
	    //
	    instance.getProperty(instance.findProperty(
		_PROPERTY_PROVIDER_NAME)).getValue().get(deletedProviderName);

	    //
	    // delete instance of PG_Provider from repository
	    //
	    _repository->deleteInstance(nameSpace, newInstancereference);

	    //
	    // delete associated instances of PG_ProviderCapability
	    //
	    Array<CIMNamedInstance> enumCapInstances;
	
	    enumCapInstances = _repository->enumerateInstances(
				nameSpace, _CLASS_PROVIDER_CAPABILITIES); 	

	    for (Uint32 i = 0, n = enumCapInstances.size(); i < n; i++)
	    {
		CIMInstance capInstance;
		CIMReference capReference;
		String _providerName;

		capInstance = enumCapInstances[i].getInstance();

		// 
		// get provider name in the instance of provider capability
		//
		capInstance.getProperty(capInstance.findProperty(
		    _PROPERTY_PROVIDERNAME)).getValue().get(_providerName);

		if (String::equalNoCase(deletedProviderName, _providerName))
		{
		    //
		    // if provider name of instance of provider capability is
		    // same as deleted provider name, delete the instance of
		    // provider capability from repository
		    //
		    capReference = enumCapInstances[i].getInstanceName(); 

    		    CIMReference newCapReference("", "",
			capReference.getClassName(),
			capReference.getKeyBindings());

	    	    _repository->deleteInstance(nameSpace, newCapReference);
		    
		}

	    }

	    //
	    // remove all entries which have same provider name from the 
	    // table; if the entry only has one instance, remove the entry;
   	    // otherwise, remove the instance. 
	    //
	    for (Table::Iterator i=_registrationTable->table.start(); i; i++)
	    {
		Array<CIMInstance> instances;
		Uint32 instancesCount;

		// 
		// remove all entries which their key's value is same as provider name 
		// from the table
		//
		if (String::equal(deletedProviderName, i.key()))
		{
		    _registrationTable->table.remove(i.key());
		}

		instances = i.value()->getInstances();
		instancesCount = instances.size();

		for (Uint32 j = 0; j < instancesCount; j++)
		{
	    	    String _providerName;
		    
		    Uint32 pos = instances[j].findProperty(_PROPERTY_PROVIDERNAME);
		    if ( pos != PEG_NOT_FOUND)
		    {
	    	    	instances[j].getProperty(pos).getValue().get(_providerName);
			if (String::equal(deletedProviderName, _providerName))
		    	{
			    if (instancesCount == 1)
			    {
			        _registrationTable->table.remove(i.key());
			    }
			    else
			    {
			        instances.remove(j);
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
        if(String::equalNoCase(className, _CLASS_PROVIDER_MODULE))
        {
	    String deletedProviderModuleName;

	    //
	    // get the key provider module name
	    //
	    instance.getProperty(instance.findProperty(
		 _PROPERTY_PROVIDERMODULE_NAME)).getValue().get(deletedProviderModuleName);

	    //
	    // delete instance of PG_ProviderModule from repository
	    //
	    _repository->deleteInstance(nameSpace, newInstancereference);

	    //
	    // delete associated instances of PG_Provider
	    //
	    Array<CIMNamedInstance> enumProviderInstances;
	
	    enumProviderInstances = _repository->enumerateInstances(
				nameSpace, _CLASS_PG_PROVIDER); 	

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

	    	    _repository->deleteInstance(nameSpace, newProviderReference);
		    
		}

	    }

	    //
	    // delete associated instances of PG_ProviderCapability
	    //
	    Array<CIMNamedInstance> enumCapInstances;
	
	    enumCapInstances = _repository->enumerateInstances(
				nameSpace, _CLASS_PROVIDER_CAPABILITIES); 	

	    for (Uint32 i = 0, n = enumCapInstances.size(); i < n; i++)
	    {
		CIMInstance capInstance;
		CIMReference capReference;
		String _providerModuleName;

		capInstance = enumCapInstances[i].getInstance();

		// 
		// get provider module name in the instance of provider capability
		//
		capInstance.getProperty(capInstance.findProperty(
		    _PROPERTY_PROVIDERMODULENAME)).getValue().get(_providerModuleName);

		if (String::equal(deletedProviderModuleName, _providerModuleName))
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

	    	    _repository->deleteInstance(nameSpace, newCapReference);
		    
		}

	    }

	    //
	    // remove all entries which have same provider module name from the 
	    // table; if the entry only has one instance, remove the entry;
   	    // otherwise, remove the instance. 
	    //
	    for (Table::Iterator i=_registrationTable->table.start(); i; i++)
	    {
		Array<CIMInstance> instances;
		Uint32 instancesCount;

		// 
		// remove all entries which key's value is same as deleted provider 
		// module name from the table
		//
		if (String::equal(deletedProviderModuleName, i.key()))
		{
		    _registrationTable->table.remove(i.key());
		}

		//
		// if the entry only has one instance and provider module name of 
		// the instance is same as deleted provider module name, remove the
		// entry; 
		// if the entry has more than one instance and provider module name
		// of the instance is same as deleted provider module name, remove
		// the instance;
		//
		instances = i.value()->getInstances();
		instancesCount = instances.size();

		for (Uint32 j = 0; j < instancesCount; j++)
		{
	    	    String _providerModuleName;
		    
		    Uint32 pos = instances[j].findProperty(_PROPERTY_PROVIDERMODULENAME);
		    if ( pos != PEG_NOT_FOUND)
		    {
	    	    	instances[j].getProperty(pos).getValue().get(_providerModuleName);
			if (String::equal(deletedProviderModuleName, _providerModuleName))
		    	{
			    if (instancesCount == 1)
			    {
			        _registrationTable->table.remove(i.key());
			    }
			    else
			    {
			        instances.remove(j);
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
	throw (exception);
    }
    catch (Exception & exception)
    {
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage ();
	throw (exception);
    }

}

// ATTN-YZ-P1-20020301:Implement this interface
Uint16 ProviderRegistrationManager::getProviderStatus(
    const String & providerName)
{
    return(2);
}

// ATTN-YZ-P1-20020301:Implement this interface
void ProviderRegistrationManager::setProviderStatus(
    const String & providerName,
    Uint16 status)
{
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
  
    try
    {
        //
        // get all instances of providerModule class
        //

        cimNamedInstances = _repository->enumerateInstances(
                INTEROPNAMESPACE,
                _CLASS_PROVIDER_MODULE);

    	for(Uint32 i = 0, n=cimNamedInstances.size(); i < n; i++)
    	{
	    Array<CIMInstance> instances;
            instance = cimNamedInstances[i].getInstance();

	    // get provider module name
            instance.getProperty(instance.findProperty
            (_PROPERTY_PROVIDERMODULE_NAME)).getValue().get(providerModuleName);
	
	    //
            // Use provider module name to be a key, add the instance to
            // the hash table
            //
            instances.append(instance);
            _addInstancesToTable(providerModuleName, instances);
    	}

        //
        // get all instances of provider class
        //
        cimNamedInstances = _repository->enumerateInstances(
                INTEROPNAMESPACE,
                _CLASS_PG_PROVIDER);

    	for(Uint32 i = 0, n=cimNamedInstances.size(); i < n; i++)
    	{
	    Array<CIMInstance> instances;

            instance = cimNamedInstances[i].getInstance();

	    // get provider name
            instance.getProperty(instance.findProperty
            (_PROPERTY_PROVIDER_NAME)).getValue().get(providerName);

	    //
            // Use provider name to be a key, add the instance to
            // the hash table
            //
            instances.append(instance);
            _addInstancesToTable(providerName, instances);
    	}

        //
        // get all instances of providerCapabilities class
        //
        cimNamedInstances = _repository->enumerateInstances(
                INTEROPNAMESPACE,
                _CLASS_PROVIDER_CAPABILITIES);

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

}

void ProviderRegistrationManager::_addInstancesToTable(
    const String & key,
    const Array<CIMInstance> & instances)
{
    ProviderRegistrationTable* elementInfo = 0;

    try
    {
	elementInfo = new ProviderRegistrationTable(instances);
    }

    catch (Exception& e)
    {
	delete elementInfo;
        throw e;
    } 

    if (!_registrationTable->table.insert(key,elementInfo))
    {
	//ATTN-YZ-P3-20020301:Is this proper exception 
        throw CIMException(CIM_ERR_FAILED, "can not insert element to the table ");
    }
}

String ProviderRegistrationManager::_generateKey(
    const String & namespaceName,
    const String & className,
    const String & providerType)
{
    String providerKey = namespaceName;
    providerKey.append(className);
    providerKey.append(providerType);

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

    return (providerKey);
}

PEGASUS_NAMESPACE_END
