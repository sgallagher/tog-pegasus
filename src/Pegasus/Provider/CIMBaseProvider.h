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

#ifndef Pegasus_CIMBaseProvider_h
#define Pegasus_CIMBaseProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Provider/ProviderException.h>
#include <Pegasus/Provider/ResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN

/** ATTN:
*/
class PEGASUS_PROVIDER_LINKAGE CIMBaseProvider
{
public:
    CIMBaseProvider(void);
    virtual ~CIMBaseProvider(void);

    /**
    The <TT>initialize</TT> method allows the provider to conduct the
    necessary preparations to handle requests or generate indications.
    It is called only once during the lifetime of the provider. This
    method must complete before the CIMOM invokes any other method of
    the provider, other than terminate.

    @param cimom represents an indirect handle to the local CIMOM. The
    provider should save a copy of the handle and use it to communicate
    with the CIMOM. This is useful when the provider requires information
    managed in the repository or other providers. The provider may use
    the handle immediately.

    @exception None
    */
    virtual void initialize(CIMOMHandle & cimom) = 0;

    /**
    The <TT>terminate</TT> method allows the provider to conduct the
    necessary preparations to prepare for unloading. This method may be called
    at any time, including initialization. Once invoked, no other provider
    methods are invoked without a call to initialize();

    @exception None
    */
    virtual void terminate(void) = 0;

};

PEGASUS_NAMESPACE_END

#endif
