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
#include <Pegasus/Config/ConfigExceptions.h>
#include <Pegasus/Config/ConfigManager.h>

/////////////////////////////////////////////////////////////////////////////
//
//  When a new property owner is created be sure to include property 
//  owner header file here.
//
/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Config/DefaultPropertyOwner.h>
#include <Pegasus/Config/TracePropertyOwner.h>
#include <Pegasus/Config/LogPropertyOwner.h>


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

static TracePropertyOwner*      traceOwner   = new TracePropertyOwner;
static LogPropertyOwner*        logOwner     = new LogPropertyOwner;
static DefaultPropertyOwner*    defaultOwner = new DefaultPropertyOwner;


/////////////////////////////////////////////////////////////////////////////
//
//   When a new property is created be sure to add the new property name 
//   and the owner object to the PropertyList below.
//
/////////////////////////////////////////////////////////////////////////////

static struct PropertyList properties[] =
{   
    {"traceLevel",          (ConfigPropertyOwner* )traceOwner},
    {"traceFilePath",       (ConfigPropertyOwner* )traceOwner},
    {"traceComponents",     (ConfigPropertyOwner* )traceOwner},
    {"logtrace",            (ConfigPropertyOwner* )logOwner},
    {"logdir",              (ConfigPropertyOwner* )logOwner},
    {"cleanlogs",           (ConfigPropertyOwner* )logOwner},
    {"logs",                (ConfigPropertyOwner* )logOwner},
    {"severity",            (ConfigPropertyOwner* )logOwner},
    {"port",                (ConfigPropertyOwner* )defaultOwner},
    {"options",             (ConfigPropertyOwner* )defaultOwner},
    {"daemon",              (ConfigPropertyOwner* )defaultOwner},
    {"install",             (ConfigPropertyOwner* )defaultOwner},
    {"remove",              (ConfigPropertyOwner* )defaultOwner},
    {"slp",                 (ConfigPropertyOwner* )defaultOwner}
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);



/**
Constants representing the command line options.
*/
static const char OPTION_VERSION     = 'v';

static const char OPTION_HELP        = 'h';

static const char OPTION_TRACE       = 't';

static const char OPTION_LOG_TRACE   = 'l';


/** Initialize. */
ConfigManager* ConfigManager::_instance = 0;


/** Constructor. */
ConfigManager::ConfigManager ()
{
    //
    // Initialize the instance variables
    //
    _propertyOwnerTable = new OwnerTable;
    _trace = false;
    _logTrace = false;
    _version = false;
    _help = false;
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
    throw (NonDynamicConfigProperty, InvalidPropertyValue, 
            UnrecognizedConfigProperty)
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
    catch (NonDynamicConfigProperty ndcp)
    {
        throw ndcp;
    }
    catch (InvalidPropertyValue ipv)
    {
        throw ipv;
    }
    catch (UnrecognizedConfigProperty ucp)
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
    throw (NonDynamicConfigProperty, InvalidPropertyValue, 
            UnrecognizedConfigProperty)
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
    catch (NonDynamicConfigProperty ndcp)
    {
        throw ndcp;
    }
    catch (InvalidPropertyValue ipv)
    {
        throw ipv;
    }
    catch (UnrecognizedConfigProperty ucp)
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
    throw (UnrecognizedConfigProperty)
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
Get current value of the specified property.
*/
String ConfigManager::getCurrentValue(const String& name)
    throw (UnrecognizedConfigProperty)
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
    throw (UnrecognizedConfigProperty)
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
    throw (UnrecognizedConfigProperty)
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
    throw (NoSuchFile, FileNotReadable, CannotRenameFile, 
        ConfigFileSyntaxError)
{
    try
    {
        _configFileHandler = new ConfigFileHandler(currentFile, plannedFile);

        _loadConfigProperties();
    }
    catch (NoSuchFile nsf)
    {
        throw nsf;
    }
    catch (FileNotReadable fnr)
    {
        throw fnr;
    }
    catch (CannotRenameFile ftrf)
    {
        throw ftrf;
    }
    catch (ConfigFileSyntaxError cfse)
    {
        throw cfse;
    }
}


/** 
Merge the config properties from the default planned config file
with the properties in the default current config file.
*/
void ConfigManager::mergeConfigFiles()
    throw (NoSuchFile, FileNotReadable, CannotRenameFile, 
        ConfigFileSyntaxError)
{
    try
    {
        _configFileHandler = new ConfigFileHandler();

        _loadConfigProperties();
    }
    catch (NoSuchFile nsf)
    {
        throw nsf;
    }
    catch (FileNotReadable fnr)
    {
        throw fnr;
    }
    catch (CannotRenameFile ftrf)
    {
        throw ftrf;
    }
    catch (ConfigFileSyntaxError cfse)
    {
        throw cfse;
    }
}


/** 
Merge config properties specified on the command line 
*/
void ConfigManager::mergeCommandLine(int& argc, char**& argv)
    throw (UnrecognizedConfigProperty, MissingCommandLineOptionArgument,
        InvalidPropertyValue)
{
    Boolean booleanOption;


    for (int i = 1; i < argc; )
    {
        const char* arg = argv[i];

        // Check for -option
        if (*arg == '-')
        {
            // Get the option
            const char* option = arg + 1;

            if (*option == OPTION_VERSION)
            {
                _version = true;
            }
            else if (*option == OPTION_HELP)
            {
                _help = true;
            }
            else if (*option == OPTION_TRACE)
            {
                _trace = true;
            }
            else if (*option == OPTION_LOG_TRACE)
            {
                _logTrace = true;
            }
            else
            {
                throw UnrecognizedConfigProperty(option);
            }
        }
        else
        {
            // Get the config option
            const char* configOption = argv[i];

            if (!_initPropertyWithCommandLineOption(configOption))
            {
                throw UnrecognizedConfigProperty(configOption); 
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
    throw (CannotRenameFile, ConfigFileSyntaxError)
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
    // initialize all the property owners and add them to 
    // the config table.
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
        //if (!String::equal(value, String::EMPTY))
        if (value != String::EMPTY)
        {
            properties[i].propertyOwner->initCurrentValue(
                properties[i].propertyName, value);
        }

        //
        // add config property and its owner object to config table
        //
        _propertyOwnerTable->table.insert(properties[i].propertyName,
            properties[i].propertyOwner);
    }
}


/** 
Initialize config property with the value specified 
in the command line.
*/
Boolean ConfigManager::_initPropertyWithCommandLineOption(
    const String& option) 
        throw (InvalidPropertyValue, UnrecognizedConfigProperty)
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
Check if the help flag is set or not.
*/
Boolean ConfigManager::isHelpFlagSet()
{
    return _help;
}


/**
Check if the version flag is set or not.
*/
Boolean ConfigManager::isVersionFlagSet()
{
    return _version;
}


/**
Check if the trace flag is set or not.
*/
Boolean ConfigManager::isTraceFlagSet()
{
    return _trace;
}


/**
Check if the log trace flag is set or not.
*/
Boolean ConfigManager::isLogTraceFlagSet()
{
    return _logTrace;
}


PEGASUS_NAMESPACE_END

