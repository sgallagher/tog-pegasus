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
//=============================================================================
//
// Author: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the timeout property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Destroyer.h>
#include "ShutdownPropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  ShutdownPropertyOwner
//
//  The operationTimeout property is the timeout value in seconds that the
//  cimom uses to wait for CIM operations to complete before shutting down
//  the cimom.  The default is 2 seconds.
//
//  The shutdownTimeout property is the timeout value in seconds that the
//  cimom uses to wait for the shutdown process to complete (this includes
//  terminating active providers).  The default value is 5 seconds.
//
//  When a new timeout property is added, make sure to add the property name
//  and the default attributes of that property in the table below.
//
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
    {"operationTimeout", "2", 0, 0, 0},
    {"shutdownTimeout", "5", 0, 0, 0},
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);

static long MIN_OPERATION_TIMEOUT = 2;
static long MIN_SHUTDOWN_TIMEOUT = 5;

/** Constructor  */
ShutdownPropertyOwner::ShutdownPropertyOwner()
{
    _operationTimeout = new ConfigProperty;
    _shutdownTimeout = new ConfigProperty;
}

/** Destructor  */
ShutdownPropertyOwner::~ShutdownPropertyOwner()
{
    delete _operationTimeout;
    delete _shutdownTimeout;
}

/**
Initialize the config properties.
*/
void ShutdownPropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        //
        // Initialize the properties with default values
        //
        if (String::equalNoCase(properties[i].propertyName, "operationTimeout"))
        {
            _operationTimeout->propertyName = properties[i].propertyName;
            _operationTimeout->defaultValue = properties[i].defaultValue;
            _operationTimeout->currentValue = properties[i].defaultValue;
            _operationTimeout->plannedValue = properties[i].defaultValue;
            _operationTimeout->dynamic = properties[i].dynamic;
            _operationTimeout->domain = properties[i].domain;
            _operationTimeout->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(properties[i].propertyName, "shutdownTimeout"))
        {
            _shutdownTimeout->propertyName = properties[i].propertyName;
            _shutdownTimeout->defaultValue = properties[i].defaultValue;
            _shutdownTimeout->currentValue = properties[i].defaultValue;
            _shutdownTimeout->plannedValue = properties[i].defaultValue;
            _shutdownTimeout->dynamic = properties[i].dynamic;
            _shutdownTimeout->domain = properties[i].domain;
            _shutdownTimeout->domainSize = properties[i].domainSize;
        }
    }
}

/** 
Get information about the specified property.
*/
void ShutdownPropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo)
{
    propertyInfo.clear();

    if (String::equalNoCase(_operationTimeout->propertyName, name))
    {
        propertyInfo.append(_operationTimeout->propertyName);
        propertyInfo.append(_operationTimeout->defaultValue);
        propertyInfo.append(_operationTimeout->currentValue);
        propertyInfo.append(_operationTimeout->plannedValue);
        if (_operationTimeout->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_shutdownTimeout->propertyName, name))
    {
        propertyInfo.append(_shutdownTimeout->propertyName);
        propertyInfo.append(_shutdownTimeout->defaultValue);
        propertyInfo.append(_shutdownTimeout->currentValue);
        propertyInfo.append(_shutdownTimeout->plannedValue);
        if (_shutdownTimeout->dynamic)
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
const String ShutdownPropertyOwner::getDefaultValue(const String& name)
{
    if (String::equalNoCase(_operationTimeout->propertyName, name))
    {
        return (_operationTimeout->defaultValue);
    }
    else if (String::equalNoCase(_shutdownTimeout->propertyName, name))
    {
        return (_shutdownTimeout->defaultValue);
    }
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get current value of the specified property.
*/
const String ShutdownPropertyOwner::getCurrentValue(const String& name)
{
    if (String::equalNoCase(_operationTimeout->propertyName, name))
    {
        return (_operationTimeout->currentValue);
    }
    else if (String::equalNoCase(_shutdownTimeout->propertyName, name))
    {
        return (_shutdownTimeout->currentValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get planned value of the specified property.
*/
const String ShutdownPropertyOwner::getPlannedValue(const String& name)
{
    if (String::equalNoCase(_operationTimeout->propertyName, name))
    {
        return (_operationTimeout->plannedValue);
    }
    else if (String::equalNoCase(_shutdownTimeout->propertyName, name))
    {
        return (_shutdownTimeout->plannedValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Init current value of the specified property to the specified value.
*/
void ShutdownPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{
    if (String::equal(value, EMPTY_VALUE))
    {
            throw InvalidPropertyValue(name,value);
    }

    // Perform validation
    if (String::equalNoCase(_operationTimeout->propertyName, name))
    {
	//
	// validate the timeout value
	//
	if ( isValid( name, value ) )
	{
            _operationTimeout->currentValue = value;
        }
    }
    else if (String::equalNoCase(_shutdownTimeout->propertyName, name))
    {
	//
	// validate the timeout value
	//
        _shutdownTimeout->currentValue = _shutdownTimeout->defaultValue;
        if ( isValid( name, value ) )
        {
            _shutdownTimeout->currentValue = value;
        }
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Init planned value of the specified property to the specified value.
*/
void ShutdownPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    if (String::equal(value, EMPTY_VALUE))
    {
            throw InvalidPropertyValue(name,value);
    }

    if (String::equalNoCase(_operationTimeout->propertyName, name))
    {
	if ( isValid ( name, value ) )
	{
            _operationTimeout->plannedValue= value;
        }
    }
    else if (String::equalNoCase(_shutdownTimeout->propertyName, name))
    {
        if ( isValid ( name, value ) )
        {
            _shutdownTimeout->plannedValue= value;
        }
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Update current value of the specified property to the specified value.
*/
void ShutdownPropertyOwner::updateCurrentValue(
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
    // no additional validations required for update, we will call
    // initCurrrentValue.
    //
    initCurrentValue(name, value);
}


/** 
Update planned value of the specified property to the specified value.
*/
void ShutdownPropertyOwner::updatePlannedValue(
    const String& name, 
    const String& value)
{
    //
    // Since the validations done in initPlannedValue are sufficient and
    // no additional validations required for update, we will call
    // initPlannedValue.
    //
    initPlannedValue(name, value);
}

/** 
Checks to see if the given value is valid or not.
*/
Boolean ShutdownPropertyOwner::isValid(const String& name, const String& value)
{
    //
    // convert timeout string to integer
    //
    char* tmp = value.allocateCString();
    long timeoutValue = strtol(tmp, (char **)0, 10);
    delete [] tmp;

    if (String::equalNoCase(_operationTimeout->propertyName, name))
    {
        // Check if the timeout value is greater than the minimum allowed
        //
        if ( timeoutValue > MIN_OPERATION_TIMEOUT )
        {
            return true;
        }
        else
        {
            return false; 
        }
    }
    else if (String::equalNoCase(_shutdownTimeout->propertyName, name))
    {
        // Check if the timeout value is greater than the minimum allowed
        //
        if ( timeoutValue > MIN_SHUTDOWN_TIMEOUT )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean ShutdownPropertyOwner::isDynamic(const String& name)
{
    if (String::equalNoCase(_operationTimeout->propertyName, name))
    {
        return (_operationTimeout->dynamic);
    }
    else if (String::equalNoCase(_shutdownTimeout->propertyName, name))
    {
        return (_shutdownTimeout->dynamic);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


PEGASUS_NAMESPACE_END
