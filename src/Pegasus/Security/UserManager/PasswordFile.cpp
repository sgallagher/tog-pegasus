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
// Author: Sushma Fernandes, Hewlett Packard Company (sushma_fernandes@hp.com)
//
// Modified By: Amit K Arora, IBM (amita@in.ibm.com) for Bug#1519
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for PEP#101
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for PEP#1791, Bug#3606
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cctype>
#include <fstream>

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>
#if defined(PEGASUS_OS_OS400)
#include "OS400ConvertChar.h"
#endif

#include <Pegasus/Security/UserManager/PasswordFile.h>
#include <Pegasus/Security/UserManager/UserExceptions.h>


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const char COLON = ':';


////////////////////////////////////////////////////////////////////////////////
//
//  PasswordFile Class
//
////////////////////////////////////////////////////////////////////////////////



/** 
    Constructor. 
*/
PasswordFile::PasswordFile (const String& fileName)
{
    PEG_METHOD_ENTER(TRC_USER_MANAGER, "PasswordFile::PasswordFile");

    _passwordFile       = fileName;

#ifdef PEGASUS_OS_VMS
    _passwordBackupFile = fileName + "_bak";
#else
    _passwordBackupFile = fileName + ".bak";
#endif

    try
    {
	PasswordTable pt;
        load(pt);
    }
    catch(const NoSuchFile&)
    {
    	//l10n
        //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
            //"Password file not found : $0.", _passwordFile);
        //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            //"Creating blank password file.");
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
            "Security.UserManager.PasswordFile.PWD_FILE_NOT_FOUND",
            "Password file not found : $0.", _passwordFile);
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            "Security.UserManager.PasswordFile.CREATING_BLANK_PWD_FILE",
            "Creating blank password file.");
        PasswordTable pt;
        save(pt);
    }
    PEG_METHOD_EXIT();
}

/** 
    Destructor. 
*/
PasswordFile::~PasswordFile ()
{
    PEG_METHOD_ENTER(TRC_USER_MANAGER, "PasswordFile::~PasswordFile");

    PEG_METHOD_EXIT();
}

/** 
    Load the username and password from the password file.
*/
void PasswordFile::load (PasswordTable& passwordTable)
{
    String line;

    PEG_METHOD_ENTER(TRC_USER_MANAGER, "PasswordFile::load");

    // 
    // Check if the backup file exists, if it does use the backup file
    // If not try to use the password file
    //
    if (FileSystem::exists(_passwordBackupFile))
    {
	if (FileSystem::exists(_passwordFile))
	{
	    if (! FileSystem::removeFile(_passwordFile))
	    {
		throw CannotRemoveFile(_passwordFile);
            }
        }
        //l10n
        //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            //"Trying to use the backup file : $0.", _passwordBackupFile);
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            "Security.UserManager.PasswordFile.TRYING_TO_BACKUP_FILE",
            "Trying to use the backup file : $0.", _passwordBackupFile);
	if (! FileSystem::renameFile(_passwordBackupFile, _passwordFile))
	{
			//l10n
            //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            //"Unable to use the backup file : $0.", _passwordBackupFile);
            Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            	"Security.UserManager.PasswordFile.CANNOT_USE_BACKUP_FILE",
            	"Unable to use the backup file : $0.", _passwordBackupFile);
	    throw CannotRenameFile(_passwordBackupFile);
        }
        //l10n
        //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            //"Recovered using the backup file : $0.", _passwordBackupFile);
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            "Security.UserManager.PasswordFile.RECOVERED_USING_BACKUP_FILE",
            "Recovered using the backup file : $0.", _passwordBackupFile);
    }
    if (! FileSystem::exists(_passwordFile))
    {
	throw NoSuchFile(_passwordFile);
    }

    //
    // Open the password file
    //
#if defined(PEGASUS_OS_OS400)
    ifstream ifs(_passwordFile.getCString(), PEGASUS_STD(_CCSID_T(1208)));
#else
    ifstream ifs(_passwordFile.getCString());
