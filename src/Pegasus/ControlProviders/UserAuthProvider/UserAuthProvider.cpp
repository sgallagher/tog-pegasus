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
#include <Pegasus/Common/PegasusVersion.h>

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/Security/UserManager/UserExceptions.h>

#include "UserAuthProvider.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The constants representing the Username and Password properties in the
    schema.
*/
static const char PROPERTY_NAME_USERNAME []       = "Username";

static const char PROPERTY_NAME_PASSWORD  []      = "Password";

static const char OLD_PASSWORD []                 = "OldPassword";

static const char NEW_PASSWORD []                 = "NewPassword";

/**
    The constants representing the namespace and authorization
    in the schema.
*/
static const char PROPERTY_NAME_NAMESPACE []      = "Namespace";

static const char PROPERTY_NAME_AUTHORIZATION []  = "Authorization";

/**
    The constant represeting the User class name.
*/
static const char CLASS_NAME_PG_USER []           = "PG_User";

static const char METHOD_NAME_MODIFY_PASSWORD []  = "modifyPassword";

/**
    The constant represeting the authorization class name
*/
static const char CLASS_NAME_PG_AUTHORIZATION []  = "PG_Authorization";

//
// Creates a new instance.
//
void UserAuthProvider::createInstance(
    const OperationContext & context,
    const CIMReference & instanceReference,
    const CIMInstance & myInstance,
    ResponseHandler<CIMReference> & handler)
{
    PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::createInstance");

    CIMValue    userName;
    CIMValue    password;
    String      userNameStr;
    String      passwordStr;
    String      namespaceStr;
    String      authorizationStr;

    CIMInstance          modifiedInst = myInstance;

    // begin processing the request
    handler.processing();

    //
    // check if the class name requested is PG_User
    //
    if (String::equal(CLASS_NAME_PG_USER , instanceReference.getClassName()))
    {
        try
        {
            //
            // Get the user name from the instance
            //
            Uint32 pos = myInstance.findProperty ( PROPERTY_NAME_USERNAME );
            CIMProperty prop = (CIMProperty)modifiedInst.getProperty(pos);
            userName = prop.getValue();
            userName.get(userNameStr);

            //
            // Get the password from the instance
            //
            pos = myInstance.findProperty ( PROPERTY_NAME_PASSWORD );
            prop = (CIMProperty) modifiedInst.getProperty(pos);
            password = prop.getValue();
            password.get(passwordStr);

            //
            // Add the user to the User Manager
            //
            _userManager->addUser( userNameStr, passwordStr);

        }
        catch ( CIMException &e )
        {
            handler.complete();
            PEG_METHOD_EXIT();
            throw e;
        }
        catch ( Exception &e )
        {
            handler.complete();
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
    }
    //
    // check if the class name requested is PG_Authorization
    //
    else if (String::equal(
        CLASS_NAME_PG_AUTHORIZATION, instanceReference.getClassName()))
    {
        try
        {
            //
            // Get the user name from the instance
            //
            Uint32 pos = myInstance.findProperty ( PROPERTY_NAME_USERNAME );
            CIMProperty prop = (CIMProperty)modifiedInst.getProperty(pos);
            prop.getValue().get(userNameStr);

            //
            // Get the namespace from the instance
            //
            pos = myInstance.findProperty ( PROPERTY_NAME_NAMESPACE );
            prop = (CIMProperty)modifiedInst.getProperty(pos);
            prop.getValue().get(namespaceStr);

            //
            // Get the authorization from the instance
            //
            pos = myInstance.findProperty ( PROPERTY_NAME_AUTHORIZATION );
            prop = (CIMProperty)modifiedInst.getProperty(pos);
            prop.getValue().get(authorizationStr);

            //
            // Check if the user is a valid system user
            //
            ArrayDestroyer<char> un(userNameStr.allocateCString());
                if ( !System::isSystemUser( un.getPointer() ) )
            {
                InvalidSystemUser isu(userNameStr);
                throw isu;
            }
            //
            // check if the user is a valid CIM user
            //
            if ( !_userManager->verifyCIMUser( userNameStr ) )
            {
                InvalidUser iu(userNameStr);
                throw iu;
            }

            //
            // check if the specified namespace is valid namespace
            //
            if ( !_userManager->verifyNamespace( namespaceStr ) )
            {
                InvalidNamespace ins(namespaceStr);
                throw ins;
            }

            _repository->createInstance(
                instanceReference.getNameSpace(), myInstance);

            //
            // set authorization in the UserManager
            //
            _userManager->setAuthorization(
                userNameStr, namespaceStr, authorizationStr );
        }
        catch ( InvalidUser &iu )
        {
        PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER, iu.getMessage());
        }
        catch ( InvalidSystemUser &isu )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER, isu.getMessage());
        }
        catch ( InvalidNamespace &ins )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER, ins.getMessage());
        }
        catch ( CIMException &e )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
        catch ( Exception &e )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
    }
    else
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION (
            CIM_ERR_NOT_SUPPORTED, instanceReference.getClassName());
    }

    handler.deliver(instanceReference);

    // complete processing the request
    handler.complete();

    PEG_METHOD_EXIT();
    return;
}

