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

#ifndef Pegasus_AuditLogger_h
#define Pegasus_AuditLogger_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMName.h>

PEGASUS_NAMESPACE_BEGIN


#ifndef PEGASUS_DISABLE_AUDIT_LOGGER

/**
    This class provides the interfaces to construct a human readable audit
    text message and write the message to log files.
*/
class PEGASUS_COMMON_LINKAGE AuditLogger
{
public:

    enum AuditType
    {
        TYPE_AUTHENTICATION,
        TYPE_AUTHORIZATION,
        TYPE_CONFIGURATION,
        TYPE_CIMOPERATION
    };

    enum AuditSubType
    {
        SUBTYPE_LOCAL_AUTHENTICATION,
        SUBTYPE_BASIC_AUTHENTICATION,
        SUBTYPE_CERTIFICATE_BASED_AUTHENTICATION,
        SUBTYPE_USER_GROUP_AUTHORIZATION,
        SUBTYPE_NAMESPACE_AUTHORIZATION,
        SUBTYPE_PRIVILEGED_USER_CHECK,
        SUBTYPE_TRUSTSTORE_CHANGE,
        SUBTYPE_CURRENT_CONFIGURATION,
        SUBTYPE_CURRENT_PROVIDER_REGISTRATION,
        SUBTYPE_CURRENT_ENVIRONMENT_VARIABLES,
        SUBTYPE_CONFIGURATION_CHANGE,
        SUBTYPE_PROVIDER_REGISTRATION_CHANGE,
        SUBTYPE_PROVIDER_MODULE_STATUS_CHANGE,
        SUBTYPE_SCHEMA_OPERATION,
        SUBTYPE_INSTANCE_OPERATION,
        SUBTYPE_INDICATION_OPERATION
    };

    enum AuditEvent
    {
        EVENT_START_UP,
        EVENT_AUTH_SUCCESS,
        EVENT_AUTH_FAILURE,
        EVENT_CREATE,
        EVENT_UPDATE,
        EVENT_DELETE,
        EVENT_INVOKE
    };

    /** Constructs and logs audit message of the current configurations
        @param propertyNames - All the current property names while the CIM
                               Server is running
        @param propertyValues - All the current property values while the CIM
                                Server is running
    */
    static void logCurrentConfig(
        const Array<String>& propertyNames,
        const Array<String>& propertyValues);

    /** Constructs and logs audit message of the currently registered
        providers while the CIM Server is running
        @param instances - all currently registered provider module
        instances while the CIM Server is running
    */
    static void logCurrentRegProvider(
        const Array < CIMInstance > & instances);

    /** Constructs and logs audit message of the current environment
        variables while the CIM Server is running
    */
    static void logCurrentEnvironmentVar();

    /** Constructs and logs audit message of setting the specified
        configuration property to the specified value
        or unset the specified configuration property to the default value
        @param userName - The user name for this operation
        @param propertyName - The specified configuration property name
        @param prePropertyValue - The previous value of the changed config
                                  property
        @param newPropertyValue - The new value of the changed config
                                  property or default value if it is unset
        @param isPlanned - True, sets planned value of the
                           specified configuration  property;
                           Otherwise, sets current value of the
                           specified configuration  property
    */
    static void logSetConfigProperty(
        const String & userName,
        const String & propertyName,
        const String & prePropertyValue,
        const String & newPropertyValue,
        Boolean isPlanned);

    /**
        Constructs and logs audit message of a CIM class update operation
        @param cimMethodName - The name of the CIM operation performed
        @param eventType - The AuditEvent associated with the CIM operation
        @param userName - User name for this operation
        @param ipAddr - Client IP address for this operation
        @param nameSpace - The namespace for the operation
        @param className - The name of the class
        @param statusCode - The CIM status code for the operation
    */
    static void logUpdateClassOperation(
        const char* cimMethodName,
        AuditEvent eventType,
        const String& userName,
        const String& ipAddr,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        CIMStatusCode statusCode);

    /**
        Constructs and logs audit message of a CIM qualifier update operation
        @param cimMethodName - The name of the CIM operation performed
        @param eventType - The AuditEvent associated with the CIM operation
        @param userName - User name for this operation
        @param ipAddr - Client IP address for this operation
        @param nameSpace - The namespace for the operation
        @param name - The name of the qualifier
        @param statusCode - The CIM status code for the operation
    */
    static void logUpdateQualifierOperation(
        const char* cimMethodName,
        AuditEvent eventType,
        const String& userName,
        const String& ipAddr,
        const CIMNamespaceName& nameSpace,
        const CIMName& name,
        CIMStatusCode statusCode);

