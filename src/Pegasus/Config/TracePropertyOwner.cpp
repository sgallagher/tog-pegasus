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
// This file has implementation for the trace property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Config/TracePropertyOwner.h>


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


/** Constructors  */
TracePropertyOwner::TracePropertyOwner()
{
    _traceLevel = new ConfigProperty;
    _traceFilePath = new ConfigProperty;
    _traceComponents = new ConfigProperty;

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
        const char* tmp = getenv("PEGASUS_HOME");

        String pegasusHome(tmp);
        pegasusHome.append("/logs/");

	// Create $PEGASUS_HOME/logs directory if it does not exist
        if (! FileSystem::isDirectory(pegasusHome))
	{
	    if (! FileSystem::makeDirectory(pegasusHome))
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
        Uint32 retCode = Tracer::setTraceFile(fileName.getPointer());

	// Check whether the filepath was set
	if ( retCode == 1 )
	{
	     Logger::put(Logger::DEBUG_LOG,"TracePropertyOwner",
	        Logger::WARNING,
	       "Unable to write to trace file $0",pegasusHome);
        }
	_traceFilePath->currentValue = pegasusHome;
    }
    if (_traceLevel->defaultValue != String::EMPTY)
    {
        if ( _traceLevel->defaultValue == "1")
        {
            Tracer::setTraceLevel(Tracer::LEVEL1);
        }
        else if ( _traceLevel->defaultValue == "2")
        {
            Tracer::setTraceLevel(Tracer::LEVEL2);
        }
        else if ( _traceLevel->defaultValue == "3")
        {
            Tracer::setTraceLevel(Tracer::LEVEL3);
        }
        else if ( _traceLevel->defaultValue == "4")
        {
            Tracer::setTraceLevel(Tracer::LEVEL4);
        }
    }
}

/** Destructor  */
TracePropertyOwner::~TracePropertyOwner()
{
    delete _traceLevel;
    delete _traceFilePath;
    delete _traceComponents;
}

/** 
Get information about the specified property.
*/
void TracePropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo) throw (UnrecognizedConfigProperty)
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
    throw (UnrecognizedConfigProperty)
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
    throw (UnrecognizedConfigProperty)
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
    throw (UnrecognizedConfigProperty)
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
    throw (UnrecognizedConfigProperty, InvalidPropertyValue)
{
    Uint32 retCode;

    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
        Tracer::setTraceComponents(value);
        _traceComponents->currentValue = value;
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
        String newValue = value;

        if (newValue == String::EMPTY)
        {
            newValue = _traceLevel->defaultValue;
        }
   
        if (newValue == "1")
        {
            Tracer::setTraceLevel(Tracer::LEVEL1);
        }
        else if (newValue == "2")
        {
            Tracer::setTraceLevel(Tracer::LEVEL2);
        }
        else if (newValue == "3")
        {
            Tracer::setTraceLevel(Tracer::LEVEL3);
        }
        else if (newValue == "4")
        {
            Tracer::setTraceLevel(Tracer::LEVEL4);
        }
        else
        {
            throw InvalidPropertyValue(name, value); 
        }
        _traceLevel->currentValue = newValue;
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
        String newValue = value;

        if (newValue == String::EMPTY)
        {
            newValue =  _traceFilePath->defaultValue;
        }

        ArrayDestroyer<char> fileName(newValue.allocateCString());
        retCode = Tracer::setTraceFile(fileName.getPointer());
        if (retCode == 1)
        {
            throw InvalidPropertyValue(name, newValue); 
        }
        _traceFilePath->currentValue = newValue;
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
    throw (UnrecognizedConfigProperty, InvalidPropertyValue)
{
    // Perform validation
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
        _traceComponents->plannedValue= value;
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
        if (value == String::EMPTY)
        {
            _traceLevel->plannedValue = _traceLevel->defaultValue;
        }
        // Validate Trace Level
        // Level should be between 1 and 4
        else if (value == "1" || value == "2" || value == "3" || value == "4")
        {
            _traceLevel->plannedValue= value;
        }
        else
        {
            throw InvalidPropertyValue(name, value); 
        }
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
        if (value == String::EMPTY)
        {
            _traceFilePath->plannedValue = _traceFilePath->defaultValue;
        }
        else
        {
            // Validate Trace File
            ofstream outFile;
    
            outFile.open(value.allocateCString(),ofstream::app);

            if (outFile.good())
            {
                _traceFilePath->plannedValue= value;
                outFile.close();
            }
            else
            {
                outFile.close();
                throw InvalidPropertyValue(name, value); 
            }
            _traceFilePath->plannedValue= value;
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
void TracePropertyOwner::updatePlannedValue(
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
Boolean TracePropertyOwner::isValid(const String& name, const String& value)
    throw (UnrecognizedConfigProperty)
{
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
        return 1;
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
        // Validate Trace Level
        // Level should be between 1 and 4
        // This validation should be updated incase if new levels are added
        // in future
        if (value == "1" || value == "2" || value == "3" || value == "4")
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
        // Validate trace file
        ofstream outFile;
        ArrayDestroyer<char> fileName(value.allocateCString());
        outFile.open(fileName.getPointer(),ofstream::app);

        if (outFile.good())
        {
            outFile.close();
            return 1;
        }
        else
        {
            outFile.close();
            return 0;
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
Boolean TracePropertyOwner::isDynamic(const String& name)
    throw (UnrecognizedConfigProperty)
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
