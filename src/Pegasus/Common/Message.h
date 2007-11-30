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

#ifndef Pegasus_Message_h
#define Pegasus_Message_h

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <cstring>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/CIMOperationType.h>
#include <Pegasus/Common/Threads.h>
#include <Pegasus/Common/Linkable.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE MessageMask
{
public:
    // Message handling is indicated by the high order 12 bits.  For example:
    // Uint32 messageHandling = flags & 0xfff00000;
    static Uint32 ha_request;
    static Uint32 ha_reply;
    static Uint32 ha_async;
};

enum HttpMethod
{
    HTTP_METHOD__POST,
    HTTP_METHOD_M_POST
};

enum MessageType
{
    DUMMY_MESSAGE,

    // CIM Message types:

    CIM_GET_CLASS_REQUEST_MESSAGE,
    CIM_GET_INSTANCE_REQUEST_MESSAGE,
    CIM_EXPORT_INDICATION_REQUEST_MESSAGE,
    CIM_DELETE_CLASS_REQUEST_MESSAGE,
    CIM_DELETE_INSTANCE_REQUEST_MESSAGE,
    CIM_CREATE_CLASS_REQUEST_MESSAGE,
    CIM_CREATE_INSTANCE_REQUEST_MESSAGE,
    CIM_MODIFY_CLASS_REQUEST_MESSAGE,
    CIM_MODIFY_INSTANCE_REQUEST_MESSAGE,
    CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE, //10
    CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE,
    CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE,
    CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE,
    CIM_EXEC_QUERY_REQUEST_MESSAGE,
    CIM_ASSOCIATORS_REQUEST_MESSAGE,
    CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE,
    CIM_REFERENCES_REQUEST_MESSAGE,
    CIM_REFERENCE_NAMES_REQUEST_MESSAGE,
    CIM_GET_PROPERTY_REQUEST_MESSAGE,
    CIM_SET_PROPERTY_REQUEST_MESSAGE, //20
    CIM_GET_QUALIFIER_REQUEST_MESSAGE,
    CIM_SET_QUALIFIER_REQUEST_MESSAGE,
    CIM_DELETE_QUALIFIER_REQUEST_MESSAGE,
    CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE,
    CIM_INVOKE_METHOD_REQUEST_MESSAGE,
    CIM_PROCESS_INDICATION_REQUEST_MESSAGE,
    CIM_HANDLE_INDICATION_REQUEST_MESSAGE,
    CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE,
    CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE,
    CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE,  // 30
    CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE,
    CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE,
    CIM_DISABLE_MODULE_REQUEST_MESSAGE,
    CIM_ENABLE_MODULE_REQUEST_MESSAGE,
    CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE,

    CIM_GET_CLASS_RESPONSE_MESSAGE,
    CIM_GET_INSTANCE_RESPONSE_MESSAGE,
    CIM_EXPORT_INDICATION_RESPONSE_MESSAGE,
    CIM_DELETE_CLASS_RESPONSE_MESSAGE,
    CIM_DELETE_INSTANCE_RESPONSE_MESSAGE,  // 40
    CIM_CREATE_CLASS_RESPONSE_MESSAGE,
    CIM_CREATE_INSTANCE_RESPONSE_MESSAGE,
    CIM_MODIFY_CLASS_RESPONSE_MESSAGE,
    CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE,
    CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE,
    CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE,
    CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE,
    CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE,
    CIM_EXEC_QUERY_RESPONSE_MESSAGE,
    CIM_ASSOCIATORS_RESPONSE_MESSAGE,  // 50
    CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE,
    CIM_REFERENCES_RESPONSE_MESSAGE,
    CIM_REFERENCE_NAMES_RESPONSE_MESSAGE,
    CIM_GET_PROPERTY_RESPONSE_MESSAGE,
    CIM_SET_PROPERTY_RESPONSE_MESSAGE,
    CIM_GET_QUALIFIER_RESPONSE_MESSAGE,
    CIM_SET_QUALIFIER_RESPONSE_MESSAGE,
    CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE,
    CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE,
    CIM_INVOKE_METHOD_RESPONSE_MESSAGE,  // 60
    CIM_PROCESS_INDICATION_RESPONSE_MESSAGE,
    CIM_NOTIFY_PROVIDER_REGISTRATION_RESPONSE_MESSAGE,
    CIM_NOTIFY_PROVIDER_TERMINATION_RESPONSE_MESSAGE,
    CIM_HANDLE_INDICATION_RESPONSE_MESSAGE,
    CIM_CREATE_SUBSCRIPTION_RESPONSE_MESSAGE,
    CIM_MODIFY_SUBSCRIPTION_RESPONSE_MESSAGE,
    CIM_DELETE_SUBSCRIPTION_RESPONSE_MESSAGE,
    CIM_DISABLE_MODULE_RESPONSE_MESSAGE,
    CIM_ENABLE_MODULE_RESPONSE_MESSAGE,
    CIM_STOP_ALL_PROVIDERS_RESPONSE_MESSAGE,  // 70