    /**
        Constructs and logs audit message of a CIM instance update operation
        @param cimMethodName - The name of the CIM operation performed
        @param eventType - The AuditEvent associated with the CIM operation
        @param userName - The user name for this operation
        @param ipAddr - Client IP address for this operation
        @param nameSpace - The namespace for the operation
        @param instanceName - The name of the affected instance
        @param moduleName - The provider module name that serves the request
        @param providerName - The provider name that serves the request
        @param statusCode - The CIM status code for the operation
    */
    static void logUpdateInstanceOperation(
        const char* cimMethodName,
        AuditEvent eventType,
        const String& userName,
        const String& ipAddr,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const String& moduleName,
        const String& providerName,
        CIMStatusCode statusCode);

    /**
        Constructs and logs audit message of a CIM InvokeMethod operation
        @param userName - The user name for this operation
        @param ipAddr - Client IP address for this operation
        @param nameSpace - The namespace for the operation
        @param objectName - The name of the object on which the method is
            invoked
        @param methodName - The name of the method to be executed
        @param moduleName - The provider module name that serves the request
        @param providerName - The provider name that serves the request
        @param statusCode - The CIM status code for the operation
    */
    static void logInvokeMethodOperation(
        const String& userName,
        const String& ipAddr,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& methodName,
        const String& moduleName,
        const String& providerName,
        CIMStatusCode statusCode);

    /** Constructs and logs audit message of a provider module status change
        @param moduleName - The name of the provider module
        @param currentModuleStatus - The current status of the provider module
        @param newModuleStatus - The new status of the provider module
    */
    static void logUpdateProvModuleStatus(
        const String & moduleName,
        const Array<Uint16> currentModuleStatus,
        const Array<Uint16> newModuleStatus);

    /** Constructs and logs audit message of local authentication
        @param userName - The user name for this operation
        @param successful - True on successful basic authentication,
                            false otherwise
    */
    static void logLocalAuthentication(
        const String& userName,
        Boolean successful);

    /** Constructs and logs audit message of basic authentication
        @param userName - The user name for this operation
        @param ipAddr - Client IP address for this operation
        @param successful - True on successful basic authentication,
                            false otherwise
    */
    static void logBasicAuthentication(
        const String& userName,
        const String& ipAddr,
        Boolean successful);

    typedef void (*PEGASUS_AUDITLOGINITIALIZE_CALLBACK_T)();

    typedef void (*PEGASUS_AUDITLOG_CALLBACK_T) (AuditType,
        AuditSubType, AuditEvent, Uint32, MessageLoaderParms &);

    /**
        Registers an audit log initialize callback
        If a non-null initialize callback function is registered,
        it will be called when the audit log is enabled.
        @param auditLogInitializeCallback - The audit log initialize
                                            callback function
    */
    static void setInitializeCallback(
        PEGASUS_AUDITLOGINITIALIZE_CALLBACK_T auditLogInitializeCallback);

    /** If the enabled is true, the audit log initialize callback function
        is called to communicate that the audit log is enabled.
        @param enabled - True on config property "enableAuditLog" is
                         enabled, false otherwise
    */
    static void setEnabled(Boolean enabled);

    static Boolean isEnabled();

    /**
        Registers writing audit messages to a file callback
        @param writeAuditMessageCallback - The callback function to write
                                           audit message
    */
    static void writeAuditLogToFileCallback(
        PEGASUS_AUDITLOG_CALLBACK_T writeAuditMessageCallback);

private:

    static Boolean _auditLogFlag;

    /**
        Callback function to be called when the audit log is enabled
    */
    static PEGASUS_AUDITLOGINITIALIZE_CALLBACK_T _auditLogInitializeCallback;

    /**
        The function to write audit messages
    */
    static PEGASUS_AUDITLOG_CALLBACK_T _writeAuditMessageToFile;

    /** Default function to write a auditMessage to a file
        @param AuditType - Type of audit record (Authentication etc)
        @param AuditSubType - Sub type of audit record(Local_Authentication etc)
        @param AuditEvent - Event of audit record (Start_Up etc)
        @param logLevel - Pegasus Severity (WARNING etc)
        All the audit messages are passed with pegasus severity
        "INFORMATION", except authentication attempts failed messages or
        authorization failed messages are passed with pegasus severity
        "WARNING"
        @param msgParms - The message loader parameters
    */
    static void _writeAuditMessage(
        AuditType auditType,
        AuditSubType auditSubType,
        AuditEvent auditEvent,
        Uint32 logLevel,
        MessageLoaderParms & msgParms);

    /**
        gets module status value
        @param moduleStatus - The module status
    */
    static String _getModuleStatusValue(const Array<Uint16>  moduleStatus);

};

inline Boolean AuditLogger::isEnabled()
{
    return _auditLogFlag;
}

# define PEG_AUDIT_LOG(T) \
    do \
    { \
        if (AuditLogger::isEnabled()) \
        { \
            AuditLogger::T; \
        } \
    } \
    while (0)

#else

# define PEG_AUDIT_LOG(T)

#endif

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AuditLogger_h */