//
// Deletes the specified instance.
//
void UserAuthProvider::deleteInstance(
    const OperationContext & context,
    const CIMReference& myInstance,
    ResponseHandler<CIMInstance> & handler)
{
    CIMValue                userName ;
    String                  userNameStr;
    String                  namespaceStr;
    Array<KeyBinding>       kbArray;

    PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::deleteInstance");

    // begin processing the request
    handler.processing();

    //
    // check if the class name requested is PG_User
    //
    if (String::equal(CLASS_NAME_PG_USER , myInstance.getClassName()))
    {
        //
        // Get the user name from the instance
        //
        try
        {
            kbArray = myInstance.getKeyBindings();
            if ( ! kbArray.size() )
            {
                throw CIMException( CIM_ERR_INVALID_PARAMETER,
                     "Unable to find Key Property Username");
            }
            if ( kbArray[0].getName() == PROPERTY_NAME_USERNAME )
            {
                userNameStr = kbArray[0].getValue();
            }
            else
            {
                throw CIMException(CIM_ERR_INVALID_PARAMETER,
                     "Unexpected Key property");
            }

            //
            // Remove the user from User Manager
            //
            _userManager->removeUser(userNameStr);
        }
        catch ( CIMException &e )
        {
            handler.complete();
            PEG_METHOD_EXIT();
            throw e;
        }
        catch ( Exception &e )
        {
            handler.complete();
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
    }
    //
    // check if the class name requested is PG_Authorization
    //
    else if (String::equal(
        CLASS_NAME_PG_AUTHORIZATION, myInstance.getClassName()))
    {
        try
        {
            //
            // Get the user name and namespace from the instance
            //
            kbArray = myInstance.getKeyBindings();
            for (Uint32 i = 0; i < kbArray.size(); i++)
            {
                if ( kbArray[i].getName() == PROPERTY_NAME_USERNAME )
                {
                    userNameStr = kbArray[i].getValue();
                }
                else if ( kbArray[i].getName() == PROPERTY_NAME_NAMESPACE )
                {
                    namespaceStr = kbArray[i].getValue();
                }
            }
        }
        catch ( CIMException &e )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }

        if ( !userNameStr.size() )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION (
                CIM_ERR_INVALID_PARAMETER,
                "Username property can not be empty.") ;
        }
        else if ( !namespaceStr.size() )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION (
                CIM_ERR_INVALID_PARAMETER,
                "Namespace property can not be empty.") ;
        }

        try
        {
            //
            // ATTN: Note that the following is a hack, because
            // deleteInstance() in repository does not like
            // the hostname and namespace included in the CIMReference
            // passed to it as a parameter.
            //
            CIMReference ref("", "",
                myInstance.getClassName(), myInstance.getKeyBindings());

            _repository->deleteInstance(
                myInstance.getNameSpace(), ref);

            //
            // remove authorization in the UserManager
            //
            _userManager->removeAuthorization(
                userNameStr, namespaceStr );

        }
        catch ( CIMException &e )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
    }
    else
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_FOUND, myInstance.getClassName());
    }

   
    // complete processing the request
    handler.complete();

    PEG_METHOD_EXIT();
    return;
}

