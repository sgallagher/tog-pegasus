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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Sushma Fernandes, Hewlett-Packard Company
//                                (sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
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
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/ModuleController.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/AutoPtr.h>


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The constants representing the string literals.
*/
static const CIMName PROPERTY_NAME       = CIMName ("PropertyName");

static const CIMName DEFAULT_VALUE       = CIMName ("DefaultValue");

static const CIMName CURRENT_VALUE       = CIMName ("CurrentValue");

static const CIMName PLANNED_VALUE       = CIMName ("PlannedValue");

static const CIMName DYNAMIC_PROPERTY    = CIMName ("DynamicProperty");

/**
    The constant representing the config setting class name
*/
static const CIMName PG_CONFIG_SETTING  = CIMName ("PG_ConfigSetting");

void ConfigSettingProvider::getInstance(
	const OperationContext & context,
        const CIMObjectPath& instanceName,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList,
	InstanceResponseHandler & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG, "ConfigSettingProvider::getInstance()");

        Array<String>     propertyInfo;
        CIMKeyBinding        kb;
        String            keyName;
        String            keyValue;

        //
        // check if the class name requested is PG_ConfigSetting
        //
        if (!instanceName.getClassName().equal (PG_CONFIG_SETTING))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                instanceName.getClassName().getString());
        }

        //
        // validate key bindings
        //
        Array<CIMKeyBinding> kbArray = instanceName.getKeyBindings();
        if ( (kbArray.size() != 1) ||
             (!kbArray[0].getName().equal (PROPERTY_NAME)))
        {
            PEG_METHOD_EXIT();
            //l10n
            //throw PEGASUS_CIM_EXCEPTION(
                //CIM_ERR_INVALID_PARAMETER,
                //"Invalid instance name");
            throw PEGASUS_CIM_EXCEPTION_L(
                CIM_ERR_INVALID_PARAMETER,
                MessageLoaderParms("ControlProviders.ConfigSettingProvider.ConfigSettingProvider.INVALID_INSTANCE_NAME",
                				   "Invalid instance name"));
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
        catch (const UnrecognizedConfigProperty&)
        {
            PEG_METHOD_EXIT();
            //l10n
            //throw PEGASUS_CIM_EXCEPTION(
                //CIM_ERR_NOT_FOUND,
                //String("Configuration property \"") + keyValue + "\"");
            throw PEGASUS_CIM_EXCEPTION_L(
                CIM_ERR_NOT_FOUND,
                MessageLoaderParms("ControlProviders.ConfigSettingProvider.ConfigSettingProvider.CONFIG_PROPERTY_NOT_FOUND",
                			       "Configuration property \"$0\"", 
                			       keyValue));
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
	const Boolean includeQualifiers,
        const CIMPropertyList& propertyList,
	ResponseHandler & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG, "ConfigSettingProvider::modifyInstance()");

        //
        // get userName
        //
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
        if (!instanceReference.getClassName().equal (PG_CONFIG_SETTING))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                instanceReference.getClassName().getString());
        }

        //
        // validate key bindings
        //
        Array<CIMKeyBinding> kbArray = instanceReference.getKeyBindings();
        if ( (kbArray.size() != 1) ||
             (!kbArray[0].getName().equal (PROPERTY_NAME)))
        {
            PEG_METHOD_EXIT();
            //l10n
            //throw PEGASUS_CIM_EXCEPTION(
                //CIM_ERR_INVALID_PARAMETER,
                //"Invalid instance name");
            throw PEGASUS_CIM_EXCEPTION_L(
                CIM_ERR_INVALID_PARAMETER,
                MessageLoaderParms("ControlProviders.ConfigSettingProvider.ConfigSettingProvider.INVALID_INSTANCE_NAME",
                				   "Invalid instance name"));
                
        }

        String configPropertyName = kbArray[0].getValue();

        // Modification of the entire instance is not supported by this provider
        if (propertyList.isNull())
        {
            PEG_METHOD_EXIT();
            //l10n
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                                        //"Modification of entire instance");
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,
                  MessageLoaderParms("ControlProviders.ConfigSettingProvider.ConfigSettingProvider.MODIFICATION_OF_ENTIRE_INSTANCE",
                                     "Modification of entire instance"));
        }

        Boolean currentValueModified = false;
        Boolean plannedValueModified = false;

        for (Uint32 i=0; i<propertyList.size(); i++)
        {
            CIMName propertyName = propertyList[i];
            if (propertyName.equal (CURRENT_VALUE))
            {
                currentValueModified = true;
            }
            else if (propertyName.equal (PLANNED_VALUE))
            {
                plannedValueModified = true;
            }
            else
            {
                PEG_METHOD_EXIT();
                //l10n
                //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                    //String("Modification of property \"") + 
                        //propertyName.getString() + "\"");
                throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,
                		MessageLoaderParms("ControlProviders.ConfigSettingProvider.ConfigSettingProvider.MODIFICATION_NOT_SUPPORTED",
                    	"Modification of property \"$0\"", 
                        propertyName.getString()));
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
                    //l10n
                    //throw PEGASUS_CIM_EXCEPTION(
                        //CIM_ERR_FAILED,
                        //"Failed to update the current value.");
                    throw PEGASUS_CIM_EXCEPTION_L(
                        CIM_ERR_FAILED,
                        MessageLoaderParms("ControlProviders.ConfigSettingProvider.ConfigSettingProvider.UPDATE_CURRENT_VALUE_FAILED",
                        				   "Failed to update the current value."));
                }
		
		// It is unset, get current value which is default
                if (currentValueIsNull)
                {
                    currentValue = _configManager->getCurrentValue(
                        configPropertyName);
                }

                // send notify config change message to ProviderManager Service
                _sendNotifyConfigChangeMessage(configPropertyName, 
                                               currentValue,
                                               true);
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
                    //l10n
                    //throw PEGASUS_CIM_EXCEPTION(
                        //CIM_ERR_FAILED,
                        //"Failed to update the planned value.");
                    throw PEGASUS_CIM_EXCEPTION_L(
                        CIM_ERR_FAILED,
                        MessageLoaderParms("ControlProviders.ConfigSettingProvider.ConfigSettingProvider.UPDATE_PLANNED_VALUE_FAILED",
                        				   "Failed to update the planned value."));
                }

		// It is unset, get planned value which is default
                if (plannedValueIsNull)
                {
                    plannedValue = _configManager->getPlannedValue(
                        configPropertyName);
                }

                // send notify config change message to ProviderManager Service
                _sendNotifyConfigChangeMessage(configPropertyName, 
                                               plannedValue,
                                               false);
            }
        }
        catch (const NonDynamicConfigProperty& ndcp)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED, ndcp.getMessage());
        }
        catch (const InvalidPropertyValue& ipv)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED, ipv.getMessage());
        }
        catch (const UnrecognizedConfigProperty&)
        {
            PEG_METHOD_EXIT();
            //l10n
            //throw PEGASUS_CIM_EXCEPTION(
                //CIM_ERR_NOT_FOUND,
                //String("Configuration property \"") +
                    //configPropertyName + "\"");
            throw PEGASUS_CIM_EXCEPTION_L(
                CIM_ERR_NOT_FOUND,
                MessageLoaderParms("ControlProviders.ConfigSettingProvider.ConfigSettingProvider.CONFIG_PROPERTY_NOT_FOUND",
                			       "Configuration property \"$0\"", 
                			       configPropertyName));
        }

        handler.complete();
        PEG_METHOD_EXIT();
        return;
    }

void ConfigSettingProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & ref,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList,
	InstanceResponseHandler & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG, "ConfigSettingProvider::enumerateInstances()");

        Array<CIMInstance> instanceArray;
        Array<String> propertyNames;

        //
        // check if the class name requested is PG_ConfigSetting
        //
        if (!ref.getClassName().equal (PG_CONFIG_SETTING))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                                        ref.getClassName().getString());
        }

	// begin processing the request
	handler.processing();

        try
        {
            _configManager->getAllPropertyNames(propertyNames, false);


            for (Uint32 i = 0; i < propertyNames.size(); i++)
            {
                 Array<String> propertyInfo;

                 CIMInstance        instance(PG_CONFIG_SETTING);

                 propertyInfo.clear();

                 _configManager->getPropertyInfo(
                 propertyNames[i], propertyInfo);

                 Array<CIMKeyBinding> keyBindings;
                 keyBindings.append(CIMKeyBinding(PROPERTY_NAME, 
                 propertyInfo[0], CIMKeyBinding::STRING));
                 CIMObjectPath instanceName(ref.getHost(), 
                 ref.getNameSpace(),
                 PG_CONFIG_SETTING, keyBindings);

                 // construct the instance
                 instance.addProperty(CIMProperty(PROPERTY_NAME, 
                                         propertyInfo[0]));
                 instance.addProperty(CIMProperty(DEFAULT_VALUE, 
                                         propertyInfo[1]));
                 instance.addProperty(CIMProperty(CURRENT_VALUE, 
                                         propertyInfo[2]));
                 instance.addProperty(CIMProperty(PLANNED_VALUE, 
                                         propertyInfo[3]));
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
        ObjectPathResponseHandler & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG,
            "ConfigSettingProvider::enumerateInstanceNames()");

        Array<CIMObjectPath> instanceRefs;
        Array<String>       propertyNames;
        Array<CIMKeyBinding>   keyBindings;
        CIMKeyBinding          kb;
        String              hostName;

        hostName.assign(System::getHostName());

	const CIMName& className = classReference.getClassName();
	const CIMNamespaceName& nameSpace = classReference.getNameSpace();

        if (!className.equal (PG_CONFIG_SETTING))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION( CIM_ERR_NOT_SUPPORTED, 
                className.getString());
        }

	// begin processing the request
	handler.processing();

        try
        {
            _configManager->getAllPropertyNames(propertyNames, false);

            Uint32 size = propertyNames.size();

            for (Uint32 i = 0; i < size; i++)
            {
                 keyBindings.append(
                              CIMKeyBinding(PROPERTY_NAME, propertyNames[i],
                              CIMKeyBinding::STRING));

                 //
                 // Convert instance names to References
                 //
                 CIMObjectPath ref(hostName, nameSpace, className, 
                                    keyBindings);

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
            //l10n
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
                //"Must be a privileged user to do this CIM operation.");
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED,
                MessageLoaderParms("ControlProviders.ConfigSettingProvider.ConfigSettingProvider.USER_NOT_PRIVILEGED",
                				   "Must be a privileged user to do this CIM operation."));
        }

        PEG_METHOD_EXIT();
    }

