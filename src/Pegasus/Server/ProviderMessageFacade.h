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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderMessageFacade_h
#define Pegasus_ProviderMessageFacade_h

#include <Pegasus/Common/Config.h>

#include <Pegasus/ProviderManager/ProviderFacade.h>
#include <Pegasus/Common/CIMMessage.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The ProviderMessageFacade class wraps a ProviderFacade instance so that
    it accepts CIMOperationRequestMessages in addition to the direct
    provider API method calls.
*/
class PEGASUS_SERVER_LINKAGE ProviderMessageFacade : public ProviderFacade
{
public:

    ProviderMessageFacade(CIMBaseProvider * provider);
    virtual ~ProviderMessageFacade(void);

    Message * handleRequestMessage(Message * message) throw();

protected:

    Message * _handleGetInstanceRequest(Message * message) throw();
    Message * _handleEnumerateInstancesRequest(Message * message) throw();
    Message * _handleEnumerateInstanceNamesRequest(Message * message) throw();
    Message * _handleCreateInstanceRequest(Message * message) throw();
    Message * _handleModifyInstanceRequest(Message * message) throw();
    Message * _handleDeleteInstanceRequest(Message * message) throw();
    Message * _handleExecuteQueryRequest(Message * message) throw();
    Message * _handleAssociatorsRequest(Message * message) throw();
    Message * _handleAssociatorNamesRequest(Message * message) throw();
    Message * _handleReferencesRequest(Message * message) throw();
    Message * _handleReferenceNamesRequest(Message * message) throw();
    Message * _handleGetPropertyRequest(Message * message) throw();
    Message * _handleSetPropertyRequest(Message * message) throw();
    Message * _handleInvokeMethodRequest(Message * message) throw();
};

PEGASUS_NAMESPACE_END

#endif