#endif
    if (!ifs)
    {
    	//l10n
        //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
            //"Error opening password file : $0.", _passwordFile);
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
            "Security.UserManager.PasswordFile.ERROR_OPENING_PWD_FILE",
            "Error opening password file : $0.", _passwordFile);
        return;
    }

    //
    // Read each line of the file
    //
    for (Uint32 lineNumber = 1; GetLine(ifs, line); lineNumber++)
    {
        // Get the userName and password

        //
        // Skip leading whitespace
        //
        const Char16* p = line.getChar16Data();

        while (*p && isspace(*p))
        {
            p++;
        }

        if (!*p)
        {
            continue;
        }

        //
        // Get the userName
        //
        String userName = String::EMPTY;

        userName.append(*p++);

        while (isalnum(*p))
        {
            userName.append(*p++);
        }

        //
        // Skip whitespace after user name
        //
        while (*p && isspace(*p))
        {
            p++;
        }

        //
        // Expect a colon sign
        //
        if (*p != COLON) 
        {
	    //
	    // Did not find Colon, log a message and skip entry
            //l10n
            //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            //"Error in reading password entry for : $0.", userName);
            Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            	"Security.UserManager.PasswordFile.ERROR_READING_PWD_ENTRY",
            	"Error in reading password entry for : $0.", userName);
	    continue;
        }

        p++;

        //
        // Skip whitespace after : sign
        //
        while (*p && isspace(*p))
        {
            p++;
        }

        //
        // Get the password
        //
        String password = String::EMPTY;

        while (*p)
        {
            password.append(*p++);
        }

        //
        // Store the user name and password in the table
        //
        if (!passwordTable.insert(userName, password))
        {
            //
            // Duplicate entry for user, ignore the new entry.
            //
            //l10n
            //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            //"Duplicate user: $0.", userName);
            Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            	"Security.UserManager.PasswordFile.DUPLICATE_USER",
            	"Duplicate user: $0.", userName);
        }
    }

    ifs.close();
    PEG_METHOD_EXIT();
}


/** 
    Save the username and password to the password file.
*/
void PasswordFile::save (const PasswordTable& passwordTable)
{
    PEG_METHOD_ENTER(TRC_USER_MANAGER, "PasswordFile::save");

    //
    // Check if backup password file exists, if it does remove the password file
    // If it does not rename the password file to password backup file
    //
    if (FileSystem::exists(_passwordBackupFile))
    {
	if ( FileSystem::exists(_passwordFile))
	{
            if ( ! FileSystem::removeFile(_passwordFile))
	    {
	    		//l10n
                //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                //"Cannot remove password file : $0.", _passwordFile);
                Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                	"Security.UserManager.PasswordFile.CANNOT_REMOVE_PWD_FILE",
                	"Cannot remove password file : $0.", _passwordFile);
	        throw CannotRemoveFile(_passwordFile);
            }
        }
    }
    else
    {
	if ( FileSystem::exists(_passwordFile))
	{
            if ( ! FileSystem::renameFile(_passwordFile, _passwordBackupFile))
	    {
	    		//l10n
                //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                //"Cannot rename password file : $0.", _passwordFile);
                Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                	"Security.UserManager.PasswordFile.CANNOT_RENAME_PWD_FILE",
                	"Cannot rename password file : $0.", _passwordFile);
	        throw CannotRenameFile(_passwordFile);
            }
        }
    }

    //
    // Open the password file for writing
    //
#if defined(PEGASUS_OS_OS400)
    ofstream ofs(_passwordFile.getCString(), PEGASUS_STD(_CCSID_T(1208)));
#else
    ofstream ofs(_passwordFile.getCString());
#endif
    if (!ofs)
    {
        PEG_METHOD_EXIT();
	throw CannotOpenFile(getFileName());
    }
	
    ofs.clear();

    //
    // Save user names and passwords to the new file
    //
    for (PasswordTable::Iterator i = passwordTable.start(); i; i++)
    {
        ofs << i.key() << ":" << i.value() << endl;
    }

    ofs.close();

    if ( FileSystem::exists(_passwordBackupFile))
    {
	if ( ! FileSystem::removeFile(_passwordBackupFile))
	{
		//l10n
            //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, 
	    //Logger::SEVERE,
          //  "Cannot remove backup password file : $0.",
	    //_passwordBackupFile);
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
            "Security.UserManager.PasswordFile.CANNOT_REMOVE_BACKUP_PWD_FILE",
            "Cannot remove backup password file : $0.", _passwordBackupFile);
	    throw CannotRemoveFile(_passwordBackupFile);
        }
    }
    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END