//
// send notify config change message to provider manager service
//
void ConfigSettingProvider::_sendNotifyConfigChangeMessage(
    const String& propertyName,
    const String& newPropertyValue,
    Boolean currentValueModified)
{
    PEG_METHOD_ENTER(TRC_CONFIG,
        "ConfigSettingProvider::_sendNotifyConfigChangeMessage");

    pegasus_internal_identity _id = peg_credential_types::PROVIDER;
    ModuleController * _controller;
    ModuleController::client_handle *_client_handle;

    _controller = &(ModuleController::get_client_handle(_id, &_client_handle));
    if(_client_handle == NULL)
    {
        PEG_METHOD_EXIT();
        throw UninitializedObjectException();      
    }

    MessageQueue * queue = MessageQueue::lookup(
        PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);

    MessageQueueService * service = dynamic_cast<MessageQueueService *>(queue);

    if (service != NULL)
    {
        // create CIMNotifyConfigChangeRequestMessage
        CIMNotifyConfigChangeRequestMessage * notify_req =
            new CIMNotifyConfigChangeRequestMessage (
            XmlWriter::getNextMessageId (),
            propertyName,
            newPropertyValue,
            currentValueModified,
            QueueIdStack(service->getQueueId()));

        // create request envelope
        AsyncLegacyOperationStart asyncRequest(
            service->get_next_xid(),
            NULL,
            service->getQueueId(),
            notify_req,
            service->getQueueId());

        AutoPtr<AsyncReply> asyncReply( 
            _controller->ClientSendWait(*_client_handle,
            service->getQueueId(),
            &asyncRequest));

        AutoPtr<CIMNotifyConfigChangeResponseMessage> response(
            reinterpret_cast<CIMNotifyConfigChangeResponseMessage *>(
            (static_cast<AsyncLegacyOperationResult *>
            (asyncReply.get()))->get_result()));

        if (response->cimException.getCode() != CIM_ERR_SUCCESS)
        {
            CIMException e = response->cimException;
            throw (e);
	}
    }
}

PEGASUS_NAMESPACE_END

