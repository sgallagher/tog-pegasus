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
// Author: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By: Yi Zhou (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the default property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DefaultPropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
//  DefaultPropertyOwner
//
//  When a new property is added with the default owner, make sure to add 
//  the property name and the default attributes of that property in 
//  the table below.
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
    {"port", "5988", 0, 0, 0},
    {"home", "./", 0, 0, 0},
    {"daemon", "false", 0, 0, 0},
    {"install", "false", 0, 0, 0},
    {"remove", "false", 0, 0, 0},
    {"slp", "false", 0, 0, 0},
    {"SSL", "false", 0, 0, 0},
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
DefaultPropertyOwner::DefaultPropertyOwner()
{
    _configProperties = new ConfigProperty[NUM_PROPERTIES];
}

/** Destructor  */
DefaultPropertyOwner::~DefaultPropertyOwner()
{
    delete _configProperties;
}

/**
Initialize the config properties.
*/
void DefaultPropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        _configProperties[i].propertyName = properties[i].propertyName;
        _configProperties[i].defaultValue = properties[i].defaultValue;
        _configProperties[i].currentValue = properties[i].defaultValue;
        _configProperties[i].plannedValue = properties[i].defaultValue;
        _configProperties[i].dynamic = properties[i].dynamic;
        _configProperties[i].domain = properties[i].domain;
        _configProperties[i].domainSize = properties[i].domainSize;
    }
}


/** 
Get information about the specified property.
*/
void DefaultPropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo)
{
    propertyInfo.clear();

    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase(_configProperties[i].propertyName, name))
        {
            propertyInfo.append(_configProperties[i].propertyName);
            propertyInfo.append(_configProperties[i].defaultValue);
            propertyInfo.append(_configProperties[i].currentValue);
            propertyInfo.append(_configProperties[i].plannedValue);
            if (_configProperties[i].dynamic)
            {
                propertyInfo.append(STRING_TRUE);
            }
            else
            {
                propertyInfo.append(STRING_FALSE);
            }
            return;
        }
    }

    //
    // specified property name is not found
    //
    throw UnrecognizedConfigProperty(name);
}

/** 
Get default value of the specified property 
*/
const String DefaultPropertyOwner::getDefaultValue(const String& name)
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase(_configProperties[i].propertyName, name))
        {
            return (_configProperties[i].defaultValue);
        }
    }

    //
    // Specified property name could not be found
    //
    throw UnrecognizedConfigProperty(name);
}

/** 
Get current value of the specified property 
*/
const String DefaultPropertyOwner::getCurrentValue(const String& name)
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase(_configProperties[i].propertyName, name))
        {
            return (_configProperties[i].currentValue);
        }
    }

    //
    // Specified property name could not be found
    //
    throw UnrecognizedConfigProperty(name);
}

/** 
Get planned value of the specified property 
*/
const String DefaultPropertyOwner::getPlannedValue(const String& name)
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase(_configProperties[i].propertyName, name))
        {
            return (_configProperties[i].plannedValue);
        }
    }

    //
    // Specified property name could not be found
    //
    throw UnrecognizedConfigProperty(name);
}

/** 
Init current value of the specified property to the specified value 
*/
void DefaultPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase(_configProperties[i].propertyName, name))
        {
            _configProperties[i].currentValue = value;
            return;
        }
    }

    //
    // Specified property name could not be found
    //
    throw UnrecognizedConfigProperty(name);
}


/** 
Init planned value of the specified property to the specified value 
*/
void DefaultPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase(_configProperties[i].propertyName, name))
        {
            _configProperties[i].plannedValue = value;
            return;
        }
    }

    //
    // Specified property name could not be found
    //
    throw UnrecognizedConfigProperty(name);
}

/** 
Update current value of the specified property to the specified value 
*/
void DefaultPropertyOwner::updateCurrentValue(
    const String& name, 
    const String& value)
{
    if (String::equal(value, EMPTY_VALUE))
    {
            throw InvalidPropertyValue(name,value);
    }

    //
    // make sure the property is dynamic before updating the value.
    //
    if (!isDynamic(name))
    {
        throw NonDynamicConfigProperty(name); 
    }

    //
    // Since the validations done in initCurrrentValue are sufficient and 
    // no additional validations required for update, we shall call 
    // initCurrrentValue.
    //
    initCurrentValue(name, value);
}


/** 
Update planned value of the specified property to the specified value 
*/
void DefaultPropertyOwner::updatePlannedValue(
    const String& name, 
    const String& value)
{
    if (String::equal(value, EMPTY_VALUE))
    {
            throw InvalidPropertyValue(name,value);
    }
    //
    // Since the validations done in initPlannedValue are sufficient and 
    // no additional validations required for update, we shall call 
    // initPlannedValue.
    //
    initPlannedValue(name, value);
}


/** 
Checks to see if the given value is valid or not.
*/
Boolean DefaultPropertyOwner::isValid(const String& name, const String& value)
{
    //
    // By default, no validation is done. It can optionally be added here
    // per property.
    //
    return 1;
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean DefaultPropertyOwner::isDynamic(const String& name)
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase(_configProperties[i].propertyName, name))
        {
            return (_configProperties[i].dynamic);
        }
    }

    //
    // Specified property name could not be found
    //
    throw UnrecognizedConfigProperty(name);
}


PEGASUS_NAMESPACE_END
