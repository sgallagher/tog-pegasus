//%////-*-c++-*-////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 - 2003 BMC Software, Hewlett-Packard Company, IBM,
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
//              Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//              Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
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

#include <Pegasus/ProviderManager2/Default/LocalProviderManager.h>
#include <Pegasus/ProviderManager2/Default/OperationResponseHandler.h>
#include <Pegasus/ProviderManager2/Default/Provider.h>

#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

typedef HashTable<String,
	EnableIndicationsResponseHandler *,
	EqualFunc<String>,
	HashFunc<String> > IndicationResponseTable;

class PEGASUS_SERVER_LINKAGE DefaultProviderManager : public ProviderManager
{
public:
    DefaultProviderManager(void);
    virtual ~DefaultProviderManager(void);

    virtual Message * processMessage(Message * request) throw();

protected:
    Message * handleUnsupportedRequest(const Message * message) throw();

    Message * handleGetInstanceRequest(const Message * message) throw();
    Message * handleEnumerateInstancesRequest(const Message * message) throw();
    Message * handleEnumerateInstanceNamesRequest(const Message * message) throw();
    Message * handleCreateInstanceRequest(const Message * message) throw();
    Message * handleModifyInstanceRequest(const Message * message) throw();
    Message * handleDeleteInstanceRequest(const Message * message) throw();

    Message * handleExecuteQueryRequest(const Message * message) throw();

    Message * handleAssociatorsRequest(const Message * message) throw();
    Message * handleAssociatorNamesRequest(const Message * message) throw();
    Message * handleReferencesRequest(const Message * message) throw();
    Message * handleReferenceNamesRequest(const Message * message) throw();

    Message * handleGetPropertyRequest(const Message * message) throw();
    Message * handleSetPropertyRequest(const Message * message) throw();

    Message * handleInvokeMethodRequest(const Message * message) throw();

    Message * handleCreateSubscriptionRequest(const Message * message) throw();
    Message * handleModifySubscriptionRequest(const Message * message) throw();
    Message * handleDeleteSubscriptionRequest(const Message * message) throw();
    Message * handleEnableIndicationsRequest(const Message * message) throw();
    Message * handleDisableIndicationsRequest(const Message * message) throw();

    Message * handleConsumeIndicationRequest(const Message * message) throw();

    Message * handleDisableModuleRequest(const Message * message) throw();
    Message * handleEnableModuleRequest(const Message * message) throw();
    Message * handleStopAllProvidersRequest(const Message * message) throw();

    /**
        Inserts an entry into the enabled indication providers table.

        @param   provider              the provider instance
        @param   handler               pointer to the indication response handler
    */
    void _insertEntry(const Provider & provider, const EnableIndicationsResponseHandler *handler);

    /**
        Generates a String key from by combining the provider and provider
	    module names.

        @param   provider              the provider instance

        @return  the generated key
     */
    EnableIndicationsResponseHandler * _removeEntry(const String & key);

    String _generateKey(const Provider & provider);

    ProviderName _resolveProviderName(const ProviderName & providerName);

protected:
    /**
        Table holding indication response handlers, one for each provider
    	that has indications enabled.
    */
    IndicationResponseTable _responseTable;

};

PEGASUS_NAMESPACE_END

#endif
