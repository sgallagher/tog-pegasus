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
// Author:  Nag Boranna,   Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By: 
//       Sushma Fernandes, Hewlett-Packard Company(sushma_fernandes@hp.com)
//       Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//       Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
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


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  SecurityPropertyOwner
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
    {"enableAuthentication", "false", 0, 0, 0},
    {"usePAMAuthentication", "false", 0, 0, 0},
    {"httpAuthType", "Basic", 0, 0, 0},
    {"passwordFilePath", "cimserver.passwd", 0, 0, 0},
    {"sslCertificateFilePath", "server.pem", 0, 0, 0}, 
    {"sslKeyFilePath", "file.pem", 0, 0, 0}, 
    {"sslTrustFilePath", "client.pem", 0, 0, 0}, 
    {"enableNamespaceAuthorization", "false", 0, 0, 0},
    {"enableRemotePrivilegedUserAccess", "false", 0, 0, 0}
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
SecurityPropertyOwner::SecurityPropertyOwner()
{
    _enableAuthentication = new ConfigProperty();
    _usePAMAuthentication = new ConfigProperty();
    _enableNamespaceAuthorization = new ConfigProperty();
    _httpAuthType = new ConfigProperty();
    _passwordFilePath = new ConfigProperty();
    _certificateFilePath = new ConfigProperty();
    _keyFilePath = new ConfigProperty();
    _trustFilePath = new ConfigProperty();
    _enableRemotePrivilegedUserAccess = new ConfigProperty();
}

/** Destructor  */
SecurityPropertyOwner::~SecurityPropertyOwner()
{
    delete _enableAuthentication;
    delete _usePAMAuthentication;
    delete _enableNamespaceAuthorization;
    delete _httpAuthType;
    delete _passwordFilePath;
    delete _certificateFilePath;
    delete _keyFilePath;
    delete _trustFilePath;
    delete _enableRemotePrivilegedUserAccess;
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
        }
        else if (String::equalNoCase(
            properties[i].propertyName, "usePAMAuthentication"))
        {
            _usePAMAuthentication->propertyName = properties[i].propertyName;
            _usePAMAuthentication->defaultValue = properties[i].defaultValue;
            _usePAMAuthentication->currentValue = properties[i].defaultValue;
            _usePAMAuthentication->plannedValue = properties[i].defaultValue;
            _usePAMAuthentication->dynamic = properties[i].dynamic;
            _usePAMAuthentication->domain = properties[i].domain;
            _usePAMAuthentication->domainSize = properties[i].domainSize;
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
        }
        else if (String::equalNoCase(
			    properties[i].propertyName, 
			    "passwordFilePath"))
        {
            _passwordFilePath->propertyName = properties[i].propertyName;
            _passwordFilePath->defaultValue = properties[i].defaultValue;
            _passwordFilePath->plannedValue = properties[i].defaultValue;
            _passwordFilePath->dynamic = properties[i].dynamic;
            _passwordFilePath->domain = properties[i].domain;
            _passwordFilePath->domainSize = properties[i].domainSize;

            // 
            // Initialize passsword file path to $PEGASUS_HOME/cimserver.passwd
            //
	    if ( _passwordFilePath->currentValue == String::EMPTY )
	    {
                _passwordFilePath->currentValue.append(ConfigManager::getPegasusHome());
                _passwordFilePath->currentValue.append("/");
                _passwordFilePath->currentValue.append(_passwordFilePath->defaultValue);
            }
        }
        else if (String::equalNoCase(
			    properties[i].propertyName, 
			    "sslCertificateFilePath"))
        {  
            _certificateFilePath->propertyName = properties[i].propertyName;
            _certificateFilePath->defaultValue = properties[i].defaultValue;
            _certificateFilePath->plannedValue = properties[i].defaultValue;
            _certificateFilePath->dynamic = properties[i].dynamic;
            _certificateFilePath->domain = properties[i].domain;
            _certificateFilePath->domainSize = properties[i].domainSize;

            // 
            // Initialize SSL cert file path to $PEGASUS_HOME/server.pem
            //
	    if ( _certificateFilePath->currentValue == String::EMPTY )
	    {
                _certificateFilePath->currentValue.append(ConfigManager::getPegasusHome());
                _certificateFilePath->currentValue.append("/");
                _certificateFilePath->currentValue.append(_certificateFilePath->defaultValue);
            }
        } 
        else if (String::equalNoCase(
			    properties[i].propertyName, 
			    "sslKeyFilePath"))
        {  
            _keyFilePath->propertyName = properties[i].propertyName;
            _keyFilePath->defaultValue = properties[i].defaultValue;
            _keyFilePath->plannedValue = properties[i].defaultValue;
            _keyFilePath->dynamic = properties[i].dynamic;
            _keyFilePath->domain = properties[i].domain;
            _keyFilePath->domainSize = properties[i].domainSize;

            // 
            // Initialize SSL key file path to $PEGASUS_HOME/file.pem
            //
	    if ( _keyFilePath->currentValue == String::EMPTY )
	    {
                _keyFilePath->currentValue.append(ConfigManager::getPegasusHome());
                _keyFilePath->currentValue.append("/");
                _keyFilePath->currentValue.append(_keyFilePath->defaultValue);
            }
        } 
        else if (String::equalNoCase(
			    properties[i].propertyName, 
			    "sslTrustFilePath"))
        {  
            _trustFilePath->propertyName = properties[i].propertyName;
            _trustFilePath->defaultValue = properties[i].defaultValue;
            _trustFilePath->plannedValue = properties[i].defaultValue;
            _trustFilePath->dynamic = properties[i].dynamic;
            _trustFilePath->domain = properties[i].domain;
            _trustFilePath->domainSize = properties[i].domainSize;

            // 
            // Initialize SSL trust file path to $PEGASUS_HOME/trust.pem
            //
	    if ( _trustFilePath->currentValue == String::EMPTY )
	    {
                _trustFilePath->currentValue.append(ConfigManager::getPegasusHome());
                _trustFilePath->currentValue.append("/");
                _trustFilePath->currentValue.append(_trustFilePath->defaultValue);
            }
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
        }
    }

}

