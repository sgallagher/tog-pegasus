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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMAssociationProvider_h
#define Pegasus_CIMAssociationProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMBaseProvider.h>

#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_NAMESPACE_BEGIN

/**
This class defines the set of methods implemented by an association provider. A providers that derives
from this class must implement all methods. The minimal method implementation simply throw the
NotSupported exception.
*/
class PEGASUS_PROVIDER_LINKAGE CIMAssociationProvider : public virtual CIMBaseProvider
{
public:
    CIMAssociationProvider(void);
    virtual ~CIMAssociationProvider(void);

    /**

    @param contex contains security and locale information relevant for the lifetime
    of this operation.

    @param objectName
    @param associationClass
    @param resultClass
    @param role
    @param resultRole
    @param flags
    @param propertyList

    @param handler asynchronusly processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    */
    virtual void associators(
	const OperationContext & context,
	const CIMReference & objectName,
	const String & associationClass,
	const String & resultClass,
	const String & role,
	const String & resultRole,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMObject> & handler) = 0;

    /**
    @param contex contains security and locale information relevant for the lifetime
    of this operation.

    @param objectName
    @param associationClass
    @param resultClass
    @param role
    @param resultRole

    @param handler asynchronusly processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    */
    virtual void associatorNames(
	const OperationContext & context,
	const CIMReference & objectName,
	const String & associationClass,
	const String & resultClass,
	const String & role,
	const String & resultRole,
	ResponseHandler<CIMReference> & handler) = 0;

    /**
    @param contex contains security and locale information relevant for the lifetime
    of this operation.

    @param objectName
    @param resultClass
    @param role
    @param resultRole
    @param flags
    @param propertyList

    @param handler asynchronusly processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    */
    virtual void references(
	const OperationContext & context,
	const CIMReference & objectName,
	const String & resultClass,
	const String & role,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMObject> & handler) = 0;

    /**
    @param contex contains security and locale information relevant for the lifetime
    of this operation.

    @param objectName
    @param resultClass
    @param role

    @param handler asynchronusly processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    */
    virtual void referenceNames(
	const OperationContext & context,
	const CIMReference & objectName,
	const String & resultClass,
	const String & role,
	ResponseHandler<CIMReference> & handler) = 0;
};

PEGASUS_NAMESPACE_END

#endif
