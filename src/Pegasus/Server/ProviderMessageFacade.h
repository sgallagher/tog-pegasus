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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Dan Gorey, IBM (djgorey@us.ibm.com)
//
// Modified By:  Seema Gupta (gseema@in.ibm.com) for Bug#1441
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderMessageFacade_h
#define Pegasus_ProviderMessageFacade_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMessage.h>

#include <Pegasus/ProviderManager2/Default/ProviderFacade.h>

#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The ProviderMessageFacade class wraps a ProviderFacade instance so that
    it accepts CIMOperationRequestMessages in addition to the direct
    provider API method calls.
*/
class PEGASUS_SERVER_LINKAGE ProviderMessageFacade : public ProviderFacade
{
public:

    ProviderMessageFacade(CIMProvider * provider);
    virtual ~ProviderMessageFacade(void);

    Message * handleRequestMessage(Message * message) throw();

protected:

    Message * _handleGetInstanceRequest(Message * message);
    Message * _handleEnumerateInstancesRequest(Message * message);
    Message * _handleEnumerateInstanceNamesRequest(Message * message);
    Message * _handleCreateInstanceRequest(Message * message);
    Message * _handleModifyInstanceRequest(Message * message);
    Message * _handleDeleteInstanceRequest(Message * message);
    Message * _handleExecuteQueryRequest(Message * message);
    Message * _handleAssociatorsRequest(Message * message);
    Message * _handleAssociatorNamesRequest(Message * message);
    Message * _handleReferencesRequest(Message * message);
    Message * _handleReferenceNamesRequest(Message * message);
    Message * _handleGetPropertyRequest(Message * message);
    Message * _handleSetPropertyRequest(Message * message);
    Message * _handleInvokeMethodRequest(Message * message);
};

PEGASUS_NAMESPACE_END

#endif
