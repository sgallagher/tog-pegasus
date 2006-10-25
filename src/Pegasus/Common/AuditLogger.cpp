//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Formatter.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/InternalException.h>

#ifndef PEGASUS_OS_TYPE_WINDOWS
# include <unistd.h>
#endif

#include <stdlib.h>

#ifndef PEGASUS_DISABLE_AUDIT_LOGGER

#include <Pegasus/Common/AuditLogger.h>

extern char ** environ;

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static const String providerModuleStatus [] = {"Unknown", "Other", "OK", "Degraded",
    "Stressed", "Predictive Failure", "Error", "Non-Recoverable Error",
    "Starting", "Stopping", "Stopped", "In Service", "No Contact", 
    "Lost Communication"};

Boolean AuditLogger::_auditLogFlag = false;

AuditLogger::PEGASUS_AUDITLOGINITIALIZE_CALLBACK_T AuditLogger::_auditLogInitializeCallback;

AuditLogger::PEGASUS_AUDITLOG_CALLBACK_T AuditLogger::_writeAuditMessageToFile =
    AuditLogger::_writeAuditMessage;

void AuditLogger::logCurrentConfig(
    const Array<String> & propertyNames,
    const Array<String> & propertyValues)
{
    String properties;

    for (Uint32 i = 0; i < propertyNames.size(); i++)
    {
        properties.append(propertyNames[i]);
        properties.append("=");
        properties.append(propertyValues[i]);
        properties.append("\n");
    }

    MessageLoaderParms msgParms("Common.AuditLogger.CURRENT_CONFIG",
       "The current configuration properties are:\n$0", properties);

    _writeAuditMessageToFile(TYPE_CONFIGURATION, 
        SUBTYPE_CURRENT_CONFIGURATION,
        EVENT_START_UP, Logger::INFORMATION, msgParms); 
    
}

void AuditLogger::logCurrentRegProvider(
    const Array < CIMInstance > & instances)
{
    String moduleName, registeredModules;
    Array<Uint16> moduleStatus;
    String statusValue;
    Uint32 pos;

    // get all the registered provider module names and status
    for (Uint32 i = 0; i <instances.size(); i++)
    {
        instances[i].getProperty(instances[i].findProperty(
            _PROPERTY_PROVIDERMODULE_NAME)).getValue().get(moduleName);    

        registeredModules.append(moduleName);
        registeredModules.append("=");

        pos = instances[i].findProperty(_PROPERTY_OPERATIONALSTATUS);

        if (pos == PEG_NOT_FOUND)
        {
            moduleStatus.append(0);
        }
        else
        {
            CIMValue theValue = instances[i].getProperty(pos).getValue();

            if (theValue.isNull())
            {
                moduleStatus.append(0);
            }
            else
            {
                theValue.get(moduleStatus);
            }
        }

        statusValue = _getModuleStatusValue(moduleStatus);

        registeredModules.append(statusValue);
        registeredModules.append("\n");
    }

    MessageLoaderParms msgParms(
        "Common.AuditLogger.CURRENT_PROVIDER_REGISTRATION",
        "The current registered provider modules are:\n$0", registeredModules);
        
    _writeAuditMessageToFile(TYPE_CONFIGURATION, 
        SUBTYPE_CURRENT_PROVIDER_REGISTRATION,
        EVENT_START_UP, Logger::INFORMATION, msgParms); 
}

void AuditLogger::logCurrentEnvironmentVar()
{

    String envList;
    char ** envp = environ;

    Uint32 i = 0;

    while (envp[i])
    {
        envList.append(envp[i]);
        envList.append("\n");
   
        i++;
    }

    MessageLoaderParms msgParms("Common.AuditLogger.CURRENT_ENV",
       "The current environment variables are:\n$0", envList);

    _writeAuditMessageToFile(TYPE_CONFIGURATION, 
        SUBTYPE_CURRENT_ENVIRONMENT_VARIABLES,
        EVENT_START_UP, Logger::INFORMATION, msgParms); 
}

