//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Markus Mueller
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CMPIBaseAdapter_h 
#define Pegasus_CMPIBaseAdapter_h 

#include <Pegasus/Common/Config.h>

#include <cctype>
#include <iostream>

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Signal.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Tracer.h>

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Provider/CIMBaseProvider.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>

#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/ProviderManager/ProviderAdapter.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The constants representing the string literals.
*/

class CMPIAdapter:     public virtual CIMBaseProvider,
                       public virtual CIMInstanceProvider,
                       public virtual CIMAssociationProvider,
                       public virtual CIMMethodProvider,
                       public virtual ProviderAdapter
{
public:

    CMPIAdapter(const String & adapterName,
                    const String & providerName,
                    const String & className = String::EMPTY):

    ProviderAdapter(adapterName, providerName, className)
    {
    }

    virtual ~CMPIAdapter()
    {
       ProviderAdapterManager::get_pamgr()->deleteAdapter(_adapterName);
       terminate();
    }

    //CIMBaseProvider Interface
    virtual void initialize(CIMOMHandle & cimom);
    virtual void terminate();

        // CIMInstanceProvider interface
    void getInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler);

    void deleteInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        InstanceResponseHandler & handler);

    void createInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        ObjectPathResponseHandler & handler);

    void modifyInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        const Boolean includeQualifiers,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler);

    void enumerateInstances(
        const OperationContext & context,
        const CIMObjectPath & classReference,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler);

    void enumerateInstanceNames(
        const OperationContext & context,
        const CIMObjectPath & classReference,
        ObjectPathResponseHandler & handler);

//
// CIMAssociationProvider interface
//

    void associators(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const String & associationClass,
        const String & resultClass,
        const String & role,
        const String & resultRole,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        ObjectResponseHandler & handler);

    void associatorNames(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const String & associationClass,
        const String & resultClass,
        const String & role,
        const String & resultRole,
        ObjectPathResponseHandler & handler);

    void references(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const String & resultClass,
        const String & role,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        ObjectResponseHandler & handler);

    void referenceNames(
        const OperationContext & context,
        const CIMObjectPath & objectName,
        const String & resultClass,
        const String & role,
        ObjectPathResponseHandler & handler);

//
// CIMMethodProvider interface
//

    void invokeMethod(
        const OperationContext & context,
        const CIMObjectPath & objectReference,
        const String & methodName,
        const Array<CIMParamValue> & inParameters,
        MethodResultResponseHandler & handler);

protected:
    CIMOMHandle _cimom;

private:
    String _libraryName;
    DynamicLibraryHandle_ * _library;

    SignalHandler * _signal;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CMPIBaseAdapter_h */
