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
// This file implements the functionality required to manage auth table. 
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMReference.h>

#include <Pegasus/Security/UserManager/AuthorizationHandler.h>
#include <Pegasus/Security/UserManager/UserExceptions.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//
//ATTN: The following constant should be placed in a common place.
//
/**
    The constant represeting the authorization class name
*/
static const String PG_AUTH_CLASS                 = "PG_Authorization";

/**
    The constant representing the namespace where the authorization
    instances reside.
*/
static const String AUTHORIZATION_NAMESPACE       = "root/cimv2";

/**
    This constant represents the  User name property in the schema
*/
static const char PROPERTY_NAME_USERNAME []       = "Username";

/**
    This constant represents the Namespace property in the schema
*/
static const char PROPERTY_NAME_NAMESPACE []      = "Namespace";

/**
    This constant represents the Authorizations property in the schema
*/
static const char PROPERTY_NAME_AUTHORIZATION []  = "Authorization";


/**
    List of CIM Operations
*/
//ATTN: Make sure to include all the CIM operations here.

static const char* READ_OPERATIONS []    = {
    "GetClass",
    "GetInstance",
    "EnumerateClassNames",
    "References",
    "ReferenceNames",
    "AssociatorNames",
    "Associators",
    "EnumerateInstanceNames",
    "GetQualifier",
    "EnumerateQualifiers",
    "EnumerateClasses",
    "EnumerateInstances",
    "ExecQuery",
    "GetProperty" };
    
static const char* WRITE_OPERATIONS []    = {
    "CreateClass",
    "CreateInstance",
    "DeleteQualifier",
    "SetQualifier",
    "ModifyClass",
    "ModifyInstance",
    "DeleteClass",
    "DeleteInstance",
    "SetProperty" };
    

//
// Constructor
//
AuthorizationHandler::AuthorizationHandler(CIMRepository* repository)
{
    _repository = repository;

    try
    {
        _loadAllAuthorizations();
    }
    catch(Exception& e)
    {
	//ATTN: 
	cerr << PG_AUTH_CLASS << " class not loaded, ";
	cerr << "No authorizations configured." << endl;

        //throw e;
    }
}

//
// Destructor. 
//
AuthorizationHandler::~AuthorizationHandler()
{

}

//
// Check if a given namespace exists
//
Boolean AuthorizationHandler::verifyNamespace( const String& nameSpace )
{
    try
    {
        //
        // call enumerateNameSpaces to get all the namespaces 
        // in the repository
        //
        Array<String> namespaceNames =
            _repository->enumerateNameSpaces();

        //
        // check for the given namespace
        //
        Uint32 size = namespaceNames.size();

        for (Uint32 i = 0; i < size; i++)
        {
             if (String::equal(nameSpace, namespaceNames[i]))
             {
                 return true;
             }
        }
    }
    catch (Exception& e)
    {
	throw InvalidNamespace(nameSpace + e.getMessage());
    }

    return false;
}

// 
// Load all user names and password
//
void AuthorizationHandler::_loadAllAuthorizations()
{
    Array<CIMNamedInstance> namedInstances;

    try
    {
        //
        // call enumerateInstances of the repository
        //
        namedInstances = _repository->enumerateInstances(
            AUTHORIZATION_NAMESPACE, PG_AUTH_CLASS); 

        //
        // get all the user names, namespaces, and authorizations
        //
        for (Uint32 i = 0; i < namedInstances.size(); i++)
        {
            CIMInstance& authInstance =
                namedInstances[i].getInstance();

            //
            // get user name
            //
            Uint32 pos = authInstance.findProperty(PROPERTY_NAME_USERNAME);
            CIMProperty prop = (CIMProperty)authInstance.getProperty(pos);
            String userName = prop.getValue().toString();

            //
            // get namespace name
            //
            pos = authInstance.findProperty(PROPERTY_NAME_NAMESPACE);
            prop = (CIMProperty)authInstance.getProperty(pos);
            String nameSpace = prop.getValue().toString();

            //
            // get authorizations
            //
            pos = authInstance.findProperty(PROPERTY_NAME_AUTHORIZATION);
            prop = (CIMProperty)authInstance.getProperty(pos);
            String auth = prop.getValue().toString();

            //
            // Add authorization to the table
            //
            _authTable.insert(userName + nameSpace, auth);
        }

    }
    catch(Exception& e)
    {
        throw e;
    }

}

void AuthorizationHandler::setAuthorization(
                            const String& userName,
                            const String& nameSpace,
			    const String& auth)
{
    //
    // Remove auth if it already exists
    //
    _authTable.remove(userName + nameSpace);

    //
    // Insert the specified authorization
    //
    if (!_authTable.insert(userName + nameSpace, auth))
    {
        throw AuthorizationCacheError();
    }

}

void AuthorizationHandler::removeAuthorization(
                            const String& userName,
                            const String& nameSpace)
{
    //
    // Remove the specified authorization
    //
    if (!_authTable.remove(userName + nameSpace))
    {
        throw AuthorizationEntryNotFound(userName, nameSpace);
    }
}

String AuthorizationHandler::getAuthorization(
                            const String& userName,
                            const String& nameSpace)
{
    String auth;

    //
    // Get authorization for the specified userName and nameSpace
    //
    if (!_authTable.lookup(userName + nameSpace, auth))
    {
        throw AuthorizationEntryNotFound(userName, nameSpace);
    }

    return auth;
}

//
// Verify whether the specified operation has authorization
// to be performed by the specified user.
//
Boolean AuthorizationHandler::verifyAuthorization(
                            const String& userName,
                            const String& nameSpace,
                            const String& cimMethodName)
{
    Boolean authorized = false;
    Boolean readOperation = false;
    Boolean writeOperation = false;

    Uint32 readOpSize = sizeof(READ_OPERATIONS) / sizeof(READ_OPERATIONS[0]);

    Uint32 writeOpSize = sizeof(WRITE_OPERATIONS) / sizeof(WRITE_OPERATIONS[0]);

    for (Uint32 i = 0; i < readOpSize; i++ )
    {
        if ( String::equal(cimMethodName, READ_OPERATIONS[i]) )
        {
            readOperation = true;
            break;
        }
    }
    if ( !readOperation )
    {
        for (Uint32 i = 0; i < writeOpSize; i++ )
        {
            if ( String::equal(cimMethodName, WRITE_OPERATIONS[i]) )
            {
                writeOperation = true;
                break;
            }
        }
    }

    //
    // Get the authorization of the specified user and namespace
    //
    String auth;
    try
    {
        auth = getAuthorization(userName, nameSpace);
    }
    catch (Exception& e)
    {
        return authorized;
    }

    if ( ( String::equal(auth, "rw") || String::equal(auth, "wr") ) &&
        ( readOperation || writeOperation ) )
    {
        authorized = true;
    }
    else if ( String::equal(auth, "r") && readOperation )
    {
        authorized = true;
    }
    else if ( String::equal(auth, "w") && writeOperation )
    {
        authorized = true;
    }

    return authorized;
}

PEGASUS_NAMESPACE_END


