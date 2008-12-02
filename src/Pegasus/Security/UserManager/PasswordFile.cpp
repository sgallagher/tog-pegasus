//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cctype>
#include <fstream>

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Executor.h>

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
PasswordFile::PasswordFile(const String& fileName)
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
    catch (const NoSuchFile&)
    {
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
            MessageLoaderParms(
                "Security.UserManager.PasswordFile.PWD_FILE_NOT_FOUND",
                "Password file not found : $0.", _passwordFile));
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            MessageLoaderParms(
                "Security.UserManager.PasswordFile.CREATING_BLANK_PWD_FILE",
                "Creating blank password file."));
        PasswordTable pt;
        save(pt);
    }
    PEG_METHOD_EXIT();
}

/**
    Destructor.
*/
PasswordFile::~PasswordFile()
{
    PEG_METHOD_ENTER(TRC_USER_MANAGER, "PasswordFile::~PasswordFile");

    PEG_METHOD_EXIT();
}

/**
    Load the username and password from the password file.
*/
void PasswordFile::load(PasswordTable& passwordTable)
{
    String line;

    PEG_METHOD_ENTER(TRC_USER_MANAGER, "PasswordFile::load");

    //
    // Check if the backup file exists, if it does use the backup file
    // If not try to use the password file
    //
    if (FileSystem::exists(_passwordBackupFile))
    {
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            MessageLoaderParms(
                "Security.UserManager.PasswordFile.TRYING_TO_BACKUP_FILE",
                "Trying to use the backup file : $0.",
                    _passwordBackupFile));
        if (Executor::renameFile(
                _passwordBackupFile.getCString(),
                _passwordFile.getCString()) != 0)
        {
            Logger::put_l(
                Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
                MessageLoaderParms(
                    "Security.UserManager.PasswordFile.CANNOT_USE_BACKUP_FILE",
                    "Unable to use the backup file : $0.",
                    _passwordBackupFile));
            throw CannotRenameFile(_passwordBackupFile);
        }
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            MessageLoaderParms(
                "Security.UserManager.PasswordFile.RECOVERED_USING_BACKUP_FILE",
                "Recovered using the backup file : $0.",
                _passwordBackupFile));
    }
    if (!FileSystem::exists(_passwordFile))
    {
        throw NoSuchFile(_passwordFile);
    }

    //
    // Open the password file
    //
    ifstream ifs(_passwordFile.getCString());

    if (!ifs)
    {
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
            MessageLoaderParms(
                "Security.UserManager.PasswordFile.ERROR_OPENING_PWD_FILE",
                "Error opening password file : $0.",
                _passwordFile));
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
        String userName;

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
            // Did not find Colon, log a message and skip entry
            Logger::put_l(
                Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
                MessageLoaderParms(
                    "Security.UserManager.PasswordFile.ERROR_READING_PWD_ENTRY",
                    "Error in reading password entry for : $0.",
                    userName));
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
        String password;

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
            Logger::put_l(
                Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
                MessageLoaderParms(
                    "Security.UserManager.PasswordFile.DUPLICATE_USER",
                    "Duplicate user: $0.", userName));
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
        if (FileSystem::exists(_passwordFile))
        {
            if (Executor::removeFile(_passwordFile.getCString()) != 0)
            {
                Logger::put_l(
                    Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                    MessageLoaderParms(
                        "Security.UserManager.PasswordFile."
                            "CANNOT_REMOVE_PWD_FILE",
                        "Cannot remove password file : $0.", _passwordFile));
                throw CannotRemoveFile(_passwordFile);
            }
        }
    }
    else
    {
        if (FileSystem::exists(_passwordFile))
        {
            if (Executor::renameFile(_passwordFile.getCString(),
                _passwordBackupFile.getCString()) != 0)
            {
                Logger::put_l(
                    Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                    MessageLoaderParms(
                        "Security.UserManager.PasswordFile."
                            "CANNOT_RENAME_PWD_FILE",
                        "Cannot rename password file : $0.",
                        _passwordFile));
                throw CannotRenameFile(_passwordFile);
            }
        }
    }

    //
    // Open the password file for writing
    //

    FILE* ofs = Executor::openFile(_passwordFile.getCString(), 'w');

    if (!ofs)
    {
        PEG_METHOD_EXIT();
        throw CannotOpenFile(getFileName());
    }

    //
    // Save user names and passwords to the new file
    //
    for (PasswordTable::Iterator i = passwordTable.start(); i; i++)
    {
        CString key = i.key().getCString();
        CString value = i.value().getCString();
        fprintf(ofs, "%s:%s\n", (const char*)key, (const char*)value);
    }

    fclose(ofs);

    if (FileSystem::exists(_passwordBackupFile))
    {
        if (Executor::removeFile(_passwordBackupFile.getCString()) != 0)
        {
            Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                MessageLoaderParms(
                    "Security.UserManager.PasswordFile."
                        "CANNOT_REMOVE_BACKUP_PWD_FILE",
                    "Cannot remove backup password file : $0.",
                    _passwordBackupFile));
            throw CannotRemoveFile(_passwordBackupFile);
        }
    }
    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
