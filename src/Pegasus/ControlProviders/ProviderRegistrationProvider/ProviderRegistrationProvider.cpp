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
// Author: Yi Zhou (yi_zhou@hp.com)
//
// Modified By: Chip Vincent (cvincent@us.ibm.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderRegistrationProvider.h"

#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/MessageLoader.h> //l10n

PEGASUS_NAMESPACE_BEGIN

/**
   The name of the CapabilityID property for provider capabilities class
*/
static const CIMName _PROPERTY_CAPABILITYID  = CIMName ("CapabilityID");

/**
   Module status
*/
static const Uint16 _MODULE_OK        = 2;

/**
   stopping provider method
*/
static const CIMName _STOP_PROVIDER     = CIMName ("Stop");

/**
   starting provider method
*/
static const CIMName _START_PROVIDER   = CIMName ("Start");

/**
   Provider status
*/
static const Uint16 _MODULE_STOPPING   = 9;

static const Uint16 _MODULE_STOPPED   = 10;

ProviderRegistrationProvider::ProviderRegistrationProvider(
    ProviderRegistrationManager * providerRegistrationManager)	
    :_id(peg_credential_types::PROVIDER)
{
    _providerRegistrationManager = providerRegistrationManager;

    _controller = &(ModuleController::get_client_handle(_id, &_client_handle));
    if(_client_handle == NULL)
        throw UninitializedObjectException();
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
    const CIMObjectPath & instanceReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{

    if(!instanceReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
            instanceReference.getNameSpace().getString());
    }

    // ensure the class existing in the specified namespace
    CIMName className = instanceReference.getClassName();

    if(!className.equal (PEGASUS_CLASSNAME_PROVIDER) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            className.getString());
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
	throw (e);
    }

    handler.deliver(instance);

    // complete processing the request
    handler.complete();
}

// get all registered providers
void ProviderRegistrationProvider::enumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{
    if(!classReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            classReference.getNameSpace().getString());
    }

    // ensure the class existing in the specified namespace
    CIMName className = classReference.getClassName();

    if(!className.equal (PEGASUS_CLASSNAME_PROVIDER) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            className.getString());
    }

    // begin processing the request
    handler.processing();

    Array<CIMInstance> enumInstances;

    try
    {
        enumInstances = _providerRegistrationManager->enumerateInstances(classReference);
    }
    catch(CIMException& e)
    {
	throw (e);
    }

    handler.deliver(enumInstances);

    // complete processing the request
    handler.complete();
}