struct ConfigProperty* SecurityPropertyOwner::_lookupConfigProperty(
    const String& name)
{
    if (String::equalNoCase(_enableAuthentication->propertyName, name))
    {
        return _enableAuthentication;
    }
    else if (String::equalNoCase(_usePAMAuthentication->propertyName, name))
    {
        return _usePAMAuthentication;
    }
    else if (String::equalNoCase(_enableNamespaceAuthorization->propertyName, name))
    {
        return _enableNamespaceAuthorization;
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        return _httpAuthType;
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        return _passwordFilePath;
    }
    else if (String::equalNoCase(_certificateFilePath->propertyName, name))
    {  
        return _certificateFilePath;
    }
    else if (String::equalNoCase(_keyFilePath->propertyName, name))
    {  
        return _keyFilePath;
    }
    else if (String::equalNoCase(_trustFilePath->propertyName, name))
    {  
        return _trustFilePath;
    }
    else if (String::equalNoCase(
                 _enableRemotePrivilegedUserAccess->propertyName, name))
    {
        return _enableRemotePrivilegedUserAccess;
    }
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
    Array<String>& propertyInfo)
{
    propertyInfo.clear();

    struct ConfigProperty* configProperty = _lookupConfigProperty(name);

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
}

/** 
Get default value of the specified property.
*/
const String SecurityPropertyOwner::getDefaultValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->defaultValue;
}

/** 
Get current value of the specified property.
*/
const String SecurityPropertyOwner::getCurrentValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->currentValue;
}

/** 
Get planned value of the specified property.
*/
const String SecurityPropertyOwner::getPlannedValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
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
Boolean SecurityPropertyOwner::isValid(const String& name, const String& value)
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
    else if (String::equalNoCase(_usePAMAuthentication->propertyName, name))
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
        if(String::equal(value, "Basic") || String::equal(value, "Digest"))
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
        if (fileName == String::EMPTY || fileName== "")
        {
            return false;
        }

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
             String::equalNoCase(_keyFilePath->propertyName, name) ||
             String::equalNoCase(_trustFilePath->propertyName, name))
    {  
	String fileName(value);

        //
        // Check if the file path is empty
        //
        if (fileName == String::EMPTY || fileName== "")
        {
            return false;
        }

	//
        // Check if the file path is a directory
	//
        FileSystem::translateSlashes(fileName);
        if (FileSystem::isDirectory(fileName))
        {
            return false;
        }

	//
        // Check if the file exists and is readable
	//
        if (FileSystem::exists(fileName))
        {
            if (!FileSystem::canRead(fileName))
            {
                return false;
            }
         }

        return true;
    }
    else if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        if(String::equal(value, "true") || String::equal(value, "false"))
        {
            retVal = true;
        }
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
    return retVal;
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean SecurityPropertyOwner::isDynamic(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->dynamic;
}


PEGASUS_NAMESPACE_END
