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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the repository property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include "RepositoryPropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  RepositoryPropertyOwner
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
    {"repositoryIsDefaultInstanceProvider", "true", 0, 0, 0}
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
RepositoryPropertyOwner::RepositoryPropertyOwner()
{
    _repositoryIsDefaultInstanceProvider = new ConfigProperty;
}

/** Destructor  */
RepositoryPropertyOwner::~RepositoryPropertyOwner()
{
    delete _repositoryIsDefaultInstanceProvider;
}


/**
Initialize the config properties.
*/
void RepositoryPropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        //
        // Initialize the properties with default values
        //
        if (String::equalNoCase(
            properties[i].propertyName, "repositoryIsDefaultInstanceProvider"))
        {
            _repositoryIsDefaultInstanceProvider->propertyName = properties[i].propertyName;
            _repositoryIsDefaultInstanceProvider->defaultValue = properties[i].defaultValue;
            _repositoryIsDefaultInstanceProvider->currentValue = properties[i].defaultValue;
            _repositoryIsDefaultInstanceProvider->plannedValue = properties[i].defaultValue;
            _repositoryIsDefaultInstanceProvider->dynamic = properties[i].dynamic;
            _repositoryIsDefaultInstanceProvider->domain = properties[i].domain;
            _repositoryIsDefaultInstanceProvider->domainSize = properties[i].domainSize;
        }
    }
}

/** 
Get information about the specified property.
*/
void RepositoryPropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo)
{
    propertyInfo.clear();

    if (String::equalNoCase(_repositoryIsDefaultInstanceProvider->propertyName, name))
    {
        propertyInfo.append(_repositoryIsDefaultInstanceProvider->propertyName);
        propertyInfo.append(_repositoryIsDefaultInstanceProvider->defaultValue);
        propertyInfo.append(_repositoryIsDefaultInstanceProvider->currentValue);
        propertyInfo.append(_repositoryIsDefaultInstanceProvider->plannedValue);
        if (_repositoryIsDefaultInstanceProvider->dynamic)
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
const String RepositoryPropertyOwner::getDefaultValue(const String& name)
{
    if (String::equalNoCase(_repositoryIsDefaultInstanceProvider->propertyName, name))
    {
        return (_repositoryIsDefaultInstanceProvider->defaultValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get current value of the specified property.
*/
const String RepositoryPropertyOwner::getCurrentValue(const String& name)
{
    if (String::equalNoCase(_repositoryIsDefaultInstanceProvider->propertyName, name))
    {
        return (_repositoryIsDefaultInstanceProvider->currentValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get planned value of the specified property.
*/
const String RepositoryPropertyOwner::getPlannedValue(const String& name)
{
    if (String::equalNoCase(_repositoryIsDefaultInstanceProvider->propertyName, name))
    {
        return (_repositoryIsDefaultInstanceProvider->plannedValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Init current value of the specified property to the specified value.
*/
void RepositoryPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{

    if (String::equalNoCase(_repositoryIsDefaultInstanceProvider->propertyName, name))
    {
        _repositoryIsDefaultInstanceProvider->currentValue = value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Init planned value of the specified property to the specified value.
*/
void RepositoryPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    if (String::equalNoCase(_repositoryIsDefaultInstanceProvider->propertyName, name))
    {
        _repositoryIsDefaultInstanceProvider->plannedValue= value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Update current value of the specified property to the specified value.
*/
void RepositoryPropertyOwner::updateCurrentValue(
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

    //
    // Validate the specified value and call initPlannedValue
    //
    if (isValid(name, value))
    {
        initCurrentValue(name, value);
    }
}


/** 
Update planned value of the specified property to the specified value.
*/
void RepositoryPropertyOwner::updatePlannedValue(
    const String& name, 
    const String& value)
{
    //
    // Validate the specified value and call initPlannedValue
    //
    if (isValid(name, value))
    {
        initPlannedValue(name, value);
    }
}

/** 
Checks to see if the given value is valid or not.
*/
Boolean RepositoryPropertyOwner::isValid(const String& name, const String& value)
{
    Boolean retVal = false;

    //
    // Validate the specified value
    //
    if (String::equalNoCase(_repositoryIsDefaultInstanceProvider->propertyName, name))
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
Boolean RepositoryPropertyOwner::isDynamic(const String& name)
{
    if (String::equalNoCase(_repositoryIsDefaultInstanceProvider->propertyName, name))
    {
        return (_repositoryIsDefaultInstanceProvider->dynamic);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


PEGASUS_NAMESPACE_END
