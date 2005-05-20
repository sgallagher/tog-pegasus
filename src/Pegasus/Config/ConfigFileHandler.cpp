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
// Modified By: Yi Zhou (yi_zhou@hp.com)
//            : Sushma Fernandes (sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Vijay Eli, IBM, (vijayeli@in.ibm.com), bug#3609.
//
//%/////////////////////////////////////////////////////////////////////////////


#include <fstream>
#include <errno.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Tracer.h>
#include "ConfigFileHandler.h"
#include "ConfigManager.h"
#if  defined(PEGASUS_OS_OS400)
#include "OS400ConvertChar.h"
#endif


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
    String cFile;
    String pFile;

    //
    // Set the current and planned config files
    //
    cFile = ConfigManager::getHomedPath(currentFile);

    pFile = ConfigManager::getHomedPath(plannedFile);

    //
    // Initialize instance variables.
    //
    
    _currentFileExist = true;
    _plannedFileExist = true;


    _currentConfFile.reset(new ConfigFile(cFile));
    _plannedConfFile.reset(new ConfigFile(pFile));

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
#if defined(PEGASUS_OS_OS400)
	ofstream ofs(cFile.getCString(), PEGASUS_STD(_CCSID_T(1208)));
#else
        ofstream ofs(cFile.getCString());
#endif
        if (!ofs)
        {
            // unable to create file
            PEG_TRACE_STRING(TRC_CONFIG, Tracer::LEVEL4,
                "Failed to create config file: " + cFile + ", " + strerror(errno));
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
        _currentFileExist = true;
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
    const CIMName& name, 
    const String& value,
    Boolean unset)
{
    // Remove the old property name and value from the table
    if (_currentConfig->table.contains(name.getString()))
    {
        if (!_currentConfig->table.remove(name.getString()))
        {
            return false;
        }
    }

    if (!unset)
    {
        // Store the new property name and value in to the table
        if (!_currentConfig->table.insert(name.getString(), value))
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
        // FUTURE: Log this message in a log file.
        //
        PEG_TRACE_STRING(TRC_CONFIG, Tracer::LEVEL3,
            "Backup configuration file creation failed: " + 
            e.getMessage() + ", " + strerror(errno));

        return false;
    }
    catch (CannotOpenFile& cof)
    {
        PEG_TRACE_STRING(TRC_CONFIG, Tracer::LEVEL3,
            "Setting permissions on current configuration file failed: " + 
            cof.getMessage() + ", " + strerror(errno));

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
    const CIMName& name, 
    const String& value,
    Boolean unset)
{
    //
    // Remove the old property name and value from the table
    //
    if (_plannedConfig->table.contains(name.getString()))
    {
        if (!_plannedConfig->table.remove(name.getString()))
        {
            return false;
        }
    }

    if (!unset)
    {
        //
        // Store the new property name and value in to the table
        //
        if (!_plannedConfig->table.insert(name.getString(), value))
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

#if defined(PEGASUS_OS_OS400)
	    ofstream ofs(pFile.getCString(), PEGASUS_STD(_CCSID_T(1208)));
#else
            ofstream ofs(pFile.getCString());
#endif
            if (!ofs)
            {
                PEG_TRACE_STRING(TRC_CONFIG, Tracer::LEVEL4,
                    "Failed to create config file: " + pFile + ", " + strerror(errno));
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
        // FUTURE: Log this message in a log file.
        //
        PEG_TRACE_STRING(TRC_CONFIG, Tracer::LEVEL3,
            "Backup configuration file creation failed: " + 
            e.getMessage() + ", " + strerror(errno));

        return false;
    }
    catch (CannotOpenFile& cof)
    {
        PEG_TRACE_STRING(TRC_CONFIG, Tracer::LEVEL3,
            "Setting permissions on planned configuration file failed: " + 
            cof.getMessage() + ", " + strerror(errno));

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
Boolean ConfigFileHandler::getCurrentValue (const CIMName& name, String& value)
const
{
    if (_currentFileExist)
    {
        return _currentConfig->table.lookup(name.getString(), value);
    }

    return false;
}


/** 
    Get the planned property value for the specified property name. 
*/
Boolean ConfigFileHandler::getPlannedValue (const CIMName& name, String& value)
const
{
    if (_plannedFileExist)
    {
        return _plannedConfig->table.lookup(name.getString(), value);
    }

    return false;
}


/** 
    Get all current property names.
*/
void ConfigFileHandler::getAllCurrentPropertyNames (Array<CIMName>& propertyNames)
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
    Array<CIMName>& propertyNames, 
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
    Array<CIMName>& propertyNames)
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
    Array<CIMName>& propertyNames, 
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

