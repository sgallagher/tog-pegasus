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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Sushma Fernandes, Hewlett-Packard Company 
//                                (sushma_fernandes@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//  ConfigSetting Provider
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <cctype>
#include <iostream>

#include "ConfigSettingProvider.h"
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>
#include <Pegasus/Provider/OperationFlag.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The constants representing the string literals.
*/
static const char PROPERTY_NAME []      = "PropertyName";

static const char DEFAULT_VALUE []      = "DefaultValue";

static const char CURRENT_VALUE []      = "CurrentValue";

static const char PLANNED_VALUE []      = "PlannedValue";

static const char DYNAMIC_PROPERTY []   = "DynamicProperty";

/**
    The constant represeting the config setting class name
*/
static const char PG_CONFIG_SETTING [] = "PG_ConfigSetting";


void ConfigSettingProvider::getInstance(
	const OperationContext & context,
        const CIMObjectPath& instanceName,
	const Uint32 flags,
        const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG, "ConfigSettingProvider::getInstance()");

        Array<String>     propertyInfo;
        KeyBinding        kb;
        String            keyName;
        String            keyValue;

        //
        // check if the class name requested is PG_ConfigSetting
        //
        if (!String::equalNoCase(PG_CONFIG_SETTING, instanceName.getClassName()))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                                        instanceName.getClassName());
        }

        //
        // validate key bindings
        //
        Array<KeyBinding> kbArray = instanceName.getKeyBindings();
        if ( (kbArray.size() != 1) ||
             (!String::equalNoCase(kbArray[0].getName(), PROPERTY_NAME)) )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER,
                "Invalid instance name");
        }

        keyValue.assign(kbArray[0].getValue());

        // begin processing the request
        handler.processing();

        //
        // Get values for the property
        //
        try
        {
            _configManager->getPropertyInfo(keyValue, propertyInfo);
        }
        catch (UnrecognizedConfigProperty& ucp)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_FOUND,
                String("Configuration property \"") + keyValue + "\"");
        }

        if (propertyInfo.size() >= 5)
        {
            CIMInstance instance(PG_CONFIG_SETTING);

            //
            // construct the instance
            //
            instance.addProperty(CIMProperty(PROPERTY_NAME, propertyInfo[0]));
            instance.addProperty(CIMProperty(DEFAULT_VALUE, propertyInfo[1]));
            instance.addProperty(CIMProperty(CURRENT_VALUE, propertyInfo[2]));
            instance.addProperty(CIMProperty(PLANNED_VALUE, propertyInfo[3]));
            instance.addProperty(CIMProperty(DYNAMIC_PROPERTY,
                Boolean(propertyInfo[4]=="true"?true:false)));

            handler.deliver(instance);

            // complete processing the request
            handler.complete();

            PEG_METHOD_EXIT();
            return ;
        }
    }

void ConfigSettingProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
        const CIMInstance& modifiedIns,
	const Uint32 flags,
        const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG, "ConfigSettingProvider::modifyInstance()");

        //
        // get userName 
        //
        String userName;
        try
        {
            IdentityContainer container = context.get(CONTEXT_IDENTITY);
            userName = container.getUserName();
        }
        catch (...)
        {
            userName = String::EMPTY;
        }

        //
        // verify user authorizations
        //
        if ( userName != String::EMPTY || userName != "" )
        {
            _verifyAuthorization(userName);
        }

        // NOTE: Qualifiers are not processed by this provider, so the
        // IncludeQualifiers flag is ignored.

        //
        // check if the class name requested is PG_ConfigSetting
        //
        if (!String::equalNoCase(PG_CONFIG_SETTING,
                                 instanceReference.getClassName()))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                                        instanceReference.getClassName());
        }

        //
        // validate key bindings
        //
        Array<KeyBinding> kbArray = instanceReference.getKeyBindings();
        if ( (kbArray.size() != 1) ||
             (!String::equalNoCase(kbArray[0].getName(), PROPERTY_NAME)) )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER,
                "Invalid instance name");
        }

        String configPropertyName = kbArray[0].getValue();

        // Modification of the entire instance is not supported by this provider
        if (propertyList.isNull())
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                                        "Modification of entire instance");
        }

        Boolean currentValueModified = false;
        Boolean plannedValueModified = false;

        for (Uint32 i=0; i<propertyList.getNumProperties(); i++)
        {
            String propertyName = propertyList.getPropertyName(i);
            if (String::equalNoCase(propertyName, CURRENT_VALUE))
            {
                currentValueModified = true;
            }
            else if (String::equalNoCase(propertyName, PLANNED_VALUE))
            {
                plannedValueModified = true;
            }
            else
            {
                PEG_METHOD_EXIT();
                throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                    String("Modification of property \"") + propertyName + "\"");
            }
        }

        String currentValue = String::EMPTY;
        String plannedValue = String::EMPTY;
        Boolean currentValueIsNull = false;
        Boolean plannedValueIsNull = false;

	// begin processing the request
	handler.processing();

        //
        // Get the current value from the instance
        //
        Uint32 pos = modifiedIns.findProperty(CURRENT_VALUE);
        if (pos == PEG_NOT_FOUND)
        {
            currentValueIsNull = true;
        }
        else
        {
            CIMConstProperty prop = modifiedIns.getProperty(pos);
            try
            {
                prop.getValue().get(currentValue);
            }
            catch (Exception& e)
            {
                PEG_METHOD_EXIT();
                throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
            }
        }

        //
        // Get the planned value from the instance
        //
        pos = modifiedIns.findProperty(PLANNED_VALUE);
        if (pos == PEG_NOT_FOUND)
        {
            plannedValueIsNull = true;
        }
        else
        {
            CIMConstProperty prop = modifiedIns.getProperty(pos);
            try
            {
                prop.getValue().get(plannedValue);
            }
            catch (Exception& e)
            {
                PEG_METHOD_EXIT();
                throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
            }
        }

        try
        {
            //
            // Update the current value, if requested
            //
            if (currentValueModified)
            {
                if ( !_configManager->updateCurrentValue(
                    configPropertyName, currentValue, currentValueIsNull) )
                {
                    handler.complete();
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_FAILED,
                        "Failed to update the current value.");
                }
            }

            //
            // Update the planned value, if requested
            //
            if (plannedValueModified)
            {
                if ( !_configManager->updatePlannedValue(
                    configPropertyName, plannedValue, plannedValueIsNull) )
                {
		    handler.complete();
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_FAILED,
                        "Failed to update the planned value.");
                }
            }
        }
        catch (NonDynamicConfigProperty& ndcp)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED, ndcp.getMessage());
        }
        catch (InvalidPropertyValue& ipv)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED, ipv.getMessage());
        }
        catch (UnrecognizedConfigProperty& ucp)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_FOUND,
                String("Configuration property \"") +
                    configPropertyName + "\"");
        }

        handler.complete();
        PEG_METHOD_EXIT();
        return;
    }

void ConfigSettingProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & ref,
	const Uint32 flags,
        const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG, "ConfigSettingProvider::enumerateInstances()");

        Array<CIMInstance> instanceArray;
        Array<String> propertyNames;

        //
        // check if the class name requested is PG_ConfigSetting
        //
        if (!String::equalNoCase(PG_CONFIG_SETTING, ref.getClassName()))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                                        ref.getClassName());
        }

	// begin processing the request
	handler.processing();

        try
        {
            _configManager->getAllPropertyNames(propertyNames);

            for (Uint32 i = 0; i < propertyNames.size(); i++)
            {
                Array<String> propertyInfo;

                CIMInstance        instance(PG_CONFIG_SETTING);

                propertyInfo.clear();

                _configManager->getPropertyInfo(propertyNames[i], propertyInfo);

                Array<KeyBinding> keyBindings;
                keyBindings.append(KeyBinding(PROPERTY_NAME, propertyInfo[0],
                    KeyBinding::STRING));
                CIMObjectPath instanceName(ref.getHost(), ref.getNameSpace(),
                    PG_CONFIG_SETTING, keyBindings);

                // construct the instance
                instance.addProperty(CIMProperty(PROPERTY_NAME, propertyInfo[0]));
                instance.addProperty(CIMProperty(DEFAULT_VALUE, propertyInfo[1]));
                instance.addProperty(CIMProperty(CURRENT_VALUE, propertyInfo[2]));
                instance.addProperty(CIMProperty(PLANNED_VALUE, propertyInfo[3]));
                instance.addProperty(CIMProperty(DYNAMIC_PROPERTY,
                    Boolean(propertyInfo[4]=="true"?true:false)));

                instance.setPath(instanceName);
                instanceArray.append(instance);
            }
        }
        catch(Exception& e)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }

	handler.deliver(instanceArray);

	// complete processing the request
	handler.complete();

        PEG_METHOD_EXIT();
    }

void ConfigSettingProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
        ResponseHandler<CIMObjectPath> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG,
            "ConfigSettingProvider::enumerateInstanceNames()");

        Array<CIMObjectPath> instanceRefs;
        Array<String>       propertyNames;
        Array<KeyBinding>   keyBindings;
        KeyBinding          kb;
        String              hostName;

        hostName.assign(System::getHostName());

	const String& className = classReference.getClassName();
	const String& nameSpace = classReference.getNameSpace();

        if (!String::equalNoCase(PG_CONFIG_SETTING, className))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION( CIM_ERR_NOT_SUPPORTED, className );
        }

	// begin processing the request
	handler.processing();

        try
        {
            _configManager->getAllPropertyNames(propertyNames);

            Uint32 size = propertyNames.size();

            for (Uint32 i = 0; i < size; i++)
            {
                keyBindings.append(KeyBinding(PROPERTY_NAME, propertyNames[i],
                    KeyBinding::STRING));

                //
                // Convert instance names to References
                //
                CIMObjectPath ref(hostName, nameSpace, className, keyBindings);

                instanceRefs.append(ref);
                keyBindings.clear();
            }
        }
        catch(Exception& e)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }

	handler.deliver(instanceRefs);

	// complete processing the request
	handler.complete();

        PEG_METHOD_EXIT();
    }

//
// Verify user authorization
//
void ConfigSettingProvider::_verifyAuthorization(const String& userName)
    {
        PEG_METHOD_ENTER(TRC_CONFIG,
            "ConfigSettingProvider::_verifyAuthorization()");

        if ( System::isPrivilegedUser(userName) == false )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, 
                "Must be a privileged user to do this CIM operation.");
        }

        PEG_METHOD_EXIT();
    }

PEGASUS_NAMESPACE_END

