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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company 
//                  (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Arthur Pichlkostner (via Markus: sedgewick_de@yahoo.de)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Message.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

Uint32 message_mask::type_legacy =            0x00000000;
Uint32 message_mask::type_CIMOperation =      0x00000001;
Uint32 message_mask::type_CIMAsyncOperation = 0x00000002;
Uint32 message_mask::type_export =            0x00000004;
Uint32 message_mask::type_lifetime =          0x00000008;
Uint32 message_mask::type_socket =            0x00000010;
Uint32 message_mask::type_connection =        0x00000020;
Uint32 message_mask::type_http =              0x00000040;
Uint32 message_mask::type_http_error =        0x00000080;
Uint32 message_mask::type_cimom =             0x00000100;
Uint32 message_mask::type_control =           0x00000200;
Uint32 message_mask::type_service =           0x00000400;
Uint32 message_mask::type_broadcast =         0x00000800;
Uint32 message_mask::type_client_exception =  0x00001000;

Uint32 message_mask::ha_no_delete =           0x00100000;
Uint32 message_mask::ha_request =             0x00200000;
Uint32 message_mask::ha_reply =               0x00400000;
Uint32 message_mask::ha_synchronous =         0x00800000;
Uint32 message_mask::ha_async =               0x01000000;
Uint32 message_mask::ha_wait =                0x02000000;

Uint32 Message::_nextKey = 0;
Mutex Message::_mut;


Message::~Message() 
{ 
}

void Message::print(ostream& os, Boolean printHeader) const
{
    if (printHeader)
    {
	os << "Message\n";
	os << "{";
    }

    os << "    messageType: " << MessageTypeToString(_type) << endl;
    os << "    key: " << _key << endl;

    if (printHeader)
    {
	os << "}";
    }
}

static const char* _MESSAGE_TYPE_STRINGS[] =
{
    "CIM_GET_CLASS_REQUEST_MESSAGE",
    "CIM_GET_INSTANCE_REQUEST_MESSAGE",
    "CIM_EXPORT_INDICATION_REQUEST_MESSAGE",
    "CIM_DELETE_CLASS_REQUEST_MESSAGE",
    "CIM_DELETE_INSTANCE_REQUEST_MESSAGE",
    "CIM_CREATE_CLASS_REQUEST_MESSAGE",
    "CIM_CREATE_INSTANCE_REQUEST_MESSAGE",
    "CIM_MODIFY_CLASS_REQUEST_MESSAGE",
    "CIM_MODIFY_INSTANCE_REQUEST_MESSAGE",
    "CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE", 
    "CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE",
    "CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE",
    "CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE",
    "CIM_EXEC_QUERY_REQUEST_MESSAGE",
    "CIM_ASSOCIATORS_REQUEST_MESSAGE",
    "CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE",
    "CIM_REFERENCES_REQUEST_MESSAGE",
    "CIM_REFERENCE_NAMES_REQUEST_MESSAGE",
    "CIM_GET_PROPERTY_REQUEST_MESSAGE",
    "CIM_SET_PROPERTY_REQUEST_MESSAGE",
    "CIM_GET_QUALIFIER_REQUEST_MESSAGE",
    "CIM_SET_QUALIFIER_REQUEST_MESSAGE",
    "CIM_DELETE_QUALIFIER_REQUEST_MESSAGE",
    "CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE",
    "CIM_INVOKE_METHOD_REQUEST_MESSAGE",
    "CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_PROCESS_INDICATION_REQUEST_MESSAGE",
    "CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE",
    "CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE",
    "CIM_HANDLE_INDICATION_REQUEST_MESSAGE",
    "CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_GET_CLASS_RESPONSE_MESSAGE",
    "CIM_GET_INSTANCE_RESPONSE_MESSAGE",
    "CIM_EXPORT_INDICATION_RESPONSE_MESSAGE",
    "CIM_DELETE_CLASS_RESPONSE_MESSAGE",
    "CIM_DELETE_INSTANCE_RESPONSE_MESSAGE",
    "CIM_CREATE_CLASS_RESPONSE_MESSAGE",
    "CIM_CREATE_INSTANCE_RESPONSE_MESSAGE",
    "CIM_MODIFY_CLASS_RESPONSE_MESSAGE",
    "CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE",
    "CIM_EXEC_QUERY_RESPONSE_MESSAGE",
    "CIM_ASSOCIATORS_RESPONSE_MESSAGE",
    "CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE",
    "CIM_REFERENCES_RESPONSE_MESSAGE",
    "CIM_REFERENCE_NAMES_RESPONSE_MESSAGE",
    "CIM_GET_PROPERTY_RESPONSE_MESSAGE",
    "CIM_SET_PROPERTY_RESPONSE_MESSAGE",
    "CIM_GET_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_SET_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE",
    "CIM_INVOKE_METHOD_RESPONSE_MESSAGE",
    "CIM_ENABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_MODIFY_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_DISABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_PROCESS_INDICATION_RESPONSE_MESSAGE",
    "CIM_NOTIFY_PROVIDER_REGISTRATION_RESPONSE_MESSAGE",
    "CIM_NOTIFY_PROVIDER_TERMINATION_RESPONSE_MESSAGE",
    "CIM_HANDLE_INDICATION_RESPONSE_MESSAGE",
    "CIM_CREATE_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_MODIFY_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_DELETE_SUBSCRIPTION_RESPONSE_MESSAGE",
    "SOCKET_MESSAGE",
    "CLOSE_CONNECTION_MESSAGE",
    "HTTP_MESSAGE",
    "HTTP_ERROR_MESSAGE",
    "CLIENT_EXCEPTION_MESSAGE",
        
    "ASYNC::REGISTER_CIM_SERVICE", //            async_message::0x00000001;
    "ASYNC::DEREGISTER_CIM_SERVICE", //          async_message::0x00000002;
    "ASYNC::UPDATE_CIM_SERVICE", //              async_message::0x00000003;
    "ASYNC::IOCTL", //                           async_message::0x00000004;
    "ASYNC::CIMSERVICE_START", //                async_message::0x00000005;
    "ASYNC::CIMSERVICE_STOP", //                 async_message::0x00000006;
    "ASYNC::CIMSERVICE_PAUSE", //                async_message::0x00000007;
    "ASYNC::CIMSERVICE_RESUME", //               async_message::0x00000008;

    "ASYNC::ASYNC_OP_START", //                  async_message::0x00000009;
    "ASYNC::ASYNC_OP_RESULT", //                 async_message::0x0000000a;
    "ASYNC::ASYNC_LEGACY_OP_START", //           async_message::0x0000000b;
    "ASYNC::ASYNC_LEGACY_OP_RESULT", //          async_message::0x0000000c;
    
    "ASYNC::FIND_SERVICE_Q", //                  async_message::0x0000000d;
    "ASYNC::FIND_SERVICE_Q_RESULT", //           async_message::0x0000000e;
    "ASYNC::ENUMERATE_SERVICE", //               async_message::0x0000000f;
    "ASYNC::ENUMERATE_SERVICE_RESULT", //        async_message::0x00000010;
    
    "ASYNC::REGISTERED_MODULE", //               async_message::0x00000011;
    "ASYNC::DEREGISTERED_MODULE", //             async_message::0x00000012;
    "ASYNC::FIND_MODULE_IN_SERVICE", //          async_message::0x00000013;
    "ASYNC::FIND_MODULE_IN_SERVICE_RESPONSE", // async_message::0x00000014;
    
    "ASYNC::ASYNC_MODULE_OP_START", //           async_message::0x00000015;
    "ASYNC::ASYNC_MODULE_OP_RESULT " //          async_message::0x00000016;
    
};

