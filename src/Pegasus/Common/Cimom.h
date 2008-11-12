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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Cimom_h
#define Pegasus_Cimom_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/AsyncQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/CimomMessage.h>
#include <Pegasus/Common/MessageQueueService.h>
//#include <Pegasus/Server/CIMOperationRequestDispatcher.h>
//#include <Pegasus/Server/CIMOperationResponseEncoder.h>
//#include <Pegasus/Server/CIMOperationRequestDecoder.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/List.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/Time.h>

PEGASUS_NAMESPACE_BEGIN

extern const Uint32 CIMOM_Q_ID;

class MessageQueueService;

class PEGASUS_COMMON_LINKAGE cimom : public MessageQueue
{
public:
    cimom();
    virtual ~cimom();

    AsyncOpNode* get_cached_op();
    void cache_op(AsyncOpNode* op);
protected:
      static void _make_response(Message* req, Uint32 code);
      static void _completeAsyncResponse(
          AsyncRequest* request,
          AsyncReply* reply);
      static void _complete_op_node(AsyncOpNode* op);
      static void _default_callback(AsyncOpNode*, MessageQueue*, void*);

private:
    AsyncQueue<AsyncOpNode> _routed_ops;

    static ThreadReturnType PEGASUS_THREAD_CDECL _routing_proc(void*);

    Thread _routing_thread;

    void _handle_cimom_op(AsyncOpNode* op);

    virtual void handleEnqueue();

    Boolean route_async(AsyncOpNode* operation);

    AtomicInt _die;
    AtomicInt _routed_queue_shutdown;

    static cimom *_global_this;

    friend class MessageQueueService;
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_Cimom_h
