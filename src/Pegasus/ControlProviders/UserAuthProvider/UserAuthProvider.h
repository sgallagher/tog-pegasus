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
// Modified By: Nag Boranna, Hewlett Packard Company (nagaraja_boranna@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef UserAuthProvider_h
#define UserAuthProvider_h

///////////////////////////////////////////////////////////////////////////////
//  User/Auth Provider
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#include <cctype>
#include <iostream>

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Destroyer.h>

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/Security/UserManager/UserExceptions.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The UserAuthProvider provides responses to the CIM Operations defined in
    the DMTF docuument CIM Operations over HTTP (Section 2.5).

    This provider implements the following functions:
    - createInstance            ( Adds a user or user authorization )
    - invokeMethod              ( Modifies a user's password )
    - enumerateInstanceNames    ( Lists all users )
    - enumerateInstances        ( Lists all user authorizations )
    - modifyInstance            ( Modifies a user authorization )
    - deleteInstance            ( Removes a user or user authorization )
*/

class PEGASUS_USERAUTHPROVIDER_LINKAGE UserAuthProvider
    : public CIMInstanceProvider, public CIMMethodProvider
{
public:

    UserAuthProvider(CIMRepository* repository)
    {
	PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::UserAuthProvider");

        //
        // get an instance of User Manager and initialize
        // the instance variable.
        //
        _userManager = UserManager::getInstance();

        _repository = repository;

	PEG_METHOD_EXIT();
    }

    virtual ~UserAuthProvider()
    {
	PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::~UserAuthProvider");

	PEG_METHOD_EXIT();
    }

    /**
    Returns the instance based on instanceName.
    */
    virtual void getInstance(
	const OperationContext & context,
        const CIMReference& instanceName,
	const Uint32 flags,
        const Array<String>& propertyList,
	ResponseHandler<CIMInstance> & handler)
    {
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED, "");
    }

    /**
    Creates a new instance.

    @param  context
    @param  instanceReference
    @param  CIMInstance
    @param  handler
    */
    virtual void createInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
        const CIMInstance& myInstance,
	ResponseHandler<CIMReference> & handler);

    /**
    Deletes the specified instance.

    @param  context
    @param  InstanceName
    @param  handler
    */
    virtual void deleteInstance(
	const OperationContext & context,
        const CIMReference& myInstance,
	ResponseHandler<CIMInstance> & handler);


    /**
    Modify instance based on modifiedInstance.

    @param context specifies security and locale information relevant for
                   the lifetime of this operation.
    @param instanceReference the fully qualified object path of the instance.
    @param modifiedInstance  the modified instance.
    @param handler enables providers to asynchronously return the results.
    */
    virtual void modifyInstance(
        const OperationContext & context,
        const CIMReference & instanceReference,
        const CIMInstance& modifiedIns,
        const Uint32 flags,
        const Array<String> & propertyList,
        ResponseHandler<CIMInstance> & handler);

    /**
    Enumerates instances.

    @param context specifies security and locale information relevant for
                   the lifetime of this operation.
    @param ref the fully qualified object path of the instance.
    @param flag specifies additional details regarding the operation.
    @param propertyList list containing the properties.
    @param handler enables providers to asynchronously return the results.
    */
    virtual void enumerateInstances(
        const OperationContext & context,
        const CIMReference & ref,
        const Uint32 flags,
        const Array<String>& propertyList,
        ResponseHandler<CIMInstance> & handler);

    /**
    Enumerates all the user names.
    */
    virtual void enumerateInstanceNames(
	const OperationContext & context,
	const CIMReference & classReference,
        ResponseHandler<CIMReference> & handler);

    /**
       Invoke Method, used to modify user's password
    */
    virtual void invokeMethod(
        const OperationContext & context,
        const CIMReference & ref,
        const String & methodName,
        const Array<CIMParamValue> & inParams,
        Array<CIMParamValue> & outParams,
        ResponseHandler<CIMValue> & handler);

    /**
    Standard initialization function for the provider.  This method should
    never be called since this is an internal control provider.
    */
    void initialize(CIMOMHandle& cimomHandle)
    {
	PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::initialize");

        //
        // get an instance of User Manager and initialize
        // the instance variable.
        //
        _userManager = UserManager::getInstance();

        // Can't get the respository handle here.

	PEG_METHOD_EXIT();
    }

    void terminate(void)
    {
	PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::terminate");

	PEG_METHOD_EXIT();
    }

private:

    //
    // User Manager Instance variable
    //
    UserManager*   _userManager;

    //
    // The repository pointer for use by the provider.
    //
    CIMRepository* _repository;


};

PEGASUS_NAMESPACE_END

#endif // UserAuthProvider_h