void AuditLogger::logSetConfigProperty(
    const String & userName,
    const String & propertyName,
    const String & prePropertyValue,
    const String & newPropertyValue,
    Boolean isPlanned)
{
    if (isPlanned)
    {
        MessageLoaderParms msgParms(
            "Common.AuditLogger.SET_PLANNED_CONFIG_PROPERTY",
            "The planned value of property \"$0\" is modified from "
                "value \"$1\" to value \"$2\" by user \"$3\".",
            propertyName, prePropertyValue, newPropertyValue, userName);

        _writeAuditMessageToFile(TYPE_CONFIGURATION, 
            SUBTYPE_CONFIGURATION_CHANGE,
            EVENT_UPDATE, Logger::INFORMATION, msgParms); 
    }
    else
    {
        MessageLoaderParms msgParms(
            "Common.AuditLogger.SET_CURRENT_CONFIG_PROPERTY",
            "The current value of property \"$0\" is modified from "
                "value \"$1\" to value \"$2\" by user \"$3\".",
            propertyName, prePropertyValue, newPropertyValue, userName);

        _writeAuditMessageToFile(TYPE_CONFIGURATION, 
            SUBTYPE_CONFIGURATION_CHANGE,
            EVENT_UPDATE, Logger::INFORMATION, msgParms); 
    }
}

void AuditLogger::logUpdateClassOperation(
    const char* cimMethodName,
    AuditEvent eventType,
    const String& userName,
    const String& ipAddr,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    CIMStatusCode statusCode)
{
    MessageLoaderParms msgParms(
        "Common.AuditLogger.OPERATION_UPDATE_CLASS",
        "A CIM $0 operation on class \"$1\" in namespace \"$2\" by user "
            "\"$3\" connected from system \"$4\" resulted in status \"$5\".",
        cimMethodName,
        className.getString(),
        nameSpace.getString(),
        userName,
        ipAddr,
        cimStatusCodeToString(statusCode));

    _writeAuditMessageToFile(TYPE_CIMOPERATION, SUBTYPE_SCHEMA_OPERATION,
        eventType, Logger::INFORMATION, msgParms);
}

void AuditLogger::logUpdateQualifierOperation(
    const char* cimMethodName,
    AuditEvent eventType,
    const String& userName,
    const String& ipAddr,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    CIMStatusCode statusCode)
{
    MessageLoaderParms msgParms(
        "Common.AuditLogger.OPERATION_UPDATE_QUALIFIER",
        "A CIM $0 operation on qualifier \"$1\" in namespace \"$2\" by user "
            "\"$3\" connected from system \"$4\" resulted in status \"$5\".",
        cimMethodName,
        className.getString(),
        nameSpace.getString(),
        userName,
        ipAddr,
        cimStatusCodeToString(statusCode));

    _writeAuditMessageToFile(TYPE_CIMOPERATION, SUBTYPE_SCHEMA_OPERATION,
        eventType, Logger::INFORMATION, msgParms);
}

void AuditLogger::logUpdateInstanceOperation(
    const char* cimMethodName,
    AuditEvent eventType,
    const String& userName,
    const String& ipAddr,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const String& moduleName,
    const String& providerName,
    CIMStatusCode statusCode)
{
    if (providerName != String::EMPTY)
    {
        MessageLoaderParms msgParms(
            "Common.AuditLogger.OPERATION_UPDATE_INSTANCE_WITH_PROVIDER",
            "A CIM $0 operation on instance \"$1\" in namespace \"$2\" by "
                "user \"$3\" connected from system \"$4\" resulted in "
                "status \"$5\".  "
                "The provider for this operation is \"$6\" in module \"$7\".",
            cimMethodName,
            CIMObjectPath("", CIMNamespaceName(), instanceName.getClassName(),
                instanceName.getKeyBindings()).toString(),
            nameSpace.getString(),
            userName,
            ipAddr,
            cimStatusCodeToString(statusCode),
            providerName,
            moduleName);

        _writeAuditMessageToFile(TYPE_CIMOPERATION, SUBTYPE_INSTANCE_OPERATION,
            eventType, Logger::INFORMATION, msgParms);
    }
    else
    {
        MessageLoaderParms msgParms(
            "Common.AuditLogger.OPERATION_UPDATE_INSTANCE",
            "A CIM $0 operation on instance \"$1\" in namespace \"$2\" by "
                "user \"$3\" connected from system \"$4\" resulted in "
                "status \"$5\".  ",
            cimMethodName,
            CIMObjectPath("", CIMNamespaceName(), instanceName.getClassName(),
                instanceName.getKeyBindings()).toString(),
            nameSpace.getString(),
            userName,
            ipAddr,
            cimStatusCodeToString(statusCode));

        _writeAuditMessageToFile(TYPE_CIMOPERATION, SUBTYPE_INSTANCE_OPERATION,
            eventType, Logger::INFORMATION, msgParms);
    }
}

