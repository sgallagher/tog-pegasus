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


///////////////////////////////////////////////////////////////////////////////
// 
// This file defines the classes necessary to manage configuration properties
// specified on the commandline and configuration files for Pegasus. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ConfigManager_h
#define Pegasus_ConfigManager_h

#include <cctype>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Config/ConfigPropertyOwner.h>
#include <Pegasus/Config/ConfigFileHandler.h>

#include <Pegasus/Config/TracePropertyOwner.h>
#include <Pegasus/Config/LogPropertyOwner.h>
#include <Pegasus/Config/DefaultPropertyOwner.h>
#include <Pegasus/Config/SecurityPropertyOwner.h>
#include <Pegasus/Config/RepositoryPropertyOwner.h>


PEGASUS_NAMESPACE_BEGIN

/** PropertyList. */
struct PropertyList
{
    const char* propertyName;
    ConfigPropertyOwner* propertyOwner;
};


struct OwnerTable;

/**
  This class reads configuration properties from the config file, maps the 
  properties to owners, and implements access methods.
*/

class PEGASUS_CONFIG_LINKAGE ConfigManager
{

private:

    // This is meant to be a singleton, so the constructor
    // and the destructor are made private
    static ConfigManager* _instance;


    /** Constructor. */
    ConfigManager();


    /** Destructor. */
    ~ConfigManager();


    /** 
    Initialize config property with the value specified as a
    command line option.

    @param configOption    name and value of the command line option.

    @exception InvalidPropertyValue  if property value is not valid.
    @exception UnrecognizedConfigProperty  if property is not defined.
    */
    Boolean _initPropertyWithCommandLineOption(
        const String& configOption);
            //throw (InvalidPropertyValue, UnrecognizedConfigProperty);


    /** 
    load config properties from the file 

    @exception CannotRenameFile  if failed to rename the config file.
    @exception ConfigFileSyntaxError  if there are synatx error 
                            while parsing the config files.
    */
    void _loadConfigProperties();
        //throw (CannotRenameFile, ConfigFileSyntaxError);


    /**
    Initialize config property owners and add them to the property owner table
    */
    void _initPropertyOwners();


    /** 
    HashTable to store the config property names and 
    property owners 
    */
    OwnerTable* _propertyOwnerTable;

    /**
    Handler to access the config files.
    */
    ConfigFileHandler*    _configFileHandler;

public:

    /**
    Constants representing the command line options.
    */
    static const char OPTION_TRACE;

    static const char OPTION_LOG_TRACE;

    static const char OPTION_DAEMON;


    /**
    Property Owners

    When a new property owner is created be sure to add static
    variable pointers for each of the new property owner.
    */
    static TracePropertyOwner*      traceOwner;

    static LogPropertyOwner*        logOwner; 

    static DefaultPropertyOwner*    defaultOwner;

    static SecurityPropertyOwner*   securityOwner; 

    static RepositoryPropertyOwner* repositoryOwner; 

    /**
    Property list
    */
    static struct PropertyList properties[];

    /** 
    Construct the singleton instance of the ConfigManager and return a 
    pointer to that instance.
    */
    static ConfigManager* getInstance();


    /** 
    Update current value of a property.

    @param  propertyName  The name of the property to update (eg. "port").
    @param  propertyValue The new value of the property.  If the value is
                          null, the property should be reset to its default
                          value.
    @return true if the property found and updated, else false.

    @exception NonDynamicConfigProperty  if property is not dynamic.
    @exception UnrecognizedConfigProperty  if property is not defined.
    @exception InvalidPropertyValue  if property value is not valid.
    */
    Boolean updateCurrentValue(const String& name, const String& value);
        //throw (NonDynamicConfigProperty, InvalidPropertyValue, 
        //    UnrecognizedConfigProperty);

    /** 
    Update planned value of a property.

    @param  propertyName  The name of the property to update (eg. "port").
    @param  propertyValue The new value of the property.  If the value is
                          null, the property should be reset to its default
                          value.
    @return Boolean       True if the property found and updated.

    @exception NonDynamicConfigProperty  if property is not dynamic.
    @exception UnrecognizedConfigProperty  if property is not defined.
    @exception InvalidPropertyValue  if property value is not valid.
    */
    Boolean updatePlannedValue(const String& name, const String& value);
        //throw (NonDynamicConfigProperty, InvalidPropertyValue, 
        //    UnrecognizedConfigProperty);


    /** 
    Validate the value of a property.

    @param  name    The name of the property.
    @param  value   The value of the property to be validated. 
    @return true if the value of the property is valid, else false.

    @exception UnrecognizedConfigProperty  if property is not defined.
    */
    Boolean validatePropertyValue(const String& name, const String& value);
        //throw (UnrecognizedConfigProperty);

    /**
    Get default value of the specified property.

    @param  name    The name of the property.
    @return string containing the default value of the specified property.

    @exception UnrecognizedConfigProperty  if property is not defined.
    */
    String getDefaultValue(const String& name);
        //throw (UnrecognizedConfigProperty);


    /** 
    Get current value of the specified property.

    @param  name    The name of the property.
    @return string containing the current value of the specified property.

    @exception UnrecognizedConfigProperty  if property is not defined.
    */
    String getCurrentValue(const String& name);
        //throw (UnrecognizedConfigProperty);


    /** 
    Get planned value of the specified property.

    @param  name    The name of the property.
    @return string containing the current value of the specified property.

    @exception UnrecognizedConfigProperty  if property is not defined.
    */
    String getPlannedValue(const String& name);
        //throw (UnrecognizedConfigProperty);


    /** 
    Get all the attributes of the specified property.

    @param name          The name of the property.
    @param propertyInfo  List containing the property info.

    @exception UnrecognizedConfigProperty  if property is not defined.
    */
    void getPropertyInfo(const String& name, Array<String>& propertyInfo);
        //throw (UnrecognizedConfigProperty);


    /** 
    Get a list of all the property names.

    @param propertyNames  List containing all the property names.
    */
    void getAllPropertyNames(Array<String>& propertyNames);

    /** 
    Merges the config properties from the specified configuration files.

    @param currentFile   Name of file that contains current config properties.
    @param plannedFile   Name of file that contains planned config properties.

    @exception NoSuchFile  if the specified config file does not exist.
    @exception FileNotReadable  if the specified config file is not readable.
    @exception CannotRenameFile  if failed to rename the config file.
    @exception ConfigFileSyntaxError  if there is synatx error 
                            while parsing the config files.
    */
    void mergeConfigFiles(const String& currentFile, const String& plannedFile);
        //throw (NoSuchFile, FileNotReadable, CannotRenameFile, 
        //    ConfigFileSyntaxError);

    /** 
    Merge the config properties from the default planned config file
    with the properties in the default current config file.

    @exception NoSuchFile  if the default config file does not exist.
    @exception FileNotReadable  if the default config file is not readable.
    @exception CannotRenameFile  if failed to rename the config file.
    @exception ConfigFileSyntaxError  if there are synatx error 
                            while parsing the config files.
    */
    void mergeConfigFiles();
        //throw (NoSuchFile, FileNotReadable, CannotRenameFile, 
        //    ConfigFileSyntaxError);


    /** 
    Merge option values from the command line. 

    @param argc number of argument on the command line.
    @param argv list of command line arguments.

    @exception  InvalidPropertyValue if validation fails.
    @exception  UnrecognizedConfigProperty  if property is not defined.
    */
    void mergeCommandLine(int& argc, char**& argv);
        //throw (UnrecognizedConfigProperty, InvalidPropertyValue);

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ConfigManager_h */

