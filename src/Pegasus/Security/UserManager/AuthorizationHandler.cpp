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
const String PG_AUTH_CLASS                     = "PG_Authorization";

/**
    The constant representing the default namespace
*/
const String AUTHORIZATION_NAMESPACE           = "root/cimv2";

/**
    This constant represents the  User name property in the schema
*/
static const char PROPERTY_NAME_USERNAME []    = "Username";

/**
    This constant represents the Namespace property in the schema
*/
static const char PROPERTY_NAME_NAMESPACE []   = "Namespace";

/**
    This constant represents the Authorizations property in the schema
*/
static const char PROPERTY_NAME_AUTHORIZATION []    = "Authorization";


/**
    List of CIM Operations
*/
//ATTN: Make sure it has all the operations covered

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
    //
    // ATTN: Implement this
    // 
/*
    try
    {
        //
        // call enumerateInstanceNames
        //
        Array<CIMReference> instanceNames =
            _repository->enumerateInstanceNames( AUTHORIZATION_NAMESPACE, PG_AUTH_CLASS);

        //
        // check for the given namespace
        //
        for (Uint32 i = 0; i < instanceNames.size(); i++)
        {
            Array<KeyBinding> kbArray = instanceNames[i].getKeyBindings();

            if (kbArray.size() > 0)
            {
                if (String::equal(nameSpace, kbArray[0].getValue();
            }
        }
    catch (CIMException& e)
    {
	throw InvalidNamespace(nameSpace+e.getMessage());
    }
*/
    return true;
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
            if (!_authTable.insert(userName+nameSpace, auth))
            {
                //ATTN: Should an exception be thrown or just ignore 
                //      that instance ?
                //throw AuthorizationCacheError();
            }
        }

    }
    catch(Exception& e)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    


}

void AuthorizationHandler::setAuthorization(
                            const String& userName,
                            const String& nameSpace,
			    const String& auth)

{
    //
    // Check if the auth already exists
    //
    if (_authTable.contains(userName+nameSpace))
    {
        //
        // remove the existing auth TODO: confirm this
        //
        if (!_authTable.remove(userName+nameSpace))
        {
            //ATTN: Should this exception be thrown here?
            throw AuthorizationCacheError();
        }
    }

    //
    // Add authorization to the table
    //
    if (!_authTable.insert(userName+nameSpace,auth))
    {
        throw AuthorizationCacheError();
    }

}

void AuthorizationHandler::removeAuthorization(
                            const String& userName,
                            const String& nameSpace)
{
    //
    // remove authorization from the table
    //
    if (!_authTable.remove(userName+nameSpace))
    {
        throw AuthorizationCacheError();
    }
}

String AuthorizationHandler::getAuthorization(
                            const String& userName,
                            const String& nameSpace)
{
    String auth;

    // Check if the user exists in the auth table
    if (!_authTable.contains(userName+nameSpace))
    {
	// TODO: change to auth entry not found
        throw InvalidUserAndNamespace(userName, nameSpace);
    }

    _authTable.lookup(userName+nameSpace,auth);

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

    Uint32 readOpSize = sizeof(READ_OPERATIONS) / sizeof(READ_OPERATIONS[0]);

    Uint32 writeOpSize = sizeof(WRITE_OPERATIONS) / sizeof(WRITE_OPERATIONS[0]);

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
cout << e.getMessage() << endl;
        return authorized;
    }

    if (String::equal(auth, "rw") || String::equal(auth, "wr"))
    {
        for (Uint32 i = 0; i < readOpSize; i++ )
        {
            if ( String::equal(cimMethodName, READ_OPERATIONS[i]) )
            {
                authorized = true;
                break;
            }
        }
        for (Uint32 i = 0; i < writeOpSize; i++ )
        {
            if ( String::equal(cimMethodName, WRITE_OPERATIONS[i]) )
            {
                authorized = true;
                break;
            }
        }
    }
    else if (String::equal(auth, "r"))
    {
        for (Uint32 i = 0; i < readOpSize; i++ )
        {
            if ( String::equal(cimMethodName, READ_OPERATIONS[i]) )
            {
                authorized = true;
                break;
            }
        }
    }
    else if (String::equal(auth, "w"))
    {
        for (Uint32 i = 0; i < writeOpSize; i++ )
        {
            if ( String::equal(cimMethodName, WRITE_OPERATIONS[i]) )
            {
                authorized = true;
                break;
            }
        }
    }

    return authorized;
}

PEGASUS_NAMESPACE_END


