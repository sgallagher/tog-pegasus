//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMInstanceProvider_h
#define Pegasus_CIMInstanceProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMBaseProvider.h>

#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_NAMESPACE_BEGIN
/**
This class defines the set of methods implemented by an instance provider. A providers that derives
from this class must implement all methods. The minimal method implementation simply throw the
NotSupported exception.
*/
class PEGASUS_PROVIDER_LINKAGE CIMInstanceProvider : public virtual CIMBaseProvider
{
public:
    CIMInstanceProvider(void);
    virtual ~CIMInstanceProvider(void);

    /**
    Instructs the provider to return a single instance.

    @param context specifies security and locale information relevant for the lifetime
    of this operation.

    @param instanceReference specifies the fully qualified object path of the instance of interest.

    @param flags specifies additional details regarding the operation.

    <UL>
	<LI>LOCAL_ONLY - include only elements (properties and qualifiers) of the specified class.
	<LI>INCLUDE_QUALIFIERS - include instance and property qualifiers.
	<LI>INCLUDE_CLASS_ORIGIN - include the parent class name.
	<LI>PARTIAL_INSTANCE - use the propertyList parameter to determine the set of properties
	to include.
	</LI>
    </UL>

    @param propertyList Assuming the flags parameter specifies the PARTIAL_INSTANCE bit, this parameter
    specifies the minimum set of properties required in instances returned by this operation. Because
    support for this parameter is optional, the instances may contain additional properties not specified
    in the list.

    @param handler enables providers to asynchronously return the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    @exception ObjectNotFound
    @exception AccessDenied
    @exception OperationFailed
    */
    virtual void getInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler) = 0;

    /**
    Instructs the provider to returns all instances of a single class.

    @param context contains security and locale information relevant for the lifetime
    of this operation.

    @param classReference specifies the fully qualified object path to the class of
    interest.

    @param flags specifies additional details regarding the operation.

    <UL>
	<LI>LOCAL_ONLY - include only elements (properties and qualifiers) of the specified class.
	<LI>DEEP_INHERITANCE - include all properties (including thos derived).
	<LI>INCLUDE_QUALIFIERS - include instance and properties qualified in the result.
	<LI>INCLUDE_CLASS_ORIGIN - include the parent class name.
	<LI>PARTIAL_INSTANCE - use the propertyList parameter to determine the set of
	properties to provide.
	</LI>
    </UL>

    @param propertyList Assuming the flags parameter specifies the PARTIAL_INSTANCE bit, this parameter
    specifies the minimum set of properties required in instances returned by this operation. Because
    support for this parameter is optional, the instances may contain additional properties not specified
    in the list.

    @param handler enables providers to asynchronously return the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    @exception ObjectNotFound
    @exception AccessDenied
    @exception OperationFailed
    */
    virtual void enumerateInstances(
	const OperationContext & context,
	const CIMReference & classReference,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler) = 0;

    /**
    Instructs the provider to returns all instance names of a single class.

    @param context contains security and locale information relevant for the lifetime
    of this operation.

    @param classReference specifies the fully qualified object path to the class of
    interest.

    @param flags specifies additional details regarding the operation.

    <UL>
	<LI>LOCAL_ONLY - include only elements (properties and qualifiers) of the specified class.
	<LI>INCLUDE_QUALIFIERS - include instance and properties qualified in the result.
	<LI>INCLUDE_CLASS_ORIGIN - include the parent class name.
	<LI>DEEP_INHERITANCE - obseverve the property 'override' qualifier.
	<LI>PARTIAL_INSTANCE - use the propertyList parameter to determine the set of
	properties to provide.
	</LI>
    </UL>

    @param propertyList Assuming the flags parameter specifies the PARTIAL_INSTANCE bit, this parameter
    specifies the minimum set of properties required in instances returned by this operation. Because
    support for this parameter is optional, the instances may contain additional properties not specified
    in the list.

    @param handler enables providers to asynchronously return the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    @exception ObjectNotFound
    @exception AccessDenied
    @exception OperationFailed
    */
    virtual void enumerateInstanceNames(
	const OperationContext & context,
	const CIMReference & classReference,
	ResponseHandler<CIMReference> & handler) = 0;

    /**
    The <TT>modifyInstance</TT> method replaces the current instance specified in the
    instanceReference parameter. Properties not present in the instance are replaced by
    the class default values or left null.

    If the instance do not exist, the method should fail.

    @param context contains security and locale information relevant for the lifetime
    of this operation.

    @param instanceReference specifies the fully qualified object path of the instance of interest.

    @param instanceObject contains the partial or complete instance to create.

    @param flags specifies additional details regarding the operation.

    <UL>
	<LI>INCLUDE_QUALIFIERS - update the qualifiers in the instance as well as the property values.
	<LI>PARTIAL_INSTANCE - use the propertyList parameter to determine the set of
	properties to update.
	</LI>
    </UL>

    @param propertyList Assuming the flags parameter specifies the PARTIAL_INSTANCE bit, this parameter
    specifies the set of properties required to be updated in the instance. Support
    for this parameter is NOT optional.  Providers that do not support this
    feature must throw a NotSupported exception whenever the PARTIAL_INSTANCE
    flag is "true".

    @param handler enables providers to asynchronously return the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    @exception ObjectNotFound
    @exception AccessDenied
    @exception OperationFailed
    */
    virtual void modifyInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const CIMInstance & instanceObject,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler) = 0;

    /**
    Instructs the provider to create a new instance using the specified instanceObject.

    @param context contains security and locale information relevant for the lifetime
    of this operation.

    @param instanceReference specifies the fully qualified object path of the instance to create.
    <b>NOTE:</b> It is possible that the values for some properties are null, and that these null
    properties may be keys.

    *** ATTN: Are the unspecified values set to default by the CIMOM? ***

    @param instanceObject contains the partial or complete instance to create.

    @param handler enables providers to asynchronously return the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    @exception ObjectAlreadyExists
    @exception AccessDenied
    @exception OperationFailed
    */
    virtual void createInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	const CIMInstance & instanceObject,
	ResponseHandler<CIMReference> & handler) = 0;

    /**
    Instructs the provider to delete the instance specified by the instanceReference parameter.

    @param context contains security and locale information relevant for the lifetime
    of this operation.

    @param instanceReference specifies the fully qualified object path of the instance to delete.

    @param handler enables providers to asynchronously return the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    @exception ObjectNotFound
    @exception AccessDenied
    @exception OperationFailed
    */
    virtual void deleteInstance(
	const OperationContext & context,
	const CIMReference & instanceReference,
	ResponseHandler<CIMInstance> & handler) = 0;
};

PEGASUS_NAMESPACE_END

#endif
