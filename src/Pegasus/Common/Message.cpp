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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company 
//                  (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Arthur Pichlkostner (via Markus: sedgewick_de@yahoo.de)
//				Willis White (whiwill@us.ibm.com) PEP 127 and 128
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

    // << Tue Jul  1 11:10:35 2003 mdd >> pep_88 
    os << "    routing code: " << _routing_code << endl;

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
    "CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_PROCESS_INDICATION_REQUEST_MESSAGE",
    "CIM_HANDLE_INDICATION_REQUEST_MESSAGE",  // 30
    "CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE",
    "CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE",
    "CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE",
    "CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE",
    "CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_DISABLE_MODULE_REQUEST_MESSAGE",
    "CIM_ENABLE_MODULE_REQUEST_MESSAGE",
    "CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE",  // 40
    "CIM_GET_CLASS_RESPONSE_MESSAGE",
    "CIM_GET_INSTANCE_RESPONSE_MESSAGE",
    "CIM_EXPORT_INDICATION_RESPONSE_MESSAGE",
    "CIM_DELETE_CLASS_RESPONSE_MESSAGE",
    "CIM_DELETE_INSTANCE_RESPONSE_MESSAGE",
    "CIM_CREATE_CLASS_RESPONSE_MESSAGE",
    "CIM_CREATE_INSTANCE_RESPONSE_MESSAGE",
    "CIM_MODIFY_CLASS_RESPONSE_MESSAGE",
    "CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE",  // 50
    "CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE",
    "CIM_EXEC_QUERY_RESPONSE_MESSAGE",
    "CIM_ASSOCIATORS_RESPONSE_MESSAGE",
    "CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE",
    "CIM_REFERENCES_RESPONSE_MESSAGE",
    "CIM_REFERENCE_NAMES_RESPONSE_MESSAGE",
    "CIM_GET_PROPERTY_RESPONSE_MESSAGE",
    "CIM_SET_PROPERTY_RESPONSE_MESSAGE",  //60
    "CIM_GET_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_SET_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE",
    "CIM_INVOKE_METHOD_RESPONSE_MESSAGE",
    "CIM_ENABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_MODIFY_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_DISABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_PROCESS_INDICATION_RESPONSE_MESSAGE",
    "CIM_NOTIFY_PROVIDER_REGISTRATION_RESPONSE_MESSAGE",  // 70
    "CIM_NOTIFY_PROVIDER_TERMINATION_RESPONSE_MESSAGE",
    "CIM_HANDLE_INDICATION_RESPONSE_MESSAGE",
    "CIM_ENABLE_INDICATIONS_RESPONSE_MESSAGE",
    "CIM_DISABLE_INDICATIONS_RESPONSE_MESSAGE",
    "CIM_CREATE_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_MODIFY_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_DELETE_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_DISABLE_MODULE_RESPONSE_MESSAGE",
    "CIM_ENABLE_MODULE_RESPONSE_MESSAGE",
    "CIM_STOP_ALL_PROVIDERS_RESPONSE_MESSAGE",  // 80
    "SOCKET_MESSAGE",
    "CLOSE_CONNECTION_MESSAGE",
    "HTTP_MESSAGE",
    "HTTP_ERROR_MESSAGE",
    "CLIENT_EXCEPTION_MESSAGE",
        
    "ASYNC::REGISTER_CIM_SERVICE", //            async_message::0x00000001;
    "ASYNC::DEREGISTER_CIM_SERVICE", //          async_message::0x00000002;
    "ASYNC::UPDATE_CIM_SERVICE", //              async_message::0x00000003;
    "ASYNC::IOCTL", //                           async_message::0x00000004;
    "ASYNC::CIMSERVICE_START", //  90            async_message::0x00000005;
    "ASYNC::CIMSERVICE_STOP", //                 async_message::0x00000006;
    "ASYNC::CIMSERVICE_PAUSE", //                async_message::0x00000007;
    "ASYNC::CIMSERVICE_RESUME", //               async_message::0x00000008;

    "ASYNC::ASYNC_OP_START", //                  async_message::0x00000009;
    "ASYNC::ASYNC_OP_RESULT", //                 async_message::0x0000000a;
    "ASYNC::ASYNC_LEGACY_OP_START", //           async_message::0x0000000b;
    "ASYNC::ASYNC_LEGACY_OP_RESULT", //          async_message::0x0000000c;
    
    "ASYNC::FIND_SERVICE_Q", //                  async_message::0x0000000d;
    "ASYNC::FIND_SERVICE_Q_RESULT", //           async_message::0x0000000e;
    "ASYNC::ENUMERATE_SERVICE", //  100          async_message::0x0000000f;
    "ASYNC::ENUMERATE_SERVICE_RESULT", //        async_message::0x00000010;
    
    "ASYNC::REGISTERED_MODULE", //               async_message::0x00000011;
    "ASYNC::DEREGISTERED_MODULE", //             async_message::0x00000012;
    "ASYNC::FIND_MODULE_IN_SERVICE", //          async_message::0x00000013;
    "ASYNC::FIND_MODULE_IN_SERVICE_RESPONSE", // async_message::0x00000014;
    
    "ASYNC::ASYNC_MODULE_OP_START", //           async_message::0x00000015;
    "ASYNC::ASYNC_MODULE_OP_RESULT", //          async_message::0x00000016;
    
    "CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE",
    "CIM_NOTIFY_PROVIDER_ENABLE_RESPONSE_MESSAGE",

    "CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE",
    "CIM_INITIALIZE_PROVIDER_RESPONSE_MESSAGE",

    "CIM_INITIALIZE_PROVIDER_AGENT_REQUEST_MESSAGE",
    "CIM_INITIALIZE_PROVIDER_AGENT_RESPONSE_MESSAGE",

    "CIM_NOTIFY_CONFIG_CHANGE_REQUEST_MESSAGE",
    "CIM_NOTIFY_CONFIG_CHANGE_RESPONSE_MESSAGE"
};

