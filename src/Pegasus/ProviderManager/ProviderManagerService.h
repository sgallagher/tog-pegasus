//%//-*-c++-*-//////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderManagerService_h
#define Pegasus_ProviderManagerService_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Thread.h>

#include <Pegasus/ProviderManager/SafeQueue.h>

PEGASUS_NAMESPACE_BEGIN

class ProviderManager;
class ProviderRegistrationManager;

class PEGASUS_SERVER_LINKAGE ProviderManagerService : public MessageQueueService
{
public:
    ProviderManagerService(ProviderRegistrationManager * providerRegistrationManager);
    virtual ~ProviderManagerService(void);

    // short term hack
    ProviderManager * getProviderManager(void);

protected:
    virtual Boolean messageOK(const Message * message);
    virtual void handleEnqueue(void);
    virtual void handleEnqueue(Message * message);

    virtual void _handle_async_request(AsyncRequest * request);

protected:
    virtual Pair<String, String> _lookupProviderForClass(const CIMObjectPath & objectPath);

    virtual void _lookupProviderForAssocClass(
        const CIMObjectPath & objectPath, const String& assocClassName,
        const String& resultClassName,
        Array<String>& Locations, Array<String>& providerNames);

protected:
    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleServiceOperation(void * arg) throw();

    //void handleStartService();
    //void handleStopService();
    //void handlePauseService();
    //void handleResumeService();

    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleCimOperation(void * arg) throw();

    //void handleGetClassRequest(const Message * message) throw();
    //void handleEnumerateClassesRequest(const Message * message) throw();
    //void handleEnumerateClassNamesRequest(const Message * message) throw();
    //void handleCreateClassRequest(const Message * message) throw();
    //void handleModifyClassRequest(const Message * message) throw();
    //void handleDeleteClassRequest(const Message * message) throw();

    void handleGetInstanceRequest(const Message * message) throw();
    void handleEnumerateInstancesRequest(const Message * message) throw();
    void handleEnumerateInstanceNamesRequest(const Message * message) throw();
    void handleCreateInstanceRequest(const Message * message) throw();
    void handleModifyInstanceRequest(const Message * message) throw();
    void handleDeleteInstanceRequest(const Message * message) throw();

    void handleExecuteQueryRequest(const Message * message) throw();

    void handleAssociatorsRequest(const Message * message) throw();
    void handleAssociatorNamesRequest(const Message * message) throw();
    void handleReferencesRequest(const Message * message) throw();
    void handleReferenceNamesRequest(const Message * message) throw();

    void handleGetPropertyRequest(const Message * message) throw();
    void handleSetPropertyRequest(const Message * message) throw();

    void handleInvokeMethodRequest(const Message * message) throw();

    void handleCreateSubscriptionRequest(const Message * message) throw();
    void handleModifySubscriptionRequest(const Message * message) throw();
    void handleDeleteSubscriptionRequest(const Message * message) throw();
    void handleEnableIndicationsRequest(const Message * message) throw();
    void handleDisableIndicationsRequest(const Message * message) throw();

protected:
    ThreadPool _threadPool;

    SafeQueue<Message *> _incomingQueue;
    //SafeQueue<Message *> _outgoingQueue;

    ProviderRegistrationManager * _providerRegistrationManager;

};

PEGASUS_NAMESPACE_END

#endif
