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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OOPProviderManagerRouter_h
#define Pegasus_OOPProviderManagerRouter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/CIMMessage.h>

#include <Pegasus/ProviderManager2/ProviderManagerRouter.h>
#include <Pegasus/ProviderManager2/ProviderManager.h>
#include <Pegasus/ProviderManager2/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

typedef void (*PEGASUS_PROVIDERMODULEFAIL_CALLBACK_T)(const String &,
    const String &, Uint16);

class ProviderAgentContainer;

typedef HashTable<String, ProviderAgentContainer*, EqualFunc<String>,
    HashFunc<String> > ProviderAgentTable;

class PEGASUS_PPM_LINKAGE OOPProviderManagerRouter
    : public ProviderManagerRouter
{
public:
    OOPProviderManagerRouter(
        PEGASUS_INDICATION_CALLBACK_T indicationCallback,
        PEGASUS_RESPONSE_CHUNK_CALLBACK_T responseChunkCallback,
        PEGASUS_PROVIDERMODULEFAIL_CALLBACK_T providerModuleFailCallback);

    virtual ~OOPProviderManagerRouter();

    virtual Message* processMessage(Message* message);

    virtual Boolean hasActiveProviders();
    virtual void unloadIdleProviders();

private:
    //
    // Private methods
    //

    /** Unimplemented */
    OOPProviderManagerRouter();
    /** Unimplemented */
    OOPProviderManagerRouter(const OOPProviderManagerRouter&);
    /** Unimplemented */
    OOPProviderManagerRouter& operator=(const OOPProviderManagerRouter&);

    /**
        Return a pointer to the ProviderAgentContainer for the specified
        module instance and requesting user.  If no appropriate
        ProviderAgentContainer exists, one is created in an uninitialized state.
     */
    ProviderAgentContainer* _lookupProviderAgent(
        const CIMInstance& providerModule,
        CIMRequestMessage* request);

    /**
        Return an array of pointers to ProviderAgentContainers for the
        specified moduleName.
     */
    Array<ProviderAgentContainer*> _lookupProviderAgents(
        const String& moduleName);

    /**
        Send the specified CIMRequestMessage to all initialized Provider
        Agents in the _providerAgentTable.
     */
    CIMResponseMessage* _forwardRequestToAllAgents(CIMRequestMessage* request);

    //
    // Private data
    //

    /**
        Callback function to which all generated indications are sent for
        processing.
     */
    PEGASUS_INDICATION_CALLBACK_T _indicationCallback;

    /**
        Callback function to which all response chunks are sent for processing.
     */
    PEGASUS_RESPONSE_CHUNK_CALLBACK_T _responseChunkCallback;

    /**
        Callback function to be called upon detection of failure of a
        provider module.
     */
    PEGASUS_PROVIDERMODULEFAIL_CALLBACK_T _providerModuleFailCallback;

    /**
        The _providerAgentTable contains a ProviderAgentContainer entry for
        each of the Provider Agent processes for which a request has been
        processed.  ProviderAgentContainer objects are persistent; once one
        is created it is never deleted.
     */
    ProviderAgentTable _providerAgentTable;
    /**
        The _providerAgentTableMutex must be locked whenever the
        _providerAgentTable is accessed.
     */
    Mutex _providerAgentTableMutex;
};

PEGASUS_NAMESPACE_END

#endif