// get all registered provider names
void ProviderRegistrationProvider::enumerateInstanceNames(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    ObjectPathResponseHandler & handler)
{
    if(!classReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            classReference.getNameSpace().getString());
    }

    // ensure the class existing in the specified namespace
    CIMName className = classReference.getClassName();

    if(!className.equal (PEGASUS_CLASSNAME_PROVIDER) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            className.getString());
    }

    // begin processing the request
    handler.processing();

    Array<CIMObjectPath> enumInstanceNames;

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
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        const Boolean includeQualifiers,
        const CIMPropertyList & propertyList,
        ResponseHandler & handler)
{
    // get userName and only privileged user can execute this operation
    String userName;
    try
    {
        IdentityContainer container = context.get(IdentityContainer::NAME);
        userName = container.getUserName();
    }
    catch (...)
    {
        userName = String::EMPTY;
    }

    if ((userName != String::EMPTY) && !System::isPrivilegedUser(userName))
    {
    	//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
	    //"You must have superuser privilege to modify the registration."); 
	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED,MessageLoaderParms(
	    		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.SUPERUSER_PRIVILEGE_REQUIRED_MODIFY_REGISTRATION",
	    		"You must have superuser privilege to modify the registration.")); 	
    }

    if(!instanceReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            instanceReference.getNameSpace().getString());
    }

    //
    // only support to modify the instance of PG_ProviderCapabilities
    //
    if (!instanceReference.getClassName().equal 
        (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            instanceReference.getClassName().getString());
    }

    //
    // only can modify the property of Namespaces, property of
    // SupportedProperties, and property of SupportedMethods
    //
    if (propertyList.isNull())
    {
    	//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
	    //"Only can modify Namespaces, SupportedProperties, and SupportedMethods.");
	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED, MessageLoaderParms(
	    		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.CAN_ONLY_MODIFY_ERR",
	    		"Only can modify Namespaces, SupportedProperties, and SupportedMethods."));
    }

    Array<CIMName> propertyArray = propertyList.getPropertyNameArray();
    for (Uint32 i=0; i<propertyArray.size(); i++)
    {
	if (!propertyArray[i].equal (_PROPERTY_NAMESPACES) &&
	    !propertyArray[i].equal (_PROPERTY_SUPPORTEDPROPERTIES) &&
	    !propertyArray[i].equal (_PROPERTY_SUPPORTEDMETHODS))
	{
	    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
                propertyArray[i].getString());
	}
    }

    // begin processing the request
    handler.processing();

    try
    {
        _providerRegistrationManager->modifyInstance(instanceReference,
	    instanceObject, includeQualifiers, propertyArray);
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
    const CIMObjectPath & instanceReference,
    const CIMInstance & instanceObject,
    ObjectPathResponseHandler & handler)
{
    // get userName and only privileged user can execute this operation
    String PG_ProviderModule_Name = "PG_ProviderModule"; //l10n
    String PG_ProviderCapabilities_Name = "PG_ProviderCapabilities"; //l10n
    String PG_Provider_Name = "PG_Provider";
    String userName;
    try
    {
        IdentityContainer container = context.get(IdentityContainer::NAME);
        userName = container.getUserName();
    }
    catch (...)
    {
        userName = String::EMPTY;
    }

    if ((userName != String::EMPTY) && !System::isPrivilegedUser(userName))
    {
    	//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
	    //"You must have superuser privilege to register providers."); 
	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED, MessageLoaderParms(
	    			"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.SUPERUSER_PRIVILEGE_REQUIRED_REGISTER_PROVIDERS",
	    			"You must have superuser privilege to register providers.")); 	
    }

    CIMName className = instanceReference.getClassName();
    CIMNamespaceName nameSpace = instanceReference.getNameSpace();

    CIMObjectPath returnReference;

    CIMInstance instance = instanceObject;

    if(!nameSpace.equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            nameSpace.getString());
    }

    // ensure the class existing in the specified namespace
    if(!className.equal (PEGASUS_CLASSNAME_PROVIDER) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            className.getString());
    }

    //
    // Check all required properties are set
    //
    if (className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	//
	// Name, Version, InterfaceType, InterfaceVersion, and Location
	// properties must be set
	// OperationalStatus property needs to be set. If not, set to default
	//
	if (instanceObject.findProperty(_PROPERTY_PROVIDERMODULE_NAME) ==
            PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Name which is required property in PG_ProviderModule class.");
		String missing = "Name";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderModule_Name));
	}

	if (instanceObject.findProperty(_PROPERTY_VENDOR) == PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Vendor which is required property in PG_ProviderModule class.");
		String missing = "Vendor";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderModule_Name));
	}

	if (instanceObject.findProperty(_PROPERTY_VERSION) == PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Version which is required property in PG_ProviderModule class.");
		String missing = "Version";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderModule_Name));
	}

        Uint32 ifcTypeIndex =
            instanceObject.findProperty(_PROPERTY_INTERFACETYPE);
	if (ifcTypeIndex == PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing InterfaceType which is required property in PG_ProviderModule class.");
		String missing = "InterfaceType";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderModule_Name));
	}
        String ifcTypeString;
        instanceObject.getProperty(ifcTypeIndex).getValue().
            get(ifcTypeString);

        if(ifcTypeString != "C++Default" && ifcTypeString != "CMPI" )
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
		//"Unsupported InterfaceType value: \"" + ifcTypeString + "\"");
		String unsupported = "InterfaceType";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.UNSUPPORTED_VALUE",
		"Unsupported $0 value: \"$1\"",unsupported,ifcTypeString));
	}

        Uint32 ifcVersionIndex =
            instanceObject.findProperty(_PROPERTY_INTERFACEVERSION);
	if (ifcVersionIndex == PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing InterfaceVersion which is required property in PG_ProviderModule class.");
		String missing = "InterfaceVersion";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderModule_Name));
	}
        String ifcVersionString;
        instanceObject.getProperty(ifcVersionIndex).getValue().
            get(ifcVersionString);
        if ((ifcVersionString != "2.1.0") &&
            (ifcVersionString != "2.2.0") &&
	    (ifcVersionString != "2.3.0"))
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
		//"Unsupported InterfaceVersion value: \"" + ifcVersionString +
                   // "\"");
        String unsupported = "InterfaceVersion";
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.UNSUPPORTED_VALUE",
		"Unsupported $0 value: \"$1\"",unsupported,ifcVersionString));
	}

	if (instanceObject.findProperty(_PROPERTY_LOCATION) == PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Location which is required property in PG_ProviderModule class.");
		String missing = "Location";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderModule_Name));
	}

	if (instanceObject.findProperty(_PROPERTY_OPERATIONALSTATUS) == 
            PEG_NOT_FOUND)
	{
	    Array<Uint16> _operationalStatus;
	    _operationalStatus.append(_MODULE_OK);
	    instance.addProperty (CIMProperty
		(_PROPERTY_OPERATIONALSTATUS, _operationalStatus));
	}
    }
    else if (className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES))
    {
	//
	// ProviderModuleName, ProviderName, InstanceID, ClassName,
 	// Namespaces, and ProviderType properties must be set
	//

	if (instanceObject.findProperty(_PROPERTY_PROVIDERMODULENAME) ==
            PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderModuleName which is required property in PG_ProviderCapabilities class.");
		String missing = "ProviderModuleName";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderCapabilities_Name));
	}

	if (instanceObject.findProperty(_PROPERTY_PROVIDERNAME) == 
            PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderName which is required property in PG_ProviderCapabilities class.");
		String missing = "ProviderName";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderCapabilities_Name));
	}

	if (instanceObject.findProperty(_PROPERTY_CAPABILITYID) == 
            PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing CapabilityID which is required property in PG_ProviderCapabilities class.");
		String missing = "CapabilityID";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderCapabilities_Name));
	}

	if (instanceObject.findProperty(_PROPERTY_CLASSNAME) == PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ClassName which is required property in PG_ProviderCapabilities class.");
		String missing = "ClassName";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderCapabilities_Name));
	}

	if (instanceObject.findProperty(_PROPERTY_NAMESPACES) == PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Namespaces which is required property in PG_ProviderCapabilities class.");
		String missing = "Namespaces";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderCapabilities_Name));
	}

	if (instanceObject.findProperty(_PROPERTY_PROVIDERTYPE) == PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderType which is required property in PG_ProviderCapabilities class.");
		String missing = "ProviderType";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_ProviderCapabilities_Name));
	}
    }
    else if (className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES))
    {
	//
	// ProviderModuleName, ProviderName, CapabilityID, ProviderType,
 	// and Destinations properties must be set
	//

	if (instanceObject.findProperty(_PROPERTY_PROVIDERMODULENAME) ==
            PEG_NOT_FOUND)
	{
//L10N_ TODO DONE
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderModuleName which is required property in PG_ConsumerCapabilities class.");
	    MessageLoaderParms parms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_PROVIDER_MODULE_NAME_WHICH_IS_REQUIRED",
		"Missing ProviderModuleName which is required property in PG_ConsumerCapabilities class.");

	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	}

	if (instanceObject.findProperty(_PROPERTY_PROVIDERNAME) == 
            PEG_NOT_FOUND)
	{
//L10N_ TODO DONE
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderName which is required property in PG_ConsumerCapabilities class.");

	    MessageLoaderParms parms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_PROVIDER_NAME_WHICH_IS_REQUIRED",
		"Missing ProviderName which is required property in PG_ConsumerCapabilities class.");

	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	}

	if (instanceObject.findProperty(_PROPERTY_CAPABILITYID) == 
            PEG_NOT_FOUND)
	{
//L10N_ TODO DONE
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing CapabilityID which is required property in PG_ConsumerCapabilities class.");

	    MessageLoaderParms parms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_CAPABILITY_ID_WHICH_IS_REQUIRED",
		"Missing CapabilityID which is required property in PG_ConsumerCapabilities class.");

	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	}

	if (instanceObject.findProperty(_PROPERTY_PROVIDERTYPE) == PEG_NOT_FOUND)
	{
//L10N_ TODO DONE
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderType which is required property in PG_ConsumerCapabilities class.");

	    MessageLoaderParms parms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_PROVIDER_TYPE_WHICH_IS_REQUIRED",
		"Missing ProviderType which is required property in PG_ConsumerCapabilities class.");

	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	}

	if (instanceObject.findProperty(_PROPERTY_INDICATIONDESTINATIONS) == 
	    PEG_NOT_FOUND)
	{
//L10N_ TODO DONE
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Destinations which is required property in PG_ConsumerCapabilities class.");

	    MessageLoaderParms parms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_DESTINATIONS_TYPE_WHICH_IS_REQUIRED",
		"Missing Destinations which is required property in PG_ConsumerCapabilities class.");

	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, parms);
	}
    }
    else // PEGASUS_CLASSNAME_PROVIDER
    {
	//
	// Name and ProviderModuleName properties must be set
	//
	if (instanceObject.findProperty(_PROPERTY_PROVIDER_NAME) == PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing Name which is required property in PG_Provider class.");
		String missing = "Name";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_Provider_Name));
	}
	
	if (instanceObject.findProperty(_PROPERTY_PROVIDERMODULENAME) == 
            PEG_NOT_FOUND)
	{
		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
		//"Missing ProviderModuleName which is required property in PG_Provider class.");
		String missing = "ProviderModuleName";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.MISSING_REQUIRED_PROPERTY",
		"Missing $0 which is required property in $1 class.",missing,PG_Provider_Name));
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
    const CIMObjectPath & instanceReference,
    ResponseHandler & handler)
{
    // get userName and only privileged user can execute this operation
    String userName;
    try
    {
        IdentityContainer container = context.get(IdentityContainer::NAME);
        userName = container.getUserName();
    }
    catch (...)
    {
        userName = String::EMPTY;
    }

    if ((userName != String::EMPTY) && !System::isPrivilegedUser(userName))
    {
    	//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
	    //"You must have superuser privilege to unregister providers.");
	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.SUPERUSER_PRIVILEGE_REQUIRED_UNREGISTER_PROVIDERS",
	    "You must have superuser privilege to unregister providers.")); 	
    }

    if(!instanceReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            instanceReference.getNameSpace().getString());
    }


