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


#include <fstream>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Config/ConfigFileHandler.h>


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
//  ConfigFileHandler Class
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// ConfigTable 
////////////////////////////////////////////////////////////////////////////////

typedef HashTable<String, String, EqualFunc<String>, HashFunc<String> > Table;

struct ConfigTable
{
    Table table;
};


/** 
    Constructor.
*/
ConfigFileHandler::ConfigFileHandler (
    const String& currentFile, 
    const String& plannedFile) throw (NoSuchFile, FileNotReadable)
{
    String cFile = currentFile;

    String pFile = plannedFile;

    try
    {
        _currentConfFile = new ConfigFile(cFile);
    }
    catch (NoSuchFile nsf) 
    { 
        // Current file need not exist, so ignore this
        // exception. The planned file will get copied
        // on to the current file.
    }

    _plannedConfFile = new ConfigFile(pFile);

    _currentConfig = new ConfigTable;
    _plannedConfig = new ConfigTable;
}

/** 
    Destructor. 
*/
ConfigFileHandler::~ConfigFileHandler ()
{
    //
    // delete file handlers
    //
    delete _currentConfFile;
    delete _plannedConfFile;

    //
    // delete tables
    //
    delete _currentConfig;
    delete _plannedConfig;
}

/** 
    Overwrites config properties in the current config file with the 
    the config properties from the planned config file. 

    The content of the current config file will be copied in to a
    backup (.bak) file before copying planned file contents over the 
    current file.
*/
void ConfigFileHandler::copyPlannedFileOverCurrentFile()
    throw (CannotRenameFile)
{
    _currentConfFile->replace(_plannedConfFile->getFileName());
}


/** 
    Load the config properties from the config files.
*/
void ConfigFileHandler::loadAllConfigProperties ()
    throw (ConfigFileSyntaxError)
{
    loadCurrentConfigProperties();

    loadPlannedConfigProperties();
}


/** 
    Load the config properties from the current config file.
*/
void ConfigFileHandler::loadCurrentConfigProperties ()
    throw (ConfigFileSyntaxError)
{
    _currentConfFile->load(_currentConfig);
}


/** 
    Load the config properties from the planned config file.
*/
void ConfigFileHandler::loadPlannedConfigProperties ()
    throw (ConfigFileSyntaxError)
{
    _plannedConfFile->load(_plannedConfig);
}


/** 
    Update the specified property name and value in the current 
    config file.  
*/
Boolean ConfigFileHandler::updateCurrentValue (
    const String& name, 
    const String& value)
{
    // Remove the old property name and value from the table
    if (_currentConfig->table.contains(name))
    {
        if (!_currentConfig->table.remove(name))
        {
            return false;
        }
    }

    if (!String::equal(value, ""))
    {
        // Store the new property name and value in to the table
        if (!_currentConfig->table.insert(name, value))
        {
            return false;
        }
    }

    try
    {
        // Store the new property in current config file.
        _currentConfFile->save(_currentConfig);
    }
    catch (CannotRenameFile e)
    {
        //
        // Back up creation failed
        //
        Logger::put(Logger::ERROR_LOG, "CIMConfig", Logger::WARNING,
            "Renaming config file as backup config file failed: $0.", 
            e.getMessage());

        return false;
    }
    return true;
}


/** 
    Update the specified property name and value in the planned 
    config file.  
*/
Boolean ConfigFileHandler::updatePlannedValue (
    const String& name, 
    const String& value)
{
    //
    // Remove the old property name and value from the table
    //
    if (_plannedConfig->table.contains(name))
    {
        if (!_plannedConfig->table.remove(name))
        {
            return false;
        }
    }

    if (!String::equal(value, ""))
    {
        //
        // Store the new property name and value in to the table
        //
        if (!_plannedConfig->table.insert(name, value))
        {
            return false;
        }
    }

    try
    {
        //
        // Store the new property in current config file.
        //
        _plannedConfFile->save(_plannedConfig);
    }
    catch (CannotRenameFile e)
    {
        //
        // Back up creation failed
        //
        Logger::put(Logger::ERROR_LOG, "CIMConfig", Logger::WARNING,
            "Renaming config file as backup config file failed: $0.", 
            e.getMessage());

        return false;
    }

    return true;
}


/** 
    Get the current property value for the specified property name. 
*/
String ConfigFileHandler::getCurrentValue (const String& name)
{
    String value;

    _currentConfig->table.lookup(name, value);

    return (value);
}


/** 
    Get the planned property value for the specified property name. 
*/
String ConfigFileHandler::getPlannedValue (const String& name)
{
    String value;

    _plannedConfig->table.lookup(name, value);

    return (value);
}


/** 
    Get all current property names.
*/
void ConfigFileHandler::getAllCurrentPropertyNames (Array<String>& propertyNames)
{
    propertyNames.clear();

    for (Table::Iterator i = _currentConfig->table.start(); i; i++)
    {
        propertyNames.append(i.key());
    }
}


/** 
    Get all current property names and values.
*/
void ConfigFileHandler::getAllCurrentProperties (
    Array<String>& propertyNames, 
    Array<String>& propertyValues)
{
    propertyNames.clear();
    propertyValues.clear();

    for (Table::Iterator i = _currentConfig->table.start(); i; i++)
    {
        propertyNames.append(i.key());
        propertyValues.append(i.value());
    }
}


/** 
    Get all planned property names and values.
*/
void ConfigFileHandler::getAllPlannedPropertyNames (
    Array<String>& propertyNames)
{
    propertyNames.clear();

    for (Table::Iterator i = _plannedConfig->table.start(); i; i++)
    {
        propertyNames.append(i.key());
    }
}


/** 
    Get all planned config property names and values.
*/
void ConfigFileHandler::getAllPlannedProperties (
    Array<String>& propertyNames, 
    Array<String>& propertyValues)
{
    propertyNames.clear();
    propertyValues.clear();

    for (Table::Iterator i = _plannedConfig->table.start(); i; i++)
    {
        propertyNames.append(i.key());
        propertyValues.append(i.value());
    }
}

PEGASUS_NAMESPACE_END

