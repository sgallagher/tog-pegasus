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
// Author: Sushma Fernandes, Hewlett Packard Company (sushma_fernandes@hp.com)
//
// Modified By:
//
//%////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file implements the functionality required to manage password file. 
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Security/UserManager/UserFileHandler.h>
#include <Pegasus/Security/UserManager/UserExceptions.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const unsigned char   UserFileHandler::_SALT_STRING[] =         
            "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

const String UserFileHandler::_PROPERTY_NAME_PASSWORD_FILEPATH = 
	    "passwordFilePath"; 

// Initialize the mutex timeout to 5000 ms.
const Uint32 UserFileHandler::_MUTEX_TIMEOUT = 5000;

//
// Generate random salt key for password encryption refer to crypt(3C)
//
void UserFileHandler::_GetSalt(char *salt)
{
    long 	randNum;
    Uint32 	sec;
    Uint32 	milliSec;
    const char  METHOD_NAME[] = "PasswordFile::_GetSalt";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    //
    // Generate a random number and get the salt 
    //
    System::getCurrentTime( sec, milliSec );

    srand( (int) sec );
    randNum = rand();

    //
    // Make sure the random number generated is between 0-63. 
    // refer to _SALT_STRING variable
    //
    *salt++ = _SALT_STRING[ randNum & 0x3f ];
    randNum >>= 6;
    *salt++ = _SALT_STRING[ randNum & 0x3f ];

    salt[2] = '\0';

    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}

//
// Constructor. 
//
UserFileHandler::UserFileHandler()
{
    const char  METHOD_NAME[] = "UserFileHandler::UserFileHandler";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    //
    // Get an instance of the ConfigManager.
    //
    ConfigManager*  configManager;
    configManager = ConfigManager::getInstance();

    //
    // Get the PasswordFilePath property from the Config Manager.
    //
    String passwdFile;
    passwdFile = configManager->getCurrentValue(
                       _PROPERTY_NAME_PASSWORD_FILEPATH);

    //
    // Construct a PasswordFile object.
    //
    _passwordFile   = new PasswordFile(passwdFile);

    //
    // Load the user information in to the cache.
    //
    try
    {
        _loadAllUsers();
    }
    catch  (Exception& e)
    {
	throw e;
    }

    //
    // Initialize the mutex, mutex lock needs to be held for any updates
    // to the password cache and password file.
    //
    _mutex = new Mutex;

    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}


//
// Destructor. 
//
UserFileHandler::~UserFileHandler()
{
    const char  METHOD_NAME[] = "UserFileHandler::~UserFileHandler";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    delete _passwordFile;
    delete _mutex;

    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}

// 
// Load all user names and password
//
void UserFileHandler::_loadAllUsers ()
{
    const char  METHOD_NAME[] = "UserFileHandler::_loadAllUsers";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    try
    {
        _passwordTable.clear();
        _passwordFile->load(_passwordTable);
    }
    catch (CannotOpenFile cof)
    {
        _passwordTable.clear();
        PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
        throw cof;
    }
    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}

void UserFileHandler::_Update(
			   char operation,
			   const String& userName,
			   const String& password)
{
    const char  METHOD_NAME[] = "UserFileHandler::_Update";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    //
    // Hold the mutex lock.
    // This will allow any one of the update operations to be performed
    // at any given time
    //

    try
    {
        _mutex->timed_lock(_MUTEX_TIMEOUT, pegasus_thread_self());
    }
    catch (TimeOut e)
    {
	throw CIMException( CIM_ERR_FAILED, 
	"Timed out trying to perform requested operation."
	"Please re-try the operation again.");
    }
    catch (WaitFailed e)
    {
	throw CIMException( CIM_ERR_FAILED, 
	"Timed out trying to perform requested operation."
	"Please re-try the operation again.");
    }
    catch (Deadlock e)
    {
	throw CIMException( CIM_ERR_FAILED, 
	"Deak lock encountered trying to perform requested operation."
	"Please re-try the operation again.");
    }

    switch (operation)
    {
	case ADD_USER:
                if (!_passwordTable.insert(userName,password))
                {
                    _mutex->unlock();
                    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
                    throw PasswordCacheError();
                }
		break; 

	case MODIFY_USER:
                if (!_passwordTable.remove(userName))
                {
                    _mutex->unlock();
                    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
                    throw PasswordCacheError();
                }
                if (!_passwordTable.insert(userName,password))
                {
                    _mutex->unlock();
                    Logger::put(Logger::ERROR_LOG, "UserManager", 
			Logger::SEVERE, 
			"Error updating user information for : $0.",userName);
                    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
                    throw PasswordCacheError();
                }
	        break; 

	case REMOVE_USER:

                //Remove the existing user name and password from the table
                if (!_passwordTable.remove(userName))
                {
                    _mutex->unlock();
                    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
                    throw InvalidUser(userName);
                }
	        break; 
        
	default:
		// Should never get here
		break;
    }
    
    // Store the entry in the password file
    try
    {
        _passwordFile->save(_passwordTable);
    }
    catch (CannotOpenFile& e)
    {
        _mutex->unlock();
        PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
        throw e;
    }
    catch (CannotRenameFile& e)
    {
        //
        // reload password hash table from file
        //
        _loadAllUsers();

        _mutex->unlock();
        PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
        throw e;
    }
    _mutex->unlock();
    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);
}


