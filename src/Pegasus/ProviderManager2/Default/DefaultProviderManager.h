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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//              Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//              Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Adrian Schuur (schuur@de.ibm.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DefaultProviderManager_h
#define Pegasus_DefaultProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/Triad.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/OperationContextInternal.h>

#include <Pegasus/ProviderManager2/ProviderManager.h>
#include <Pegasus/ProviderManager2/ProviderName.h>

#include <Pegasus/ProviderManager2/Default/LocalProviderManager.h>
#include <Pegasus/ProviderManager2/OperationResponseHandler.h>
#include <Pegasus/ProviderManager2/Default/Provider.h>

#include <Pegasus/ProviderManager2/Default/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

typedef HashTable<String,
        EnableIndicationsResponseHandler *,
        EqualFunc<String>,
        HashFunc<String> > IndicationResponseTable;

class PEGASUS_DEFPM_LINKAGE DefaultProviderManager : public ProviderManager
{
public:
    DefaultProviderManager(void);
    virtual ~DefaultProviderManager(void);

    virtual Message * processMessage(Message * request);

    virtual Boolean hasActiveProviders();
    virtual void unloadIdleProviders();

protected:
    Message * handleUnsupportedRequest(const Message * message);

    Message * handleGetInstanceRequest(const Message * message);
    Message * handleEnumerateInstancesRequest(const Message * message);
    Message * handleEnumerateInstanceNamesRequest(const Message * message);
    Message * handleCreateInstanceRequest(const Message * message);
    Message * handleModifyInstanceRequest(const Message * message);
    Message * handleDeleteInstanceRequest(const Message * message);

    Message * handleExecQueryRequest(const Message * message);

    Message * handleAssociatorsRequest(const Message * message);
    Message * handleAssociatorNamesRequest(const Message * message);
    Message * handleReferencesRequest(const Message * message);
    Message * handleReferenceNamesRequest(const Message * message);

    Message * handleGetPropertyRequest(const Message * message);
    Message * handleSetPropertyRequest(const Message * message);

    Message * handleInvokeMethodRequest(const Message * message);

    Message * handleCreateSubscriptionRequest(const Message * message);
    Message * handleModifySubscriptionRequest(const Message * message);
    Message * handleDeleteSubscriptionRequest(const Message * message);

    Message * handleExportIndicationRequest(const Message * message);

    Message * handleDisableModuleRequest(const Message * message);
    Message * handleEnableModuleRequest(const Message * message);
    Message * handleStopAllProvidersRequest(const Message * message);
    Message * handleInitializeProviderRequest(const Message * message);
    Message * handleSubscriptionInitCompleteRequest
        (const Message * message);

    void _insertEntry(const Provider & provider, const EnableIndicationsResponseHandler *handler);
    EnableIndicationsResponseHandler * _removeEntry(const String & key);

    String _generateKey(const Provider & provider);
    String _generateKey(const String & providerName,const String & providerFileName);

    ProviderName _resolveProviderName(const ProviderIdContainer & providerId);

protected:
    IndicationResponseTable _responseTable;
    LocalProviderManager providerManager;

private:

    /**
        Calls the provider's enableIndications() method.
        If successful, the indications response handler is stored in the 
        _responseTable.

        @param  req_provider  CIMInstance for the provider to be enabled
        @param  _indicationCallback  PEGASUS_INDICATION_CALLBACK for indications
        @param  ph  OpProviderHolder for the provider to be enabled

        Note that since an exception thrown by the provider's 
        enableIndications() method is considered a provider error, any such
        exception is ignored, and no exceptions are thrown by this method.
     */
    void _callEnableIndications
        (CIMInstance & req_provider,
         PEGASUS_INDICATION_CALLBACK _indicationCallback,
         OpProviderHolder & ph);
};

PEGASUS_NAMESPACE_END

#endif
