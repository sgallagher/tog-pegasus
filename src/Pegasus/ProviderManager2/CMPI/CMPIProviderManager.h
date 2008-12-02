//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CMPIProviderManager_h
#define Pegasus_CMPIProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/ProviderManager2/ProviderName.h>
#include <Pegasus/ProviderManager2/ProviderManager.h>
#include <Pegasus/Common/OperationContextInternal.h>

#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/ProviderManager2/OperationResponseHandler.h>

#include <Pegasus/ProviderManager2/CMPI/CMPILocalProviderManager.h>
#include <Pegasus/ProviderManager2/CMPI/Linkage.h>
#include <Pegasus/ProviderManager2/CMPI/CMPI_ContextArgs.h>

#include <Pegasus/Provider/CIMOMHandleQueryContext.h>
PEGASUS_NAMESPACE_BEGIN

struct CMPI_SelectExp;

class PEGASUS_CMPIPM_LINKAGE CMPIProviderManager : public ProviderManager
{
public:
    CMPIProviderManager();
    virtual ~CMPIProviderManager();

    virtual Message * processMessage(Message * request);

    virtual Boolean hasActiveProviders();
    virtual void unloadIdleProviders();

    virtual Boolean supportsRemoteNameSpaces()
    {
        return true;
    }

    struct indProvRecord
    {
        indProvRecord() : enabled(false), count(1), handler(NULL)
        {
        }
        Boolean enabled;
        int count;
#ifdef PEGASUS_ENABLE_REMOTE_CMPI
        String remoteInfo; 
#endif
        EnableIndicationsResponseHandler* handler;
    };

    struct indSelectRecord
    {
        indSelectRecord() : eSelx(NULL), count(1)
        {
        }
        CMPI_SelectExp *eSelx;
        CIMOMHandleQueryContext *qContext;
        int count;
    };

    typedef HashTable<String,indProvRecord*, \
        EqualFunc<String>,HashFunc<String> > IndProvTab;
    typedef HashTable<CIMObjectPath,indSelectRecord*, \
        EqualFunc<CIMObjectPath>,HashFunc<CIMObjectPath> > IndSelectTab;

    static IndProvTab provTab;
    static ReadWriteSem  rwSemProvTab;
    static IndSelectTab selxTab;
    static ReadWriteSem  rwSemSelxTab;
protected:
    CMPILocalProviderManager providerManager;

    void _setupCMPIContexts(
        CMPI_ContextOnStack * eCtx,
        OperationContext * context,
        ProviderIdContainer * pidc,
        const String & nameSpace,
        Boolean remote,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        Boolean setFlags = false);

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
//    Message * handleModifySubscriptionRequest(const Message * message);
    Message * handleDeleteSubscriptionRequest(const Message * message);

//  Not supported by CMPI
//    Message * handleExportIndicationRequest(const Message * message);

    Message * handleDisableModuleRequest(const Message * message);
    Message * handleEnableModuleRequest(const Message * message);
    Message * handleStopAllProvidersRequest(const Message * message);
    Message * handleSubscriptionInitCompleteRequest (const Message * message);

    ProviderName _resolveProviderName(const ProviderIdContainer & providerId);

    /**
        Calls the provider's enableIndications() method, if the provider 
        version supports enableIndications().

        Note that since an exception thrown by the provider's
        enableIndications() method is considered a provider error, any such
        exception is ignored, and no exceptions are thrown by this method.

        @param  req_provider  CIMInstance for the provider to be enabled
        @param  _indicationCallback  PEGASUS_INDICATION_CALLBACK_T for
            indications
        @param  ph  OpProviderHolder for the provider to be enabled
        @param remoteInfo Remote Information
     */
    void _callEnableIndications(
        CIMInstance & req_provider,
        PEGASUS_INDICATION_CALLBACK_T _indicationCallback,
        OpProviderHolder & ph,
        const char *remoteInfo);

    /**
        Calls the provider's disableIndications() method, if the provider
        version supports disableIndications().

        @param  ph  OpProviderHolder for the provider to be enabled
        @param remoteInfo Remote Information
     */
    void _callDisableIndications(
        OpProviderHolder & ph, 
        const char *remoteInfo);

    String _getClassNameFromQuery(
        CIMOMHandleQueryContext *context,
        String &query,
        String &lang);
};

PEGASUS_NAMESPACE_END

#endif
    
