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
// This file has implementation for the log property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include "LogPropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  LogPropertyOwner
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
    {"logtrace", "false", 0, 0, 0},
    {"logdir", "./logs", 1, 0, 0},
    {"cleanlogs", "false", 0, 0, 0},
    {"trace", "false", 0, 0, 0},
    {"severity", "ALL", 0, 0, 0}
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
LogPropertyOwner::LogPropertyOwner()
{
    _logtrace    = new ConfigProperty;
    _logdir        = new ConfigProperty;
    _cleanlogs    = new ConfigProperty;
    _trace       = new ConfigProperty;
    _severity    = new ConfigProperty;
}

/** Destructor  */
LogPropertyOwner::~LogPropertyOwner()
{
    delete _logtrace;
    delete _logdir;
    delete _cleanlogs;
    delete _trace;
    delete _severity;
}


/**
Initialize the config properties.
*/
void LogPropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        //
        // Initialize the properties with default values
        //
        if (String::equalNoCase(properties[i].propertyName, "logtrace"))
        {
            _logtrace->propertyName = properties[i].propertyName;
            _logtrace->defaultValue = properties[i].defaultValue;
            _logtrace->currentValue = properties[i].defaultValue;
            _logtrace->plannedValue = properties[i].defaultValue;
            _logtrace->dynamic = properties[i].dynamic;
            _logtrace->domain = properties[i].domain;
            _logtrace->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(properties[i].propertyName, "logdir"))
        {
            _logdir->propertyName = properties[i].propertyName;
            _logdir->defaultValue = properties[i].defaultValue;
            _logdir->currentValue = properties[i].defaultValue;
            _logdir->plannedValue = properties[i].defaultValue;
            _logdir->dynamic = properties[i].dynamic;
            _logdir->domain = properties[i].domain;
            _logdir->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(properties[i].propertyName, "cleanlogs"))
        {
            _cleanlogs->propertyName = properties[i].propertyName;
            _cleanlogs->defaultValue = properties[i].defaultValue;
            _cleanlogs->currentValue = properties[i].defaultValue;
            _cleanlogs->plannedValue = properties[i].defaultValue;
            _cleanlogs->dynamic = properties[i].dynamic;
            _cleanlogs->domain = properties[i].domain;
            _cleanlogs->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(properties[i].propertyName, "trace"))
        {
            _trace->propertyName = properties[i].propertyName;
            _trace->defaultValue = properties[i].defaultValue;
            _trace->currentValue = properties[i].defaultValue;
            _trace->plannedValue = properties[i].defaultValue;
            _trace->dynamic = properties[i].dynamic;
            _trace->domain = properties[i].domain;
            _trace->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(properties[i].propertyName, "severity"))
        {
            _severity->propertyName = properties[i].propertyName;
            _severity->defaultValue = properties[i].defaultValue;
            _severity->currentValue = properties[i].defaultValue;
            _severity->plannedValue = properties[i].defaultValue;
            _severity->dynamic = properties[i].dynamic;
            _severity->domain = properties[i].domain;
            _severity->domainSize = properties[i].domainSize;
        }
    }
}

struct ConfigProperty* LogPropertyOwner::_lookupConfigProperty(
    const String& name)
{
    if (String::equalNoCase(_logtrace->propertyName, name))
    {
        return _logtrace;
    }
    else if (String::equalNoCase(_logdir->propertyName, name))
    {
        return _logdir;
    }
    else if (String::equalNoCase(_cleanlogs->propertyName, name))
    {
        return _cleanlogs;
    }
    else if (String::equalNoCase(_trace->propertyName, name))
    {
        return _trace;
    }
    else if (String::equalNoCase(_severity->propertyName, name))
    {
        return _severity;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get information about the specified property.
*/
void LogPropertyOwner::getPropertyInfo(
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
const String LogPropertyOwner::getDefaultValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->defaultValue;
}

/** 
Get current value of the specified property.
*/
const String LogPropertyOwner::getCurrentValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->currentValue;
}

/** 
Get planned value of the specified property.
*/
const String LogPropertyOwner::getPlannedValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->plannedValue;
}


/** 
Init current value of the specified property to the specified value.
*/
void LogPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->currentValue = value;
}


/** 
Init planned value of the specified property to the specified value.
*/
void LogPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->plannedValue = value;
}

/** 
Update current value of the specified property to the specified value.
*/
void LogPropertyOwner::updateCurrentValue(
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
void LogPropertyOwner::updatePlannedValue(
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
Boolean LogPropertyOwner::isValid(const String& name, const String& value)
{
    // ATTN: Add validation code
    return 1;
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean LogPropertyOwner::isDynamic(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->dynamic;
}


PEGASUS_NAMESPACE_END
