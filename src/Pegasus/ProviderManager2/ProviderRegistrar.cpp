//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/ProviderManager2/ProviderType.h>

PEGASUS_NAMESPACE_BEGIN

static ProviderRegistrationManager * _prm = 0;

static CIMValue _getPropertyValue(const CIMInstance & cimInstance, const String & propertyName)
{
    CIMValue value;

    Uint32 pos = 0;

    // get ClassName property
    if((pos = cimInstance.findProperty(propertyName)) != PEG_NOT_FOUND)
    {
        value = cimInstance.getProperty(pos).getValue();
    }

    return(value);
}

static ProviderName _lookupProvider(const CIMObjectPath & cimObjectPath)
{
    String providerName;
    String moduleName;

    try
    {
        // get the PG_ProviderCapabilities instances for the specified namespace:class_name. use the matching
        // instance to gather the PG_Provider instance name (logical name).

        Array<CIMObjectPath> cimInstanceNames = _prm->enumerateInstanceNames(CIMObjectPath(String::EMPTY, "root/PG_Interop", "PG_ProviderCapabilities"));

        for(Uint32 i = 0, n = cimInstanceNames.size(); i < n; i++)
        {
            CIMInstance cimInstance = _prm->getInstance(cimInstanceNames[i]);

            // check ClassName property value
            if(String::equalNoCase(cimObjectPath.getClassName().getString(), _getPropertyValue(cimInstance, "ClassName").toString()))
            {
                // check Namespaces property value
                Array<String> nameSpaces;

                _getPropertyValue(cimInstance, "Namespaces").get(nameSpaces);

                // ATTN: need to walk the array
                if(String::equalNoCase(cimObjectPath.getNameSpace().getString(), nameSpaces[0]))
                {
                    providerName = _getPropertyValue(cimInstance, "ProviderName").toString();

                    break;
                }
            }
        }
    }
    catch(...)
    {
    }

    // ensure the provider name is valid
    if(providerName.size() == 0)
    {
        throw Exception("Could not determine PG_Provider instance for specified class.");
    }

    try
    {
        // get the PG_Provider instances associated with the specified namespace:class_name. use the matching
        // instance to gather the PG_ProviderModule instance name.

        Array<CIMObjectPath> cimInstanceNames = _prm->enumerateInstanceNames(CIMObjectPath(String::EMPTY, "root/PG_Interop", "PG_Provider"));

        for(Uint32 i = 0, n = cimInstanceNames.size(); i < n; i++)
        {
            CIMInstance cimInstance = _prm->getInstance(cimInstanceNames[i]);

            if(String::equalNoCase(providerName, _getPropertyValue(cimInstance, "Name").toString()))
            {
                moduleName = _getPropertyValue(cimInstance, "ProviderModuleName").toString();

                break;
            }
        }
    }
    catch(...)
    {
    }

    // ensure the module name is valid
    if(moduleName.size() == 0)
    {
        throw Exception("Could not determine PG_ProviderModule instance for specified class.");
    }

    String interfaceType;
    String moduleLocation;

    try
    {
        // get the PG_ProviderModule instances associated with the specified namespace:class_name. use the matching
        // instance to gather the module status and location (physical name).

        Array<CIMObjectPath> cimInstanceNames = _prm->enumerateInstanceNames(CIMObjectPath(String::EMPTY, "root/PG_Interop", "PG_ProviderModule"));

        for(Uint32 i = 0, n = cimInstanceNames.size(); i < n; i++)
        {
            CIMInstance cimInstance = _prm->getInstance(cimInstanceNames[i]);

            if(String::equalNoCase(moduleName, _getPropertyValue(cimInstance, "Name").toString()))
            {
                // ATTN: check operational status

                // get interface
                interfaceType = _getPropertyValue(cimInstance, "InterfaceType").toString();

                // get location
                moduleLocation = _getPropertyValue(cimInstance, "Location").toString();

                break;
            }
        }
    }
    catch(...)
    {
    }

    // ensure the interface and location are valid
    if((interfaceType.size() == 0) || (moduleLocation.size() == 0))
    {
        throw Exception("Could not determine PG_ProviderModule.InterfaceType or PG_ProviderModule.Location or module is disabled.");
    }

    // DEBUG
    CString s1 = interfaceType.getCString();
    const char * p1 = s1;

    CString s2 = moduleLocation.getCString();
    const char * p2 = s2;

    ProviderName temp(
        cimObjectPath.toString(),
        providerName,
        moduleLocation,
        interfaceType,
        0);

    return(temp);
}

ProviderRegistrar::ProviderRegistrar(void)
{
}

ProviderRegistrar::~ProviderRegistrar(void)
{
}

// need at least the object and the one capability.
// for example,
//  "//localhost/root/cimv2:CIM_ComputerSystem", INSTANCE
//  "//localhost/root/cimv2:CIM_ComputerSystem", METHOD
//  "//localhost/root/cimv2:CIM_AssociatedComputerSystem", ASSOCIATION
//  "//localhost/root/cimv2:CIM_ComputerSystemFailure", INDICATION

// the method will return the logical and physical provider names associated with the object and capability.

ProviderName ProviderRegistrar::findProvider(const ProviderName & providerName)
{
    CIMObjectPath objectName = providerName.getObjectName();
    Uint32 flags = providerName.getCapabilitiesMask();

    // validate arguments
    if(objectName.getNameSpace().isNull() || objectName.getClassName().isNull())
    {
        throw Exception("Invalid argument.");
    }
    
    CIMInstance provider;
    CIMInstance providerModule;
    ProviderName temp;
   
   switch (flags) {
       case 2: //ProviderType::INSTANCE
          if (_prm->lookupInstanceProvider(objectName.getNameSpace(),objectName.getClassName(),
                provider,providerModule,0)) {
	      return ProviderName(providerName.getObjectName(),
	            provider.getProperty(providerModule.findProperty
                       ("Name")).getValue ().toString (),
		    providerModule.getProperty(providerModule.findProperty
                       ("Location")).getValue().toString(),
	            providerModule.getProperty(providerModule.findProperty
                       ("InterfaceType")).getValue().toString(),
		    ProviderType::INSTANCE);
          }
          break;
       case 5: //ProviderType::ASSOCIATION
          if (_prm->lookupInstanceProvider(objectName.getNameSpace(),objectName.getClassName(),
                provider,providerModule,1)) {
	      return ProviderName(providerName.getObjectName(),
	            provider.getProperty(providerModule.findProperty
                       ("Name")).getValue ().toString (),
		    providerModule.getProperty(providerModule.findProperty
                       ("Location")).getValue().toString(),
	            providerModule.getProperty(providerModule.findProperty
                       ("InterfaceType")).getValue().toString(),
		    ProviderType::ASSOCIATION);
          }
          break;
       default:
          temp = _lookupProvider(objectName);
    }
    
    return(temp);
}

Boolean ProviderRegistrar::insertProvider(const ProviderName & providerName)
{
    // assume the providerName is in ProviderName format
    ProviderName name(providerName);

    return(false);
}

Boolean ProviderRegistrar::removeProvider(const ProviderName & providerName)
{
    // assume the providerName is in ProviderName format
    ProviderName name(providerName);

    return(false);
}

void SetProviderRegistrationManager(ProviderRegistrationManager * p)
{
    _prm = p;
}

ProviderRegistrationManager * GetProviderRegistrationManager(void)
{
    return(_prm);
}

PEGASUS_NAMESPACE_END
