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
// Author: Sushma Fernandes (sushma_fernandes@hp.com)
//
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
#include <Pegasus/Common/Destroyer.h>
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
    {"repositoryDir", "repository", 0, 0, 0},
    {"providerDir", "lib", 0, 0, 0},
    {"consumerDir", "", 0, 0, 0}
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
FileSystemPropertyOwner::FileSystemPropertyOwner()
{
    _repositoryDir = new ConfigProperty;
    _providerDir = new ConfigProperty;
    _consumerDir = new ConfigProperty;
}

/** Destructor  */
FileSystemPropertyOwner::~FileSystemPropertyOwner()
{
    delete _repositoryDir;
    delete _providerDir;
    delete _consumerDir;
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
        }
        else if (String::equalNoCase(properties[i].propertyName, "providerDir"
))
        {
            _providerDir->propertyName = properties[i].propertyName;
            _providerDir->defaultValue = properties[i].defaultValue;
            _providerDir->currentValue = properties[i].defaultValue;
            _providerDir->plannedValue = properties[i].defaultValue;
            _providerDir->dynamic = properties[i].dynamic;
            _providerDir->domain = properties[i].domain;
            _providerDir->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(properties[i].propertyName, "consumerDir"
))
        {
            _consumerDir->propertyName = properties[i].propertyName;
            _consumerDir->defaultValue = properties[i].defaultValue;
            _consumerDir->currentValue = properties[i].defaultValue;
            _consumerDir->plannedValue = properties[i].defaultValue;
            _consumerDir->dynamic = properties[i].dynamic;
            _consumerDir->domain = properties[i].domain;
            _consumerDir->domainSize = properties[i].domainSize;
        }
    }
}

/** 
Get information about the specified property.
*/
void FileSystemPropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo)
{
    propertyInfo.clear();

    if (String::equalNoCase(_repositoryDir->propertyName, name))
    {
        propertyInfo.append(_repositoryDir->propertyName);
        propertyInfo.append(_repositoryDir->defaultValue);
        propertyInfo.append(_repositoryDir->currentValue);
        propertyInfo.append(_repositoryDir->plannedValue);
        if (_repositoryDir->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_providerDir->propertyName, name))
    {
        propertyInfo.append(_providerDir->propertyName);
        propertyInfo.append(_providerDir->defaultValue);
        propertyInfo.append(_providerDir->currentValue);
        propertyInfo.append(_providerDir->plannedValue);
        if (_providerDir->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_consumerDir->propertyName, name))
    {
        propertyInfo.append(_consumerDir->propertyName);
        propertyInfo.append(_consumerDir->defaultValue);
        propertyInfo.append(_consumerDir->currentValue);
        propertyInfo.append(_consumerDir->plannedValue);
        if (_consumerDir->dynamic)
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
const String FileSystemPropertyOwner::getDefaultValue(const String& name)
{
    if (String::equalNoCase(_repositoryDir->propertyName, name))
    {
        return (_repositoryDir->defaultValue);
    }
    else if (String::equalNoCase(_providerDir->propertyName, name))
    {
        return (_providerDir->defaultValue);
    }
    else if (String::equalNoCase(_consumerDir->propertyName, name))
    {
        return (_consumerDir->defaultValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get current value of the specified property.
*/
const String FileSystemPropertyOwner::getCurrentValue(const String& name)
{
    if (String::equalNoCase(_repositoryDir->propertyName, name))
    {
        return (_repositoryDir->currentValue);
    }
    else if (String::equalNoCase(_providerDir->propertyName, name))
    {
        return (_providerDir->currentValue);
    }
    else if (String::equalNoCase(_consumerDir->propertyName, name))
    {
        return (_consumerDir->currentValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get planned value of the specified property.
*/
const String FileSystemPropertyOwner::getPlannedValue(const String& name)
{
    if (String::equalNoCase(_repositoryDir->propertyName, name))
    {
        return (_repositoryDir->plannedValue);
    }
    else if (String::equalNoCase(_providerDir->propertyName, name))
    {
        return (_providerDir->plannedValue);
    }
    else if (String::equalNoCase(_consumerDir->propertyName, name))
    {
        return (_consumerDir->plannedValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Init current value of the specified property to the specified value.
*/
void FileSystemPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{
    // Perform validation
    if (String::equal(value, EMPTY_VALUE))
    {
            throw InvalidPropertyValue(name,value);
    }
    
    if (String::equalNoCase(_repositoryDir->propertyName, name))
    {
        _repositoryDir->currentValue = value;
    }
    else if (String::equalNoCase(_providerDir->propertyName, name))
    {
        _providerDir->currentValue = value;
    }
    else if (String::equalNoCase(_consumerDir->propertyName, name))
    {
        _consumerDir->currentValue = value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Init planned value of the specified property to the specified value.
*/
void FileSystemPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    // Perform validation
    if (String::equal(value, EMPTY_VALUE))
    {
        throw InvalidPropertyValue(name,value);
    }

    if (String::equalNoCase(_repositoryDir->propertyName, name))
    {
        _repositoryDir->plannedValue = value;
    }
    else if (String::equalNoCase(_providerDir->propertyName, name))
    {
        _providerDir->plannedValue = value;
    }
    else if (String::equalNoCase(_consumerDir->propertyName, name))
    {
        _consumerDir->plannedValue = value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Update current value of the specified property to the specified value.
*/
void FileSystemPropertyOwner::updateCurrentValue(
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
}


/** 
Update planned value of the specified property to the specified value.
*/
void FileSystemPropertyOwner::updatePlannedValue(
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

    if (String::equalNoCase(_repositoryDir->propertyName, name))
    {
        _repositoryDir->plannedValue = value;
    }
    else if (String::equalNoCase(_providerDir->propertyName, name))
    {
        _providerDir->plannedValue = value;
    }
    else if (String::equalNoCase(_consumerDir->propertyName, name))
    {
        _consumerDir->plannedValue = value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Checks to see if the given value is valid or not.
*/
Boolean FileSystemPropertyOwner::isValid(const String& name, const String& value)
{
    Boolean retVal = false;

    if (String::equal(value, EMPTY_VALUE))
    {
        throw InvalidPropertyValue(name, value);
    }

    if (!isDirValid( value ))
    {
        throw InvalidPropertyValue(name, value);
    }
 
    return true;
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean FileSystemPropertyOwner::isDynamic(const String& name)
{
    if (String::equalNoCase(_repositoryDir->propertyName, name))
    {
        return (_repositoryDir->dynamic);
    }
    else if (String::equalNoCase(_providerDir->propertyName, name))
    {
        return (_providerDir->dynamic);
    }
    else if (String::equalNoCase(_consumerDir->propertyName, name))
    {
        return (_consumerDir->dynamic);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


PEGASUS_NAMESPACE_END
