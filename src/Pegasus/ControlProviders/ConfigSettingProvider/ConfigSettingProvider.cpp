//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
//=============================================================================
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

#include <cctype>
#include <iostream>

#include "ConfigSettingProvider.h"
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMReference.h>
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
        const CIMReference& instanceName,
	const Uint32 flags,
        const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG, "ConfigSettingProvider::getInstance()");

        Array<String>     propertyInfo;
        KeyBinding        kb;
        String            keyName;
        String            keyValue;


        Array<KeyBinding> kbArray = instanceName.getKeyBindings();

        // validate key bindings
        if ( kbArray.size() != 1)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                "No key was specified");
        }

        // begin processing the request
        handler.processing();

        keyName.assign(kbArray[0].getName());
        keyValue.assign(kbArray[0].getValue());

        if(String::equal(keyName, PROPERTY_NAME))
        {
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

        //
        // failed to get the instance
        //
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED,
            "Invalid key name was specified");

    }

void ConfigSettingProvider::modifyInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
        const CIMInstance& modifiedIns,
	const Uint32 flags,
        const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG, "ConfigSettingProvider::modifyInstance()");

        // ATTN: Partial modification is not yet supported by this provider
        // Qualifiers are not processed anyway, so the IncludeQualifiers
        // flag is ignored
        if (!propertyList.isNull())
        {
            PEG_METHOD_EXIT();
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }

        String propertyName = String::EMPTY;
        String defaultValue = String::EMPTY;
        String currentValue = String::EMPTY;
        String plannedValue = String::EMPTY;
        Boolean dynamicProperty;

        String myCurrentValue = String::EMPTY;
        String myPlannedValue = String::EMPTY;
        Boolean currentValueModified = false;
        Boolean plannedValueModified = false;

        CIMInstance modifiedInstance = modifiedIns;

	// begin processing the request
	handler.processing();

        // ATTN: Add error handling to check if the properties exist
        // ATTN: Check for TypeMismatch exceptions from CIMValue.get()
        Uint32 pos = modifiedInstance.findProperty(PROPERTY_NAME);
        CIMProperty prop = (CIMProperty)modifiedInstance.getProperty(pos);
        prop.getValue().get(propertyName);

        pos = modifiedInstance.findProperty(DEFAULT_VALUE);
        prop = (CIMProperty)modifiedInstance.getProperty(pos);
        prop.getValue().get(defaultValue);

        pos = modifiedInstance.findProperty(CURRENT_VALUE);
        prop = (CIMProperty)modifiedInstance.getProperty(pos);
        prop.getValue().get(currentValue);

        pos = modifiedInstance.findProperty(PLANNED_VALUE);
        prop = (CIMProperty)modifiedInstance.getProperty(pos);
        prop.getValue().get(plannedValue);

        pos = modifiedInstance.findProperty(DYNAMIC_PROPERTY);
        prop = (CIMProperty)modifiedInstance.getProperty(pos);
        prop.getValue().get(dynamicProperty);

        try
        {
            //
            // check what properties are modified, validate the new values
            // and then update the new values with the Config Manager.
            //
            myCurrentValue = _configManager->getCurrentValue(propertyName);
            myPlannedValue = _configManager->getPlannedValue(propertyName);

            //
            // check whether current value is modified or not
            //
         //   if ( !String::equal(currentValue, myCurrentValue) )
            {
                currentValueModified = true;

                //
                // If the current value is empty, then it must be an unset
                // request, so get the default value.
                //
                if ( String::equal(currentValue, String::EMPTY) )
                {
                    currentValue = _configManager->getDefaultValue(propertyName);
                }
                //
                // Otherwise validate the new current value.
                //
                else if ( !_configManager->validatePropertyValue(
                    propertyName, currentValue) )
                {
                    //
                    // Invalid current value
                    //
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_TYPE_MISMATCH,
                        "Specified current value is not valid");
                }
            }
            //
            // check whether planned value is modified or not
            //
          //  if ( !String::equal(plannedValue, myPlannedValue) )
            {
                plannedValueModified = true;

                //
                // If the planned value is empty, then it must be an unset
                // request, so do nothing.
                //
                if ( String::equal(plannedValue, String::EMPTY) )
                {
		    ;
                }
                //
                // Otherwise validate the new planned value.
                //
                else if ( !_configManager->validatePropertyValue(
                    propertyName, plannedValue) )
                {
                    //
                    // Invalid planned value
                    //
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_TYPE_MISMATCH,
                        "Specified planned value is not valid");
                }
            }
        }
        catch (UnrecognizedConfigProperty& ucp)
        {
            //
            // Invalid property name was specified
            //
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_FOUND,
                String("Configuration property \"") + propertyName + "\"");
        }

        try
        {
            Boolean retValue = false;

            if (currentValueModified)
            {
                retValue = _configManager->updateCurrentValue(
                    propertyName, currentValue);

                if (retValue == false)
                {
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_FAILED,
                        "Failed to update the current value.");
                }
            }

            if (plannedValueModified)
            {
                retValue = _configManager->updatePlannedValue(
                    propertyName, plannedValue);

                if (retValue == false)
                {
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_FAILED,
                        "Failed to update the planned value.");
                }
            }

            //
            // Modifications done, so return.
            //
            if (currentValueModified || plannedValueModified)
            {
		handler.complete();
                PEG_METHOD_EXIT();
                return;
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
                CIM_ERR_TYPE_MISMATCH, ipv.getMessage());
        }
        catch (UnrecognizedConfigProperty& ucp)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_FOUND,
                String("Configuration property \"") + propertyName + "\"");
        }


        //
        // Should not get here if the modifications were done.
        // This must be an attempt to modifiy the property by
        // spcecifying its existing value.
        //
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_ALREADY_EXISTS,
            "Specified property is not modified.");
    }

void ConfigSettingProvider::enumerateInstances(
	const OperationContext & context,
	const CIMReference & ref,
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
        if (!String::equal(PG_CONFIG_SETTING, ref.getClassName()))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, ref.getClassName());
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
                CIMReference instanceName(ref.getHost(), ref.getNameSpace(),
                    PG_CONFIG_SETTING, keyBindings);

                // construct the instance
                instance.addProperty(CIMProperty(PROPERTY_NAME, propertyInfo[0]));
                instance.addProperty(CIMProperty(DEFAULT_VALUE, propertyInfo[1]));
                instance.addProperty(CIMProperty(CURRENT_VALUE, propertyInfo[2]));
                instance.addProperty(CIMProperty(PLANNED_VALUE, propertyInfo[3]));
                instance.addProperty(CIMProperty(DYNAMIC_PROPERTY,
                    Boolean(propertyInfo[4]=="true"?true:false)));

                //namedInstanceArray.append(
                //    CIMNamedInstance(instanceName, instance));
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
	const CIMReference & classReference,
        ResponseHandler<CIMReference> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG,
            "ConfigSettingProvider::enumerateInstanceNames()");

        Array<CIMReference> instanceRefs;
        Array<String>       propertyNames;
        Array<KeyBinding>   keyBindings;
        KeyBinding          kb;
        String              hostName;

        hostName.assign(System::getHostName());

	const String& className = classReference.getClassName();
	const String& nameSpace = classReference.getNameSpace();

        if (!String::equal(PG_CONFIG_SETTING, className))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION( CIM_ERR_NOT_FOUND, className );
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
                CIMReference ref(hostName, nameSpace, className, keyBindings);

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

PEGASUS_NAMESPACE_END

