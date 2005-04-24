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
// Author: Sushma Fernandes (sushma_fernandes@hp.com)
//
// Modified By: Nag Boranna, Hewlett Packard Company (nagaraja_boranna@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for PEP#101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// This file implements the functionality required to manage auth table.
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlWriter.h>

#include "AuthorizationHandler.h"
#include "UserExceptions.h"

#ifdef PEGASUS_OS_OS400
#include "qycmutiltyUtility.H"
#include "OS400ConvertChar.h"
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


//
// This constant represents the  User name property in the schema
//
static const CIMName PROPERTY_NAME_USERNAME        = CIMName ("Username");

//
// This constant represents the Namespace property in the schema
//
static const CIMName PROPERTY_NAME_NAMESPACE       = CIMName ("Namespace");

//
// This constant represents the Authorizations property in the schema
//
static const CIMName PROPERTY_NAME_AUTHORIZATION   = CIMName ("Authorization");


//
// List of all the CIM Operations
//
// Note: The following tables contain all the existing CIM Operations.
//       Any new CIM Operations created must be included in one of these tables,
//       otherwise no CIM requests will have authorization to execute those
//       new operations.
//

//
// List of read only CIM Operations
//
static const CIMName READ_OPERATIONS []    = {
    CIMName ("GetClass"),
    CIMName ("GetInstance"),
    CIMName ("EnumerateClassNames"),
    CIMName ("References"),
    CIMName ("ReferenceNames"),
    CIMName ("AssociatorNames"),
    CIMName ("Associators"),
    CIMName ("EnumerateInstanceNames"),
    CIMName ("GetQualifier"),
    CIMName ("EnumerateQualifiers"),
    CIMName ("EnumerateClasses"),
    CIMName ("EnumerateInstances"),
    CIMName ("ExecQuery"),
    CIMName ("GetProperty") };

//
// List of write CIM Operations
//
static const CIMName WRITE_OPERATIONS []    = {
    CIMName ("CreateClass"),
    CIMName ("CreateInstance"),
    CIMName ("DeleteQualifier"),
    CIMName ("SetQualifier"),
    CIMName ("ModifyClass"),
    CIMName ("ModifyInstance"),
    CIMName ("DeleteClass"),
    CIMName ("DeleteInstance"),
    CIMName ("SetProperty"),
    CIMName ("InvokeMethod"),
    CIMName ("EnableIndicationSubscription"),
    CIMName ("ModifyIndicationSubscription"),
    CIMName ("DisableIndicationSubscription") };


//
// Constructor
//
AuthorizationHandler::AuthorizationHandler(CIMRepository* repository)
{
    PEG_METHOD_ENTER(
        TRC_AUTHORIZATION, "AuthorizationHandler::AuthorizationHandler()");

    _repository = repository;

    try
    {
        _loadAllAuthorizations();
    }
    catch(Exception& e)
    {
	//ATTN-NB-03-20020402: Should this exception be thrown or ignored ?
        //throw e;

      //	cerr << PEGASUS_CLASSNAME_AUTHORIZATION << " class not loaded, ";
      //	cerr << "No authorizations configured." << endl;
    }

    PEG_METHOD_EXIT();
}

//
// Destructor.
//
AuthorizationHandler::~AuthorizationHandler()
{
    PEG_METHOD_ENTER(
        TRC_AUTHORIZATION, "AuthorizationHandler::~AuthorizationHandler()");

    PEG_METHOD_EXIT();
}

//
// Check if a given namespace exists
//
Boolean AuthorizationHandler::verifyNamespace(
    const CIMNamespaceName& nameSpace )
{
    PEG_METHOD_ENTER(
        TRC_AUTHORIZATION, "AuthorizationHandler::verifyNamespace()");

    try
    {
        //
        // call enumerateNameSpaces to get all the namespaces
        // in the repository
        //
        Array<CIMNamespaceName> namespaceNames =
            _repository->enumerateNameSpaces();

        //
        // check for the given namespace
        //
        Uint32 size = namespaceNames.size();

        for (Uint32 i = 0; i < size; i++)
        {
             if (nameSpace.equal (namespaceNames[i]))
             {
                 PEG_METHOD_EXIT();
                 return true;
             }
        }
    }
    catch (Exception& e)
    {
        PEG_METHOD_EXIT();
	throw InvalidNamespace(nameSpace.getString() + e.getMessage());
    }

    PEG_METHOD_EXIT();

    return false;
}

