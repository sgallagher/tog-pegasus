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
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Stack.h>

PEGASUS_NAMESPACE_BEGIN

class ProviderManager;

class PEGASUS_SERVER_LINKAGE SafeMessageQueue
{
public:
	SafeMessageQueue(void);
	virtual ~SafeMessageQueue(void);

	const Message * front(void);
		
	void enqueue(Message * message);
	Message * dequeue(void);
	
	void lock(void);
	void unlock(void);

protected:
	Mutex _mutex;
	Stack<Message *> _stack;

};

class PEGASUS_SERVER_LINKAGE ProviderManagerService : public MessageQueueService
{
public:
	ProviderManagerService(void);
	virtual ~ProviderManagerService(void);

	// short term hack
	ProviderManager * getProviderManager(void);

protected:
	virtual Boolean messageOK(const Message * message);
	virtual void handleEnqueue(void);
	virtual void handleEnqueue(Message * message);
	
	virtual void _handle_async_request(AsyncRequest * request);

protected:
	Pair<String, String> _lookupProviderForClass(const CIMObjectPath & objectPath);

protected:	
	void handleOperation(void);

	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleGetInstanceRequest(void *) throw();
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleEnumerateInstancesRequest(void *) throw();
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleEnumerateInstanceNamesRequest(void *) throw();
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleCreateInstanceRequest(void *) throw();
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleModifyInstanceRequest(void *) throw();
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleDeleteInstanceRequest(void *) throw();
	
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleExecuteQueryRequest(void *) throw();
	
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleAssociatorsRequest(void *) throw();
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleAssociatorNamesRequest(void *) throw();
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleReferencesRequest(void *) throw();
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleReferenceNamesRequest(void *) throw();
	
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleGetPropertyRequest(void *) throw();
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleSetPropertyRequest(void *) throw();
	
	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleInvokeMethodRequest(void *) throw();

  	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleEnableIndicationRequest(void *) throw();
  	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleModifyIndicationRequest(void *) throw();
  	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleDisableIndicationRequest(void *) throw();

protected:
	ThreadPool _threadPool;
	Semaphore _threadSemaphore;
	
	SafeMessageQueue messageQueue;

};

PEGASUS_NAMESPACE_END

#endif
