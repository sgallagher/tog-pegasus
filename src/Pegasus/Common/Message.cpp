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

#include "Message.h"
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusAssert.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

Uint32 MessageMask::ha_request =             0x00100000;
Uint32 MessageMask::ha_reply =               0x00200000;
Uint32 MessageMask::ha_async =               0x00400000;

Message::~Message()
{
}

#ifdef PEGASUS_DEBUG
void Message::print(ostream& os, Boolean printHeader) const
{
    if (printHeader)
    {
        os << "Message\n";
        os << "{";
    }

    os << "    messageType: " << MessageTypeToString(_type) << endl;

    if (printHeader)
    {
        os << "}";
    }
}
#endif

static const char* _MESSAGE_TYPE_STRINGS[] =
{
    "HEARTBEAT/REPLY",

    "CIM_GET_CLASS_REQUEST_MESSAGE",
    "CIM_GET_INSTANCE_REQUEST_MESSAGE",
    "CIM_EXPORT_INDICATION_REQUEST_MESSAGE",
    "CIM_DELETE_CLASS_REQUEST_MESSAGE",
    "CIM_DELETE_INSTANCE_REQUEST_MESSAGE",
    "CIM_CREATE_CLASS_REQUEST_MESSAGE",
    "CIM_CREATE_INSTANCE_REQUEST_MESSAGE",
    "CIM_MODIFY_CLASS_REQUEST_MESSAGE",
    "CIM_MODIFY_INSTANCE_REQUEST_MESSAGE",
    "CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE",  // 10
    "CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE",
    "CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE",
    "CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE",
    "CIM_EXEC_QUERY_REQUEST_MESSAGE",
    "CIM_ASSOCIATORS_REQUEST_MESSAGE",
    "CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE",
    "CIM_REFERENCES_REQUEST_MESSAGE",
    "CIM_REFERENCE_NAMES_REQUEST_MESSAGE",
    "CIM_GET_PROPERTY_REQUEST_MESSAGE",
    "CIM_SET_PROPERTY_REQUEST_MESSAGE",  // 20
    "CIM_GET_QUALIFIER_REQUEST_MESSAGE",
    "CIM_SET_QUALIFIER_REQUEST_MESSAGE",
    "CIM_DELETE_QUALIFIER_REQUEST_MESSAGE",
    "CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE",
    "CIM_INVOKE_METHOD_REQUEST_MESSAGE",
    "CIM_PROCESS_INDICATION_REQUEST_MESSAGE",
    "CIM_HANDLE_INDICATION_REQUEST_MESSAGE",
    "CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE",
    "CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE",
    "CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE",  // 30
    "CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_DISABLE_MODULE_REQUEST_MESSAGE",
    "CIM_ENABLE_MODULE_REQUEST_MESSAGE",
    "CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE",
    "CIM_GET_CLASS_RESPONSE_MESSAGE",
    "CIM_GET_INSTANCE_RESPONSE_MESSAGE",
    "CIM_EXPORT_INDICATION_RESPONSE_MESSAGE",
    "CIM_DELETE_CLASS_RESPONSE_MESSAGE",
    "CIM_DELETE_INSTANCE_RESPONSE_MESSAGE",  // 40
    "CIM_CREATE_CLASS_RESPONSE_MESSAGE",
    "CIM_CREATE_INSTANCE_RESPONSE_MESSAGE",
    "CIM_MODIFY_CLASS_RESPONSE_MESSAGE",
    "CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE",
    "CIM_EXEC_QUERY_RESPONSE_MESSAGE",
    "CIM_ASSOCIATORS_RESPONSE_MESSAGE",  // 50
    "CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE",
    "CIM_REFERENCES_RESPONSE_MESSAGE",
    "CIM_REFERENCE_NAMES_RESPONSE_MESSAGE",
    "CIM_GET_PROPERTY_RESPONSE_MESSAGE",
    "CIM_SET_PROPERTY_RESPONSE_MESSAGE",
    "CIM_GET_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_SET_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE",
    "CIM_INVOKE_METHOD_RESPONSE_MESSAGE",  // 60
    "CIM_PROCESS_INDICATION_RESPONSE_MESSAGE",
    "CIM_NOTIFY_PROVIDER_REGISTRATION_RESPONSE_MESSAGE",
    "CIM_NOTIFY_PROVIDER_TERMINATION_RESPONSE_MESSAGE",
    "CIM_HANDLE_INDICATION_RESPONSE_MESSAGE",
    "CIM_CREATE_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_MODIFY_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_DELETE_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_DISABLE_MODULE_RESPONSE_MESSAGE",
    "CIM_ENABLE_MODULE_RESPONSE_MESSAGE",
    "CIM_STOP_ALL_PROVIDERS_RESPONSE_MESSAGE",  // 70
    "SOCKET_MESSAGE",
    "CLOSE_CONNECTION_MESSAGE",
    "HTTP_MESSAGE",
    "HTTP_ERROR_MESSAGE",
    "CLIENT_EXCEPTION_MESSAGE",

    "ASYNC::REGISTER_CIM_SERVICE",
    "ASYNC::DEREGISTER_CIM_SERVICE",
    "ASYNC::UPDATE_CIM_SERVICE",
    "ASYNC::IOCTL",
    "ASYNC::CIMSERVICE_START", // 80
    "ASYNC::CIMSERVICE_STOP",
    "ASYNC::CIMSERVICE_PAUSE",
    "ASYNC::CIMSERVICE_RESUME",

    "ASYNC::ASYNC_OP_START",
    "ASYNC::ASYNC_OP_RESULT",
    "ASYNC::ASYNC_LEGACY_OP_START",
    "ASYNC::ASYNC_LEGACY_OP_RESULT",

    "ASYNC::FIND_SERVICE_Q",
    "ASYNC::FIND_SERVICE_Q_RESULT",
    "ASYNC::ENUMERATE_SERVICE",
    "ASYNC::ENUMERATE_SERVICE_RESULT",

    "ASYNC::REGISTERED_MODULE",
    "ASYNC::DEREGISTERED_MODULE",
    "ASYNC::FIND_MODULE_IN_SERVICE",
    "ASYNC::FIND_MODULE_IN_SERVICE_RESPONSE",

    "ASYNC::ASYNC_MODULE_OP_START",
    "ASYNC::ASYNC_MODULE_OP_RESULT",

    "CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE",
    "CIM_NOTIFY_PROVIDER_ENABLE_RESPONSE_MESSAGE",

    "CIM_NOTIFY_PROVIDER_FAIL_REQUEST_MESSAGE", //  100
    "CIM_NOTIFY_PROVIDER_FAIL_RESPONSE_MESSAGE",

    "CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE",
    "CIM_INITIALIZE_PROVIDER_RESPONSE_MESSAGE",

    "CIM_INITIALIZE_PROVIDER_AGENT_REQUEST_MESSAGE",
    "CIM_INITIALIZE_PROVIDER_AGENT_RESPONSE_MESSAGE",

    "CIM_NOTIFY_CONFIG_CHANGE_REQUEST_MESSAGE",
    "CIM_NOTIFY_CONFIG_CHANGE_RESPONSE_MESSAGE",

    "CIM_SUBSCRIPTION_INIT_COMPLETE_REQUEST_MESSAGE",
    "CIM_SUBSCRIPTION_INIT_COMPLETE_RESPONSE_MESSAGE"
};

