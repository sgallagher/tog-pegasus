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

#include <Pegasus/Common/PegasusVersion.h>

#include "ProviderRegistrationProvider.h"

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
   The name of the name property for PG_Provider class
*/
static const char _PROPERTY_PROVIDER_NAME [] = "Name";

/**
   The name of the Name property for PG_ProviderModule class
*/
static const char _PROPERTY_PROVIDERMODULE_NAME [] = "Name";

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
static const Uint16 _PROVIDER_OK        = 2;

static const Uint16 _PROVIDER_STOPPED   = 10;

ProviderRegistrationProvider::ProviderRegistrationProvider(
    ProviderRegistrationManager * providerRegistrationManager)	
{
    _providerRegistrationManager = providerRegistrationManager;
}

ProviderRegistrationProvider::~ProviderRegistrationProvider(void)	
{
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

    // ensure the class existing in the specified namespace
    String className = instanceReference.getClassName();

    if(!String::equalNoCase(className, _CLASS_PG_PROVIDER) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_CAPABILITIES) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_MODULE))
    {
	throw CIMException(CIM_ERR_INVALID_CLASS);
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
    // ensure the class existing in the specified namespace
    String className = classReference.getClassName();

    if(!String::equalNoCase(className, _CLASS_PG_PROVIDER) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_CAPABILITIES) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_MODULE))
    {
	throw CIMException(CIM_ERR_INVALID_CLASS);
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
    // ensure the class existing in the specified namespace
    String className = classReference.getClassName();

    if(!String::equalNoCase(className, _CLASS_PG_PROVIDER) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_CAPABILITIES) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_MODULE))
    {
	throw CIMException(CIM_ERR_INVALID_CLASS);
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
        const OperationContext & context,
        const CIMReference & instanceReference,
        const CIMInstance & instanceObject,
        const Uint32 flags,
        const CIMPropertyList & propertyList,
        ResponseHandler<CIMInstance> & handler)
{
    //
    // only support to modify the instance of PG_ProviderCapabilities
    //
    if (!String::equalNoCase(instanceReference.getClassName(), 
			     _CLASS_PROVIDER_CAPABILITIES))
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED);
    }

    //
    // only can modify the property of Namespaces, property of
    // SupportedProperties, and property of SupportedMethods
    //
    if (propertyList.isNull())
    {
	throw CIMException (CIM_ERR_NOT_SUPPORTED);
    }

    Array<String> propertyArray = propertyList.getPropertyNameArray();
    for (Uint32 i=0; i<propertyArray.size(); i++)
    {
	if (!String::equalNoCase(propertyArray[i], _PROPERTY_NAMESPACES) &&
	    !String::equalNoCase(propertyArray[i], _PROPERTY_SUPPORTEDPROPERTIES) &&
	    !String::equalNoCase(propertyArray[i], _PROPERTY_SUPPORTEDMETHODS))
	{
	    throw CIMException (CIM_ERR_NOT_SUPPORTED);
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

    // ensure the class existing in the specified namespace
    if(!String::equalNoCase(className, _CLASS_PG_PROVIDER) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_CAPABILITIES) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_MODULE))
    {
	throw CIMException(CIM_ERR_INVALID_CLASS);
    }

    //
    // Check all required properties are set
    //
    if(String::equalNoCase(className, _CLASS_PROVIDER_MODULE))
    {
	//
	// Name, Version, InterfaceType, InterfaceVersion, and Location
	// properties must be set
	// OperationalStatus property needs to be set. If not, set to default
	//
	if (!instanceObject.existsProperty(_PROPERTY_PROVIDERMODULE_NAME))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}

	if (!instanceObject.existsProperty(_PROPERTY_VERSION))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}

	if (!instanceObject.existsProperty(_PROPERTY_INTERFACETYPE))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}

	if (!instanceObject.existsProperty(_PROPERTY_INTERFACEVERSION))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}

	if (!instanceObject.existsProperty(_PROPERTY_LOCATION))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}

	if (!instanceObject.existsProperty(_PROPERTY_OPERATIONALSTATUS))
	{
	    Array<Uint16> _operationalStatus;
	    _operationalStatus.append(_PROVIDER_OK);
	    instance.addProperty (CIMProperty
		(_PROPERTY_OPERATIONALSTATUS, _operationalStatus));
	}
    }
    else if(String::equalNoCase(className, _CLASS_PROVIDER_CAPABILITIES))
    {
	//
	// ProviderModuleName, ProviderName, InstanceID, ClassName,
 	// Namespaces, and ProviderType properties must be set
	//

	if (!instanceObject.existsProperty(_PROPERTY_PROVIDERMODULENAME))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}

	if (!instanceObject.existsProperty(_PROPERTY_PROVIDERNAME))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}

	if (!instanceObject.existsProperty(_PROPERTY_CAPABILITYID))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}

	if (!instanceObject.existsProperty(_PROPERTY_CLASSNAME))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}

	if (!instanceObject.existsProperty(_PROPERTY_NAMESPACES))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}

	if (!instanceObject.existsProperty(_PROPERTY_PROVIDERTYPE))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}
    }
    else // _CLASS_PG_PROVIDER
    {
	//
	// Name and ProviderModuleName properties must be set
	//
	if (!instanceObject.existsProperty(_PROPERTY_PROVIDER_NAME))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
	}
	
	if (!instanceObject.existsProperty(_PROPERTY_PROVIDERMODULENAME))
	{
	    throw CIMException (CIM_ERR_INVALID_PARAMETER);
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
    String className = instanceReference.getClassName();

    // ensure the class existing in the specified namespace
    if(!String::equalNoCase(className, _CLASS_PG_PROVIDER) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_CAPABILITIES) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_MODULE))
    {
	throw CIMException(CIM_ERR_INVALID_CLASS);
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
// ATTN-YZ-20020411: Add code to send message to provider manager to 
// execute the stop and start methods

   throw CIMException (CIM_ERR_NOT_SUPPORTED);

/* ATTN-YZ-P1-20020301: Modify this function because schema is changed
    String className = objectReference.getClassName();
	
    // ensure the class existing in the specified namespace
    if(!String::equalNoCase(className, _CLASS_PG_PROVIDER) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_CAPABILITIES) &&
       !String::equalNoCase(className, _CLASS_PROVIDER_MODULE))
    {
	throw CIMException(CIM_ERR_INVALID_CLASS);
    }

    handler.processing();
    Uint32 retValue;
    String providerName;

    // get provider name from reference
    Array<KeyBinding> keys = objectReference.getKeyBindings();

    Uint32 i;
    for(i=0; i<keys.size() ; i++)
    {
	if(String::equal(keys[i].getName(), "Name"))
	{
	    providerName = keys[i].getValue();
	}
    }

    // if Name key not found
    if(i > keys.size())
    {
	// ATTN: may need diff exception
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
		"key Name was not found");
    }

    CIMReference newInstancereference("", "",
	objectReference.getClassName(),
	objectReference.getKeyBindings());

    if(String::equalNoCase(methodName, _STOP_PROVIDER))
    {

        Boolean force = false;
        Uint32 timeoutValue = 0;
        String forceStr = String::EMPTY;
        String timeoutStr = String::EMPTY;

        // Get the input parameter values
        //
        // ATTN: Currently the server only returns String values even
        //       though the types of the parameters are not defined
        //       as String type.
        //
        for (Uint32 i = 0; i < inParameters.size(); i++)
        {
            String parmName = inParameters[i].getParameter().getName();
            if (String::equalNoCase(parmName, "force"))
            {
                //
                // get the force parameter
                //
                inParameters[i].getValue().get(forceStr);
                if (String::equalNoCase(forceStr, "TRUE"))
                {
                    force = true;
                }
            }
            else
            {
                if (String::equalNoCase(parmName, "timeout"))
                {
                    //
                    // get the timeout value
                    //
                    inParameters[i].getValue().get(timeoutStr);

                    //
                    // convert the timeout string to integer
                    //
                    if (timeoutStr != String::EMPTY)
                    {
                        char* tmp = timeoutStr.allocateCString();
                        timeoutValue = strtol(tmp, (char **)0, 10);
                        delete [] tmp;
                    }
                }
                else
                {
                    throw PEGASUS_CIM_EXCEPTION( CIM_ERR_INVALID_PARAMETER,
                                        "Input parameters are not valid.");
                }
            }
        }

	// change provider status to be stopping
	// do not accept any new requests
        // ATTN: Change this to send a message
	//retValue = _cimom.getProviderManager()->stoppingProvider(providerName);
	if ( retValue ) // return value is 1 -- provider not found
	{
	    // ATTN: throw exception
	   // throw();
	}

	// ATTN: need a way to find if there are any outstanding requests
	// if there are outstanding requests, wait periodically until
	// all requests are processed or timeout expires

	// Uint32 requestCount = getOutstandingRequestCount(providerName);

	Uint32 waitTime = 1000;                 // one second wait interval

	// ATTN: for testing purpose, set requestCount = 3
	Uint32 requestCount = 3;

	while (requestCount > 0 && timeoutValue > 0)
	{	

	    // any outstanding requests given a grace period to complete;
	    System::sleep(waitTime);

	    // ATTN:
	    // requestCount = getOutstandingRequestCount(providerName);

	    // ATTN: for testing purpose
	    requestCount--;

	    timeoutValue = timeoutValue - waitTime;
	}

	// If no more requests or force stop option is specified, proceed
	// to stop the provider
	if (requestCount == 0 || force)
	{
	    // change provider status to be stopped
            // ATTN: Change this to send a message
	    //retValue = _cimom.getProviderManager()->providerStopped(providerName);

	   // stop the provider
           // ATTN: Change this to send a message
	   //retValue = _cimom.getProviderManager()->stopProvider(providerName);
	   // ATTN: Update repository, but if a provider can not set property
	   // AutomaticallyStarted = FALSE, do not need update repository
	   if(retValue == 0)
	   {
	        // update repository
	        //_cimom.getRepository()->setProperty(objectReference.getNameSpace(),
		//  newInstancereference, "OperationalStatus", _PROVIDER_STOPPED);
	   }
	}
	else
	{
	    // if there are still outstanding requests after the grace period
	    // and force stop option is not specified, change provider status to be OK
            // ATTN: Change this to send a message
	    //_cimom.getProviderManager()->startProvider(providerName);

	    //ATTN:  inform the client that provider not stopped
	    //throw ();
	
	}
    }
    else if(String::equalNoCase(methodName, _START_PROVIDER))
    {
        // ATTN: Send a message to the ProviderManager
	//retValue = _cimom.getProviderManager()->startProvider(providerName);
	if(retValue == 0)
	{
	    // update repository
            // ATTN: Do we really need to do this?  If we do, should we do it
            // through the ProviderRegistrationManager instead of directly
            // accessing the repository?
	    //_cimom.getRepository()->setProperty(objectReference.getNameSpace(),
	    //    newInstancereference, "OperationalStatus", _PROVIDER_OK);
	}
	else
	{
		// ATTN: need new exception
		// throw();		
	}
    }
    else
    {
	throw CIMException(CIM_ERR_METHOD_NOT_AVAILABLE);
    }

    CIMValue cimValue_output(retValue);
	
    handler.deliver(cimValue_output);

    handler.complete();
*/
}

PEGASUS_NAMESPACE_END
