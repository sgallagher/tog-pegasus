//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author:  Nag Boranna,   Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By: 
//       Sushma Fernandes, Hewlett-Packard Company(sushma_fernandes@hp.com)
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
    {"requireAuthentication", "false", 0, 0, 0},
    {"requireAuthorization", "false", 0, 0, 0},
    {"httpAuthType", "Basic", 0, 0, 0},
    {"passwordFilePath", "cimserver.passwd", 0, 0, 0},
    {"enableRemotePrivilegedUserAccess", "false", 0, 0, 0},
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
SecurityPropertyOwner::SecurityPropertyOwner()
{
    _requireAuthentication = new ConfigProperty;
    _requireAuthorization = new ConfigProperty;
    _httpAuthType = new ConfigProperty;
    _passwordFilePath = new ConfigProperty();
    _enableRemotePrivilegedUserAccess = new ConfigProperty();
}

/** Destructor  */
SecurityPropertyOwner::~SecurityPropertyOwner()
{
    delete _requireAuthentication;
    delete _requireAuthorization;
    delete _httpAuthType;
    delete _passwordFilePath;
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
            properties[i].propertyName, "requireAuthentication"))
        {
            _requireAuthentication->propertyName = properties[i].propertyName;
            _requireAuthentication->defaultValue = properties[i].defaultValue;
            _requireAuthentication->currentValue = properties[i].defaultValue;
            _requireAuthentication->plannedValue = properties[i].defaultValue;
            _requireAuthentication->dynamic = properties[i].dynamic;
            _requireAuthentication->domain = properties[i].domain;
            _requireAuthentication->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(
            properties[i].propertyName, "requireAuthorization"))
        {
            _requireAuthorization->propertyName = properties[i].propertyName;
            _requireAuthorization->defaultValue = properties[i].defaultValue;
            _requireAuthorization->currentValue = properties[i].defaultValue;
            _requireAuthorization->plannedValue = properties[i].defaultValue;
            _requireAuthorization->dynamic = properties[i].dynamic;
            _requireAuthorization->domain = properties[i].domain;
            _requireAuthorization->domainSize = properties[i].domainSize;
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
                _passwordFilePath->currentValue += ConfigManager::getPegasusHome();
                _passwordFilePath->currentValue.append("/");
                _passwordFilePath->currentValue += _passwordFilePath->defaultValue;
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

/** 
Get information about the specified property.
*/
void SecurityPropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo)
{
    propertyInfo.clear();

    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        propertyInfo.append(_requireAuthentication->propertyName);
        propertyInfo.append(_requireAuthentication->defaultValue);
        propertyInfo.append(_requireAuthentication->currentValue);
        propertyInfo.append(_requireAuthentication->plannedValue);
        if (_requireAuthentication->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_requireAuthorization->propertyName, name))
    {
        propertyInfo.append(_requireAuthorization->propertyName);
        propertyInfo.append(_requireAuthorization->defaultValue);
        propertyInfo.append(_requireAuthorization->currentValue);
        propertyInfo.append(_requireAuthorization->plannedValue);
        if (_requireAuthorization->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        propertyInfo.append(_httpAuthType->propertyName);
        propertyInfo.append(_httpAuthType->defaultValue);
        propertyInfo.append(_httpAuthType->currentValue);
        propertyInfo.append(_httpAuthType->plannedValue);
        if (_httpAuthType->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        propertyInfo.append(_passwordFilePath->propertyName);
        propertyInfo.append(_passwordFilePath->defaultValue);
        propertyInfo.append(_passwordFilePath->currentValue);
        propertyInfo.append(_passwordFilePath->plannedValue);
        if (_passwordFilePath->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        propertyInfo.append(_enableRemotePrivilegedUserAccess->propertyName);
        propertyInfo.append(_enableRemotePrivilegedUserAccess->defaultValue);
        propertyInfo.append(_enableRemotePrivilegedUserAccess->currentValue);
        propertyInfo.append(_enableRemotePrivilegedUserAccess->plannedValue);
        if (_requireAuthorization->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get default value of the specified property.
*/
const String SecurityPropertyOwner::getDefaultValue(const String& name)
{
    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        return (_requireAuthentication->defaultValue);
    }
    else if (String::equalNoCase(_requireAuthorization->propertyName, name))
    {
        return (_requireAuthorization->defaultValue);
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        return (_httpAuthType->defaultValue);
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        return (_passwordFilePath->defaultValue);
    }
    else if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        return (_enableRemotePrivilegedUserAccess->defaultValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get current value of the specified property.
*/
const String SecurityPropertyOwner::getCurrentValue(const String& name)
{
    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        return (_requireAuthentication->currentValue);
    }
    else if (String::equalNoCase(_requireAuthorization->propertyName, name))
    {
        return (_requireAuthorization->currentValue);
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        return (_httpAuthType->currentValue);
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        return (_passwordFilePath->currentValue);
    }
    else if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        return (_enableRemotePrivilegedUserAccess->currentValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get planned value of the specified property.
*/
const String SecurityPropertyOwner::getPlannedValue(const String& name)
{
    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        return (_requireAuthentication->plannedValue);
    }
    else if (String::equalNoCase(_requireAuthorization->propertyName, name))
    {
        return (_requireAuthorization->plannedValue);
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        return (_httpAuthType->plannedValue);
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        return (_passwordFilePath->plannedValue);
    }
    else if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        return (_enableRemotePrivilegedUserAccess->plannedValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Init current value of the specified property to the specified value.
*/
void SecurityPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{

    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        _requireAuthentication->currentValue = value;
    }
    else if (String::equalNoCase(_requireAuthorization->propertyName, name))
    {
        _requireAuthorization->currentValue = value;
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        _httpAuthType->currentValue = value;
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        _passwordFilePath->currentValue = value;
    }
    else if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        _enableRemotePrivilegedUserAccess->currentValue = value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Init planned value of the specified property to the specified value.
*/
void SecurityPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        _requireAuthentication->plannedValue= value;
    }
    else if (String::equalNoCase(_requireAuthorization->propertyName, name))
    {
        _requireAuthorization->plannedValue= value;
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        _httpAuthType->plannedValue= value;
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        _passwordFilePath->plannedValue= value;
    }
    else if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        _enableRemotePrivilegedUserAccess->plannedValue= value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Update current value of the specified property to the specified value.
*/
void SecurityPropertyOwner::updateCurrentValue(
    const String& name, 
    const String& value) 
{
    //
    // Validate the specified value 
    //
    if (!isValid(name, value))
    {
        throw InvalidPropertyValue(name, value);
    }

    //
    // make sure the property is dynamic before updating the value.
    //
    if (!isDynamic(name))
    {
        throw NonDynamicConfigProperty(name); 
    }

    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        _requireAuthentication->currentValue = value;
    }
    else if (String::equalNoCase(_requireAuthorization->propertyName, name))
    {
        _requireAuthorization->currentValue = value;
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        _httpAuthType->currentValue = value;
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        _passwordFilePath->currentValue = value;
    }
    else if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        _enableRemotePrivilegedUserAccess->currentValue = value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Update planned value of the specified property to the specified value.
*/
void SecurityPropertyOwner::updatePlannedValue(
    const String& name, 
    const String& value)
{
    //
    // Validate the specified value 
    //
    if (!isValid(name, value))
    {
        throw InvalidPropertyValue(name, value);
    }

    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        _requireAuthentication->plannedValue = value;
    }
    else if (String::equalNoCase(_requireAuthorization->propertyName, name))
    {
        _requireAuthorization->plannedValue = value;
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        _httpAuthType->plannedValue = value;
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        _passwordFilePath->plannedValue = value;
    }
    else if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        _enableRemotePrivilegedUserAccess->plannedValue = value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
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
    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        if(String::equal(value, "true") || String::equal(value, "false"))
        {
            retVal = true;
        }
    }
    else if (String::equalNoCase(_requireAuthorization->propertyName, name))
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
            String dirSeparator("/");

            Uint32 pos = fileName.reverseFind(*(dirSeparator.getData()));

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
    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        return (_requireAuthentication->dynamic);
    }
    if (String::equalNoCase(_requireAuthorization->propertyName, name))
    {
        return (_requireAuthorization->dynamic);
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        return (_httpAuthType->dynamic);
    }
    else if (String::equalNoCase(_passwordFilePath->propertyName, name))
    {
        return (_passwordFilePath->dynamic);
    }
    if (String::equalNoCase(_enableRemotePrivilegedUserAccess->propertyName, name))
    {
        return (_enableRemotePrivilegedUserAccess->dynamic);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


PEGASUS_NAMESPACE_END
