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
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/Triad.h>
#include <Pegasus/Common/Thread.h>

#include <Pegasus/ProviderManager/SafeQueue.h>

PEGASUS_NAMESPACE_BEGIN

class ProviderRegistrationManager;

class PEGASUS_SERVER_LINKAGE ProviderManagerService : public MessageQueueService
{
public:
    ProviderManagerService(ProviderRegistrationManager * providerRegistrationManager);
    virtual ~ProviderManagerService(void);
    
protected:
    virtual Boolean messageOK(const Message * message);
    virtual void handleEnqueue(void);
    virtual void handleEnqueue(Message * message);

    virtual void _handle_async_request(AsyncRequest * request);

protected:

    virtual Triad<String, String, String>
        _lookupProviderForClass(const CIMObjectPath & objectPath);

    virtual Triad<String, String, String> _lookupMethodProviderForClass(
	const CIMObjectPath & objectPath,
	const String & methodName);

    virtual void _lookupProviderForAssocClass(
        const CIMObjectPath & objectPath, const String& assocClassName,
        const String& resultClassName,
        Array<String>& Locations, Array<String>& providerNames,
        Array<String>& interfaceNames);

protected:
    //static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleServiceOperation(void * arg) throw();

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

    void handleGetInstanceRequest(AsyncOpNode *op, const Message *message) throw();
    void handleEnumerateInstancesRequest(AsyncOpNode *op, const Message *message) throw();
    void handleEnumerateInstanceNamesRequest(AsyncOpNode *op, const Message *message) throw();
    void handleCreateInstanceRequest(AsyncOpNode *op, const Message *message) throw();
    void handleModifyInstanceRequest(AsyncOpNode *op, const Message *message) throw();
    void handleDeleteInstanceRequest(AsyncOpNode *op, const Message *message) throw();

    void handleExecuteQueryRequest(AsyncOpNode *op, const Message *message) throw();

    void handleAssociatorsRequest(AsyncOpNode *op, const Message *message) throw();
    void handleAssociatorNamesRequest(AsyncOpNode *op, const Message *message) throw();
    void handleReferencesRequest(AsyncOpNode *op, const Message *message) throw();
    void handleReferenceNamesRequest(AsyncOpNode *op, const Message *message) throw();

    void handleGetPropertyRequest(AsyncOpNode *op, const Message *message) throw();
    void handleSetPropertyRequest(AsyncOpNode *op, const Message *message) throw();

    void handleInvokeMethodRequest(AsyncOpNode *op, const Message *message) throw();

    void handleCreateSubscriptionRequest(AsyncOpNode *op, const Message *message) throw();
    void handleModifySubscriptionRequest(AsyncOpNode *op, const Message *message) throw();
    void handleDeleteSubscriptionRequest(AsyncOpNode *op, const Message *message) throw();
    void handleEnableIndicationsRequest(AsyncOpNode *op, const Message *message) throw();
    void handleDisableIndicationsRequest(AsyncOpNode *op, const Message *message) throw();

    void handleDisableModuleRequest(AsyncOpNode *op, const Message *message) throw();
    void handleEnableModuleRequest(AsyncOpNode *op, const Message *message) throw();
    void handleStopAllProvidersRequest(AsyncOpNode *op, const Message *message) throw();

protected:

    SafeQueue<AsyncOpNode *> _incomingQueue;
    //SafeQueue<Message *> _outgoingQueue;

    ProviderRegistrationManager * _providerRegistrationManager;

};

PEGASUS_NAMESPACE_END

#endif