const char* MessageTypeToString(Uint32 messageType)
{
    if (messageType > DUMMY_MESSAGE && messageType < NUMBER_OF_MESSAGES )
	return _MESSAGE_TYPE_STRINGS[messageType - 1];
    if(messageType == 0 )
       return "HEARTBEAT/REPLY" ;
    return "Unknown message type";
}

#ifdef PEGASUS_HAS_PERFINST
inline void Message::startServer()
{
    pegasus_gettimeofday(&_timeServerStart);
}

void Message::endServer()
{
    pegasus_gettimeofday(&_timeServerEnd);

    //Uint16 statType = (Uint16)((_type>36)?_type-36:_type-1);
    // CIM_GET_CLASS_RESPONSE_MESSAGE is the first response message
    Uint16 statType = (Uint16)((_type > CIM_GET_CLASS_RESPONSE_MESSAGE) ?
       _type-CIM_GET_CLASS_RESPONSE_MESSAGE:_type-1);

    Uint64 providerTime = (Uint64)
        ((_timeProviderEnd.tv_sec - _timeProviderStart.tv_sec)*1000000 +
        (_timeProviderEnd.tv_usec - _timeProviderStart.tv_usec));

    _totalTime = (Uint64)((_timeServerEnd.tv_sec -
        _timeServerStart.tv_sec)*1000000 +
        (_timeServerEnd.tv_usec - _timeServerStart.tv_usec));

    Uint64 serverTime =  _totalTime - providerTime;

    StatisticalData::current()->addToValue(serverTime ,
        statType, StatisticalData::SERVER );

    StatisticalData::current()->addToValue(providerTime ,
        statType, StatisticalData::PROVIDER );
}

inline void Message::startProvider()
{
    pegasus_gettimeofday(&_timeProviderStart);
}

inline void Message::endProvider()
{
    pegasus_gettimeofday(&_timeProviderEnd);
}
#endif

PEGASUS_NAMESPACE_END
