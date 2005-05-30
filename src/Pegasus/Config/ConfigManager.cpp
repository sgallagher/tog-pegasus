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
// Author: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By:
//      Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//      Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//      Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//      Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//      Carol Ann Krug Graves, Hewlett-Packard Company (carolann_graves@hp.com)
//      Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//      Heather Sterling, IBM (hsterl@us.ibm.com)
//      Dave Sudlik, IBM (dsudlik@us.ibm.com)
//      Dave Rosckes (rosckes@us.ibm.com)
//      Chip Vincent (cvincent@us.ibm.com)
//      David Dillard, VERITAS Software Corp.
//          (david.dillard@veritas.com)
//      Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3612
//      Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3610
//
//%////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  ConfigManager
/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/FileSystem.h>

#include "ConfigExceptions.h"
#include "ConfigManager.h"

PEGASUS_NAMESPACE_BEGIN

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
ProviderDirPropertyOwner* ConfigManager::providerDirOwner= new ProviderDirPropertyOwner;
NormalizationPropertyOwner* ConfigManager::normalizationOwner = new NormalizationPropertyOwner;

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
#if !defined(PEGASUS_USE_SYSLOGS)
    {"logdir",              (ConfigPropertyOwner* )ConfigManager::logOwner},
#endif
    {"logLevel",            (ConfigPropertyOwner* )ConfigManager::logOwner},
    {"enableHttpConnection", (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"enableHttpsConnection",(ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"httpPort",            (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"httpsPort",           (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"home",                (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"daemon",              (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"slp",                 (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"enableAssociationTraversal", (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"enableIndicationService", (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    // Removed because unresolved PEP 66 KS{"maximumEnumerationBreadth", (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"enableAuthentication", (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"enableNamespaceAuthorization", (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"httpAuthType",        (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"passwordFilePath",    (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"sslCertificateFilePath", (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"sslKeyFilePath",      (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"sslTrustStore",      (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"exportSSLTrustStore",      (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"crlStore",      (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"sslClientVerificationMode", (ConfigPropertyOwner* )ConfigManager::securityOwner},
	{"sslTrustStoreUserName", (ConfigPropertyOwner* )ConfigManager::securityOwner},
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    {"kerberosServiceName", (ConfigPropertyOwner* )ConfigManager::securityOwner},
#endif
    {"repositoryIsDefaultInstanceProvider", (ConfigPropertyOwner* )ConfigManager::repositoryOwner},
    {"enableBinaryRepository", (ConfigPropertyOwner* )ConfigManager::repositoryOwner}, // PEP 164
    {"shutdownTimeout",     (ConfigPropertyOwner* )ConfigManager::shutdownOwner},
    {"repositoryDir",       (ConfigPropertyOwner* )ConfigManager::fileSystemOwner},
    {"providerDir",         (ConfigPropertyOwner* )ConfigManager::providerDirOwner},
    {"enableRemotePrivilegedUserAccess", (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"enableSubscriptionsForNonprivilegedUsers", (ConfigPropertyOwner* )ConfigManager::securityOwner},
    {"enableSSLExportClientVerification", (ConfigPropertyOwner* )ConfigManager::securityOwner},
#ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION
    {"authorizedUserGroups", (ConfigPropertyOwner* )ConfigManager::securityOwner},
#endif
    {"messageDir",          (ConfigPropertyOwner* )ConfigManager::fileSystemOwner},
#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    { "enableNormalization", (ConfigPropertyOwner *)ConfigManager::normalizationOwner },
    { "excludeModulesFromNormalization", (ConfigPropertyOwner *)ConfigManager::normalizationOwner },
#endif
    {"forceProviderProcesses", (ConfigPropertyOwner* )ConfigManager::defaultOwner},
    {"maxProviderProcesses", (ConfigPropertyOwner* )ConfigManager::defaultOwner}
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

//
// Initialize ConfigManager instance
//
ConfigManager* ConfigManager::_instance = 0;


/** Constructor. */
ConfigManager::ConfigManager()
    : useConfigFiles(false)
{
    //
    // Initialize the instance variables
    //
    _propertyTable.reset(new PropertyTable);

    //
    // Initialize the property owners
    //
    _initPropertyTable();
}

/**
 Terminate the ConfigManager
*/
void
ConfigManager::destroy( void )
{
   if (_instance)
   {
       delete _instance->traceOwner;
       delete _instance->logOwner;
       delete _instance->defaultOwner;
       delete _instance->securityOwner;
       delete _instance->repositoryOwner;
       delete _instance->shutdownOwner;
       delete _instance->fileSystemOwner;
       delete _instance->providerDirOwner;
       delete _instance->normalizationOwner;
       delete _instance;
       _instance = 0;	
   }
}
/**
    Get a reference to the singleton ConfigManager instance.  If no
    ConfigManager instance exists, construct one.
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
    Initialize the current value of a config property
*/
Boolean ConfigManager::initCurrentValue(
    const String& propertyName,
    const String& propertyValue)
{
    ConfigPropertyOwner* propertyOwner = 0;

    //
    // get property owner object from the config table.
    //
    if (!_propertyTable->ownerTable.lookup(propertyName, propertyOwner))
    {
        throw UnrecognizedConfigProperty(propertyName);
    }

    if (useConfigFiles && !propertyOwner->isValid(propertyName, propertyValue))
    {
        throw InvalidPropertyValue(propertyName, propertyValue);
    }

    //
    // update the value with the property owner
    //
    propertyOwner->initCurrentValue(propertyName, propertyValue);

    if (useConfigFiles)
    {
        //
        // update the value in the current config file
        //
        return (_configFileHandler->updateCurrentValue(
            propertyName, propertyValue, false));
    }

    return true;
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
        if (useConfigFiles && !propertyOwner->isValid(name, value))
        {
            throw InvalidPropertyValue(name, value);
        }

        propertyOwner->updateCurrentValue(name, value);
    }

    if (useConfigFiles)
    {
        //
        // update the new value in the current config file
        //
        if (!_configFileHandler->updateCurrentValue(name, value, unset))
        {
            // Failed to update the current value, so roll back.
            propertyOwner->updateCurrentValue(name, prevValue);
            return false;
        }
    }

    return true;
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
        if (useConfigFiles && !propertyOwner->isValid(name, value))
        {
            throw InvalidPropertyValue(name, value);
        }

        propertyOwner->updatePlannedValue(name, value);
    }

    if (useConfigFiles)
    {
        //
        // update the new value in the planned config file
        //
        if (!_configFileHandler->updatePlannedValue(name, value, unset))
        {
            // Failed to update the planned value, so roll back.
            propertyOwner->updatePlannedValue(name, prevValue);
            return false;
        }
    }

    return true;
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
String ConfigManager::getDefaultValue(const String& name) const
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
String ConfigManager::getCurrentValue(const String& name) const
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
String ConfigManager::getPlannedValue(const String& name) const
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
    Array<String>& propertyInfo) const
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
void ConfigManager::getAllPropertyNames(
    Array<String>& propertyNames,
    Boolean includeHiddenProperties) const
{
    Array<String> propertyInfo;
    propertyNames.clear();

    for (OwnerTable::Iterator i = _propertyTable->ownerTable.start(); i; i++)
    {
        if (includeHiddenProperties)
        {
            propertyNames.append(i.key());
        }
        else
        {
            //
            // Check if property is to be externally visible or not.
            // If the property should not be externally visible do not list the
            // property information.
            //
            propertyInfo.clear();
            getPropertyInfo(i.key(), propertyInfo);

            if (propertyInfo[5] == STRING_TRUE)
            {
                propertyNames.append(i.key());
            }
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
    PEGASUS_ASSERT(useConfigFiles);

    _configFileHandler.reset(new ConfigFileHandler(currentFile, plannedFile));

    _loadConfigProperties();
}


/**
Merge the config properties from the default planned config file
with the properties in the default current config file.
*/
void ConfigManager::mergeConfigFiles()
{
    PEGASUS_ASSERT(useConfigFiles);

    _configFileHandler.reset(new ConfigFileHandler());

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
    PEGASUS_ASSERT(useConfigFiles);

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

    Array<CIMName> propertyNames;
    Array<String>  propertyValues;

    _configFileHandler->getAllCurrentProperties(propertyNames, propertyValues);

    Uint32 size = propertyNames.size();

    //
    // initialize all the property owners with the values
    // from the config files.
    //
    for (Uint32 i = 0; i < size; i++)
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

            String propertyName = propertyNames[i].getString();

            if (_propertyTable->ownerTable.lookup(
                propertyName, propertyOwner))
            {
                if (propertyOwner->isValid(
                    propertyName, propertyValues[i]))
                {
                    propertyOwner->initCurrentValue(
                        propertyName, propertyValues[i]);

                    propertyOwner->initPlannedValue(
                        propertyName, propertyValues[i]);
                }
                else
                {
                    throw InvalidPropertyValue(propertyName, propertyValues[i]);
                }
            }
            else
            {
                throw UnrecognizedConfigProperty(propertyName);
            }
        }
        catch(UnrecognizedConfigProperty& ucp)
        {
            PEG_TRACE_STRING(TRC_CONFIG, Tracer::LEVEL2, ucp.getMessage());
            throw;
        }
        catch(InvalidPropertyValue& ipv)
        {
            PEG_TRACE_STRING(TRC_CONFIG, Tracer::LEVEL2, ipv.getMessage());
            throw;
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
    Uint32 pos = option.find('=');
    if (pos == PEG_NOT_FOUND)
    {
        //
        // The property value was not specified
        //
        throw UnrecognizedConfigProperty(option);
    }

    //
    // Get the property name and value
    //
    String propertyName = option.subString(0, pos);
    String propertyValue = option.subString(pos+1);

    return initCurrentValue(propertyName, propertyValue);
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
void ConfigManager::setPegasusHome(const String& home)
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

    String temp = value;
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
      if  (System::is_absolute_path((const char *)temp.subString(0,pos).getCString()))
      {
	homedPath.append(temp.subString(0,pos));
      } else
         homedPath.append( _pegasusHome + "/" + temp.subString(0, pos));

      if (token == 1)
	homedPath.append(FileSystem::getPathDelimiter());
      temp.remove(0,pos+token);
    }
    while ( temp.size() > 0 );

  }
  return homedPath;
}

PEGASUS_NAMESPACE_END

