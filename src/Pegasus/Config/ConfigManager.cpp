//%////////////////////////////////////////////////////////////////////////////
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
// Author: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By:
//
//%////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  ConfigManager 
/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Destroyer.h>
#include "ConfigExceptions.h"
#include "ConfigManager.h"

/////////////////////////////////////////////////////////////////////////////
//
//  When a new property owner is created be sure to include property 
//  owner header file here.
//
/////////////////////////////////////////////////////////////////////////////

#include "DefaultPropertyOwner.h"
#include "TracePropertyOwner.h"
#include "LogPropertyOwner.h"
#include "SecurityPropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


/////////////////////////////////////////////////////////////////////////////
//  PropertyOwnerTable 
/////////////////////////////////////////////////////////////////////////////

typedef HashTable<String,
    ConfigPropertyOwner*,EqualFunc<String>,HashFunc<String> > Table;

struct OwnerTable
{
    Table table;
};


/////////////////////////////////////////////////////////////////////////////
//
//  When a new property owner is created be sure to create a static
//  object of the new property owner.
//
/////////////////////////////////////////////////////////////////////////////

TracePropertyOwner*      ConfigManager::traceOwner   = new TracePropertyOwner;
LogPropertyOwner*        ConfigManager::logOwner     = new LogPropertyOwner;
DefaultPropertyOwner*    ConfigManager::defaultOwner = new DefaultPropertyOwner;
SecurityPropertyOwner*   ConfigManager::securityOwner= new SecurityPropertyOwner;

/////////////////////////////////////////////////////////////////////////////
//
//   When a new property is created be sure to add the new property name 
//   and the owner object to the PropertyList below.
//
/////////////////////////////////////////////////////////////////////////////