// l10n
    // Get the client's list of preferred languages for the response
    AcceptLanguages al = AcceptLanguages::EMPTY;
    try 
    {
        AcceptLanguageListContainer al_container = 
		(AcceptLanguageListContainer)context.get(AcceptLanguageListContainer::NAME);
        al = al_container.getLanguages();
    }
    catch (...)
    {
        ;   // Leave AcceptLanguages empty
    }

    CIMName className = instanceReference.getClassName();

    // ensure the class existing in the specified namespace
    if(!className.equal (PEGASUS_CLASSNAME_PROVIDER) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_CONSUMERCAPABILITIES) &&
       !className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            className.getString());
    }

    // begin processing the request
    handler.processing();

    String moduleName;
    Boolean moduleFound = false;
    Array<CIMKeyBinding> keys = instanceReference.getKeyBindings();

    //
    // disable provider before delete provider 
    // registration if the class is PG_Provider
    //
    if (className.equal (PEGASUS_CLASSNAME_PROVIDER))
    {
    	// get module name from reference

    	for(Uint32 i=0; i<keys.size() ; i++)
    	{
	    if(keys[i].getName().equal (_PROPERTY_PROVIDERMODULENAME))
	    {
	        moduleName = keys[i].getValue();
	        moduleFound = true;
	    }
	}

    	// if _PROPERTY_PROVIDERMODULENAME key not found
    	if( !moduleFound)
    	{
    		//l10n
	    //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
		//"key ProviderModuleName was not found");
		String sub = "ProviderModuleName";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.KEY_NOT_FOUND",
		"key $0 was not found",sub));
    	}

	// 
	// disable the provider 
	//
	try
	{
	     Sint16 ret_value = _disableModule(instanceReference, moduleName, true, al);

             //
             // if the provider disable failed
             //
// l10n
             if (ret_value == -1)
             {
             	//l10n
                 //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                     //"disable the provider failed.");
                 throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
					"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.DISABLE_PROVIDER_FAILED",
                    "disable the provider failed."));
             }
	     //
             // The provider disable failed since there are pending requests
             //
             if (ret_value == -2)
             {
//L10N TODO
                 throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                     "disable the provider failed: Provider is busy.");
             }
	}
    	catch(CIMException&)
    	{
	    throw;
    	}
    }

    //
    // disable provider module before remove provider registration
    // if the class is PG_ProviderModule 
    //

    if (className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE))
    {
    	// get module name from reference

    	for(Uint32 i=0; i<keys.size() ; i++)
    	{
	    if(keys[i].getName().equal (_PROPERTY_PROVIDERMODULE_NAME))
	    {
	        moduleName = keys[i].getValue();
	        moduleFound = true;
	    }
	}

    	// if _PROPERTY_PROVIDERMODULE_NAME key not found
    	if( !moduleFound)
    	{
    		//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
		//"key Name was not found");
		String sub = "Name";
		throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.KEY_NOT_FOUND",
		"key $0 was not found",sub));
    	}

	// 
	// disable the provider module
	//
	try
	{
	    Sint16 ret_value = _disableModule(instanceReference, moduleName, false, al);

            //
            // if the provider module disable failed
            //
// l10n
            if (ret_value == -1)
            {
            	//l10n
                 //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                     //"disable the provider module failed.");
                     throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,MessageLoaderParms(
						"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.DISABLE_PROVIDER_MODULE_FAILED",
                     	"disable the provider module failed."));
            }
	    
	    //
            // The provider module disable failed since there are pending requests
            //
            if (ret_value == -2)
            {
//L10N TODO
                 throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                     "disable the provider module failed: Provider is busy.");
            }
	}
    	catch(CIMException& e)
    	{
	    throw (e);
    	}
    }

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
    const CIMObjectPath & objectReference,
    const CIMName & methodName,
    const Array<CIMParamValue> & inParameters,
    MethodResultResponseHandler & handler)
{
    // get userName and only privileged user can execute this operation
    String userName;
    try
    {
        IdentityContainer container = context.get(IdentityContainer::NAME);
        userName = container.getUserName();
    }
    catch (...)
    {
        userName = String::EMPTY;
    }

    if ((userName != String::EMPTY) && !System::isPrivilegedUser(userName))
    {
    	//l10n
	//throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
	    //"You must have superuser privilege to disable or enable providers."); 
	    throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.SUPERUSER_PRIVILEGE_REQUIRED_DISABLE_ENABLE_PROVIDERS",
	    "You must have superuser privilege to disable or enable providers.")); 	
    }

    if(!objectReference.getNameSpace().equal (PEGASUS_NAMESPACENAME_INTEROP))
    {
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, 
            objectReference.getNameSpace().getString());
    }