//
// Modify instance based on modifiedInstance.
//
void UserAuthProvider::modifyInstance(
    const OperationContext & context,
    const CIMReference & instanceReference,
    const CIMInstance& modifiedIns,
    const Uint32 flags,
    const CIMPropertyList & propertyList,
    ResponseHandler<CIMInstance> & handler)
{
    PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::modifyInstance"); 

    //
    // check if the class name requested is PG_Authorization
    //
    if (!String::equal(
        CLASS_NAME_PG_AUTHORIZATION, instanceReference.getClassName()))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION (
            CIM_ERR_NOT_SUPPORTED, instanceReference.getClassName());
    }

    CIMInstance newInstance = modifiedIns;

    // begin processing the request
    handler.processing();

    try
    {
        //
        // Get the user name from the instance
        //
        String userNameStr;
        String namespaceStr;
        String authorizationStr;

        Uint32 pos = modifiedIns.findProperty ( PROPERTY_NAME_USERNAME );
        CIMProperty prop = (CIMProperty)newInstance.getProperty(pos);
        prop.getValue().get(userNameStr);

        //
        // Get the namespace from the instance
        //
        pos = modifiedIns.findProperty ( PROPERTY_NAME_NAMESPACE );
        prop = (CIMProperty)newInstance.getProperty(pos);
        prop.getValue().get(namespaceStr);

        //
        // Get the authorization from the instance
        //
        pos = modifiedIns.findProperty ( PROPERTY_NAME_AUTHORIZATION );
        prop = (CIMProperty)newInstance.getProperty(pos);
        prop.getValue().get(authorizationStr);

        //
        // ATTN: Note that the following is a hack, because
        // modifyInstance() in repository does not like
        // the hostname and namespace included in the CIMReference
        // passed to it as a parameter.
        //
        CIMReference ref("", "",
            modifiedIns.getClassName(), instanceReference.getKeyBindings());

        CIMNamedInstance namedInstance(ref, modifiedIns);

        //
        // call modifyInstances of the repository
        //
        _repository->modifyInstance(
            instanceReference.getNameSpace(), namedInstance);

        //
        // set authorization in the UserManager
        //
        _userManager->setAuthorization(
            userNameStr, namespaceStr, authorizationStr );

    }
    catch(Exception& e)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }

    // complete processing the request
    handler.complete();

    PEG_METHOD_EXIT();
    return;
}

//
// Enumerates instances.
//
void UserAuthProvider::enumerateInstances(
    const OperationContext & context,
    const CIMReference & ref,
    const Uint32 flags,
    const CIMPropertyList& propertyList,
    ResponseHandler<CIMInstance> & handler)
{
    PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::enumerateInstances");

    Array<CIMInstance> instanceArray;
    Array<CIMNamedInstance> namedInstances;

    //
    // check if the class name requested is PG_Authorization
    //
    if (!String::equal(CLASS_NAME_PG_AUTHORIZATION, ref.getClassName()))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED, ref.getClassName());
    }

    // begin processing the request
    handler.processing();

    try
    {
        //
        // call enumerateInstances of the repository
        //
        namedInstances = _repository->enumerateInstances(
            ref.getNameSpace(), ref.getClassName());

    }
    catch(Exception& e)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }

    for(Uint32 i = 0, n = namedInstances.size(); i < n; i++)
    {
        handler.deliver(namedInstances[i].getInstance());
    }

    // complete processing the request
    handler.complete();

    PEG_METHOD_EXIT();
    return;
}