    // Monitor-related messages:

    SOCKET_MESSAGE,

    // Connection-oriented messages:

    CLOSE_CONNECTION_MESSAGE,

    // HTTP messages:

    HTTP_MESSAGE,
    HTTP_ERROR_MESSAGE,

    // Exception messages to be passed to a CIM client application:

    CLIENT_EXCEPTION_MESSAGE,

    ASYNC_REGISTER_CIM_SERVICE,
    ASYNC_DEREGISTER_CIM_SERVICE,
    ASYNC_UPDATE_CIM_SERVICE,
    ASYNC_IOCTL,
    ASYNC_CIMSERVICE_START,  // 80
    ASYNC_CIMSERVICE_STOP,
    ASYNC_CIMSERVICE_PAUSE,
    ASYNC_CIMSERVICE_RESUME,

    ASYNC_ASYNC_OP_START,
    ASYNC_ASYNC_OP_RESULT,
    ASYNC_ASYNC_LEGACY_OP_START,
    ASYNC_ASYNC_LEGACY_OP_RESULT,

    ASYNC_FIND_SERVICE_Q,
    ASYNC_FIND_SERVICE_Q_RESULT,
    ASYNC_ENUMERATE_SERVICE,  // 90
    ASYNC_ENUMERATE_SERVICE_RESULT,

    ASYNC_REGISTERED_MODULE,
    ASYNC_DEREGISTERED_MODULE,
    ASYNC_FIND_MODULE_IN_SERVICE,
    ASYNC_FIND_MODULE_IN_SERVICE_RESPONSE,

    ASYNC_ASYNC_MODULE_OP_START,
    ASYNC_ASYNC_MODULE_OP_RESULT,

    CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE,
    CIM_NOTIFY_PROVIDER_ENABLE_RESPONSE_MESSAGE,

    CIM_NOTIFY_PROVIDER_FAIL_REQUEST_MESSAGE,  // 100
    CIM_NOTIFY_PROVIDER_FAIL_RESPONSE_MESSAGE,

    CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE,
    CIM_INITIALIZE_PROVIDER_RESPONSE_MESSAGE,

    CIM_INITIALIZE_PROVIDER_AGENT_REQUEST_MESSAGE,
    CIM_INITIALIZE_PROVIDER_AGENT_RESPONSE_MESSAGE,

    CIM_NOTIFY_CONFIG_CHANGE_REQUEST_MESSAGE,
    CIM_NOTIFY_CONFIG_CHANGE_RESPONSE_MESSAGE,

    CIM_SUBSCRIPTION_INIT_COMPLETE_REQUEST_MESSAGE,
    CIM_SUBSCRIPTION_INIT_COMPLETE_RESPONSE_MESSAGE,

    NUMBER_OF_MESSAGES
};

PEGASUS_COMMON_LINKAGE const char* MessageTypeToString(MessageType messageType);


/** The Message class and derived classes are used to pass messages between
    modules. Messages are passed between modules using the message queues
    (see MessageQueue class). Derived classes may add their own fields.
    This base class defines a common type field, which is the type of
    the message.
*/
class PEGASUS_COMMON_LINKAGE Message : public Linkable
{
public:

    Message(
        MessageType type,
        Uint32 destination = 0,
        Uint32 mask = 0)
        :
        _type(type),
        _mask(mask),
        _httpMethod (HTTP_METHOD__POST),
        _close_connect(false),
        _last_thread_id(Threads::self()),
        _async(0),
        dest(destination),
        _isComplete(true),
        _index(0)
    {
    }

