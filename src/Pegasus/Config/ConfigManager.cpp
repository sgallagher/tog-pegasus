//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//                  (sushma_fernandes@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
// Modified By: Dave Rosckes (rosckes@us.ibm.com)
//
//%////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  ConfigManager 
/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/FileSystem.h>

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
#include "RepositoryPropertyOwner.h"
#include "ShutdownPropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


/////////////////////////////////////////////////////////////////////////////
//  PropertyTable 
/////////////////////////////////////////////////////////////////////////////

typedef HashTable<String,
    ConfigPropertyOwner*,EqualFunc<String>,HashFunc<String> > OwnerTable;
typedef HashTable<String,
    const char*,EqualFunc<String>,HashFunc<String> > FixedValueTable;

struct PropertyTable
{
    OwnerTable ownerTable;
    FixedValueTable fixedValueTable;
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
RepositoryPropertyOwner* ConfigManager::repositoryOwner= new RepositoryPropertyOwner;
ShutdownPropertyOwner*   ConfigManager::shutdownOwner= new ShutdownPropertyOwner;
FileSystemPropertyOwner* ConfigManager::fileSystemOwner= new FileSystemPropertyOwner;

/////////////////////////////////////////////////////////////////////////////
//
//   When a new property is created be sure to add the new property name 
//   and the owner object to the OwnerEntry table below.
//
/////////////////////////////////////////////////////////////////////////////

struct OwnerEntry
{
    const char* propertyName;
    ConfigPropertyOwner* propertyOwner;
};

static struct OwnerEntry _properties[] =
{
    {"traceLevel",          (ConfigPropertyOwner* )ConfigManager::traceOwner},
    {"traceComponents",     (ConfigPropertyOwner* )ConfigManager::traceOwner},
    {"traceFilePath",       (ConfigPropertyOwner* )ConfigManager::traceOwner},
    {"logdir",              (ConfigPropertyOwner* )ConfigManager::logOwner},
    {"logLevel",            (ConfigPropertyOwner* )ConfigManager::logOwner},
    {"enableHttpConnection", (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"enableHttpsConnection",(ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"httpPort",            (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"httpsPort",           (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"home",                (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"daemon",              (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"slp",                 (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"tempLocalAuthDir",    (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"enableClientCertification", (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"enableAssociationTraversal", (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"enableIndicationService", (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    // Removed because unresolved PEP 66 KS{"maximumEnumerationBreadth", (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"enableAuthentication", (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"usePAMAuthentication", (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"enableNamespaceAuthorization", (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"httpAuthType",        (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"passwordFilePath",    (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"sslCertificateFilePath", (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"sslKeyFilePath",      (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"sslTrustFilePath",      (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"repositoryIsDefaultInstanceProvider", (ConfigPropertyOwner* )ConfigManager::repositoryOwner},
    {"shutdownTimeout",     (ConfigPropertyOwner* )ConfigManager::shutdownOwner},
    {"repositoryDir",       (ConfigPropertyOwner* )ConfigManager::fileSystemOwner},
    {"providerDir",         (ConfigPropertyOwner* )ConfigManager::fileSystemOwner},
    {"enableRemotePrivilegedUserAccess", (ConfigPropertyOwner* )ConfigManager::securityOwner},
};

const Uint32 NUM_PROPERTIES = sizeof(_properties) / sizeof(struct OwnerEntry);


/////////////////////////////////////////////////////////////////////////////
//
//   To use a fixed value for a property rather than delegating to a property
//   owner, add the property to the FixedValueEntry table below.  An entry in
//   the OwnerEntry table above for this same property will be initialized
//   and given the (fixed) initial current value, but will thereafter be
//   ignored.
//
//   Fixed values are only returned by getDefaultValue(), getCurrentValue(),
//   and getPlannedValue().  All other methods will treat fixed properties as
//   unrecognized properties.
//
/////////////////////////////////////////////////////////////////////////////

struct FixedValueEntry
{
    const char* propertyName;
    const char* fixedValue;
};

static struct FixedValueEntry _fixedValues[] =
{
#include "FixedPropertyTable.h"
};

const Uint32 NUM_FIXED_PROPERTIES = 
    sizeof(_fixedValues) / sizeof(struct FixedValueEntry);


/**
Initialize the default PEGASUS_HOME location, the default is set to the current directory.
*/
const String ConfigManager::PEGASUS_HOME_DEFAULT  = ".";

String ConfigManager::_pegasusHome = PEGASUS_HOME_DEFAULT;

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
    _propertyTable = new PropertyTable;

    //
    // Initialize the property owners
    //
    _initPropertyTable();
}

/** Destructor. */
ConfigManager::~ConfigManager()
{
    if (_propertyTable)
    {
        delete _propertyTable;
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
    const String& value,
    Boolean unset)
{
    String prevValue = String::EMPTY;

    //
    // get property owner object from the config table.
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyTable->ownerTable.lookup(name, propertyOwner))
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
        // ask owner to update the current value
        //
        if (unset)
        {
            propertyOwner->updateCurrentValue(name,
                propertyOwner->getDefaultValue(name));
        }
        else
        {
            if (propertyOwner->isValid(name, value))
            {
                propertyOwner->updateCurrentValue(name, value);
            }
            else
            {
                throw InvalidPropertyValue(name, value);
            }
        }

        //
        // update the new value in the current config file
        //
        if (!_configFileHandler->updateCurrentValue(name, value, unset))
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
    const String& value,
    Boolean unset)
{
    String prevValue = String::EMPTY;

    //
    // get property owner object from the config table.
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyTable->ownerTable.lookup(name, propertyOwner))
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
        if (unset)
        {
            propertyOwner->updatePlannedValue(name,
                propertyOwner->getDefaultValue(name));
        }
        else
        {
            if (propertyOwner->isValid(name, value))
            {
                propertyOwner->updatePlannedValue(name, value);
            }
            else
            {
                throw InvalidPropertyValue(name, value);
            }
        }

        //
        // update the new value in the planned config file
        //
        if (!_configFileHandler->updatePlannedValue(name, value, unset))
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

    if (!_propertyTable->ownerTable.lookup(name, propertyOwner))
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
    // Check for a property with a fixed value
    //
    const char* fixedValue;

    if (_propertyTable->fixedValueTable.lookup(name, fixedValue))
    {
        return fixedValue;
    }

    //
    // get property owner object from config table
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyTable->ownerTable.lookup(name, propertyOwner))
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
    // Check for a property with a fixed value
    //
    const char* fixedValue;

    if (_propertyTable->fixedValueTable.lookup(name, fixedValue))
    {
        return fixedValue;
    }

    //
    // get property owner object from config table
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyTable->ownerTable.lookup(name, propertyOwner))
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
    // Check for a property with a fixed value
    //
    const char* fixedValue;

    if (_propertyTable->fixedValueTable.lookup(name, fixedValue))
    {
        return fixedValue;
    }

    //
    // get property owner object from config table
    //
    ConfigPropertyOwner* propertyOwner;

    if (!_propertyTable->ownerTable.lookup(name, propertyOwner))
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

    if (!_propertyTable->ownerTable.lookup(name, propertyOwner))
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
    Array<String> propertyInfo;
    propertyNames.clear();

    for (OwnerTable::Iterator i = _propertyTable->ownerTable.start(); i; i++)
    {
        //
        // Check if property is to be externally visible or not.
        // If the property should not be externally visible do not list the
        // property information.
        //
        propertyInfo.clear();
        getPropertyInfo(i.key(), propertyInfo);

        if ( propertyInfo[5] == STRING_TRUE )
        {
            propertyNames.append(i.key());
        }
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
    if (argc > 0)
    {
        memmove(&argv[0], &argv[1], (argc) * sizeof(char*));
        argc--;
    }

    //
    //  Merge properties from the command line
    //
    for (Sint32 i = 0; i < argc; )
    {
        const char* arg = argv[i];

        if (*arg == '-')
        {
            throw UnrecognizedCommandLineOption();
        }

        // Get the config option
        //const char* configOption = argv[i];
        if (!_initPropertyWithCommandLineOption(arg))
        {
            throw UnrecognizedConfigProperty(arg); 
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

        if (_configFileHandler->getCurrentValue(
                CIMName (_properties[i].propertyName), value))
        {
            //
            // initialize the current value of the property owner
            // with the value from the config file handler
            //
            try
            {
                //
                // get property owner object from the config table.
                //
                ConfigPropertyOwner* propertyOwner;

                if (_propertyTable->ownerTable.lookup(
                    _properties[i].propertyName, propertyOwner))
                {
                    if (propertyOwner->isValid(
                        _properties[i].propertyName, value))
                    {
                        propertyOwner->initCurrentValue(
                            _properties[i].propertyName, value);

                        propertyOwner->initPlannedValue(
                            _properties[i].propertyName, value);
                    }
                    else
                    {
                        throw InvalidPropertyValue(_properties[i].propertyName,
                                                   value);
                    }
                }
            }
            catch(UnrecognizedConfigProperty& ucp)
            {
                PEG_TRACE_STRING(TRC_CONFIG, Tracer::LEVEL2, ucp.getMessage());
                continue;
            }
            catch(InvalidPropertyValue& ipv)
            {
                PEG_TRACE_STRING(TRC_CONFIG, Tracer::LEVEL2, 
                    ipv.getMessage() + " : default value is used.");
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
    if (!_propertyTable->ownerTable.lookup(propertyName, propertyOwner))
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
            propertyName, propertyValue, false));
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
void ConfigManager::_initPropertyTable()
{
    //
    // add config property and its fixed value to fixed value table
    //
    for (Uint32 i = 0; i < NUM_FIXED_PROPERTIES; i++)
    {
        _propertyTable->fixedValueTable.insert(_fixedValues[i].propertyName,
            _fixedValues[i].fixedValue);
    }

    //
    // add config property and its owner object to owners table (but only if
    // the property is not also listed in the fixed value table.
    //
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        const char* fixedValue;

        _properties[i].propertyOwner->initialize();

        if (!_propertyTable->fixedValueTable.lookup(
                _properties[i].propertyName, fixedValue))
        {
            _propertyTable->ownerTable.insert(_properties[i].propertyName,
                _properties[i].propertyOwner);
        }
        else
        {
            //
            // Set the value for the fixed properties
            //
            _properties[i].propertyOwner->initCurrentValue(
                _properties[i].propertyName, fixedValue);
        }
    }
}

/**
Get Pegasus Home
*/
String ConfigManager::getPegasusHome()
{
    return _pegasusHome;
}

/**
Set Pegasus Home variable
*/
void ConfigManager::setPegasusHome(String& home)
{
    if ( home != String::EMPTY )
    {
       _pegasusHome = home;
    }
}

/**
Get the homed path for a given property. 
*/
String ConfigManager::getHomedPath(const String& value)
{
  String homedPath = String::EMPTY;
  
  if ( value != String::EMPTY )
  {
    if ( System::is_absolute_path((const char *)value.getCString()) )
      {
        return value; 
      }

    //
    // Get the pegasusHome and prepend it
    //
    homedPath = _pegasusHome + "/" + value;
  }

  return homedPath;
}
     
PEGASUS_NAMESPACE_END

