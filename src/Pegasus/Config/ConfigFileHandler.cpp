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
//
//%/////////////////////////////////////////////////////////////////////////////


#include <fstream>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Logger.h>
#include "ConfigFileHandler.h"


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
    const String& plannedFile,
    const Boolean offLine)
    : _offLine(offLine)
{
    String cFile = currentFile;

    String pFile = plannedFile;

    //
    // Initialize instance variables.
    //
    _currentConfFile = 0;
    _plannedConfFile = 0;

    _currentFileExist = true;
    _plannedFileExist = true;


    _currentConfFile = new ConfigFile(cFile);
    _plannedConfFile = new ConfigFile(pFile);

    _currentConfig = new ConfigTable;
    _plannedConfig = new ConfigTable;

    //
    // check whether the planned file exists or not
    //
    if (!FileSystem::exists(pFile))
    {
        _plannedFileExist = false;
        return;
    }

    //
    // check whether the file is readable or not
    //
    if (!FileSystem::canRead(pFile))
    {
        throw FileNotReadable(pFile);
    }

    //
    // check whether the current file exists or not
    //
    if (!FileSystem::exists(cFile))
    {
        _currentFileExist = false;
        //
        // Current file need not exist.
        // try creating one so that planned file contents
        // can be copied over.
        //
        ArrayDestroyer<char> p(cFile.allocateCString());
        ofstream ofs(p.getPointer());
        if (!ofs)
        {
            throw NoSuchFile(cFile);
        }
        ofs.close();
    }

    //
    // check whether the file is readable or not
    //
    if (!FileSystem::canRead(cFile))
    {
        throw FileNotReadable(cFile);
    }

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
{
    if (_plannedFileExist)
    {
        _currentConfFile->replace(_plannedConfFile->getFileName());
    }
    else if (_currentFileExist)
    {
        //
        // Remove the current file
        //
        FileSystem::removeFileNoCase(_currentConfFile->getFileName());
    }
}


/** 
    Load the config properties from the config files.
*/
void ConfigFileHandler::loadAllConfigProperties ()
{
    loadCurrentConfigProperties();

    loadPlannedConfigProperties();
}


/** 
    Load the config properties from the current config file.
*/
void ConfigFileHandler::loadCurrentConfigProperties ()
{
    if (_currentFileExist)
    {
        _currentConfFile->load(_currentConfig);
    }
}


/** 
    Load the config properties from the planned config file.
*/
void ConfigFileHandler::loadPlannedConfigProperties ()
{
    if (_plannedFileExist)
    {
        _plannedConfFile->load(_plannedConfig);
    }
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

    String newVal = value;
    if (String::equal(value, EMPTY_VALUE))
        newVal = "";

    if (!String::equal(value, ""))
    {
        // Store the new property name and value in to the table
        if (!_currentConfig->table.insert(name, newVal))
        {
            return false;
        }
    }

    try
    {
        // Store the new property in current config file.
        _currentConfFile->save(_currentConfig);
    }
    catch (CannotRenameFile& e)
    {
        //
        // Back up creation failed
        //
        Logger::put(Logger::ERROR_LOG, "CIMConfig", Logger::WARNING,
            "Renaming config file as backup config file failed: $0.", 
            e.getMessage());

        return false;
    }
    //
    // The current config file would now been created,
    // so set the flag to true.
    //
    _currentFileExist = true;

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

    String newVal = value;
    if (String::equal(value, EMPTY_VALUE))
        newVal = "";

    if (!String::equal(value, "")) 
    {
        //
        // Store the new property name and value in to the table
        //
        if (!_plannedConfig->table.insert(name, newVal))
        {
            return false;
        }
    }

    try
    {
        //
        // Planned file need not exist for off line
        // configuration setting update.
        //
        if (_offLine)
        {
            String pFile = _plannedConfFile->getFileName();

            ArrayDestroyer<char> p(pFile.allocateCString());
            ofstream ofs(p.getPointer());
            if (!ofs)
            {
                throw NoSuchFile(pFile);
            }
            ofs.close();
        }

        //
        // Store the new property in planned config file.
        //
        _plannedConfFile->save(_plannedConfig);

    }
    catch (CannotRenameFile& e)
    {
        //
        // Back up creation failed
        //
        Logger::put(Logger::ERROR_LOG, "CIMConfig", Logger::WARNING,
            "Renaming config file as backup config file failed: $0.", 
            e.getMessage());

        return false;
    }
    //
    // The planned config file would now been created,
    // so set the flag to true.
    //
    _plannedFileExist = true;

    return true;
}


/** 
    Get the current property value for the specified property name. 
*/
String ConfigFileHandler::getCurrentValue (const String& name)
{
    String value = String::EMPTY;

    if (_currentFileExist)
    {
        _currentConfig->table.lookup(name, value);
    }

    return (value);
}


/** 
    Get the planned property value for the specified property name. 
*/
String ConfigFileHandler::getPlannedValue (const String& name)
{
    String value = String::EMPTY;

    if (_plannedFileExist)
    {
        _plannedConfig->table.lookup(name, value);
    }

    return (value);
}


/** 
    Get all current property names.
*/
void ConfigFileHandler::getAllCurrentPropertyNames (Array<String>& propertyNames)
{
    propertyNames.clear();

    if (_currentFileExist)
    {
        for (Table::Iterator i = _currentConfig->table.start(); i; i++)
        {
            propertyNames.append(i.key());
        }
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

    if (_currentFileExist)
    {
        for (Table::Iterator i = _currentConfig->table.start(); i; i++)
        {
            propertyNames.append(i.key());
            propertyValues.append(i.value());
        }
    }
}


/** 
    Get all planned property names and values.
*/
void ConfigFileHandler::getAllPlannedPropertyNames (
    Array<String>& propertyNames)
{
    propertyNames.clear();

    if (_plannedFileExist)
    {
        for (Table::Iterator i = _plannedConfig->table.start(); i; i++)
        {
            propertyNames.append(i.key());
        }
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

    if (_plannedFileExist)
    {
        for (Table::Iterator i = _plannedConfig->table.start(); i; i++)
        {
            propertyNames.append(i.key());
            propertyValues.append(i.value());
        }
    }
}

PEGASUS_NAMESPACE_END

