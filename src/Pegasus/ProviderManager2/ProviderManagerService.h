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
//              Adrian Schuur, IBM (schuur@de.ibm.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderManagerService_h
#define Pegasus_ProviderManagerService_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContextInternal.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

#include <Pegasus/ProviderManager2/SafeQueue.h>
#include <Pegasus/ProviderManager2/ProviderManagerRouter.h>

#include <Pegasus/ProviderManager2/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

#define IDLE_LIMIT 300

class PEGASUS_PPM_LINKAGE ProviderManagerService : public MessageQueueService
{
public:
    ProviderManagerService(
        ProviderRegistrationManager* providerRegistrationManager,
        CIMRepository* repository);

    virtual ~ProviderManagerService();

    void unloadIdleProviders();

    static void indicationCallback(CIMProcessIndicationRequestMessage* request);
    static void handleCimResponse(
        CIMRequestMessage& request, CIMResponseMessage& response);

private:
    ProviderManagerService();

    virtual Boolean messageOK(const Message* message);
    virtual void handleEnqueue();
    virtual void handleEnqueue(Message* message);

    virtual void _handle_async_request(AsyncRequest* request);

    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleCimOperation(
        void* arg) ;

    void handleCimRequest(AsyncOpNode* op, Message* message);

    Message* _processMessage(CIMRequestMessage* request);

    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
        _unloadIdleProvidersHandler(void* arg) throw();

    void _updateProviderModuleStatus(
        CIMInstance& providerModule,
        Uint16 fromStatus,
        Uint16 toStatus);

    static ProviderManagerService* providerManagerService;

    CIMRepository* _repository;

    SafeQueue<AsyncOpNode *> _incomingQueue;
    SafeQueue<AsyncOpNode *> _outgoingQueue;

    ProviderManagerRouter* _basicProviderManagerRouter;
    ProviderManagerRouter* _oopProviderManagerRouter;

    ProviderRegistrationManager* _providerRegistrationManager;

    static Uint32 _indicationServiceQueueId;

    /**
        Indicates the number of threads currently attempting to unload idle
        providers.  This value is used to prevent multiple threads from
        unloading idle providers at the same time.
     */
    AtomicInt _unloadIdleProvidersBusy;


};

PEGASUS_NAMESPACE_END

#endif
