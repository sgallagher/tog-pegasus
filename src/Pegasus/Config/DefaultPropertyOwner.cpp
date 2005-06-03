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
// Author: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By: Yi Zhou (yi_zhou@hp.com)
//              Warren Otsuka (warren.otsuka@hp.com)
//              Sushma Fernandes, Hewlett-Packard Company
//                     (sushma_fernandes@hp.com)
//              Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3614
//              Vijay Eli, IBM, (vijayeli@in.ibm.com) for Bug# 3613
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
#include "DefaultPropertyTable.h"
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
DefaultPropertyOwner::DefaultPropertyOwner()
{
    _configProperties.reset(new ConfigProperty[NUM_PROPERTIES]);
}


/**
Initialize the config properties.
*/
void DefaultPropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        (_configProperties.get())[i].propertyName = properties[i].propertyName;
        (_configProperties.get())[i].defaultValue = properties[i].defaultValue;
        (_configProperties.get())[i].currentValue = properties[i].defaultValue;
        (_configProperties.get())[i].plannedValue = properties[i].defaultValue;
        (_configProperties.get())[i].dynamic = properties[i].dynamic;
        (_configProperties.get())[i].domain = properties[i].domain;
        (_configProperties.get())[i].domainSize = properties[i].domainSize;
        (_configProperties.get())[i].externallyVisible = properties[i].externallyVisible;
    }
}


/**
Get information about the specified property.
*/
void DefaultPropertyOwner::getPropertyInfo(
    const String& name,
    Array<String>& propertyInfo) const
{
    propertyInfo.clear();

    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase((_configProperties.get())[i].propertyName, name))
        {
            propertyInfo.append((_configProperties.get())[i].propertyName);
            propertyInfo.append((_configProperties.get())[i].defaultValue);
            propertyInfo.append((_configProperties.get())[i].currentValue);
            propertyInfo.append((_configProperties.get())[i].plannedValue);
            if ((_configProperties.get())[i].dynamic)
            {
                propertyInfo.append(STRING_TRUE);
            }
            else
            {
                propertyInfo.append(STRING_FALSE);
            }
            if ((_configProperties.get())[i].externallyVisible)
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
String DefaultPropertyOwner::getDefaultValue(const String& name) const
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase((_configProperties.get())[i].propertyName, name))
        {
            return ((_configProperties.get())[i].defaultValue);
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
String DefaultPropertyOwner::getCurrentValue(const String& name) const
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase((_configProperties.get())[i].propertyName, name))
        {
            return ((_configProperties.get())[i].currentValue);
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
String DefaultPropertyOwner::getPlannedValue(const String& name) const
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase((_configProperties.get())[i].propertyName, name))
        {
            return ((_configProperties.get())[i].plannedValue);
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
        if (String::equalNoCase((_configProperties.get())[i].propertyName, name))
        {
            (_configProperties.get())[i].currentValue = value;
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
        if (String::equalNoCase((_configProperties.get())[i].propertyName, name))
        {
            (_configProperties.get())[i].plannedValue = value;
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
const
{
    //
    // By default, no validation is done. It can optionally be added here
    // per property.
    //
    return(true);
}

/**
Checks to see if the specified property is dynamic or not.
*/
Boolean DefaultPropertyOwner::isDynamic(const String& name) const
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        if (String::equalNoCase((_configProperties.get())[i].propertyName, name))
        {
            return ((_configProperties.get())[i].dynamic == IS_DYNAMIC);
        }
    }

    //
    // Specified property name could not be found
    //
    throw UnrecognizedConfigProperty(name);
}


PEGASUS_NAMESPACE_END
