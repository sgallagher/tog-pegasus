//%/////////////////////////////////////////////////////////////////////////////
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

#ifndef Pegasus_ProviderManagerService_h
#define Pegasus_ProviderManagerService_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/MessageQueueService.h>

#include <Pegasus/ProviderManager2/ProviderManagerModule.h>
#include <Pegasus/ProviderManager2/ProviderManager.h>
#include <Pegasus/ProviderManager2/SafeQueue.h>

#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_SERVER_LINKAGE ProviderManagerService : public MessageQueueService
{
public:
    ProviderManagerService(void);
    // temp
    ProviderManagerService(ProviderRegistrationManager * providerRegistrationManager);

    virtual ~ProviderManagerService(void);

    // temp
    void unload_idle_providers(void);

protected:
    virtual Boolean messageOK(const Message * message);
    virtual void handleEnqueue(void);
    virtual void handleEnqueue(Message * message);

    virtual void _handle_async_request(AsyncRequest * request);

private:
    //static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleServiceOperation(void * arg) throw();

    //void handleStartService() thorw();
    //void handleStopService() thorw();
    //void handlePauseService() thorw();
    //void handleResumeService() thorw();

    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleCimOperation(void * arg) throw();

    void handleCimRequest(AsyncOpNode *op, const Message * message) throw();

private:
    SafeQueue<AsyncOpNode *> _incomingQueue;
    SafeQueue<AsyncOpNode *> _outgoingQueue;

private:
    Array<Pair<ProviderManager *, ProviderManagerModule> > _providerManagers;

};

PEGASUS_NAMESPACE_END

#endif