void AuditLogger::logInvokeMethodOperation(
    const String& userName,
    const String& ipAddr,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& methodName,
    const String& moduleName,
    const String& providerName,
    CIMStatusCode statusCode)
{
    if (providerName != String::EMPTY)
    {
        MessageLoaderParms msgParms(
            "Common.AuditLogger.OPERATION_INVOKE_METHOD_WITH_PROVIDER",
            "A CIM InvokeMethod operation on method \"$0\" of object \"$1\" "
                "in namespace \"$2\" by user \"$3\" connected from system "
                "\"$4\" resulted in status \"$5\".  The provider for this "
                "operation is \"$6\" in module \"$7\".",
            methodName.getString(),
            CIMObjectPath("", CIMNamespaceName(), objectName.getClassName(),
                objectName.getKeyBindings()).toString(),
            nameSpace.getString(),
            userName,
            ipAddr,
            cimStatusCodeToString(statusCode),
            providerName,
            moduleName);

        _writeAuditMessageToFile(TYPE_CIMOPERATION, SUBTYPE_INSTANCE_OPERATION,
            EVENT_INVOKE, Logger::INFORMATION, msgParms);
    }
    else
    {
        MessageLoaderParms msgParms(
            "Common.AuditLogger.OPERATION_INVOKE_METHOD",
            "A CIM InvokeMethod operation on method \"$0\" of object \"$1\" "
                "in namespace \"$2\" by user \"$3\" connected from system "
                "\"$4\" resulted in status \"$5\".",
            methodName.getString(),
            CIMObjectPath("", CIMNamespaceName(), objectName.getClassName(),
                objectName.getKeyBindings()).toString(),
            nameSpace.getString(),
            userName,
            ipAddr,
            cimStatusCodeToString(statusCode));

        _writeAuditMessageToFile(TYPE_CIMOPERATION, SUBTYPE_INSTANCE_OPERATION,
            EVENT_INVOKE, Logger::INFORMATION, msgParms);
    }
}

void AuditLogger::setInitializeCallback(
    PEGASUS_AUDITLOGINITIALIZE_CALLBACK_T auditLogInitializeCallback)
{
    _auditLogInitializeCallback = auditLogInitializeCallback; 
}

void AuditLogger::setEnabled(Boolean enabled)
{
    if (enabled)
    {
        if (!_auditLogFlag)
        {
            _auditLogInitializeCallback();
        }
    }
    else
    {
        if (_auditLogFlag)
        {
            MessageLoaderParms msgParms(
                "Common.AuditLogger.DISABLE_AUDIT_LOG",
                "Audit logging is disabled."); 

            _writeAuditMessageToFile(TYPE_CONFIGURATION, 
                SUBTYPE_CONFIGURATION_CHANGE,
                EVENT_UPDATE, Logger::INFORMATION, msgParms); 
        }
    }

    _auditLogFlag = enabled;
}

void AuditLogger::writeAuditLogToFileCallback(
    PEGASUS_AUDITLOG_CALLBACK_T writeAuditLogToFileCallback)
{
    _writeAuditMessageToFile = writeAuditLogToFileCallback;
}

void AuditLogger::_writeAuditMessage(
    AuditType auditType,
    AuditSubType auditSubType,
    AuditEvent auditEvent,
    Uint32 logLevel,
    MessageLoaderParms & msgParms)
{
    String localizedMsg = MessageLoader::getMessage(msgParms);

    String identifier = "CIM Server Audit";

    Logger::put(Logger::AUDIT_LOG, identifier, logLevel, localizedMsg);
}

String AuditLogger::_getModuleStatusValue(
    const Array<Uint16>  moduleStatus)
{
    String moduleStatusValue, statusValue;
    Uint32 moduleStatusSize = moduleStatus.size();

    for (Uint32 j=0; j < moduleStatusSize; j++)
    {
        statusValue = providerModuleStatus[moduleStatus[j]];
        moduleStatusValue.append(statusValue);
       
        if (j < moduleStatusSize - 1)
        {
            moduleStatusValue.append(",");
        }
    }

    return (moduleStatusValue);
}

PEGASUS_NAMESPACE_END

#endif
