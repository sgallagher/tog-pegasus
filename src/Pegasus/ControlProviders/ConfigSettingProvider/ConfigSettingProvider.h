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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////


#ifndef ConfigSettingProvider_h
#define ConfigSettingProvider_h

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
    The ConfigSettingProvider provides responses to the CIM Operations
    defined in the DMTF document CIM Operations over HTTP (Section 2.5).

    This provider implements the following functions:
    - getInstance
    - modifyInstance
    - enumerateInstances
    - enumerateInstanceNames
*/

class PEGASUS_CONFIGSETTINGPROVIDER_LINKAGE ConfigSettingProvider
    : public CIMInstanceProvider
{
public:

    ConfigSettingProvider()
    {
        _configManager = ConfigManager::getInstance();
    }

    virtual ~ConfigSettingProvider()
    {

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
	ResponseHandler<CIMReference> & handler)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "");
    }

    /**
    Deletes the specified instance.

    @param  context
    @param  InstanceName
    @param  handler
    */
    virtual void deleteInstance(
	const OperationContext & context,
        const CIMReference& instanceName,
	ResponseHandler<CIMInstance> & handler)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "");
    }

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
	ResponseHandler<CIMInstance> & handler);

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
        //
        // get an instance of Config Manager and initialize
        // the instance variable.
        //
        _configManager = ConfigManager::getInstance();
    }

    void terminate(void)
    {
    }

private:

    //
    // Config Manager Instance variable
    //
    ConfigManager*   _configManager;

};

PEGASUS_NAMESPACE_END

#endif // ConfigSettingProvider_h
