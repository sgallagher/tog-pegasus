//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 - 2003 BMC Software, Hewlett-Packard Company, IBM,
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderRegistrar.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/CIMObjectPath.h>

#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_NAMESPACE_BEGIN

ProviderRegistrationManager * _providerRegistrationManager = 0;

ProviderRegistrar::ProviderRegistrar(void)
{
}

ProviderRegistrar::~ProviderRegistrar(void)
{
}

// resolve a partial internal name into a fully qualified (as much as possible) internal
// provider name. for example, given a namespace and class name (embedded in the object
// name component), this method will determine the physical provider name, the logical
// provider name, and the provider capabilities for the specific object name
//
// given X, this method will provide O.
//
// physical_name    logical_name    object_name     capabilities
// =============================================================
//      X
//      O                X
//      O                O              X                O
//
String ProviderRegistrar::findProvider(const String & providerName)
{
    // assume the providerName is in InternalProviderName format
    InternalProviderName name(providerName);

    CIMObjectPath cimObjectPath(name.getObjectName());

    CIMInstance moduleInstance;
    CIMInstance providerInstance;

    _providerRegistrationManager->lookupInstanceProvider(
        cimObjectPath.getNameSpace(),
        cimObjectPath.getClassName(),
        providerInstance,
        moduleInstance);

    Uint32 pos = 0;

    // get the provider name from the provider instance
    if((pos = providerInstance.findProperty(CIMName("Name"))) == PEG_NOT_FOUND)
    {
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
                            "ProviderManager.ProviderManagerService.PROVIDER_LOOKUP_FAILED",
                            "provider lookup failed."));
    }

    String logicalName;

    providerInstance.getProperty(pos).getValue().get(logicalName);

    name.setLogicalName(logicalName);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "providerName = " + providerName + " found.");

    // get the provider location from the provider module instance
    if((pos = moduleInstance.findProperty(CIMName("Location"))) == PEG_NOT_FOUND)
    {
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
                            "ProviderManager.ProviderManagerService.PROVIDER_LOOKUP_FAILED",
                            "provider lookup failed."));
    }

    String location;

    moduleInstance.getProperty(pos).getValue().get(location);

    // get the provider location from the provider module instance
    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "location = " + location + " found.");

    String fileName;

    #ifdef PEGASUS_OS_TYPE_WINDOWS
    fileName = location + String(".dll");
    #elif defined(PEGASUS_OS_HPUX)
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        #ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
        fileName.append(String("/lib") + location + String(".sl"));
        #else
        fileName.append(String("/lib") + location + String(".so"));
        #endif
    #elif defined(PEGASUS_OS_OS400)
    fileName = location;
    #else
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    fileName.append(String("/lib") + location + String(".so"));
    #endif

    name.setPhysicalName(fileName);

    return(name.toString());

    /*
    ATTN: goal is to be able to look up a provider by object name alone. that is, no information
    should be needed other than the fully qualified object name because an object has an implicit
    type associated with it.

    ATTN: what happens if a provider has 2 registration instances (capabilities); 1 is for the
    instance, 1 is for the method.

    DynamicRoutingTable router = DynamicRoutingTable::get_route_table();

    get_route(className, nameSpace, 0, 0, provider, module);

    static const Uint32 INTERNAL; // "control providers"
    static const Uint32 INSTANCE;
    static const Uint32 CLASS;
    static const Uint32 METHOD;
    static const Uint32 ASSOCIATION;
    static const Uint32 QUERY;
    static const Uint32 INDICATION;
    static const Uint32 CONSUMER;
    */
}

Boolean ProviderRegistrar::insertProvider(const String & providerName)
{
    // assume the providerName is in InternalProviderName format
    InternalProviderName name(providerName);

    return(false);
}

Boolean ProviderRegistrar::removeProvider(const String & providerName)
{
    // assume the providerName is in InternalProviderName format
    InternalProviderName name(providerName);

    return(false);
}

// temp
void ProviderRegistrar::setProviderRegistrationManager(ProviderRegistrationManager * p)
{
    _providerRegistrationManager = p;
}

ProviderRegistrationManager * ProviderRegistrar::getProviderRegistrationManager(void)
{
    return(_providerRegistrationManager);
}

PEGASUS_NAMESPACE_END
