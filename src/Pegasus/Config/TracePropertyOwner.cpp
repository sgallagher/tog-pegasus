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
//              Sushma Fernandes (sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the trace property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Destroyer.h>
#include "ConfigManager.h"
#include "TracePropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  TracePropertyOwner
//
//  When a new trace property is added, make sure to add the property name
//  and the default attributes of that property in the table below.
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
    {"traceLevel", "1", 1, 0, 0},
    {"traceFilePath", "cimom.trace", 1, 0, 0},
    {"traceComponents", "", 1, 0, 0},
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


// 
// Checks if the trace level is valid
//
Boolean isLevelValid(const String& traceLevel)
{
    //
    // Check if the level is valid
    //
    if ( traceLevel == "1" || traceLevel == "2" || 
	 traceLevel == "3" || traceLevel == "4")
    {
        return 1;
    }
    else
    {
        return 0; 
    }
}

//
// Get the appropriate trace level
//
Uint32 getTraceLevel(const String& traceLevel)
{
    if ( traceLevel == "1" )
    {
        return (Tracer::LEVEL1);
    }
    else if ( traceLevel == "2" )
    {
        return (Tracer::LEVEL2);
    }
    else if ( traceLevel == "3" )
    {
        return (Tracer::LEVEL3);
    }
    else 
    {
        return (Tracer::LEVEL4);
    }
}

/** Constructors  */
TracePropertyOwner::TracePropertyOwner()
{
    _traceLevel = new ConfigProperty;
    _traceFilePath = new ConfigProperty;
    _traceComponents = new ConfigProperty;
}

/** Destructor  */
TracePropertyOwner::~TracePropertyOwner()
{
    delete _traceLevel;
    delete _traceFilePath;
    delete _traceComponents;
}

/**
Initialize the config properties.
*/
void TracePropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        //
        // Initialize the properties with default values
        //
        if (String::equalNoCase(properties[i].propertyName, "traceComponents"))
        {
            _traceComponents->propertyName = properties[i].propertyName;
            _traceComponents->defaultValue = properties[i].defaultValue;
            _traceComponents->currentValue = properties[i].defaultValue;
            _traceComponents->plannedValue = properties[i].defaultValue;
            _traceComponents->dynamic = properties[i].dynamic;
            _traceComponents->domain = properties[i].domain;
            _traceComponents->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(properties[i].propertyName, "traceLevel"))
        {
            _traceLevel->propertyName = properties[i].propertyName;
            _traceLevel->defaultValue = properties[i].defaultValue;
            _traceLevel->currentValue = properties[i].defaultValue;
            _traceLevel->plannedValue = properties[i].defaultValue;
            _traceLevel->dynamic = properties[i].dynamic;
            _traceLevel->domain = properties[i].domain;
            _traceLevel->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(properties[i].propertyName, "traceFilePath"))
        {
            _traceFilePath->propertyName = properties[i].propertyName;
            _traceFilePath->defaultValue = properties[i].defaultValue;
            _traceFilePath->currentValue = properties[i].defaultValue;
            _traceFilePath->plannedValue = properties[i].defaultValue;
            _traceFilePath->dynamic = properties[i].dynamic;
            _traceFilePath->domain = properties[i].domain;
            _traceFilePath->domainSize = properties[i].domainSize;
        }
    }

    if (_traceFilePath->defaultValue != String::EMPTY)
    {
	// Get the value of environment variable PEGASUS_HOME to set the
	// default trace filepath
	// The default trace file location is <$PEGASUS_HOME/logs>
        String pegasusHome = ConfigManager::getHomedPath(ConfigManager::getPegasusHome() + String("/logs/"));

	// Create $PEGASUS_HOME/logs directory if it does not exist
	// If unable to create the directory then create the traceFile
	// in the current working directory
        if (!FileSystem::isDirectory(pegasusHome))
	{
	    if (!FileSystem::makeDirectory(pegasusHome))
	    {
	        Logger::put(Logger::DEBUG_LOG,"TracePropertyOwner",
		    Logger::WARNING,
	            "Unable to create $0 directory",pegasusHome);
	        Logger::put(Logger::DEBUG_LOG,"TracePropertyOwner",
		    Logger::WARNING,
	            "Creating the trace file in the current directory");
		pegasusHome = String::EMPTY; 
            }
        }
        pegasusHome += _traceFilePath->defaultValue;
        FileSystem::translateSlashes(pegasusHome);

        ArrayDestroyer<char> fileName(pegasusHome.allocateCString());
	if (Tracer::isValidFileName(fileName.getPointer()))
	{ 
            Uint32 retCode = Tracer::setTraceFile(fileName.getPointer());
	    // Check whether the filepath was set
	    if ( retCode == 1 )
	    {
	        Logger::put(Logger::DEBUG_LOG,"TracePropertyOwner",
	            Logger::WARNING,
	            "Unable to write to trace file $0",pegasusHome);

	        _traceFilePath->currentValue = "";
            }
	    else
	    {
	        _traceFilePath->currentValue = pegasusHome;
	    }
        }
    }
    if (_traceLevel->defaultValue != String::EMPTY)
    {
        if (_traceLevel->defaultValue == "1")
        {
            Tracer::setTraceLevel(Tracer::LEVEL1);
        }
        else if (_traceLevel->defaultValue == "2")
        {
            Tracer::setTraceLevel(Tracer::LEVEL2);
        }
        else if (_traceLevel->defaultValue == "3")
        {
            Tracer::setTraceLevel(Tracer::LEVEL3);
        }
        else if (_traceLevel->defaultValue == "4")
        {
            Tracer::setTraceLevel(Tracer::LEVEL4);
        }
    }
}