const char* MessageTypeToString(Uint32 messageType)
{
    if (messageType > DUMMY_MESSAGE && messageType < NUMBER_OF_MESSAGES )
	return _MESSAGE_TYPE_STRINGS[messageType - 1];
    if(messageType == 0 )
       return "HEARTBEAT/REPLY" ;
    return "Unknown message type";
}


/*static CIMOperationType Message::convertMessageTypetoCIMOpType(const Uint32 type)
{
    Uint32 in_type, enum_type;
    CIMOperationType cT;


    in_type = type%40;      /* groups request/response message by type ie. getClassRequestMessage 
                                (type = 1) gives the same result as getClassResponseMessage (type = 41)

    if (in_type < 3) {
        enum_type = in_type;
    }
    else if((3 < in_type) && (in_type < 25)){
        enum_type = in_type -1;
    }
    else if (in_type == 25) {
        enum_type = 0;
    }
    else{
        return 41; //error condition
    }   

    cT = (CIMOperationType)enum_type;
    return cT;

}*/


#ifndef PEGASUS_DISABLE_PERFINST
void Message::startServer()
{
    _timeServerStart = CIMDateTime::getCurrentDateTime();
}

void Message::endServer()
{
	
	_timeServerEnd = CIMDateTime::getCurrentDateTime();

    
    Uint16 statType = (Uint16)((_type >= CIM_GET_CLASS_RESPONSE_MESSAGE) ?
       _type-CIM_GET_CLASS_RESPONSE_MESSAGE:_type-1);

	Sint64 _provTi, _totTi, _servTi;

	/*Programs that call STAT_SERVEREND and STAT_SERVEREND_ERROR which in trun call 
	endServer()] are not expecting to get any exceptions. So exceptions are commented out*/
	try
    {
	_provTi = CIMDateTime::getDifference(_timeProviderStart, _timeProviderEnd);
	}
	//If there is an error in the provider no time is added to the count
	catch (InvalidDateTimeFormatException e)
	{_provTi = 0;
	//throw e;
	}
	catch (DateTimeOutOfRangeException e)
	{_provTi = 0;
	//throw e;
	}


	try
	{
	_totTi =  CIMDateTime::getDifference(_timeServerStart, _timeServerEnd);
	}
	// if there is an error in the CIMOM no time is added to the count
	catch (InvalidDateTimeFormatException e) 
	{_totTi = 0;
	//throw e;
	}
	catch (DateTimeOutOfRangeException e) 
	{_totTi =0;
	//throw e;
	}		

	totServerTime = _totTi;
		
	// totoal time subtract the provider time equall the server time

     _servTi =  _totTi-_provTi;

    StatisticalData::current()->addToValue(_servTi,
        statType, StatisticalData::SERVER );

    StatisticalData::current()->addToValue(_provTi,
        statType, StatisticalData::PROVIDER );

}

void Message::startProvider()
{
    _timeProviderStart = CIMDateTime::getCurrentDateTime();
}

void Message::endProvider()
{
    _timeProviderEnd = CIMDateTime::getCurrentDateTime();
}
#endif

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
