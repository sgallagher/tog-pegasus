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
// Author:  Nag Boranna,   Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By: 
//       Sushma Fernandes, Hewlett-Packard Company(sushma_fernandes@hp.com)
//       Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//       Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//       Heather Sterling, IBM (hsterl@us.ibm.com)
//       Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3614
//       Vijay Eli, IBM, (vijayeli@in.ibm.com) for Bug# 3613
//       Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3613
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the security property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include "ConfigManager.h"
#include "SecurityPropertyOwner.h"
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/System.h>


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  SecurityPropertyOwner
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
#ifdef PEGASUS_OS_OS400
    {"enableAuthentication", "true", IS_STATIC, 0, 0, IS_VISIBLE},
#else
    {"enableAuthentication", "false", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
//#if defined(PEGASUS_OS_OS400) && defined(PEGASUS_KERBEROS_AUTHENTICATION)
//    {"httpAuthType", "Kerberos", IS_STATIC, 0, 0, IS_VISIBLE},
//#else
    {"httpAuthType", "Basic", IS_STATIC, 0, 0, IS_VISIBLE},
//#endif
    {"passwordFilePath", "cimserver.passwd", IS_STATIC, 0, 0, IS_VISIBLE},
#ifdef PEGASUS_OS_HPUX
    {"sslCertificateFilePath", "cert.pem", IS_STATIC, 0, 0, IS_VISIBLE},
#else
# ifdef PEGASUS_OS_OS400
    {"sslCertificateFilePath", "ssl/keystore/servercert.pem", IS_STATIC, 0, 0, IS_VISIBLE},
# else
    {"sslCertificateFilePath", "server.pem", IS_STATIC, 0, 0, IS_VISIBLE}, 
# endif
#endif
#ifdef PEGASUS_OS_OS400
    {"sslKeyFilePath", "ssl/keystore/serverkey.pem", IS_STATIC, 0, 0, IS_VISIBLE},
#else
    {"sslKeyFilePath", "file.pem", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
#ifdef PEGASUS_OS_OS400
    {"sslTrustStore", "ssl/truststore/", IS_STATIC, 0, 0, IS_VISIBLE},
#else
    {"sslTrustStore", "cimserver_trust", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
#ifdef PEGASUS_OS_OS400
    {"exportSSLTrustStore", "ssl/exporttruststore/", IS_STATIC, 0, 0, IS_VISIBLE},
#else
    {"exportSSLTrustStore", "indication_trust", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
#ifdef PEGASUS_OS_OS400
    {"crlStore", "ssl/crlstore/", IS_STATIC, 0, 0, IS_VISIBLE},
#else
    {"crlStore", "crl", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
#ifdef PEGASUS_OS_OS400
    {"sslClientVerificationMode", "optional", IS_STATIC, 0, 0, IS_VISIBLE},
#else
    {"sslClientVerificationMode", "disabled", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
    {"sslTrustStoreUserName", "", IS_STATIC, 0, 0, IS_VISIBLE},
#ifdef PEGASUS_OS_OS400
    {"enableNamespaceAuthorization", "true", IS_STATIC, 0, 0, IS_VISIBLE},
#else
    {"enableNamespaceAuthorization", "false", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    {"kerberosServiceName", "cimom", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_LINUX)
# ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
    {"enableSubscriptionsForNonprivilegedUsers", "false", IS_STATIC, 0, 0, IS_VISIBLE},
# else
    {"enableSubscriptionsForNonprivilegedUsers", "true", IS_STATIC, 0, 0, IS_VISIBLE},
# endif
#else
# ifdef PEGASUS_OS_OS400
    {"enableSubscriptionsForNonprivilegedUsers", "false", IS_STATIC, 0, 0, IS_VISIBLE},
# else
    {"enableSubscriptionsForNonprivilegedUsers", "true", IS_STATIC, 0, 0, IS_HIDDEN},
# endif
#endif
    {"enableRemotePrivilegedUserAccess", "true", IS_STATIC, 0, 0, IS_VISIBLE},
#ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION
    {"authorizedUserGroups", "", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
#ifdef PEGASUS_OS_OS400
    {"enableSSLExportClientVerification", "true", IS_STATIC, 0, 0, IS_VISIBLE}
#else
    {"enableSSLExportClientVerification", "false", IS_STATIC, 0, 0, IS_VISIBLE}
#endif
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
SecurityPropertyOwner::SecurityPropertyOwner()
{
    _enableAuthentication.reset(new ConfigProperty());
    _enableNamespaceAuthorization.reset(new ConfigProperty());
    _httpAuthType.reset(new ConfigProperty());
    _passwordFilePath.reset(new ConfigProperty());
    _certificateFilePath.reset(new ConfigProperty());
    _keyFilePath.reset(new ConfigProperty());
    _trustStore.reset(new ConfigProperty());
    _exportSSLTrustStore.reset(new ConfigProperty());
	_crlStore.reset(new ConfigProperty());
    _sslClientVerificationMode.reset(new ConfigProperty());
    _sslTrustStoreUserName.reset(new ConfigProperty());
    _enableRemotePrivilegedUserAccess.reset(new ConfigProperty());
    _enableSubscriptionsForNonprivilegedUsers.reset(new ConfigProperty());
#ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION
    _authorizedUserGroups.reset(new ConfigProperty());
#endif
    _enableSSLExportClientVerification.reset(new ConfigProperty());
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
        _kerberosServiceName.reset(new ConfigProperty());
#endif
}


/**
Initialize the config properties.
*/
void SecurityPropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        //
        // Initialize the properties with default values
        //
        if (String::equalNoCase(
            properties[i].propertyName, "enableAuthentication"))
        {
            _enableAuthentication->propertyName = properties[i].propertyName;
            _enableAuthentication->defaultValue = properties[i].defaultValue;
            _enableAuthentication->currentValue = properties[i].defaultValue;
            _enableAuthentication->plannedValue = properties[i].defaultValue;
            _enableAuthentication->dynamic = properties[i].dynamic;
            _enableAuthentication->domain = properties[i].domain;
            _enableAuthentication->domainSize = properties[i].domainSize;
            _enableAuthentication->externallyVisible = properties[i].externallyVisible;
        }
        else if (String::equalNoCase(
            properties[i].propertyName, "enableNamespaceAuthorization"))
        {
            _enableNamespaceAuthorization->propertyName = properties[i].propertyName;
            _enableNamespaceAuthorization->defaultValue = properties[i].defaultValue;
            _enableNamespaceAuthorization->currentValue = properties[i].defaultValue;
            _enableNamespaceAuthorization->plannedValue = properties[i].defaultValue;
            _enableNamespaceAuthorization->dynamic = properties[i].dynamic;
            _enableNamespaceAuthorization->domain = properties[i].domain;
            _enableNamespaceAuthorization->domainSize = properties[i].domainSize;
            _enableNamespaceAuthorization->externallyVisible = properties[i].externallyVisible;
        }
        else if (String::equalNoCase(properties[i].propertyName, "httpAuthType"))
        {
            _httpAuthType->propertyName = properties[i].propertyName;
            _httpAuthType->defaultValue = properties[i].defaultValue;
            _httpAuthType->currentValue = properties[i].defaultValue;
            _httpAuthType->plannedValue = properties[i].defaultValue;
            _httpAuthType->dynamic = properties[i].dynamic;
            _httpAuthType->domain = properties[i].domain;
            _httpAuthType->domainSize = properties[i].domainSize;
            _httpAuthType->externallyVisible = properties[i].externallyVisible;
        }
        else if (String::equalNoCase(
			    properties[i].propertyName, 
			    "passwordFilePath"))
        {
            _passwordFilePath->propertyName = properties[i].propertyName;
            _passwordFilePath->defaultValue = properties[i].defaultValue;
            _passwordFilePath->currentValue = properties[i].defaultValue;
            _passwordFilePath->plannedValue = properties[i].defaultValue;
            _passwordFilePath->dynamic = properties[i].dynamic;
            _passwordFilePath->domain = properties[i].domain;
            _passwordFilePath->domainSize = properties[i].domainSize;
            _passwordFilePath->externallyVisible = properties[i].externallyVisible;
        }
        else if (String::equalNoCase(
			    properties[i].propertyName, 
			    "sslCertificateFilePath"))
        {  
            _certificateFilePath->propertyName = properties[i].propertyName;
            _certificateFilePath->defaultValue = properties[i].defaultValue;
            _certificateFilePath->currentValue = properties[i].defaultValue;
            _certificateFilePath->plannedValue = properties[i].defaultValue;
            _certificateFilePath->dynamic = properties[i].dynamic;
            _certificateFilePath->domain = properties[i].domain;
            _certificateFilePath->domainSize = properties[i].domainSize;
            _certificateFilePath->externallyVisible = properties[i].externallyVisible;
        } 
        else if (String::equalNoCase(
			    properties[i].propertyName, 
			    "sslKeyFilePath"))
        {  
            _keyFilePath->propertyName = properties[i].propertyName;
            _keyFilePath->defaultValue = properties[i].defaultValue;
            _keyFilePath->currentValue = properties[i].defaultValue;
            _keyFilePath->plannedValue = properties[i].defaultValue;
            _keyFilePath->dynamic = properties[i].dynamic;
            _keyFilePath->domain = properties[i].domain;
            _keyFilePath->domainSize = properties[i].domainSize;
            _keyFilePath->externallyVisible = properties[i].externallyVisible;
        } 
        else if (String::equalNoCase(
			    properties[i].propertyName, 
                "sslTrustStore"))
        {  
            _trustStore->propertyName = properties[i].propertyName;
            _trustStore->defaultValue = properties[i].defaultValue;
            _trustStore->currentValue = properties[i].defaultValue;
            _trustStore->plannedValue = properties[i].defaultValue;
            _trustStore->dynamic = properties[i].dynamic;
            _trustStore->domain = properties[i].domain;
            _trustStore->domainSize = properties[i].domainSize;
            _trustStore->externallyVisible = properties[i].externallyVisible;

            // do not initialize trustpath; a truststore is not required for SSL handshakes
            // a server may wish to connect on HTTPS but not verify clients
        } 
        else if (String::equalNoCase(
                            properties[i].propertyName,
                            "exportSSLTrustStore"))
        {
            _exportSSLTrustStore->propertyName = properties[i].propertyName;
            _exportSSLTrustStore->defaultValue = properties[i].defaultValue;
            _exportSSLTrustStore->currentValue = properties[i].defaultValue;
            _exportSSLTrustStore->plannedValue = properties[i].defaultValue;
            _exportSSLTrustStore->dynamic = properties[i].dynamic;
            _exportSSLTrustStore->domain = properties[i].domain;
            _exportSSLTrustStore->domainSize = properties[i].domainSize;
            _exportSSLTrustStore->externallyVisible = properties[i].externallyVisible;
        }
        else if (String::equalNoCase(
			    properties[i].propertyName, 
                "crlStore"))
        {  
            _crlStore->propertyName = properties[i].propertyName;
            _crlStore->defaultValue = properties[i].defaultValue;
            _crlStore->currentValue = properties[i].defaultValue;
            _crlStore->plannedValue = properties[i].defaultValue;
            _crlStore->dynamic = properties[i].dynamic;
            _crlStore->domain = properties[i].domain;
            _crlStore->domainSize = properties[i].domainSize;
            _crlStore->externallyVisible = properties[i].externallyVisible;

        } 
        else if (String::equalNoCase(
            properties[i].propertyName, "sslClientVerificationMode")) 
        {
            _sslClientVerificationMode->propertyName = properties[i].propertyName;
            _sslClientVerificationMode->defaultValue = properties[i].defaultValue;
            _sslClientVerificationMode->currentValue = properties[i].defaultValue;
            _sslClientVerificationMode->plannedValue = properties[i].defaultValue;
            _sslClientVerificationMode->dynamic = properties[i].dynamic;
            _sslClientVerificationMode->domain = properties[i].domain;
            _sslClientVerificationMode->domainSize = properties[i].domainSize;
            _sslClientVerificationMode->externallyVisible = properties[i].externallyVisible;
        }
        else if (String::equalNoCase(
            properties[i].propertyName, "sslTrustStoreUserName")) 
        {
            _sslTrustStoreUserName->propertyName = properties[i].propertyName;
            _sslTrustStoreUserName->defaultValue = properties[i].defaultValue;
            _sslTrustStoreUserName->currentValue = properties[i].defaultValue;
            _sslTrustStoreUserName->plannedValue = properties[i].defaultValue;
            _sslTrustStoreUserName->dynamic = properties[i].dynamic;
            _sslTrustStoreUserName->domain = properties[i].domain;
            _sslTrustStoreUserName->domainSize = properties[i].domainSize;
            _sslTrustStoreUserName->externallyVisible = properties[i].externallyVisible;
        }
        else if (String::equalNoCase(
            properties[i].propertyName, "enableRemotePrivilegedUserAccess"))
        {
            _enableRemotePrivilegedUserAccess->propertyName = properties[i].propertyName;
            _enableRemotePrivilegedUserAccess->defaultValue = properties[i].defaultValue;
            _enableRemotePrivilegedUserAccess->currentValue = properties[i].defaultValue;
            _enableRemotePrivilegedUserAccess->plannedValue = properties[i].defaultValue;
            _enableRemotePrivilegedUserAccess->dynamic = properties[i].dynamic;
            _enableRemotePrivilegedUserAccess->domain = properties[i].domain;
            _enableRemotePrivilegedUserAccess->domainSize = properties[i].domainSize;
            _enableRemotePrivilegedUserAccess->externallyVisible = properties[i].externallyVisible;
        }
        else if (String::equalNoCase(
            properties[i].propertyName, "enableSubscriptionsForNonprivilegedUsers"))
        {
            _enableSubscriptionsForNonprivilegedUsers->propertyName = properties[i].propertyName;
            _enableSubscriptionsForNonprivilegedUsers->defaultValue = properties[i].defaultValue;
            _enableSubscriptionsForNonprivilegedUsers->currentValue = properties[i].defaultValue;
            _enableSubscriptionsForNonprivilegedUsers->plannedValue = properties[i].defaultValue;
            _enableSubscriptionsForNonprivilegedUsers->dynamic = properties[i].dynamic;
            _enableSubscriptionsForNonprivilegedUsers->domain = properties[i].domain;
            _enableSubscriptionsForNonprivilegedUsers->domainSize = properties[i].domainSize;
            _enableSubscriptionsForNonprivilegedUsers->externallyVisible = properties[i].externallyVisible;
        }
#ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION
        else if (String::equalNoCase(properties[i].propertyName, "authorizedUserGroups"))
        {
            _authorizedUserGroups->propertyName = properties[i].propertyName;
            _authorizedUserGroups->defaultValue = properties[i].defaultValue;
            _authorizedUserGroups->currentValue = properties[i].defaultValue;
            _authorizedUserGroups->plannedValue = properties[i].defaultValue;
            _authorizedUserGroups->dynamic = properties[i].dynamic;
            _authorizedUserGroups->domain = properties[i].domain;
            _authorizedUserGroups->domainSize = properties[i].domainSize;
            _authorizedUserGroups->externallyVisible = properties[i].externallyVisible;
        }
#endif
        else if (String::equalNoCase(
            properties[i].propertyName, "enableSSLExportClientVerification"))
        {
            _enableSSLExportClientVerification->propertyName = properties[i].propertyName;
            _enableSSLExportClientVerification->defaultValue = properties[i].defaultValue;
            _enableSSLExportClientVerification->currentValue = properties[i].defaultValue;
            _enableSSLExportClientVerification->plannedValue = properties[i].defaultValue;
            _enableSSLExportClientVerification->dynamic = properties[i].dynamic;
            _enableSSLExportClientVerification->domain = properties[i].domain;
            _enableSSLExportClientVerification->domainSize = properties[i].domainSize;
            _enableSSLExportClientVerification->externallyVisible = properties[i].externallyVisible;
        }
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
        else if (String::equalNoCase(properties[i].propertyName, "kerberosServiceName"))
        {
            _kerberosServiceName->propertyName = properties[i].propertyName;
            _kerberosServiceName->defaultValue = properties[i].defaultValue;
            _kerberosServiceName->currentValue = properties[i].defaultValue;
            _kerberosServiceName->plannedValue = properties[i].defaultValue;
            _kerberosServiceName->dynamic = properties[i].dynamic;
            _kerberosServiceName->domain = properties[i].domain;
            _kerberosServiceName->domainSize = properties[i].domainSize;
            _kerberosServiceName->externallyVisible = properties[i].externallyVisible;
        }
#endif
    }

}

struct ConfigProperty* SecurityPropertyOwner::_lookupConfigProperty(
    const String& name) const
{
    if (String::equalNoCase(_enableAuthentication->propertyName, name))
    {
        return _enableAuthentication.get();
    }
    else if (String::equalNoCase(_enableNamespaceAuthorization->propertyName, name))
    {
        return _enableNamespaceAuthorization.get();
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        return _httpAuthType.get();
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        return _passwordFilePath.get();
    }
    else if (String::equalNoCase(_certificateFilePath->propertyName, name))
    {  
        return _certificateFilePath.get();
    }
    else if (String::equalNoCase(_keyFilePath->propertyName, name))
    {  
        return _keyFilePath.get();
    }
    else if (String::equalNoCase(_trustStore->propertyName, name))
    {  
        return _trustStore.get();
    }
    else if (String::equalNoCase(_exportSSLTrustStore->propertyName, name))
    {
        return _exportSSLTrustStore.get();
    }
	else if (String::equalNoCase(_crlStore->propertyName, name))
    {
        return _crlStore.get();
    }
    else if (String::equalNoCase(
                 _sslClientVerificationMode->propertyName, name))
    {
        return _sslClientVerificationMode.get();
    }
    else if (String::equalNoCase(
                 _sslTrustStoreUserName->propertyName, name))
    {
        return _sslTrustStoreUserName.get();
    }
    else if (String::equalNoCase(
                 _enableRemotePrivilegedUserAccess->propertyName, name))
    {
        return _enableRemotePrivilegedUserAccess.get();
    }
    else if (String::equalNoCase(
                 _enableSubscriptionsForNonprivilegedUsers->propertyName, name))
    {
        return _enableSubscriptionsForNonprivilegedUsers.get();
    }
#ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION
    else if (String::equalNoCase(_authorizedUserGroups->propertyName, name))
    {
        return _authorizedUserGroups.get();
    }
#endif
    else if (String::equalNoCase(
                 _enableSSLExportClientVerification->propertyName, name))
    {
        return _enableSSLExportClientVerification.get();
    }
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    else if (String::equalNoCase(_kerberosServiceName->propertyName, name))
    {
        return _kerberosServiceName.get();
    }
#endif
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get information about the specified property.
*/
void SecurityPropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo) const
{
    propertyInfo.clear();
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    propertyInfo.append(configProperty->propertyName);
    propertyInfo.append(configProperty->defaultValue);
    propertyInfo.append(configProperty->currentValue);
    propertyInfo.append(configProperty->plannedValue);
    if (configProperty->dynamic)
    {
        propertyInfo.append(STRING_TRUE);
    }
    else
    {
        propertyInfo.append(STRING_FALSE);
    }
    if (configProperty->externallyVisible)
    {
        propertyInfo.append(STRING_TRUE);
    }
    else
    {
        propertyInfo.append(STRING_FALSE);
    }
}

/** 
Get default value of the specified property.
*/
String SecurityPropertyOwner::getDefaultValue(const String& name) const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);
    return configProperty->defaultValue;
}

/** 
Get current value of the specified property.
*/
String SecurityPropertyOwner::getCurrentValue(const String& name) const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);
    return configProperty->currentValue;
}

/** 
Get planned value of the specified property.
*/
String SecurityPropertyOwner::getPlannedValue(const String& name) const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);
    return configProperty->plannedValue;
}