// l10n
    // Get the client's list of preferred languages for the response
    AcceptLanguages al = AcceptLanguages::EMPTY;
    try 
    {
        AcceptLanguageListContainer al_container = 
		(AcceptLanguageListContainer)context.get(AcceptLanguageListContainer::NAME);
        al = al_container.getLanguages();
    }
    catch (...)
    {
        ;   // Leave AcceptLanguages empty
    }

    String moduleName;
    Boolean moduleFound = false;

    // get module name from reference
    Array<CIMKeyBinding> keys = objectReference.getKeyBindings();

    for(Uint32 i=0; i<keys.size() ; i++)
    {
	if(keys[i].getName().equal (_PROPERTY_PROVIDERMODULE_NAME))
	{
	    moduleName = keys[i].getValue();
	    moduleFound = true;
	}
    }

    // if _PROPERTY_PROVIDERMODULE_NAME key not found
    if( !moduleFound)
    {
    	//l10n
    	String sub = "Name";
	throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,MessageLoaderParms(
		"ControlProviders.ProviderRegistrationProvider.ProviderRegistrationProvider.KEY_NOT_FOUND",
		"key $0 was not found",sub));
    }

    handler.processing();

    Sint16 ret_value;

    if(methodName.equal(_STOP_PROVIDER))
    {
	// disable module
	try
	{
// l10n
    	     ret_value =  _disableModule(objectReference, moduleName, false, al);
	}
    	catch(CIMException& e)
    	{
	     throw (e);
        }

	CIMValue retValue(ret_value);
	handler.deliver(retValue);
    	handler.complete();
	return;
    }
    else if(methodName.equal(_START_PROVIDER))
    {
    	//
    	// get module status
    	//
    	Array<Uint16> _OperationalStatus =
	    _providerRegistrationManager->getProviderModuleStatus( moduleName);

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

	    // retValue equals 2 if module is stopping
	    // at this stage, module can not be started
	    if (_OperationalStatus[i] == _MODULE_STOPPING)
	    {
		ret_value = 2;
		CIMValue retValue(ret_value);
                handler.deliver(retValue);
                handler.complete();
                return;
	    }
	}

        // get module instance
        CIMInstance mInstance = 
	    _providerRegistrationManager->getInstance(objectReference);

        //
        // get provider manager service
        //
        MessageQueueService * _service = _getProviderManagerService();

	if (_service != NULL)
	{
	    // create CIMEnableModuleRequestMessage
	    CIMEnableModuleRequestMessage * enable_req =
	        new CIMEnableModuleRequestMessage(
		    XmlWriter::getNextMessageId (),
		    mInstance,
		    QueueIdStack(_service->getQueueId()));
// l10n
            enable_req->acceptLanguages = al;

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
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_METHOD_NOT_AVAILABLE, String::EMPTY);
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
    const CIMObjectPath & ref, const String & moduleName,
    const Boolean disableProviderOnly,
    const AcceptLanguages & al)
{
    CIMInstance instance;
    String _moduleName;
    Array<CIMInstance> instances;

    if (!disableProviderOnly)
    {
        CIMObjectPath reference("", PEGASUS_NAMESPACENAME_INTEROP,
	    PEGASUS_CLASSNAME_PROVIDER, ref.getKeyBindings());

        Array<CIMObjectPath> instanceNames =
	    _providerRegistrationManager->enumerateInstanceNames(reference);

        // find all the instances which have same module name as moduleName
        for (Uint32 i = 0, n=instanceNames.size(); i < n; i++)
        {
	    //
            // get provider module name from reference
            //

            Array<CIMKeyBinding> keys = instanceNames[i].getKeyBindings();

            for(Uint32 j=0; j < keys.size(); j++)
            {
                if(keys[j].getName().equal (_PROPERTY_PROVIDERMODULENAME))
                {
                    _moduleName = keys[j].getValue();
                }
            }

	    if (String::equalNoCase(moduleName, _moduleName))
	    {
	        reference.setKeyBindings(keys);
	        instance = _providerRegistrationManager->getInstance(reference);
	        instances.append(instance);
	    }
        }
    }
    else
    {
	    instance = _providerRegistrationManager->getInstance(ref);
	    instances.append(instance);
    }

    //
    // get indication server queueId
    //
    MessageQueueService * _service = _getIndicationService();

    if (_service != NULL)
    {
        Uint32 _queueId = _service->getQueueId();

        CIMNotifyProviderTerminationRequestMessage * termination_req =
	    new CIMNotifyProviderTerminationRequestMessage(
	        XmlWriter::getNextMessageId (),
	        instances,
	        QueueIdStack(_service->getQueueId()));

// l10n
        termination_req->acceptLanguages = al;

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
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, String::EMPTY);
        }
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