//
// Load all user names and password
//
void AuthorizationHandler::_loadAllAuthorizations()
{
    PEG_METHOD_ENTER(
        TRC_AUTHORIZATION, "AuthorizationHandler::_loadAllAuthorizations()");

    Array<CIMInstance> namedInstances;

    try
    {
        //
        // call enumerateInstances of the repository
        //
        namedInstances = _repository->enumerateInstances(
            PEGASUS_NAMESPACENAME_AUTHORIZATION, PEGASUS_CLASSNAME_AUTHORIZATION);

        //
        // get all the user names, namespaces, and authorizations
        //
        for (Uint32 i = 0; i < namedInstances.size(); i++)
        {
            CIMInstance& authInstance = namedInstances[i];

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
    catch(const Exception&)
    {
        PEG_METHOD_EXIT();
        throw;
    }

    PEG_METHOD_EXIT();
}

void AuthorizationHandler::setAuthorization(
                            const String& userName,
                            const CIMNamespaceName& nameSpace,
			    const String& auth)
{
    PEG_METHOD_ENTER(
        TRC_AUTHORIZATION, "AuthorizationHandler::setAuthorization()");

    //
    // Remove auth if it already exists
    //
    _authTable.remove(userName + nameSpace.getString());

    //
    // Insert the specified authorization
    //
    if (!_authTable.insert(userName + nameSpace.getString(), auth))
    {
        PEG_METHOD_EXIT();
        throw AuthorizationCacheError();
    }

    PEG_METHOD_EXIT();
}

void AuthorizationHandler::removeAuthorization(
                            const String& userName,
                            const CIMNamespaceName& nameSpace)
{
    PEG_METHOD_ENTER(
        TRC_AUTHORIZATION, "AuthorizationHandler::removeAuthorization()");

    //
    // Remove the specified authorization
    //
    if (!_authTable.remove(userName + nameSpace.getString()))
    {
        PEG_METHOD_EXIT();
        throw AuthorizationEntryNotFound(userName, nameSpace.getString());
    }
    PEG_METHOD_EXIT();
}

String AuthorizationHandler::getAuthorization(
                            const String& userName,
                            const CIMNamespaceName& nameSpace)
{
    PEG_METHOD_ENTER(
        TRC_AUTHORIZATION, "AuthorizationHandler::getAuthorization()");

    String auth;

    //
    // Get authorization for the specified userName and nameSpace
    //
    if (!_authTable.lookup(userName + nameSpace.getString(), auth))
    {
        PEG_METHOD_EXIT();
        throw AuthorizationEntryNotFound(userName, nameSpace.getString());
    }

    PEG_METHOD_EXIT();

    return auth;
}

//
// Verify whether the specified operation has authorization
// to be performed by the specified user.
//
Boolean AuthorizationHandler::verifyAuthorization(
                            const String& userName,
                            const CIMNamespaceName& nameSpace,
                            const CIMName& cimMethodName)
{
    PEG_METHOD_ENTER(
        TRC_AUTHORIZATION, "AuthorizationHandler::verifyAuthorization()");

    Boolean authorized = false;
    Boolean readOperation = false;
    Boolean writeOperation = false;

    Uint32 readOpSize = sizeof(READ_OPERATIONS) / sizeof(READ_OPERATIONS[0]);

    Uint32 writeOpSize = sizeof(WRITE_OPERATIONS) / sizeof(WRITE_OPERATIONS[0]);

    for (Uint32 i = 0; i < readOpSize; i++ )
    {
        if (cimMethodName.equal (READ_OPERATIONS[i]))
        {
            readOperation = true;
            break;
        }
    }
    if ( !readOperation )
    {
        for (Uint32 i = 0; i < writeOpSize; i++ )
        {
            if (cimMethodName.equal (WRITE_OPERATIONS[i]))
            {
                writeOperation = true;
                break;
            }
        }
    }

#ifdef PEGASUS_OS_OS400
    if (readOperation || writeOperation)
    {
        // Use OS/400 Application Administration to do cim operation verification
        // (note - need to convert to EBCDIC before calling ycm)
	CString userCStr = userName.getCString();
	const char * user = (const char *)userCStr;
	AtoE((char *)user);
	CString nsCStr = nameSpace.getString().getCString();
	const char * ns = (const char *)nsCStr;
	AtoE((char *)ns);
	CString cimMethCStr = cimMethodName.getString().getCString();
	const char * cimMeth = (const char *)cimMethCStr;
	AtoE((char *)cimMeth);
	int os400auth =
	  ycmVerifyFunctionAuthorization(user,
					 ns,
					 cimMeth);
	if (os400auth == TRUE)
	    authorized = true;
    }
#else
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
        PEG_METHOD_EXIT();
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
#endif

    PEG_METHOD_EXIT();

    return authorized;
}

PEGASUS_NAMESPACE_END


