//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:  
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the Tuning property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include "TuningPropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  TuningPropertyOwner
//
//  The shutdownTimeout property is the timeout value in seconds that the
//  cimom uses to wait for all the outstanding CIM operations to complete
//  before shutting down the cimserver.  The default value is 10 seconds.
//
//  When a new timeout property is added, make sure to add the property name
//  and the default attributes of that property in the table below.
//
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
   {"connections", "30", 1, 0, 0, 1},
   {"threads", "40", 1, 0, 0, 1  }
   
      
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);

static long MIN_SHUTDOWN_TIMEOUT = 2;

/** Constructor  */
TuningPropertyOwner::TuningPropertyOwner()
{
    _connections = new ConfigProperty;
    _threads = new ConfigProperty;
}

/** Destructor  */
TuningPropertyOwner::~TuningPropertyOwner()
{
    delete _threads;
    delete _connections;
    
}

/**
Initialize the config properties.
*/
void TuningPropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        //
        // Initialize the properties with default values
        //
        if (String::equalNoCase(properties[i].propertyName, "connections"))
        {
	   _connections->propertyName = properties[i].propertyName;
            _connections->defaultValue = properties[i].defaultValue;
            _connections->currentValue = properties[i].defaultValue;
            _connections->plannedValue = properties[i].defaultValue;
            _connections->dynamic = properties[i].dynamic;
            _connections->domain = properties[i].domain;
            _connections->domainSize = properties[i].domainSize;
            _connections->externallyVisible = properties[i].externallyVisible;
        }
	else if (String::equalNoCase(properties[i].propertyName, "threads"))
        {
	   _threads->propertyName = properties[i].propertyName;
            _threads->defaultValue = properties[i].defaultValue;
            _threads->currentValue = properties[i].defaultValue;
            _threads->plannedValue = properties[i].defaultValue;
            _threads->dynamic = properties[i].dynamic;
            _threads->domain = properties[i].domain;
            _threads->domainSize = properties[i].domainSize;
            _threads->externallyVisible = properties[i].externallyVisible;
        }
    }
}

struct ConfigProperty* TuningPropertyOwner::_lookupConfigProperty(
    const String& name)
{
    if (String::equalNoCase(_connections->propertyName, name))
    {
        return _connections;
    }
    else  if (String::equalNoCase(_threads->propertyName, name))
    {
        return _threads;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get information about the specified property.
*/
void TuningPropertyOwner::getPropertyInfo(
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
const String TuningPropertyOwner::getDefaultValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->defaultValue;
}

/** 
Get current value of the specified property.
*/
const String TuningPropertyOwner::getCurrentValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->currentValue;
}

/** 
Get planned value of the specified property.
*/
const String TuningPropertyOwner::getPlannedValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->plannedValue;
}

/** 
Init current value of the specified property to the specified value.
*/
void TuningPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->currentValue = value;
}


/** 
Init planned value of the specified property to the specified value.
*/
void TuningPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->plannedValue = value;
}

/** 
Update current value of the specified property to the specified value.
*/
void TuningPropertyOwner::updateCurrentValue(
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
void TuningPropertyOwner::updatePlannedValue(
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
Boolean TuningPropertyOwner::isValid(const String& name, const String& value)
{
    //
    // convert timeout string to integer
    //
    long timeoutValue = strtol(value.getCString(), (char **)0, 10);

    if (String::equalNoCase(_connections->propertyName, name))
    {
       return true;
    }
    else 
    if (String::equalNoCase(_threads->propertyName, name))
    {
       return true;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean TuningPropertyOwner::isDynamic(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->dynamic;
}


PEGASUS_NAMESPACE_END