struct PropertyList ConfigManager::properties[] =
{
    {"traceLevel",          (ConfigPropertyOwner* )ConfigManager::traceOwner},
    {"traceFilePath",       (ConfigPropertyOwner* )ConfigManager::traceOwner},
    {"traceComponents",     (ConfigPropertyOwner* )ConfigManager::traceOwner},
    {"trace",               (ConfigPropertyOwner* )ConfigManager::logOwner},
    {"logtrace",            (ConfigPropertyOwner* )ConfigManager::logOwner},
    {"logdir",              (ConfigPropertyOwner* )ConfigManager::logOwner},
    {"cleanlogs",           (ConfigPropertyOwner* )ConfigManager::logOwner},
    {"severity",            (ConfigPropertyOwner* )ConfigManager::logOwner},
    {"port",                (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"home",                (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"daemon",              (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"install",             (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"remove",              (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"slp",                 (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"requireAuthentication", (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"httpAuthType",        (ConfigPropertyOwner* )ConfigManager::securityOwner}
};

const Uint32 NUM_PROPERTIES = 
    sizeof(ConfigManager::properties) / sizeof(ConfigManager::properties[0]);



/**
Initialize the constants representing the command line options.
*/
const char ConfigManager::OPTION_TRACE       = 't';

const char ConfigManager::OPTION_LOG_TRACE   = 'l';

const char ConfigManager::OPTION_DAEMON      = 'd';


/** 
Initialize ConfigManager instance 
*/
ConfigManager* ConfigManager::_instance = 0;


/** Constructor. */
ConfigManager::ConfigManager ()
{
    //
    // Initialize the instance variables
    //
    _propertyOwnerTable = new OwnerTable;

    //
    // Initialize the property owners
    //
    _initPropertyOwners();
}

/** Destructor. */
ConfigManager::~ConfigManager()
{
    if (_propertyOwnerTable)
    {
        delete _propertyOwnerTable;
    }

    if (_configFileHandler)
    {
        delete _configFileHandler;
    }
}


/** 
    Construct the singleton instance ConfigManager and return 
    a pointer to that instance.
*/
ConfigManager* ConfigManager::getInstance() 
{
    if (!_instance) 
    {
        _instance = new ConfigManager();
    }
    return _instance;
}


/** 
Update current value of a property.
*/
Boolean ConfigManager::updateCurrentValue(
    const String& name, 
    const String& value) 
{
    String prevValue = String::EMPTY;

    //
    // get property owner object from the config table.
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyOwnerTable->table.lookup(name, propertyOwner))
    {
        throw UnrecognizedConfigProperty(name); 
    }

    try
    {
        //
        // keep a copy of the existing config value
        //
        prevValue = propertyOwner->getCurrentValue(name);

        //
        // ask owner to update the current value to new value
        //
        propertyOwner->updateCurrentValue(name, value);

        //
        // update the new value in the current config file
        //
        if (!_configFileHandler->updateCurrentValue(name, value))
        {
            // Failed to update the current value, so roll back.
            propertyOwner->updateCurrentValue(name, prevValue);
            return 0;
        }
    }
    catch (NonDynamicConfigProperty& ndcp)
    {
        throw ndcp;
    }
    catch (InvalidPropertyValue& ipv)
    {
        throw ipv;
    }
    catch (UnrecognizedConfigProperty& ucp)
    {
        throw ucp;
    }
    return 1;
}


/** 
Update planned value of a property.
*/
Boolean ConfigManager::updatePlannedValue(
    const String& name, 
    const String& value)
{
    String prevValue = String::EMPTY;

    //
    // get property owner object from the config table.
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyOwnerTable->table.lookup(name, propertyOwner))
    {
        throw UnrecognizedConfigProperty(name); 
    }

    try
    {
        //
        // keep a copy of the existing config value
        //
        prevValue = propertyOwner->getPlannedValue(name);

        //
        // ask owner to update the planned value to new value
        //
        propertyOwner->updatePlannedValue(name, value);

        //
        // update the new value in the planned config file
        //
        if (!_configFileHandler->updatePlannedValue(name, value))
        {
            // Failed to update the planned value, so roll back.
            propertyOwner->updatePlannedValue(name, prevValue);
            return 0;
        }
    }
    catch (NonDynamicConfigProperty& ndcp)
    {
        throw ndcp;
    }
    catch (InvalidPropertyValue& ipv)
    {
        throw ipv;
    }
    catch (UnrecognizedConfigProperty& ucp)
    {
        throw ucp;
    }
    return 1;
}


/** 
Validate the value of a specified property.
*/
Boolean ConfigManager::validatePropertyValue(
    const String& name, 
    const String& value)
{
    //
    // get property owner object from config table
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyOwnerTable->table.lookup(name, propertyOwner))
    {
        throw UnrecognizedConfigProperty(name); 
    }

    return (propertyOwner->isValid(name, value));
}

/**
Get default value of the specified property.
*/
String ConfigManager::getDefaultValue(const String& name)
{
    //
    // get property owner object from config table
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyOwnerTable->table.lookup(name, propertyOwner))
    {
        throw UnrecognizedConfigProperty(name);
    }

    return (propertyOwner->getDefaultValue(name));
}

/** 
Get current value of the specified property.
*/
String ConfigManager::getCurrentValue(const String& name)
{
    //
    // get property owner object from config table
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyOwnerTable->table.lookup(name, propertyOwner))
    {
        throw UnrecognizedConfigProperty(name); 
    }

    return (propertyOwner->getCurrentValue(name));
}


/** 
Get planned value of the specified property.
*/
String ConfigManager::getPlannedValue(const String& name)
{
    //
    // get property owner object from config table
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyOwnerTable->table.lookup(name, propertyOwner))
    {
        throw UnrecognizedConfigProperty(name); 
    }

    return (propertyOwner->getPlannedValue(name));
}


/** 
Get all the attributes of the specified property.
*/
void ConfigManager::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo)
{
    //
    // get property owner object from config table
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyOwnerTable->table.lookup(name, propertyOwner))
    {
        throw UnrecognizedConfigProperty(name); 
    }

    propertyOwner->getPropertyInfo(name, propertyInfo);
}


/** 
Get a list of all property names.
*/
void ConfigManager::getAllPropertyNames(Array<String>& propertyNames)
{
    propertyNames.clear();

    for (Table::Iterator i = _propertyOwnerTable->table.start(); i; i++)
    {
        propertyNames.append(i.key());
    }
}


/** 
Merge the config properties from the specified planned config file
with the properties in the specified current config file.
*/
void ConfigManager::mergeConfigFiles(
    const String& currentFile, 
    const String& plannedFile)
{
    try
    {
        _configFileHandler = new ConfigFileHandler(currentFile, plannedFile);

    }
    catch (NoSuchFile& nsf)
    {
        throw nsf;
    }
    catch (FileNotReadable& fnr)
    {
        throw fnr;
    }

    _loadConfigProperties();
}


/** 
Merge the config properties from the default planned config file
with the properties in the default current config file.
*/
void ConfigManager::mergeConfigFiles()
{
    try
    {
        _configFileHandler = new ConfigFileHandler();

    }
    catch (NoSuchFile& nsf)
    {
        throw nsf;
    }
    catch (FileNotReadable& fnr)
    {
        throw fnr;
    }

    _loadConfigProperties();
}


