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
//
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the security property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SecurityPropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  SecurityPropertyOwner
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
    {"requireAuthentication", "false", 0, 0, 0},
    {"httpAuthType", "Basic", 0, 0, 0}
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
SecurityPropertyOwner::SecurityPropertyOwner()
{
    _requireAuthentication = new ConfigProperty;
    _httpAuthType = new ConfigProperty;
}

/** Destructor  */
SecurityPropertyOwner::~SecurityPropertyOwner()
{
    delete _requireAuthentication;
    delete _httpAuthType;
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
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        return (_httpAuthType->defaultValue);
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
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        return (_httpAuthType->currentValue);
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
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        return (_httpAuthType->plannedValue);
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

    if (String::equal(value, EMPTY_VALUE))
    {
            throw InvalidPropertyValue(name,value);
    }

    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        _requireAuthentication->currentValue = value;
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        _httpAuthType->currentValue = value;
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
    if (String::equal(value, EMPTY_VALUE))
    {
            throw InvalidPropertyValue(name,value);
    }

    if (String::equalNoCase(_requireAuthentication->propertyName, name))
    {
        _requireAuthentication->plannedValue= value;
    }
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        _httpAuthType->plannedValue= value;
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
void SecurityPropertyOwner::updatePlannedValue(
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
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        if(String::equal(value, "Basic") || String::equal(value, "Digest"))
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
    else if (String::equalNoCase(_httpAuthType->propertyName, name))
    {
        return (_httpAuthType->dynamic);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


PEGASUS_NAMESPACE_END
