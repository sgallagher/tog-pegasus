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


////////////////////////////////////////////////////////////////////////////////
//  This file contains the exception classes used in the UserManager
//  classes.
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_UserExceptions_h
#define Pegasus_UserExceptions_h

#include <Pegasus/Common/Exception.h>
#include <Pegasus/Security/UserManager/Linkage.h>

PEGASUS_NAMESPACE_BEGIN


/** 
DuplicateUser Exception class 
*/
class PEGASUS_USERMANAGER_LINKAGE DuplicateUser : public Exception
{
public:
    DuplicateUser(const String& userName)
        : Exception("User already exists, user name: " + userName) { }
};


/** 
InvalidUser Exception class 
*/
class PEGASUS_USERMANAGER_LINKAGE InvalidUser : public Exception
{
public:
    InvalidUser(const String& userName)
        : Exception("The specified user is not a valid CIM user, user name: " 
		     + userName + ".") { } 
};


/** 
InvalidSystemUser Exception class 
*/
class PEGASUS_USERMANAGER_LINKAGE InvalidSystemUser : public Exception
{
public:
    InvalidSystemUser(const String& userName)
        : Exception("The specified user is not a valid user on the local system , user name: " + userName ) { }
};

/** 
PasswordMismatch Exception class 
*/
class PEGASUS_USERMANAGER_LINKAGE PasswordMismatch : public Exception
{
public:
    PasswordMismatch(const String& userName)
        : Exception("The specified password does not match user's current password, user name: " + userName ) { }
};

/** 
PasswordCacheError Exception class 
*/
class PEGASUS_USERMANAGER_LINKAGE PasswordCacheError : public Exception
{
public:
    PasswordCacheError()
        : Exception("Internal error while processing password cache table." ) { }
};

/**
InvalidUserAndNamespace Exception class
*/
class PEGASUS_USERMANAGER_LINKAGE InvalidUserAndNamespace : public Exception
{
public:
    InvalidUserAndNamespace(const String& userName, const String& nameSpace)
        : Exception("The specified user '" + userName + "' and namespace '"
            + nameSpace + "' are not authorized.") { }
};

/**
AuthorizationCacheError Exception class
*/
class PEGASUS_USERMANAGER_LINKAGE AuthorizationCacheError : public Exception
{
public:
    AuthorizationCacheError()
        : Exception("Internal error while processing authorization cache table." ) { }
};

/**
AuthorizationEntryNotFound Exception class
*/
class PEGASUS_USERMANAGER_LINKAGE AuthorizationEntryNotFound : public Exception
{
public:
    AuthorizationEntryNotFound(const String& userName, const String& nameSpace)
        : Exception("Authorization entry not found for user '" +
            userName + "' with the namespace '" + nameSpace + "'." ) { }
};

/**
InvalidNamespace Exception class
*/
class PEGASUS_USERMANAGER_LINKAGE InvalidNamespace : public Exception
{
public:
    InvalidNamespace(String nameSpace)
        : Exception("The specified namespace does not exist, " + nameSpace ) { }

};

/**
CannotCreateUserManagerInstance Exception class
*/
class PEGASUS_USERMANAGER_LINKAGE CannotCreateUserManagerInstance: public Exception
{
public:
    CannotCreateUserManagerInstance()
        : Exception("Cannot create UserManager instance, repository handle passed may be invalid.") { }

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_UserExceptions_h */