//
// Enumerates all the user names.
//
void UserAuthProvider::enumerateInstanceNames(
    const OperationContext & context,
    const CIMReference & classReference,
    ResponseHandler<CIMReference> & handler)
{
    PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::enumerateInstanceNames");

    Array<CIMReference> instanceRefs;
    Array<String>       userNames;
    Array<KeyBinding>   keyBindings;
    KeyBinding          kb;
    String            hostName;

    const String& className = classReference.getClassName();
    const String& nameSpace = classReference.getNameSpace();

    // begin processing the request
    handler.processing();

    //
    // check if the class name requested is PG_User
    //
    if (String::equal(CLASS_NAME_PG_USER , className))
    {
        try
        {
            hostName.assign(System::getHostName());

            _userManager->getAllUserNames(userNames);

            Uint32 size = userNames.size();

            for (Uint32 i = 0; i < size; i++)
            {
                keyBindings.append(KeyBinding(PROPERTY_NAME_USERNAME, userNames[i],
                   KeyBinding::STRING));

                //
                // Convert instance names to References
                //
                CIMReference ref(hostName, nameSpace, className, keyBindings);

                handler.deliver(ref);

                keyBindings.clear();
            }
        }
        catch(CIMException& e)
        {
            handler.complete();
            PEG_METHOD_EXIT();
            throw e;
        }
        catch(Exception& e)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
    }
    //
    // check if the class name requested is PG_Authorization
    //
    else if (String::equal(CLASS_NAME_PG_AUTHORIZATION, className))
    {
        try
        {
            //
            // call enumerateInstanceNames of the repository
            //
            instanceRefs = _repository->enumerateInstanceNames(
                nameSpace, className);

            handler.deliver(instanceRefs);

        }
        catch ( CIMException &e )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
    }
    else
    {
        handler.complete();
        
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED, className);
    }

    // complete processing the request
    handler.complete();

    PEG_METHOD_EXIT();
    return;
}

//
// Invoke Method, used to modify user's password
//
void UserAuthProvider::invokeMethod(
    const OperationContext & context,
    const CIMReference & ref,
    const String & methodName,
    const Array<CIMParamValue> & inParams,
    Array<CIMParamValue> & outParams,
    ResponseHandler<CIMValue> & handler)
{
    String            userName;
    String            password;
    String             newPassword;
    Array<KeyBinding>     kbArray;

    PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::invokeMethod");

    // Begin processing the request
    handler.processing();

    // Check if the class name is PG_USER
    if ( !String::equal (CLASS_NAME_PG_USER, ref.getClassName()))
    {
        handler.complete();
            throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED, ref.getClassName());
    }

    // Check if the method name is correct
    if ( !String::equal( methodName, METHOD_NAME_MODIFY_PASSWORD ))
    {
        handler.complete();
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION (
            CIM_ERR_FAILED,
            "Unsupported method name, " + methodName );
    }

    // Check if all the input parameters are passed.
    if ( inParams.size() < 2 )
    {
        handler.complete();
        PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION( CIM_ERR_INVALID_PARAMETER,
                                    "Input parameters are not valid.");
    }

    try
    {
        kbArray = ref.getKeyBindings();

        if ( !kbArray.size() )
        {
            PEG_METHOD_EXIT();
            throw CIMException( CIM_ERR_INVALID_PARAMETER,
                     "Unable to find Key Property Username");
        }

        //
        // Get the user name
        //
            if ( kbArray[0].getName() == PROPERTY_NAME_USERNAME )
        {
            userName = kbArray[0].getValue();
        }
        else
        {
            PEG_METHOD_EXIT();
            throw CIMException( CIM_ERR_INVALID_PARAMETER,
                    "Unexpected key property");
        }

        //
        // Get the old and the new password params
        //
        for ( Uint32 i=0; i < 2; i++)
        {
            //
            // check the param name
            //
            if ( inParams[i].getParameterName() == OLD_PASSWORD )
            {
                inParams[i].getValue().get(password);
            }
            if ( inParams[i].getParameterName() == NEW_PASSWORD )
            {
                inParams[i].getValue().get(newPassword);
            }
        }

        // Modify the user's password in User Manager
        _userManager->modifyUser(
            userName,
            password,
            newPassword);

    }
    catch (CIMException& e)
    {
        handler.complete();
        PEG_METHOD_EXIT();
        throw e;
    }
    catch (Exception& e)
    {
        handler.complete();
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }

    // Return zero as there is no error
    Uint32 retCode = 0;
    handler.deliver(CIMValue(retCode));

    // complete processing the request
    handler.complete();

    PEG_METHOD_EXIT();
    return;
}

PEGASUS_NAMESPACE_END