/** 
Init current value of the specified property to the specified value.
*/
void SecurityPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->currentValue = value;
}


/** 
Init planned value of the specified property to the specified value.
*/
void SecurityPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->plannedValue = value;
}

/** 
Update current value of the specified property to the specified value.
*/
void SecurityPropertyOwner::updateCurrentValue(
    const String& name, 
    const String& value) 
{
    //
    // make sure the property is dynamic before updating the value.
    //
    if (!isDynamic(name))
    {
        throw NonDynamicConfigProperty(name); 
    }

    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->currentValue = value;
}


/** 
Update planned value of the specified property to the specified value.
*/
void SecurityPropertyOwner::updatePlannedValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->plannedValue = value;
}

/** 
Checks to see if the given value is valid or not.
*/
Boolean SecurityPropertyOwner::isValid(const String& name, 
                             const String& value) const
{
    Boolean retVal = false;

    //
    // Validate the specified value
    //
    if (String::equalNoCase(_enableAuthentication->propertyName, name))
    {
        if(String::equal(value, "true") || String::equal(value, "false"))
        {
            retVal = true;
        }
    }
    else if (String::equalNoCase(_enableNamespaceAuthorization->propertyName, name))
    {
        if(String::equal(value, "true") || String::equal(value, "false"))
        {
            retVal = true;
        }
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
#ifdef PEGASUS_OS_OS400
        if(String::equal(value, "Basic")
#else
        if(String::equal(value, "Basic") || String::equal(value, "Digest")
#endif
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
            || String::equal(value, "Kerberos") 
#endif
        )
        {
            retVal = true;
        }
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
	String fileName(value);

        //
        // Check if the file path is empty
        //
        if (fileName == String::EMPTY)
        {
            return false;
        }

		fileName = ConfigManager::getHomedPath(fileName);

	//
        // Check if the file path is a directory
	//
        FileSystem::translateSlashes(fileName);
        if (FileSystem::isDirectory(fileName))
        {
            return false;
        }

	//
        // Check if the file exists and is writable
	//
        if (FileSystem::exists(fileName))
        {
            if (!FileSystem::canWrite(fileName))
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
	    //
            // Check if directory is writable
	    // 
            Uint32 pos = fileName.reverseFind('/');

            if (pos != PEG_NOT_FOUND)
            {
                String dirName = fileName.subString(0,pos);
                if (!FileSystem::isDirectory(dirName))
                {
                    return false;
                }
                if (!FileSystem::canWrite(dirName) )
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                String currentDir;

		//
                // Check if there is permission to write in the	
                // current working directory
		//
                FileSystem::getCurrentDirectory(currentDir);

                if (!FileSystem::canWrite(currentDir))
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
    }
    else if (String::equalNoCase(_certificateFilePath->propertyName, name) ||
             String::equalNoCase(_keyFilePath->propertyName, name))
    {
        //
        // Check if the file path is empty
        //
        if (value == String::EMPTY)
        {
            return false;
        }

		String fileName = ConfigManager::getHomedPath(value);

        //
        // Check if the file path is a directory
        //
        FileSystem::translateSlashes(fileName);
        if (FileSystem::isDirectory(fileName))
        {
            return false;
        }

        //
        // Check if the file exists and is readable and is not empty.
        //
        if (FileSystem::exists(fileName) && FileSystem::canRead(fileName))
        {
            Uint32 size;
            if (FileSystem::getFileSize(fileName, size))
            {
                if (size > 0)
                {
                    return true;
                }
            }
        }

         return false;
    }
    else if (String::equalNoCase(_trustStore->propertyName, name) ||
             String::equalNoCase(_exportSSLTrustStore->propertyName, name) || 
			 String::equalNoCase(_crlStore->propertyName, name))
    {
        //
        // Allow the exportSSLTrustStore and sslTrustStore file paths to be empty
        //
        if (value == String::EMPTY)
        {
            return true;
        }

		String fileName = ConfigManager::getHomedPath(value);

        //
        // Check if the file path is a directory
        //
        FileSystem::translateSlashes(fileName);
        if (FileSystem::isDirectory(fileName))
        {
            //
            // Truststore can be a directory, congruent with OpenSSL standards
            // Check if the directoy has read and write permissions
            //
            if (FileSystem::canRead(fileName) && FileSystem::canWrite(fileName)) 
            {
                return true;  
            } 
        }
        //
        // Check if the file exists and is readable
        //
        else if (FileSystem::exists(fileName) && FileSystem::canRead(fileName))
        {
            return true;
        }

        return false;
    }
    else if (String::equalNoCase(_sslClientVerificationMode->propertyName, name))
    {
        if(String::equal(value, "disabled") || String::equal(value, "required") || String::equal(value, "optional"))
        {
            retVal = true;
        }
    }
	else if (String::equalNoCase(_sslTrustStoreUserName->propertyName, name))
    {
        if (System::isSystemUser((const char*)value.getCString()))
        {
            return true;
        }
    }
    else if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        if(String::equal(value, "true") || String::equal(value, "false"))
        {
            retVal = true;
        }
    }
    else if (String::equalNoCase(_enableSubscriptionsForNonprivilegedUsers->propertyName, name))
    {
        if(String::equal(value, "true") || String::equal(value, "false"))
        {
            retVal = true;
        }
    }
#ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION
    else if (String::equalNoCase(_authorizedUserGroups->propertyName, name))
    {
        retVal = true;
    }
#endif
    else if (String::equalNoCase(_enableSSLExportClientVerification->propertyName, name))
    {
        if(String::equal(value, "true") || String::equal(value, "false"))
        {
            retVal = true;
        }
    }
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    else if (String::equalNoCase(_kerberosServiceName->propertyName, name))
    {
        String serviceName(value);

        //
        // Check if the service name is empty
        //
        if (serviceName == String::EMPTY || serviceName== "")
        {
            retVal =  false;
        }
       else
        {
           retVal =  true;
        }
    }
#endif
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
    return retVal;
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean SecurityPropertyOwner::isDynamic(const String& name) const
{
    struct ConfigProperty * configProperty =_lookupConfigProperty(name);

    return (configProperty->dynamic==IS_DYNAMIC);
}


PEGASUS_NAMESPACE_END