// disable provider module, return 0 if module is disabled successfully,
// return 1 if module is already disabled, return -2 if module can not be
// disabled since there are pending requests, otherwise, return -1
Sint16 ProviderRegistrationProvider::_disableModule(
    const CIMObjectPath & objectReference, 
    const String & moduleName,
    Boolean disableProviderOnly,
    const AcceptLanguages & al)         // l10n
{
    	//
    	// get module status
    	//
    	Array<Uint16> _OperationalStatus =
	    _providerRegistrationManager->getProviderModuleStatus( moduleName);

	for (Uint32 i = 0; i<_OperationalStatus.size(); i++)
	{
	    // retValue equals 1 if module is already disabled
	    if (_OperationalStatus[i] == _MODULE_STOPPED ||
		_OperationalStatus[i] == _MODULE_STOPPING)
	    {
		return (1);
	    }
	}

	CIMInstance instance;
	Array<CIMInstance> instances;
        CIMInstance mInstance;
	String _moduleName;
	Uint16 providers;
	CIMObjectPath providerRef;

	// disable a provider module or delete a provider module
	if (!disableProviderOnly)
	{
	    providerRef = CIMObjectPath(objectReference.getHost(),
				 objectReference.getNameSpace(),
				 PEGASUS_CLASSNAME_PROVIDER,
				 objectReference.getKeyBindings());

            // get module instance
            mInstance = 
	        _providerRegistrationManager->getInstance(objectReference);

	}
	else // disable a provider
	{
            // get module instance
	    Array <CIMKeyBinding> moduleKeyBindings;
	    moduleKeyBindings.append (CIMKeyBinding
		(_PROPERTY_PROVIDERMODULE_NAME, moduleName, 
		 CIMKeyBinding::STRING)); 

	    CIMObjectPath moduleRef(objectReference.getHost(),
				    objectReference.getNameSpace(),
				    PEGASUS_CLASSNAME_PROVIDERMODULE,
				    moduleKeyBindings);
				    
            mInstance = 
	        _providerRegistrationManager->getInstance(moduleRef);
	}

        if (!disableProviderOnly)
        {
	    // get all provider instances which have same module name as 
	    // moduleName
 	    Array<CIMObjectPath> instanceNames = 
	        _providerRegistrationManager->enumerateInstanceNames(providerRef);

	    for(Uint32 i = 0, n=instanceNames.size(); i < n; i++)
	    {
	        //
                // get provider module name from reference
                //

                Array<CIMKeyBinding> keys = instanceNames[i].getKeyBindings();

                for(Uint32 j=0; j < keys.size(); j++)
                {
                    if(keys[j].getName().equal (_PROPERTY_PROVIDERMODULENAME))
                    {
                        _moduleName = keys[j].getValue();
                    }
                }

	        if (String::equalNoCase(_moduleName, moduleName))
	        {
		    providerRef.setKeyBindings(keys);
		    instance = _providerRegistrationManager->getInstance
			(providerRef);
		    instances.append(instance);
	        }

	    }
        }
        else
        {
	    instances.append(_providerRegistrationManager->getInstance
	         (objectReference));
        }

        //
        // get provider manager service
        //
        MessageQueueService * _service = _getProviderManagerService();

	if (_service != NULL)
	{
	    // create CIMDisableModuleRequestMessage
	    CIMDisableModuleRequestMessage * disable_req =
	        new CIMDisableModuleRequestMessage(
		    XmlWriter::getNextMessageId (),
		    mInstance,
		    instances,
		    disableProviderOnly,
		    QueueIdStack(_service->getQueueId()));
// l10n
            disable_req->acceptLanguages = al;

  	    Array<Uint16> _opStatus =
	        _sendDisableMessageToProviderManager(disable_req);

	    if (!disableProviderOnly) // disable provider module
	    {
	        for (Uint32 i = 0; i<_opStatus.size(); i++)
	        {
		    // module was disabled successfully
	            if (_opStatus[i] == _MODULE_STOPPED)
	            {
	 	        // send termination message to subscription service
		        _sendTerminationMessageToSubscription(objectReference,
				moduleName, false, al);
		        return (0);
	            }

		    // module is not disabled since there are pending
                    // requests for the providers in the module
                    if (_opStatus[i] == _MODULE_OK)
                    {
                        return (-2);
                    }
	        }
	    }
	    else // disable provider
	    {
	        _sendTerminationMessageToSubscription(objectReference,
			moduleName, true, al);
	        return (0);
	    }
  	}

        // disable failed
	return (-1);
}

PEGASUS_NAMESPACE_END
