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
// Author: Sushma Fernandes (sushma_fernandes@hp.com)
//
// Modified By:  Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3614
//              Vijay Eli, IBM, (vijayeli@in.ibm.com) for Bug# 3613
//              Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3613
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the file system property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Config/ConfigManager.h>
#include "FileSystemPropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  FileSystemPropertyOwner
//
//  When a new FileSystem property is added, make sure to add the property name
//  and the default attributes of that property in the table below.
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
    {"repositoryDir", "repository", IS_STATIC, 0, 0, IS_VISIBLE},
#ifdef PEGASUS_OS_OS400
    {"messageDir", "/QIBM/ProdData/OS400/CIM/msg", IS_STATIC, 0, 0, IS_VISIBLE},
#else
    {"messageDir", "msg", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
FileSystemPropertyOwner::FileSystemPropertyOwner()
{
    _repositoryDir.reset(new ConfigProperty);
    _messageDir.reset(new ConfigProperty);
}


/**
Checks if the given directory is existing and writable
*/
Boolean isDirValid(const String& dirName)
{
    if (FileSystem::isDirectory(dirName) && FileSystem::canWrite(dirName))
    {
        return true;
    }
    return false;
}
 
/**
Initialize the config properties.
*/
void FileSystemPropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        //
        // Initialize the properties with default values
        //
        if (String::equalNoCase(properties[i].propertyName, "repositoryDir"))
        {
            _repositoryDir->propertyName = properties[i].propertyName;
            _repositoryDir->defaultValue = properties[i].defaultValue;
            _repositoryDir->currentValue = properties[i].defaultValue;
            _repositoryDir->plannedValue = properties[i].defaultValue;
            _repositoryDir->dynamic = properties[i].dynamic;
            _repositoryDir->domain = properties[i].domain;
            _repositoryDir->domainSize = properties[i].domainSize;
            _repositoryDir->externallyVisible = properties[i].externallyVisible;
        }
        else if (String::equalNoCase(properties[i].propertyName, "messageDir"))
        {
            _messageDir->propertyName = properties[i].propertyName;
            _messageDir->defaultValue = properties[i].defaultValue;
            _messageDir->currentValue = properties[i].defaultValue;
            _messageDir->plannedValue = properties[i].defaultValue;
            _messageDir->dynamic = properties[i].dynamic;
            _messageDir->domain = properties[i].domain;
            _messageDir->domainSize = properties[i].domainSize;
            _messageDir->externallyVisible = properties[i].externallyVisible;
        }
    }
}

struct ConfigProperty* FileSystemPropertyOwner::_lookupConfigProperty(
    const String& name) const
{
    if (String::equalNoCase(_repositoryDir->propertyName, name))
    {
        return _repositoryDir.get();
    }
    if (String::equalNoCase(_messageDir->propertyName, name))
    {
        return _messageDir.get();
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get information about the specified property.
*/
void FileSystemPropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo) const
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
String FileSystemPropertyOwner::getDefaultValue(const String& name) const
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);    

    return configProperty->defaultValue;
}

/** 
Get current value of the specified property.
*/
String FileSystemPropertyOwner::getCurrentValue(const String& name) const
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);

    return configProperty->currentValue;
}

/** 
Get planned value of the specified property.
*/
String FileSystemPropertyOwner::getPlannedValue(const String& name) const
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);

    return configProperty->plannedValue;
}

/** 
Init current value of the specified property to the specified value.
*/
void FileSystemPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->currentValue = value;
}


/** 
Init planned value of the specified property to the specified value.
*/
void FileSystemPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->plannedValue = value;
}

/** 
Update current value of the specified property to the specified value.
*/
void FileSystemPropertyOwner::updateCurrentValue(
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
void FileSystemPropertyOwner::updatePlannedValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->plannedValue = value;
}

/** 
Checks to see if the given value is valid or not.
*/
Boolean FileSystemPropertyOwner::isValid(const String& name, 
                                 const String& value) const
{
    if (!isDirValid( value ))
    {
        throw InvalidPropertyValue(name, value);
    }
 
    return true;
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean FileSystemPropertyOwner::isDynamic(const String& name) const
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);

    return (configProperty->dynamic==IS_DYNAMIC);
}


PEGASUS_NAMESPACE_END