    virtual ~Message();

    // NOTE: The compiler default implementation of the copy constructor
    // is used for this class.

    Boolean getCloseConnect() const { return _close_connect; }
    void setCloseConnect(Boolean close_connect)
    {
        _close_connect = close_connect;
    }

    MessageType getType() const { return _type; }

    void setType(MessageType type) { _type = type; }

    Uint32 getMask() const { return _mask; }

    void setMask(Uint32 mask) { _mask = mask; }

    HttpMethod getHttpMethod() const { return _httpMethod; }

    void setHttpMethod(HttpMethod httpMethod) {_httpMethod = httpMethod;}

    static CIMOperationType convertMessageTypetoCIMOpType(MessageType type);

#ifdef PEGASUS_DEBUG
    virtual void print(
        PEGASUS_STD(ostream)& os,
        Boolean printHeader = true) const;
#endif

    Message* get_async()
    {
        Message *ret = _async;
        _async = 0;
        return ret;
    }

    void put_async(Message* msg)
    {
        _async = msg;
    }

    // << Tue Jul  1 11:02:49 2003 mdd >> pep_88 and helper for i18n and l10n
    Boolean thread_changed()
    {
        if (!Threads::equal(_last_thread_id, Threads::self()))
        {
            _last_thread_id = Threads::self();
            return true;
        }

        return false;
    }

    // set the message index indicating what piece (or sequence) this is
    // message indexes start at zero
    void setIndex(Uint32 index) { _index = index; }

    // increment the message index
    void incrementIndex() { _index++; }

    // set the complete flag indicating if this message piece is the
    // last or not
    void setComplete(Boolean isComplete)
    {
        _isComplete = isComplete ? true:false;
    }

    // get the message index (or sequence number)
    Uint32 getIndex() const { return _index; }

    // is this the first piece of the message ?
    Boolean isFirst() const { return _index == 0 ? true : false; }

    // is this message complete? (i.e the last in a one or more sequence)
    Boolean isComplete() const { return _isComplete; }

private:
    MessageType _type;
    Uint32 _mask;
    HttpMethod _httpMethod;

    Boolean _close_connect;

    // << Tue Jul  1 11:02:35 2003 mdd >> pep_88 and helper for i18n and l10n
    ThreadType _last_thread_id;

public:
    Message *_async;
    Uint32 dest;

private:
    Message& operator=(const Message& msg);

    Boolean _isComplete;
    Uint32 _index;
};


/** This class implements a stack of queue-ids. Many messages must keep a
    stack of queue-ids of queues which they must be returned to. This provides
    a light efficient stack for this purpose.
*/
class PEGASUS_COMMON_LINKAGE QueueIdStack
{
public:

    QueueIdStack() : _size(0)
    {
    }

    QueueIdStack(const QueueIdStack& x);

    explicit QueueIdStack(Uint32 x);

    explicit QueueIdStack(Uint32 x1, Uint32 x2);

    ~QueueIdStack()
    {
    }

    QueueIdStack& operator=(const QueueIdStack& x);

    Uint32 size() const
    {
        return _size;
    }

    Boolean isEmpty() const
    {
        return _size == 0;
    }

    void push(Uint32 x)
    {
#ifdef PEGASUS_DEBUG
        if (_size == MAX_SIZE)
            throw StackOverflow();
#endif
        _items[_size++] = x;
    }

    Uint32& top()
    {
#ifdef PEGASUS_DEBUG
        if (_size == 0)
            throw StackUnderflow();
#endif
        return _items[_size-1];
    }

    Uint32 top() const
    {
        return ((QueueIdStack*)this)->top();
    }

    void pop()
    {
#ifdef PEGASUS_DEBUG
        if (_size == 0)
            throw StackUnderflow();
#endif
        _size--;
    }

    /** Make a copy of this stack and then pop the top element. */
    QueueIdStack copyAndPop() const;

private:

    // Copy the given stack but then pop the top element:
    QueueIdStack(const QueueIdStack& x, int);

    enum { MAX_SIZE = 5 };
    Uint32 _items[MAX_SIZE];
    Uint32 _size;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Message_h */