/** 
Merge config properties specified on the command line 
*/
void ConfigManager::mergeCommandLine(int& argc, char**& argv)
{
    // Remove the command name from the command line
    memmove(&argv[0], &argv[1], (argc) * sizeof(char*));
    argc--;

    //
    //  Merge properties from the command line
    //
    for (Uint32 i = 0; i < argc; )
    {
        const char* arg = argv[i];

        // Check for -option
        if (*arg == '-')
        {
            // Get the option
            const char* option = arg + 1;

            if (*option == OPTION_TRACE)
            {
                _initPropertyWithCommandLineOption("trace=true");
            }
            else if (*option == OPTION_LOG_TRACE)
            {
                _initPropertyWithCommandLineOption("logtrace=true");
            }
            else if (*option == OPTION_DAEMON)
            {
                _initPropertyWithCommandLineOption("daemon=true");
            }
            else if (!strcmp(option,"install"))
            {
                _initPropertyWithCommandLineOption("install=true");
            }
            else if (!strcmp(option,"remove"))
            {
                _initPropertyWithCommandLineOption("remove=true");
            }
            else if (!strcmp(option,"cleanlogs"))
            {
                _initPropertyWithCommandLineOption("cleanlogs=true");
            }
            else if (!strcmp(option,"slp"))
            {
                _initPropertyWithCommandLineOption("slp=true");
            }
            else
            {
                throw UnrecognizedConfigProperty(arg);
            }
        }
        else 
        {
            // Get the config option
            //const char* configOption = argv[i];
            if (!_initPropertyWithCommandLineOption(arg))
            {
                throw UnrecognizedConfigProperty(arg); 
            }
        }

        // Remove the option from the command line
        memmove(&argv[i], &argv[i + 1], (argc-i) * sizeof(char*));
        argc--;
    }
}


/** 
load config properties from the file 
*/
void ConfigManager::_loadConfigProperties()
{
    //
    // copy the contents of planned config file over
    // the current config file
    //
    _configFileHandler->copyPlannedFileOverCurrentFile();

    //
    // load all the properties from the current and planned
    // config files in to tables.
    //
    _configFileHandler->loadAllConfigProperties();

    //
    // initialize all the property owners with the values
    // from the config files.
    //
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        String value = String::EMPTY;

        value.assign(_configFileHandler->getCurrentValue(
            properties[i].propertyName));

        //
        // initialize the current value of the property owner
        // with the value from the config file handler
        //
        if (value != String::EMPTY)
        {
            try
            {
                //
                // get property owner object from the config table.
                //
                ConfigPropertyOwner* propertyOwner;

                if (_propertyOwnerTable->table.lookup(
                    properties[i].propertyName, propertyOwner))
                {
                    propertyOwner->initCurrentValue(
                        properties[i].propertyName, value);

                    propertyOwner->initPlannedValue(
                        properties[i].propertyName, value);
                }
            }
            catch(UnrecognizedConfigProperty& ucp)
            {
                cerr << ucp.getMessage() <<
                    " : Default value is used." << endl;
                continue;
            }
            catch(InvalidPropertyValue& ipv)
            {
                cerr << ipv.getMessage() << 
                    " : Default value is used." << endl;
                continue;
            }
        }
    }

}


/** 
Initialize config property with the value specified 
in the command line.
*/
Boolean ConfigManager::_initPropertyWithCommandLineOption(
    const String& option) 
{
    ConfigPropertyOwner* propertyOwner = 0;

    String propertyName = String::EMPTY;
    String propertyValue = String::EMPTY;

    String configOption = option;

    Uint32 pos = configOption.find('=');
    if (pos == PEG_NOT_FOUND)
    {
        //
        // The property value was not specified
        //
        throw UnrecognizedConfigProperty(configOption); 
    }

    //
    // Get the property name and value
    //
    propertyName.append(configOption.subString(0, pos));
    propertyValue.append(configOption.subString(pos+1));

    //
    // get property owner object from the config table.
    //
    if (!_propertyOwnerTable->table.lookup(propertyName, propertyOwner))
    {
        throw UnrecognizedConfigProperty(propertyName); 
    }

    if (propertyOwner->isValid(propertyName, propertyValue))
    {
        //
        // update the value with the property owner
        //
        propertyOwner->initCurrentValue(propertyName, propertyValue);

        //
        // update the value in the current config file
        //
        return (_configFileHandler->updateCurrentValue(
            propertyName, propertyValue));
    }
    else
    {
        throw InvalidPropertyValue(propertyName, propertyValue);
    }

    return 0;
}

/** 
Initialize config property owners and add them to the property owner table
*/
void ConfigManager::_initPropertyOwners()
{
    //
    // add config property and its owner object to config table
    //
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        properties[i].propertyOwner->initialize();

        _propertyOwnerTable->table.insert(properties[i].propertyName,
            properties[i].propertyOwner);
    }
}

PEGASUS_NAMESPACE_END