/** 
Get information about the specified property.
*/
void TracePropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo)
{
    propertyInfo.clear();

    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
        propertyInfo.append(_traceComponents->propertyName);
        propertyInfo.append(_traceComponents->defaultValue);
        propertyInfo.append(_traceComponents->currentValue);
        propertyInfo.append(_traceComponents->plannedValue);
        if (_traceComponents->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
        propertyInfo.append(_traceLevel->propertyName);
        propertyInfo.append(_traceLevel->defaultValue);
        propertyInfo.append(_traceLevel->currentValue);
        propertyInfo.append(_traceLevel->plannedValue);
        if (_traceLevel->dynamic)
        {
            propertyInfo.append(STRING_TRUE);
        }
        else
        {
            propertyInfo.append(STRING_FALSE);
        }
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
        propertyInfo.append(_traceFilePath->propertyName);
        propertyInfo.append(_traceFilePath->defaultValue);
        propertyInfo.append(_traceFilePath->currentValue);
        propertyInfo.append(_traceFilePath->plannedValue);
        if (_traceFilePath->dynamic)
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
const String TracePropertyOwner::getDefaultValue(const String& name)
{
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
        return (_traceComponents->defaultValue);
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
        return (_traceLevel->defaultValue);
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
        return (_traceFilePath->defaultValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get current value of the specified property.
*/
const String TracePropertyOwner::getCurrentValue(const String& name)
{
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
        return (_traceComponents->currentValue);
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
        return (_traceLevel->currentValue);
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
        return (_traceFilePath->currentValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get planned value of the specified property.
*/
const String TracePropertyOwner::getPlannedValue(const String& name)
{
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
        return (_traceComponents->plannedValue);
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
        return (_traceLevel->plannedValue);
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
        return (_traceFilePath->plannedValue);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Init current value of the specified property to the specified value.
*/
void TracePropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{
    // Perform validation
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
	//
	// Check if the trace components are valid;
	// if not set it to default value
	//
	String newValue = value;

        _traceComponents->currentValue = _traceComponents->defaultValue;
	if ( String::equal( newValue, EMPTY_VALUE ) )
	{
	    newValue = "";
        }
	if ( Tracer::isValidComponents ( newValue ) )
	{
            _traceComponents->currentValue = newValue;
        }

	Tracer::setTraceComponents(_traceComponents->currentValue);
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
	//
	// Check if the trace level is valid, if not set it to default value
	//
        _traceLevel->currentValue = _traceLevel->defaultValue;
        if ( isLevelValid( value ) )
        {
            _traceLevel->currentValue = value;
        }
	Uint32 traceLevel = getTraceLevel( _traceLevel->currentValue );
        Tracer::setTraceLevel(traceLevel);
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
	//
	// Check if the file path is valid, if not set it to default value
	//
	_traceFilePath->currentValue = _traceFilePath->defaultValue;

        ArrayDestroyer<char> fileName(value.allocateCString());
	if ( Tracer::isValidFileName( fileName.getPointer() ) )
        {
            _traceFilePath->currentValue = value;
        }

	Tracer::setTraceFile( fileName.getPointer() );
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Init planned value of the specified property to the specified value.
*/
void TracePropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    // Perform validation
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
	String newValue = value;

	//
	// Check if the trace components are valid;
	// if not set it to default value
	//
        _traceComponents->plannedValue= _traceComponents->defaultValue;

	if ( String::equal( newValue, EMPTY_VALUE) )
	{
	    newValue = "";
        }
	if ( Tracer::isValidComponents ( newValue ) )
	{
            _traceComponents->plannedValue= newValue;
        }
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
	//
	// Check if the trace level is valid, if not set it to default value
	//
        _traceLevel->plannedValue = _traceLevel->defaultValue;
        if ( isLevelValid( value ) )
        {
            _traceLevel->plannedValue= value;
        }
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
	//
	// Check if the file path is valid, if not set it to default value
	//
	_traceFilePath->plannedValue = _traceFilePath->defaultValue;

        ArrayDestroyer<char> fileName(value.allocateCString());
	if (Tracer::isValidFileName(fileName.getPointer()))
        {
            _traceFilePath->plannedValue = value;
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
void TracePropertyOwner::updateCurrentValue(
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
    // Perform validation
    //
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
	String newValue          = value;
	String invalidComponents = String::EMPTY;
	if ( String::equal( newValue, EMPTY_VALUE) )
	{
	    newValue = "";
        }
	//
	// Check if the components are valid, if not throw exception
	//
	if ( Tracer::isValidComponents ( newValue, invalidComponents ) )
        {
            _traceComponents->currentValue = newValue;
            Tracer::setTraceComponents(_traceComponents->currentValue);
        }
        else
        {
            throw InvalidPropertyValue(name,invalidComponents);
        } 
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
	//
	// Check if the trace level is valid, if not throw exception
	//
        if ( isLevelValid( value ) )
        {
            _traceLevel->currentValue = value;
            Tracer::setTraceLevel( getTraceLevel( value ) );
        }
	else 
	{
	    throw InvalidPropertyValue(name,value);
        }
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
	//
	// Check if the filepath are valid, if not throw exception
	//
        ArrayDestroyer<char> fileName(value.allocateCString());
        if (Tracer::isValidFileName(fileName.getPointer()))
        {
            _traceFilePath->currentValue = value;
	    Tracer::setTraceFile(fileName.getPointer());
        }
	else 
	{
	    throw InvalidPropertyValue(name,value);
        }
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Update planned value of the specified property to the specified value.
*/
void TracePropertyOwner::updatePlannedValue(
    const String& name, 
    const String& value)
{
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
	String newValue          = value;
	String invalidComponents = String::EMPTY;

	if ( String::equal( newValue, EMPTY_VALUE) )
	{
	    newValue = "";
        }
	//
	// Check if the components are valid, if not throw exception
	//
        if (Tracer::isValidComponents(newValue,invalidComponents))
        {
            _traceComponents->plannedValue = newValue;
        }
        else
        {
            throw InvalidPropertyValue(name,invalidComponents);
        }
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
	//
	// Check if the trace level is valid, if not throw exception
	//
        if ( isLevelValid( value ) )
        {
            _traceLevel->plannedValue = value;
        }
        else
        {
            throw InvalidPropertyValue(name,value);
        }
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
	//
	// Check if the file path is valid, if not throw exception
	//
        ArrayDestroyer<char> fileName(value.allocateCString());
        if (Tracer::isValidFileName(fileName.getPointer()))
        {
            _traceFilePath->plannedValue = value;
        }
        else
        {
            throw InvalidPropertyValue(name,value);
        }
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Checks to see if the given value is valid or not.
*/
Boolean TracePropertyOwner::isValid(const String& name, const String& value)
{
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
	String newValue          = value;
	String invalidComponents = String::EMPTY;

	if ( String::equal( newValue, EMPTY_VALUE) )
	{
	    newValue = "";
        }
	//
	// Check if the trace components are valid
	//
	if (!Tracer::isValidComponents(newValue,invalidComponents))
	{
	    throw InvalidPropertyValue(name, invalidComponents);
        }

        return 1;
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
	//
        // Check if the level is valid
	//
        if ( isLevelValid( value ) )
        {
            return 1;
        }
	else
	{
	    throw InvalidPropertyValue(name, value);
        }
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
	//
        // Check if the file path is valid
	//
        ArrayDestroyer<char> fileName(value.allocateCString());
	if (!Tracer::isValidFileName(fileName.getPointer()))
	{
	    throw InvalidPropertyValue(name, value);
	}
        return 1;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean TracePropertyOwner::isDynamic(const String& name)
{
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
        return (_traceComponents->dynamic);
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
        return (_traceLevel->dynamic);
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
        return (_traceFilePath->dynamic);
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


PEGASUS_NAMESPACE_END
