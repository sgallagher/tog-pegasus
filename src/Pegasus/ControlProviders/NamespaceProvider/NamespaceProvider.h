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
// Author: Karl Schopmeyer (k.schopmeyer@opengrooup.org)
//
// Modified By: 
//
//%////////////////////////////////////////////////////////////////////////////


#ifndef NamespaceProvider_h
#define NamespaceProvider_h

///////////////////////////////////////////////////////////////////////////////
//  ConfigSetting Provider
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include "Linkage.h"

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>
#include <Pegasus/Provider/OperationFlag.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The NamespaceProvider provides responses to the CIM Operations
    defined in the DMTF document CIM Operations over HTTP (Section 2.5).
    In particular, It provides information on the namespaces and allows
    creation and deletion of namespaces using the __namespace class
    Note however, that the DMTF specification model does not define a class for
    __namespace so that it is, a "false" class.  We assume that it has the
    same characteristics as the CIM_namespace class defined in CIM 2.6.

    This provider implements the following functions:
    - createInstance		( adds a new namespace to the repository)
    - getInstance		( Gets one instance containing a namespace name)
    - modifyInstance		( Not supported )
    - enumerateInstances	( Lists all namespaces as Instances)
    - enumerateInstanceNames	( Lists all namespace names )
    
*/

class PEGASUS_NAMESPACEPROVIDER_LINKAGE NamespaceProvider
    : public CIMInstanceProvider
{
public:

    NamespaceProvider(CIMRepository* repository)
    {
	PEG_METHOD_ENTER(TRC_USER_MANAGER,"NamespaceProvider::NamespaceProvider");
        _repository = repository;
	PEG_METHOD_EXIT();
    }

    virtual ~NamespaceProvider()
    {
	PEG_METHOD_ENTER(TRC_USER_MANAGER,"NamespaceProvider::~NamespaceProvider");

	PEG_METHOD_EXIT();
    }

    /**
    Creates a new instance. This function is used to create new namespaces.

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
    Deletes the specified instance. In the namespace provider, this deletes
    the defined namespace.

    @param  context
    @param  InstanceName
    @param  handler
    */
    virtual void deleteInstance(
	const OperationContext & context,
        const CIMReference& instanceName,
	ResponseHandler<CIMInstance> & handler);

    /**
    Returns the instance based on instanceName.

    @param context specifies security and locale information relevant for
		   the lifetime of this operation.
    @param instanceName name of the class for which instance is requested.
    @param flags specifies additional details regarding the operation.
    @param propertyList list containing the properties.
    @param handler enables providers to asynchronously return the results.
    */
    virtual void getInstance(
	const OperationContext & context,
        const CIMReference& instanceName,
	const Uint32 flags,
        const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler);

    /**
    Modify instance based on modifiedInstance.

    @param context specifies security and locale information relevant for
		   the lifetime of this operation.
    @param instanceReference the fully qualified object path of the instance.
    @param modifiedInstance  the modified instance.
    @param flags specifies additional details regarding the operation.
    @param propertyList list containing the properties to which the modify
                        operation must be limited.
    @param handler enables providers to asynchronously return the results.
    */
    void modifyInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
        const CIMInstance& modifiedIns,
	const Uint32 flags,
        const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "");
    }

    /**
    Enumerates all the config properties and values.

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
        const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler);

    /**
    Enumerates all the config property names.

    @param context specifies security and locale information relevant for
		   the lifetime of this operation.
    @param classReference the fully qualified object path of the instance.
    @param handler enables providers to asynchronously return the results.
    */
    virtual void enumerateInstanceNames(
	const OperationContext & context,
	const CIMReference & classReference,
        ResponseHandler<CIMReference> & handler);

    /**
    Standard initialization function for the provider.
    */
    void initialize(CIMOMHandle& cimomHandle)
    {
        // derefence repository pointer and save for later.
	//ATTN: Cannot get repository here. 
	// _repository = cimomHandle.getRepository();
    }

    void terminate(void)
    {
    }

private:

    //
    // Repository Instance variable
    //
       CIMRepository*   _repository;

};

PEGASUS_NAMESPACE_END

#endif // NamespaceProvider_h
