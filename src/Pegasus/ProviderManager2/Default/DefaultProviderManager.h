//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

    virtual Message * processMessage(Message * request, ProviderName providerName);

protected:
    Message * handleUnsupportedRequest(const Message * message, const ProviderName providerName);

    Message * handleGetInstanceRequest(const Message * message, const ProviderName providerName);
    Message * handleEnumerateInstancesRequest(const Message * message, const ProviderName providerName);
    Message * handleEnumerateInstanceNamesRequest(const Message * message, const ProviderName providerName);
    Message * handleCreateInstanceRequest(const Message * message, const ProviderName providerName);
    Message * handleModifyInstanceRequest(const Message * message, const ProviderName providerName);
    Message * handleDeleteInstanceRequest(const Message * message, const ProviderName providerName);

    Message * handleExecQueryRequest(const Message * message, const ProviderName providerName);

    Message * handleAssociatorsRequest(const Message * message, const ProviderName providerName);
    Message * handleAssociatorNamesRequest(const Message * message, const ProviderName providerName);
    Message * handleReferencesRequest(const Message * message, const ProviderName providerName);
    Message * handleReferenceNamesRequest(const Message * message, const ProviderName providerName);

    Message * handleGetPropertyRequest(const Message * message, const ProviderName providerName);
    Message * handleSetPropertyRequest(const Message * message, const ProviderName providerName);

    Message * handleInvokeMethodRequest(const Message * message, const ProviderName providerName);

    Message * handleCreateSubscriptionRequest(const Message * message, const ProviderName providerName);
    Message * handleModifySubscriptionRequest(const Message * message, const ProviderName providerName);
    Message * handleDeleteSubscriptionRequest(const Message * message, const ProviderName providerName);
    Message * handleEnableIndicationsRequest(const Message * message, const ProviderName providerName);
    Message * handleDisableIndicationsRequest(const Message * message, const ProviderName providerName);

    Message * handleConsumeIndicationRequest(const Message * message, const ProviderName providerName);
    Message * handleExportIndicationRequest(const Message * message, const ProviderName providerName);

    Message * handleDisableModuleRequest(const Message * message, const ProviderName providerName);
    Message * handleEnableModuleRequest(const Message * message, const ProviderName providerName);
    Message * handleStopAllProvidersRequest(const Message * message, const ProviderName providerName);

    void _insertEntry(const Provider & provider, const EnableIndicationsResponseHandler *handler);
    EnableIndicationsResponseHandler * _removeEntry(const String & key);

    String _generateKey(const Provider & provider);
    String _generateKey(const String & providerName,const String & providerFileName);

    ProviderName _resolveProviderName(const ProviderName & providerName);
    ProviderName _resolveProviderName(String & destinationPath);

    void unload_idle_providers();

protected:
    IndicationResponseTable _responseTable;

};

PEGASUS_NAMESPACE_END

#endif
