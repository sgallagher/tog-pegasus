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
// Author: Sushma Fernandes, Hewlett Packard Company (sushma_fernandes@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cctype>
#include <fstream>

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>

#include <Pegasus/Security/UserManager/PasswordFile.h>
#include <Pegasus/Security/UserManager/UserExceptions.h>


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const char COLON = ':';

/*
    Password file header information
*/
static const char* PasswordFileHeader [] = {
    "########################################################################",
    "##                                                                    ##",
    "##                  CIM Server Password file                          ##",
    "##                                                                    ##",
    "########################################################################",
    " ",    
    "########################################################################",
    "#                                                                      #",
    "# This is the password file for the CIMOM. The username/passowrd       #",
    "# in this file are loaded in to CIMOM by the User Manager Provider.    #",
    "# CIMOM updates this file with the changes .                           #",
    "#                                                                      #",
    "# The password file stores the user information in username:password   #",
    "# format in order to be compatible with Apache's htpasswd generated    #",
    "# password file.                                                       #",
    "#                                                                      #",
    "# The user must not edit this file, instead use                        #",
    "# cimuser CLI to make any changes to the CIMOM user information.       #",
    "#                                                                      #",
    "########################################################################",
    " "
};

static const int HEADER_SIZE = sizeof(PasswordFileHeader)/sizeof(PasswordFileHeader[0]);


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
    const char METHOD_NAME[] = "PasswordFile::PasswordFile";
    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    _passwordFile       = fileName;

    _passwordBackupFile = fileName + ".bak";

    try
    {
	PasswordTable pt;
        load(pt);
    }
    catch( NoSuchFile& e )
    {
        Logger::put(Logger::ERROR_LOG, "UserManager", Logger::SEVERE,
            "Password file not found : $0.", _passwordFile);
        Logger::put(Logger::ERROR_LOG, "UserManager", Logger::INFORMATION,
            "Creating blank password file.");
        PasswordTable pt;
        save(pt);
    }
    catch ( Exception& e)
    {
	throw e;
    }
    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}

/** 
    Destructor. 
*/
PasswordFile::~PasswordFile ()
{
    const char METHOD_NAME[] = "PasswordFile::~PasswordFile";
    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}

/** 
    Load the username and password from the password file.
*/
void PasswordFile::load (PasswordTable& passwordTable)
{
    String line;
    const char METHOD_NAME[] = "PasswordFile::load";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

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
        Logger::put(Logger::ERROR_LOG, "UserManager", Logger::INFORMATION,
            "Trying to use the backup file : $0.", _passwordBackupFile);
	if (! FileSystem::renameFile(_passwordBackupFile, _passwordFile))
	{
            Logger::put(Logger::ERROR_LOG, "UserManager", Logger::INFORMATION,
            "Unable to use the backup file : $0.", _passwordBackupFile);
	    throw CannotRenameFile(_passwordBackupFile);
        }
        Logger::put(Logger::ERROR_LOG, "UserManager", Logger::INFORMATION,
            "Recovered using the backup file : $0.", _passwordBackupFile);
    }
    if (! FileSystem::exists(_passwordFile))
    {
	throw NoSuchFile(_passwordFile);
    }

    //
    // Open the password file
    //
    ArrayDestroyer<char> p(_passwordFile.allocateCString());
    ifstream ifs(p.getPointer());
    if (!ifs)
    {
        Logger::put(Logger::ERROR_LOG, "UserManager", Logger::SEVERE,
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
        const Char16* p = line.getData();

        while (*p && isspace(*p))
        {
            p++;
        }

        if (!*p)
        {
            continue;
        }

        //
        // Skip comment lines
        //
        if (*p == '#')
        {
            continue;
        }

        //
        // Get the userName
        //
        String userName = String::EMPTY;

        userName += *p++;

        while (isalnum(*p))
        {
            userName += *p++;
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
            //
            Logger::put(Logger::ERROR_LOG, "CIMPassword", Logger::INFORMATION,
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
            password += *p++;
        }

        //
        // Store the user name and password in the table
        //
        if (!passwordTable.insert(userName, password))
        {
            //
            // Duplicate entry for user, ignore the new entry.
            //
            Logger::put(Logger::ERROR_LOG, "CIMPassword", Logger::INFORMATION,
            "Duplicate user: $0.", userName);
        }
    }

    ifs.close();
    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}


/** 
    Save the username and password to the password file.
*/
void PasswordFile::save (PasswordTable& passwordTable)
{
    const char METHOD_NAME[] = "PasswordFile::save";

    PEG_FUNC_ENTER(TRC_USER_MANAGER, METHOD_NAME);

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
                Logger::put(Logger::ERROR_LOG, "CIMPassword", Logger::SEVERE,
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
                Logger::put(Logger::ERROR_LOG, "CIMPassword", Logger::SEVERE,
                "Cannot rename password file : $0.", _passwordFile);
	        throw CannotRenameFile(_passwordFile);
            }
        }
    }

    //
    // Open the password file for writing
    //
    ArrayDestroyer<char> p(_passwordFile.allocateCString());
    ofstream ofs(p.getPointer());
    if (!ofs)
    {
        PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
	throw CannotOpenFile(getFileName());
    }
	
    ofs.clear();

    //
    // Write password file header information
    //
    
    for (int index = 0; index < HEADER_SIZE; index++)
    {
        ofs << PasswordFileHeader[index] << endl;
    }

    ofs << endl; 

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
            Logger::put(Logger::ERROR_LOG, "CIMPassword", 
	    Logger::SEVERE,
            "Cannot remove backup password file : $0.",
	    _passwordBackupFile);
	    throw CannotRemoveFile(_passwordBackupFile);
        }
    }
    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}

PEGASUS_NAMESPACE_END

