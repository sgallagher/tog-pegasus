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
// Author: Konrad Rzeszutek <konradr@us.ibm.com>
//
// Modified By:  Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3614
//              Vijay Eli, IBM, (vijayeli@in.ibm.com) for Bug# 3613
//              Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3613
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the providerDir property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Config/ConfigManager.h>
#include "ProviderDirPropertyOwner.h"


PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  ProviderDirPropertyOwner
//
//  When a new ProviderDir property is added, make sure to add the property name
//  and the default attributes of that property in the table below.
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
    {"providerDir", "lib;bin", IS_STATIC, 0, 0, IS_VISIBLE},
#else
    {"providerDir", "lib", IS_STATIC, 0, 0, IS_VISIBLE},
#endif
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


/** Constructors  */
ProviderDirPropertyOwner::ProviderDirPropertyOwner()
{
    _providerDir = new ConfigProperty;
}

/** Destructor  */
ProviderDirPropertyOwner::~ProviderDirPropertyOwner()
{
    delete _providerDir;
}

/**
Checks if the given directory is existing and writable
*/
Boolean isProviderDirValid(const String& dirName)
{
      String temp = dirName;
      String path = String::EMPTY;
      Uint32 pos =0;
      Uint32 token=0;

      do {
	if (( pos = temp.find(FileSystem::getPathDelimiter())) == PEG_NOT_FOUND) {
		pos = temp.size();
		token = 0;
	}
	else {
		token = 1;
	}
	path = temp.subString(0,pos);
	if (FileSystem::canWrite(path)) {
		Logger::put_l(Logger::ERROR_LOG,System::CIMSERVER,
                        Logger::WARNING,
                        "$0 is writeable! Possible security risk.",
                        path);
	}
	if ( !FileSystem::isDirectory(path)  ||
	     !FileSystem::canRead(path)) {
		if (!FileSystem::isDirectory(path)) {
 			Logger::put_l(Logger::ERROR_LOG,System::CIMSERVER,
                        Logger::SEVERE,
                        "$0 is not a directory!",
                        path);
		}
		if (!FileSystem::canRead(path)) {
		 	Logger::put_l(Logger::ERROR_LOG,System::CIMSERVER,
                        Logger::SEVERE,
                        "Cannot $0 is not readable!",
                        path);
		}
		//cerr << "Not a good directory.\n";
		return false;
	}	
	temp.remove(0,pos+token);	
      }
      while ( temp.size() > 0 );	
      
    return true;
}
 
/**
Initialize the config properties.
*/
void ProviderDirPropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        //
        // Initialize the properties with default values
        //
        if (String::equalNoCase(properties[i].propertyName, "providerDir"
))
        {
            _providerDir->propertyName = properties[i].propertyName;
            _providerDir->defaultValue = properties[i].defaultValue;
            _providerDir->currentValue = properties[i].defaultValue;
            _providerDir->plannedValue = properties[i].defaultValue;
            _providerDir->dynamic = properties[i].dynamic;
            _providerDir->domain = properties[i].domain;
            _providerDir->domainSize = properties[i].domainSize;
            _providerDir->externallyVisible = properties[i].externallyVisible;
        }
    }
}

struct ConfigProperty* ProviderDirPropertyOwner::_lookupConfigProperty(
    const String& name) const
{
    if (String::equalNoCase(_providerDir->propertyName, name))
    {
        return _providerDir;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get information about the specified property.
*/
void ProviderDirPropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo) const
{
    propertyInfo.clear();
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

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
String ProviderDirPropertyOwner::getDefaultValue(const String& name) const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    return configProperty->defaultValue;
}

/** 
Get current value of the specified property.
*/
String ProviderDirPropertyOwner::getCurrentValue(const String& name) const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    return configProperty->currentValue;
}

/** 
Get planned value of the specified property.
*/
String ProviderDirPropertyOwner::getPlannedValue(const String& name) const
{
   struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    return configProperty->plannedValue;
}

/** 
Init current value of the specified property to the specified value.
*/
void ProviderDirPropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->currentValue = value;
}


/** 
Init planned value of the specified property to the specified value.
*/
void ProviderDirPropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->plannedValue = value;
}

/** 
Update current value of the specified property to the specified value.
*/
void ProviderDirPropertyOwner::updateCurrentValue(
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

    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->currentValue = value;
}


/** 
Update planned value of the specified property to the specified value.
*/
void ProviderDirPropertyOwner::updatePlannedValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->plannedValue = value;
}

/** 
Checks to see if the given value is valid or not.
*/
Boolean ProviderDirPropertyOwner::isValid(const String& name, 
                                const String& value) const
{

    if (!isProviderDirValid( value ))
    {
        throw InvalidPropertyValue(name, value);
    }
 
    return true;
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean ProviderDirPropertyOwner::isDynamic(const String& name) const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    return (configProperty->dynamic==IS_DYNAMIC);
}


PEGASUS_NAMESPACE_END
