//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Adrian Schuur (schuur@de.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMInstanceQueryProvider_h
#define Pegasus_CIMInstanceQueryProvider_h

#include "CIMInstanceProvider.h"
#include <Pegasus/Query/QueryExpression/QueryExpression.h>

PEGASUS_NAMESPACE_BEGIN

/** This class extends then CIMInstanceProvider class enabling query support.

<p>In addition to
functions inherited from the
{@link CIMInstanceProvider CIMInstanceProvider} interface,
the functions in the Instance Query Provider interface are:</p>

<p><ul>
<li>{@link execQuery execQuery}</li>
</ul></p>

<p>The Instance Query Provider receives operation requests from
clients through calls to these functions by the CIM Server. Its
purpose is to convert these to calls to system services,
operations on system resources, or whatever platform-specific
behavior is required to perform the operation modeled by
the request. The specific requirements for each of the interface
functions are discussed in their respective sections.</p>
*/

class PEGASUS_PROVIDER_LINKAGE CIMInstanceQueryProvider : public CIMInstanceProvider
{
public:
    /**
    Constructor.
    The constructor should not do anything.
    */
    CIMInstanceQueryProvider(void);

    /**
    Destructor.
    The destructor should not do anything.
    */
    virtual ~CIMInstanceQueryProvider(void);

    /**
    \Label{execQuery}
    Return all instances of the specified class filtered by the query.

    @param context contains security and locale information relevant for the lifetime of this operation.
    @param objectPath contains namespace and classname for which the query is to be performed.
    @param query enables access to query details like query statement and language.
    @param handler asynchronusly processes the results of this operation.

    @exception CIMNotSupportedException
    @exception CIMAccessDeniedException
    @exception CIMOperationFailedException
    */

    virtual void execQuery(
       const OperationContext & context,
       const CIMObjectPath & objectPath,
       const QueryExpression & query,
       InstanceResponseHandler & handler) = 0;
};

PEGASUS_NAMESPACE_END

#endif
