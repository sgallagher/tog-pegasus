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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the log property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Config/LogPropertyOwner.h>


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
    {"logs", "ALL", 0, 0, 0},
    {"severity", "ALL", 0, 0, 0}
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
LogPropertyOwner::LogPropertyOwner()
{
    _logtrace    = new ConfigProperty;
    _logdir        = new ConfigProperty;
    _cleanlogs    = new ConfigProperty;
    _logs        = new ConfigProperty;
    _severity    = new ConfigProperty;


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
        else if (String::equalNoCase(properties[i].propertyName, "logs"))
        {
            _logs->propertyName = properties[i].propertyName;
            _logs->defaultValue = properties[i].defaultValue;
            _logs->currentValue = properties[i].defaultValue;
            _logs->plannedValue = properties[i].defaultValue;
            _logs->dynamic = properties[i].dynamic;
            _logs->domain = properties[i].domain;
            _logs->domainSize = properties[i].domainSize;
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

/** Destructor  */
LogPropertyOwner::~LogPropertyOwner()
{
    delete _logtrace;
    delete _logdir;
    delete _cleanlogs;
    delete _logs;
    delete _severity;
}

/** 
Get information about the specified property.
*/
void LogPropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo) throw (UnrecognizedConfigProperty)
{
    propertyInfo.clear();

    if (String::equalNoCase(_logtrace->propertyName, name))
    {
        propertyInfo.append(_logtrace->propertyName);
        propertyInfo.append(_logtrace->defaultValue);
        propertyInfo.append(_logtrace->currentValue);
        propertyInfo.append(_logtrace->plannedValue);
        if (_logtrace->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_logdir->propertyName, name))
    {
        propertyInfo.append(_logdir->propertyName);
        propertyInfo.append(_logdir->defaultValue);
        propertyInfo.append(_logdir->currentValue);
        propertyInfo.append(_logdir->plannedValue);
        if (_logdir->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_cleanlogs->propertyName, name))
    {
        propertyInfo.append(_cleanlogs->propertyName);
        propertyInfo.append(_cleanlogs->defaultValue);
        propertyInfo.append(_cleanlogs->currentValue);
        propertyInfo.append(_cleanlogs->plannedValue);
        if (_cleanlogs->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_logs->propertyName, name))
    {
        propertyInfo.append(_logs->propertyName);
        propertyInfo.append(_logs->defaultValue);
        propertyInfo.append(_logs->currentValue);
        propertyInfo.append(_logs->plannedValue);
        if (_logs->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_severity->propertyName, name))
    {
        propertyInfo.append(_severity->propertyName);
        propertyInfo.append(_severity->defaultValue);
        propertyInfo.append(_severity->currentValue);
        propertyInfo.append(_severity->plannedValue);
        if (_severity->dynamic)
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
const String LogPropertyOwner::getDefaultValue(const String& name)
    throw (UnrecognizedConfigProperty)
{
    if (String::equalNoCase(_logtrace->propertyName, name))
    {
        return (_logtrace->defaultValue);
    }
    else if (String::equalNoCase(_logdir->propertyName, name))
    {
        return (_logdir->defaultValue);
    }
    else if (String::equalNoCase(_cleanlogs->propertyName, name))
    {
        return (_cleanlogs->defaultValue);
    }
    else if (String::equalNoCase(_logs->propertyName, name))
    {
        return (_logs->defaultValue);
    }
    else if (String::equalNoCase(_severity->propertyName, name))
    {
        return (_severity->defaultValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get current value of the specified property.
*/
const String LogPropertyOwner::getCurrentValue(const String& name)
    throw (UnrecognizedConfigProperty)
{
    if (String::equalNoCase(_logtrace->propertyName, name))
    {
        return (_logtrace->currentValue);
    }
    else if (String::equalNoCase(_logdir->propertyName, name))
    {
        return (_logdir->currentValue);
    }
    else if (String::equalNoCase(_cleanlogs->propertyName, name))
    {
        return (_cleanlogs->currentValue);
    }
    else if (String::equalNoCase(_logs->propertyName, name))
    {
        return (_logs->currentValue);
    }
    else if (String::equalNoCase(_severity->propertyName, name))
    {
        return (_severity->currentValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get planned value of the specified property.
*/
const String LogPropertyOwner::getPlannedValue(const String& name)
    throw (UnrecognizedConfigProperty)
{
    if (String::equalNoCase(_logtrace->propertyName, name))
    {
        return (_logtrace->plannedValue);
    }
    else if (String::equalNoCase(_logdir->propertyName, name))
    {
        return (_logdir->plannedValue);
    }
    else if (String::equalNoCase(_cleanlogs->propertyName, name))
    {
        return (_cleanlogs->plannedValue);
    }
    else if (String::equalNoCase(_logs->propertyName, name))
    {
        return (_logs->plannedValue);
    }
    else if (String::equalNoCase(_severity->propertyName, name))
    {
        return (_severity->plannedValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Init current value of the specified property to the specified value.
*/
void LogPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
    throw (UnrecognizedConfigProperty, InvalidPropertyValue)
{
    if (String::equalNoCase(_logtrace->propertyName, name))
    {
        _logtrace->currentValue = value;
    }
    else if (String::equalNoCase(_logdir->propertyName, name))
    {
        _logdir->currentValue = value;
    }
    else if (String::equalNoCase(_cleanlogs->propertyName, name))
    {
       _cleanlogs->currentValue = value;
    }
    else if (String::equalNoCase(_logs->propertyName, name))
    {
        _logs->currentValue = value;
    }
    else if (String::equalNoCase(_severity->propertyName, name))
    {
        _severity->currentValue = value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Init planned value of the specified property to the specified value.
*/
void LogPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
    throw (UnrecognizedConfigProperty, InvalidPropertyValue)
{
    if (String::equalNoCase(_logtrace->propertyName, name))
    {
        _logtrace->plannedValue= value;
    }
    else if (String::equalNoCase(_logdir->propertyName, name))
    {
        _logdir->plannedValue= value;
    }
    else if (String::equalNoCase(_cleanlogs->propertyName, name))
    {
       _cleanlogs->plannedValue= value;
    }
    else if (String::equalNoCase(_logs->propertyName, name))
    {
        _logs->plannedValue= value;
    }
    else if (String::equalNoCase(_severity->propertyName, name))
    {
        _severity->plannedValue= value;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Update current value of the specified property to the specified value.
*/
void LogPropertyOwner::updateCurrentValue(
    const String& name, 
    const String& value) 
    throw (NonDynamicConfigProperty, InvalidPropertyValue,
            UnrecognizedConfigProperty)
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
    throw (InvalidPropertyValue, UnrecognizedConfigProperty)
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
    throw (UnrecognizedConfigProperty)
{
    // TODO: Add validation code
    return 1;
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean LogPropertyOwner::isDynamic(const String& name)
    throw (UnrecognizedConfigProperty)
{
    if (String::equalNoCase(_logtrace->propertyName, name))
    {
        return (_logtrace->dynamic);
    }
    else if (String::equalNoCase(_logdir->propertyName, name))
    {
        return (_logdir->dynamic);
    }
    else if (String::equalNoCase(_cleanlogs->propertyName, name))
    {
        return (_cleanlogs->dynamic);
    }
    else if (String::equalNoCase(_logs->propertyName, name))
    {
        return (_logs->dynamic);
    }
    else if (String::equalNoCase(_severity->propertyName, name))
    {
        return (_severity->dynamic);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


PEGASUS_NAMESPACE_END
