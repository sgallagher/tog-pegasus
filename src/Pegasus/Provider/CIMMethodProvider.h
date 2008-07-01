//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMMethodProvider_h
#define Pegasus_CIMMethodProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>

#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Provider/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
    This class defines a set of functions that support the invocation of
    extrinsic methods on a CIM class or instance.

    <p>A method provider is not required to implement all the methods defined
    for a CIM class.  Multiple method providers may be registered for methods
    of the same class, but not for the same method.  (See the
    SupportedMethods property of the PG_ProviderCapabilities class.)</p>
*/
class PEGASUS_PROVIDER_LINKAGE CIMMethodProvider : public virtual CIMProvider
{
public:
    /** Constructs a CIMMethodProvider object with null values (default
        constructor).
    */
    CIMMethodProvider();

    /** CIMMethodProvider destructor.
    */
    virtual ~CIMMethodProvider();

    /**
        Invokes the specified method on the specified class or instance.

        @param context Specifies the client user's context for this operation,
            including the user ID.
        @param objectReference Specifies the fully qualified object path
            of the class or instance of interest.
        @param methodName Specifies the name of the method of interest.
        @param inParameters Specifies the input parameters of the method.
        @param handler A {@link ResponseHandler ResponseHandler} object used
            to deliver results to the CIM Server.

        @exception CIMNotSupportedException If the method is not supported.
        @exception CIMInvalidParameterException If a parameter is invalid.
        @exception CIMObjectNotFoundException If the object is not found.
        @exception CIMAccessDeniedException If the user requesting the action
            is not authorized to perform the action.
        @exception CIMOperationFailedException If the operation fails.
    */
    virtual void invokeMethod(
        const OperationContext& context,
        const CIMObjectPath& objectReference,
        const CIMName& methodName,
        const Array<CIMParamValue>& inParameters,
        MethodResultResponseHandler& handler) = 0;
};

PEGASUS_NAMESPACE_END

#endif
