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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//  This file contains the exception classes used in the UserManager
//  classes.
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_UserExceptions_h
#define Pegasus_UserExceptions_h

#include <Pegasus/Common/Exception.h>
#include <Pegasus/Security/UserManager/Linkage.h>
#include <Pegasus/Common/MessageLoader.h>  //l10n

PEGASUS_NAMESPACE_BEGIN


/** 
DuplicateUser Exception class 
*/
class PEGASUS_USERMANAGER_LINKAGE DuplicateUser : public Exception
{
public:
//l10n
    //DuplicateUser(const String& userName)
        //: Exception("User already exists, user name: " + userName) { }
    DuplicateUser(const String& userName)
        : Exception(MessageLoaderParms("Security.UserManager.UserExceptions.USER_ALREADY_EXISTS",
        							   "User already exists, user name: $0",
        							   userName)) { }
};


/** 
InvalidUser Exception class 
*/
class PEGASUS_USERMANAGER_LINKAGE InvalidUser : public Exception
{
public:
//l10n
    //InvalidUser(const String& userName)
        //: Exception("The specified user is not a valid CIM user, user name: " 
		     //+ userName + ".") { }
	InvalidUser(const String& userName)
        : Exception(MessageLoaderParms("Security.UserManager.UserExceptions.INVALID_CIM_USER",
        			"The specified user is not a valid CIM user, user name: $0.",
		  			userName)) { } 
};


/** 
InvalidSystemUser Exception class 
*/
class PEGASUS_USERMANAGER_LINKAGE InvalidSystemUser : public Exception
{
public:
//l10n
    //InvalidSystemUser(const String& userName)
        //: Exception("The specified user is not a valid user on the local system , user name: " + userName ) { }
    InvalidSystemUser(const String& userName)
        : Exception(MessageLoaderParms("Security.UserManager.UserExceptions.INVALID_USER_ON_LOCAL_SYSTEM",
                      "The specified user is not a valid user on the local system , user name: $0",
                      userName )) { }
};

/** 
PasswordMismatch Exception class 
*/
class PEGASUS_USERMANAGER_LINKAGE PasswordMismatch : public Exception
{
public:
//l10n
    //PasswordMismatch(const String& userName)
        //: Exception("The specified password does not match user's current password, user name: " + userName ) { }
    PasswordMismatch(const String& userName)
        : Exception(MessageLoaderParms("Security.UserManager.UserExceptions.PASSWORD_MISMATCH",
                    "The specified password does not match user's current password, user name: $0",
                    userName )) { }
};

/** 
PasswordCacheError Exception class 
*/
class PEGASUS_USERMANAGER_LINKAGE PasswordCacheError : public Exception
{
public:
//l10n
    //PasswordCacheError()
        //: Exception("Internal error while processing password cache table." ) { }
    PasswordCacheError()
        : Exception(MessageLoaderParms("Security.UserManager.UserExceptions.INTERNAL_ERROR_PWD_CACHE",
                                       "Internal error while processing password cache table." )) { }
};

/**
InvalidUserAndNamespace Exception class
*/
class PEGASUS_USERMANAGER_LINKAGE InvalidUserAndNamespace : public Exception
{
public:
//l10n
    //InvalidUserAndNamespace(const String& userName, const String& nameSpace)
        //: Exception("The specified user '" + userName + "' and namespace '"
            //+ nameSpace + "' are not authorized.") { }
    InvalidUserAndNamespace(const String& userName, const String& nameSpace)
        : Exception(MessageLoaderParms("Security.UserManager.UserExceptions.USER_NAMESPACE_NOT_AUTHORIZED",
                  	"The specified user '$0' and namespace '$1' are not authorized.",
            		userName,
            		nameSpace)) { }
};

/**
AuthorizationCacheError Exception class
*/
class PEGASUS_USERMANAGER_LINKAGE AuthorizationCacheError : public Exception
{
public:
//l10n
    //AuthorizationCacheError()
        //: Exception("Internal error while processing authorization cache table." ) { }
    AuthorizationCacheError()
        : Exception(MessageLoaderParms("Security.UserManager.UserExceptions.INTERNAL_ERROR_AUTH_CACHE",
        							   "Internal error while processing authorization cache table." )) { }
};

/**
AuthorizationEntryNotFound Exception class
*/
class PEGASUS_USERMANAGER_LINKAGE AuthorizationEntryNotFound : public Exception
{
public:
//l10n
    //AuthorizationEntryNotFound(const String& userName, const String& nameSpace)
        //: Exception("Authorization entry not found for user '" +
            //userName + "' with the namespace '" + nameSpace + "'." ) { }
    AuthorizationEntryNotFound(const String& userName, const String& nameSpace)
        : Exception(MessageLoaderParms("Security.UserManager.UserExceptions.AUTH_ENTRY_NOT_FOUND",
        				"Authorization entry not found for user '$0' with the namespace '$1'.",
        				userName, nameSpace )) { }
};

/**
InvalidNamespace Exception class
*/
class PEGASUS_USERMANAGER_LINKAGE InvalidNamespace : public Exception
{
public:
//l10n
    //InvalidNamespace(String nameSpace)
        //: Exception("The specified namespace does not exist, " + nameSpace ) { }
    InvalidNamespace(String nameSpace)
        : Exception(MessageLoaderParms("Security.UserManager.UserExceptions.NAMESPACE_DOES_NOT_EXIST",
                                       "The specified namespace does not exist, $0",
                                       nameSpace )) { }

};

/**
CannotCreateUserManagerInstance Exception class
*/
class PEGASUS_USERMANAGER_LINKAGE CannotCreateUserManagerInstance: public Exception
{
public:
//l10n
    //CannotCreateUserManagerInstance()
        //: Exception("Cannot create UserManager instance, repository handle passed may be invalid.") { }
    CannotCreateUserManagerInstance()
        : Exception(MessageLoaderParms("Security.UserManager.UserExceptions.CANT_CREATE_USERMANAGER_INSTANCE",
                                       "Cannot create UserManager instance, repository handle passed may be invalid.")) { }

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_UserExceptions_h */
