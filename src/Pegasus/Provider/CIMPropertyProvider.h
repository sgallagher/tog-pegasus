//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMPropertyProvider_h
#define Pegasus_CIMPropertyProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMBaseProvider.h>

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Provider/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
This class defines the set of methods implemented by an property provider. A providers that derives
from this class must implement all methods. The minimal method implementation simply throw the
NotSupported exception.
*/
class PEGASUS_PROVIDER_LINKAGE CIMPropertyProvider : public virtual CIMBaseProvider
{
public:
    CIMPropertyProvider(void);
    virtual ~CIMPropertyProvider(void);

    /**
    Instructs the provider to get the value specified by the propertyName parameter of the
    instance specified by the instanceReference parameter.
	
    @param contex contains security and locale information relevant for the lifetime
    of this operation.

    @param instanceReference specifies the fully qualified object path of the instance of interest.

    @param propertyName specifies the name of the property of interest.

    @param handler asynchronusly processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    @exception ObjectNotFound
    @exception AccessDenied
    @exception OperationFailed
    */
    virtual void getProperty(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const String & propertyName,
	ResponseHandler<CIMValue> & handler) = 0;

    /**
    Instructs the provider to set the value specified by the propertyName parameter of the
    instance specified by the instanceReference parameter.

    @param contex contains security and locale information relevant for the lifetime
    of this operation.

    @param instanceReference specifies the fully qualified object path of the instance of interest.

    @param propertyName specifies the name of the property of interest.

    @param handler asynchronusly processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    @exception ObjectNotFound
    @exception AccessDenied
    @exception OperationFailed
    */
    virtual void setProperty(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const String & propertyName,
	const CIMValue & newValue,
	ResponseHandler<CIMValue> & handler) = 0;

};

PEGASUS_NAMESPACE_END

#endif