const char* MessageTypeToString(MessageType messageType)
{
    if (messageType < NUMBER_OF_MESSAGES)
    {
        return _MESSAGE_TYPE_STRINGS[messageType];
    }

    PEG_TRACE((TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL2,
        "MessageTypeToString: Unknown message type 0x%04X", messageType));
    return "UNKNOWN";
}


CIMOperationType Message::convertMessageTypetoCIMOpType(MessageType type)
{
    CIMOperationType enum_type = CIMOPTYPE_GET_CLASS;
    switch (type)
    {
        case CIM_GET_CLASS_REQUEST_MESSAGE:
        case CIM_GET_CLASS_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_GET_CLASS;
            break;

        case CIM_GET_INSTANCE_REQUEST_MESSAGE:
        case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_GET_INSTANCE;
             break;

        case CIM_DELETE_CLASS_REQUEST_MESSAGE:
        case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_DELETE_CLASS;
             break;

        case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
        case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_DELETE_INSTANCE;
             break;

        case CIM_CREATE_CLASS_REQUEST_MESSAGE:
        case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_CREATE_CLASS;
             break;

        case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
        case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_CREATE_INSTANCE;
             break;

        case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
        case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_MODIFY_CLASS;
             break;

        case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
        case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_MODIFY_INSTANCE;
             break;

        case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
        case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_ENUMERATE_CLASSES;
             break;

        case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
        case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_ENUMERATE_CLASS_NAMES;
             break;

        case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
        case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_ENUMERATE_INSTANCES;
             break;

        case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
        case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_ENUMERATE_INSTANCE_NAMES;
             break;

        case CIM_EXEC_QUERY_REQUEST_MESSAGE:
        case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_EXEC_QUERY;
             break;

        case CIM_ASSOCIATORS_REQUEST_MESSAGE:
        case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_ASSOCIATORS;
             break;

        case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
        case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_ASSOCIATORS;
             break;

        case CIM_REFERENCES_REQUEST_MESSAGE:
        case CIM_REFERENCES_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_REFERENCES;
             break;

        case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
        case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_REFERENCE_NAMES;
             break;

        case CIM_GET_PROPERTY_REQUEST_MESSAGE:
        case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_GET_PROPERTY;
             break;

        case CIM_SET_PROPERTY_REQUEST_MESSAGE:
        case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_SET_PROPERTY;
             break;

        case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
        case CIM_GET_QUALIFIER_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_GET_QUALIFIER;
             break;

        case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
        case CIM_SET_QUALIFIER_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_SET_QUALIFIER;
             break;

        case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
        case CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_DELETE_QUALIFIER;
             break;

        case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
        case CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_ENUMERATE_QUALIFIERS;
             break;

        case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
        case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
             enum_type = CIMOPTYPE_INVOKE_METHOD;
             break;

        default:
             // exicution should never get to this point
             PEGASUS_ASSERT(false);
    }
    return enum_type;
}


////////////////////////////////////////////////////////////////////////////////
//
// QueueIdStack
//
////////////////////////////////////////////////////////////////////////////////

QueueIdStack::QueueIdStack(const QueueIdStack& x) : _size(x._size)
{
    memcpy(_items, x._items, sizeof(_items));
}

QueueIdStack::QueueIdStack(Uint32 x) : _size(0)
{
    push(x);
}

QueueIdStack::QueueIdStack(Uint32 x1, Uint32 x2) : _size(0)
{
    push(x1);
    push(x2);
}

QueueIdStack& QueueIdStack::operator=(const QueueIdStack& x)
{
    if (this != &x)
    {
        memcpy(_items, x._items, sizeof(_items));
        _size = x._size;
    }
    return *this;
}

QueueIdStack QueueIdStack::copyAndPop() const
{
    return QueueIdStack(*this, 0);
}

QueueIdStack::QueueIdStack(const QueueIdStack& x, int) : _size(x._size)
{
    memcpy(_items, x._items, sizeof(_items));
    pop();
}

PEGASUS_NAMESPACE_END