// 
// Add user entry to file
// 
void UserFileHandler::addUserEntry(
			    const String& userName, 
			    const String& password)
{
    char 	salt[3];
    String 	encryptedPassword = String::EMPTY;
    const char  METHOD_NAME[] = "UserFileHandler::addUserEntry";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    // Check if the user already exists
    if (_passwordTable.contains(userName))
    {
        PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
	throw DuplicateUser(userName);
    }

    // encrypt password
    _GetSalt(salt);

    ArrayDestroyer<char> pw(password.allocateCString());

    encryptedPassword = System::encryptPassword(pw.getPointer(),salt);

    // add the user to the cache and password file
    _Update(ADD_USER,userName, encryptedPassword);

    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}

// 
// Modify user entry in file 
//
void UserFileHandler::modifyUserEntry(
	     const String& userName,
	     const String& password,
	     const String& newPassword )
{
    char 	salt[3];
    String 	encryptedPassword = String::EMPTY;
    const char  METHOD_NAME[] = "UserFileHandler::modifyUserEntry";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    //
    // Check if the given password matches the passwd in the file
    //
    try
    {
        if ( !verifyCIMUserPassword (userName,password) )
        {
            PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
	    throw PasswordMismatch(userName);
        }
    }
    catch (Exception& e)
    {
        PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
	throw e;
    }

    // encrypt new password
    _GetSalt(salt);

    ArrayDestroyer<char> npw(newPassword.allocateCString());

    encryptedPassword = System::encryptPassword(npw.getPointer(),salt);

    _Update(MODIFY_USER, userName, encryptedPassword);

    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}

// 
// Remove user entry from file 
// 
void UserFileHandler::removeUserEntry(const String& userName)
{
    const char  METHOD_NAME[] = "UserFileHandler::removeUserEntry";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    _Update(REMOVE_USER, userName);

    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}

//
// Get a list of all the user names.
//
void UserFileHandler::getAllUserNames(Array<String>& userNames)
{
    const char  METHOD_NAME[] = "UserFileHandler::getAllUserNames";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    userNames.clear();

    for (PasswordTable::Iterator i = _passwordTable.start(); i; i++)
    {
        userNames.append(i.key());
    }
    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
}

//
// Verify whether the specified CIM user is valid
//
Boolean UserFileHandler::verifyCIMUser (const String& userName)
{
    const char  METHOD_NAME[] = "UserFileHandler::verifyCIMUser";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
    return _passwordTable.contains(userName);
}

//
// Verify whether the specified user's password is valid
//
Boolean UserFileHandler::verifyCIMUserPassword (
			    const String& userName, 
			    const String& password)
{
    const char  METHOD_NAME[] = "UserFileHandler::verifyCIMUserPassword";

    PEG_FUNC_ENTER(TRC_USER_MANAGER,METHOD_NAME);

    // Check if the user's password mathches the specified password
    String curPassword 		= String::EMPTY;
    String encryptedPassword 	= String::EMPTY;
    String saltStr     		= String::EMPTY;

    // Check if the user exists in the password table
    if ( !_passwordTable.lookup(userName,curPassword) )
    {
        PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
        throw InvalidUser(userName);
    }

    saltStr = curPassword.subString(0,2);
    ArrayDestroyer<char> oldsalt(saltStr.allocateCString());
    ArrayDestroyer<char> pw(password.allocateCString());

    encryptedPassword =
                System::encryptPassword(pw.getPointer(),oldsalt.getPointer());

    if ( curPassword != encryptedPassword )
    {
        PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
        return false;
    }

    PEG_FUNC_EXIT(TRC_USER_MANAGER,METHOD_NAME);
    return true;
}
PEGASUS_NAMESPACE_END


