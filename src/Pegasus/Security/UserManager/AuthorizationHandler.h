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
// Author: Sushma Fernandes (sushma_fernandes@hp.com)
//
// Modified By: Nag Boranna, Hewlett Packard Company (nagaraja_boranna@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file implements the functionality required to manage password file. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AuthorizationHandler_h
#define Pegasus_AuthorizationHandler_h

#include <cctype>
#include <fstream>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Repository/CIMRepository.h>

#include <Pegasus/Security/UserManager/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
  This class implements the functionality required to manage authorizations.
*/

///////////////////////////////////////////////////////////////////////////////
// Auth Table
//////////////////////////////////////////////////////////////////////////////

typedef HashTable<String, String, EqualFunc<String>, HashFunc<String> > AuthTable;


class PEGASUS_USERMANAGER_LINKAGE AuthorizationHandler
{

private:

    //
    // Authorization cache
    //
    AuthTable       	          _authTable;

    //
    // Repository handle
    //
    CIMRepository*                _repository;

protected:

    /**
    Load the user information from the Repository.

    */
    void _loadAllAuthorizations ();


public:

    /** Constructor. */
    AuthorizationHandler(CIMRepository* repository);

    /** Destructor. */
    ~AuthorizationHandler();

    /**
    Check if the namespace exists.

    */
    Boolean verifyNamespace( const String& nameSpace );

    /*
    Verify whether the specified operation has authorization
    to be performed by the specified user.

    */
    Boolean verifyAuthorization(
                            const String& userName,
                            const String& nameSpace,
                            const String& cimMethodName);

    /**
    Set the authorization.

    */
    void setAuthorization(
                            const String& userName,
                            const String& nameSpace,
			    const String& auth);

    /**
    Remove the authorization.

    */
    void removeAuthorization(
                            const String& userName,
                            const String& nameSpace);

    /**
    Get the authorization.

    */
    String getAuthorization(
                            const String& userName,
                            const String& nameSpace);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AuthorizationHandler_h */

