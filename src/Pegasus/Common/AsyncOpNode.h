//%///////////-*-c++-*-//////////////////////////////////////////////////////
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_AsyncOpNode_h
#define Pegasus_AsyncOpNode_h


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Provider2/CIMBaseProviderHandle.h>
#include <Pegasus/Provider2/CIMIndicationProvider.h>

PEGASUS_NAMESPACE_BEGIN

#define ASYNC_OPFLAGS_UNKNOWN           0x00000000
#define ASYNC_OPFLAGS_DELIVER           0x00000001 
#define ASYNC_OPFLAGS_RESERVE           0x00000002
#define ASYNC_OPFLAGS_PROCESSING        0x00000004
#define ASYNC_OPFLAGS_COMPLETE          0x00000008
#define ASYNC_OPFLAGS_INTERVAL_REPEAT   0x00000010
#define ASYNC_OPFLAGS_INDICATION        0x00000020
#define ASYNC_OPFLAGS_REMOTE            0x00000040
#define ASYNC_OPFLAGS_LOCAL_OUT_OF_PROC 0x00000080
#define ASYNC_OPFLAGS_STARTED           0x00000100


#define ASYNC_OPSTATE_NORMAL            0x00000000
#define ASYNC_OPSTATE_PHASED            0x00000001
#define ASYNC_OPSTATE_PARTIAL           0x00000002
#define ASYNC_OPSTATE_TIMEOUT           0x00000004
#define ASYNC_OPSTATE_SINGLE            0x00000008
#define ASYNC_OPSTATE_MULTIPLE          0x00000010
#define ASYNC_OPSTATE_TOTAL             0x00000020


// class PEGASUS_EXPORT AsyncOpFlags 
// { 
//    public:
//         static const Uint32 UNKNOWN;
//         static const Uint32 DELIVER;
//         static const Uint32 RESERVE;
//         static const Uint32 PROCESSING;
//         static const Uint32 COMPLETE;
//         static const Uint32 INTERVAL_REPEAT;;
//         static const Uint32 INDICATION;
//         static const Uint32 REMOTE;
//         static const Uint32 LOCAL_OUT_OF_PROC;
// };

// class PEGASUS_EXPORT AsyncOpState
// {
//    public:
//         static const Uint32 NORMAL;
//         static const Uint32 PHASED;
//         static const Uint32 PARTIAL;
//         static const Uint32 TIMEOUT;
//         static const Uint32 SINGLE;
//         static const Uint32 MULTIPLE;
//         static const Uint32 TOTAL;
// };

// enum PEGASUS_EXPORT ResponseHandlerType 
// {
//    UNDEFINED,
//    CIM_CLASS,
//    CIM_INSTANCE,
//    CIM_OBJECT,
//    CIM_OBJECT_WITH_PATH,
//    CIM_VALUE,
//    CIM_INDICATION,
//    CIM_REFERENCE, 
//    CIM_LAST

// };


#define RESPONSE_HANDLER_TYPE_UNDEFINED 0
#define RESPONSE_HANDLER_TYPE_CIM_CLASS 1
#define RESPONSE_HANDLER_TYPE_CIM_INSTANCE 2
#define RESPONSE_HANDLER_TYPE_CIM_OBJECT 3
#define RESPONSE_HANDLER_TYPE_CIM_OBJECT_WITH_PATH 4
#define RESPONSE_HANDLER_TYPE_CIM_VALUE 5
#define RESPONSE_HANDLER_TYPE_CIM_INDICATION 6
#define RESPONSE_HANDLER_TYPE_CIM_REFERENCE 7
#define RESPONSE_HANDLER_TYPE_CIM_LAST 8

// ok, create a wrapper facade around the responsehandler, 
//  include type info and acces functions, 
// use static casting to retrieve from array<object_type>



class PEGASUS_EXPORT AsyncOpNode
{
  
   public:
      AsyncOpNode(void) { }
      virtual ~AsyncOpNode(void) { }
      
      //-------- Pure Virtual Methods --------//

      // clear the node so it can be reused
      virtual void reset(void) throw(IPCException) = 0;
      virtual Boolean  operator == (const void *key) const = 0;
      virtual Boolean operator == (const AsyncOpNode & node) const = 0;
      virtual Boolean timeout(void) throw(IPCException) = 0;

      virtual void notify(const void *key,
			  OperationContext *context,
			  Uint32 flag,
			  Uint32 state,
			  int type) throw(IPCException) = 0;

      virtual void put_req_context(OperationContext *context) throw(IPCException) = 0;
      virtual void put_proc_context(OperationContext *context) throw(IPCException)= 0;
      virtual void put_completion_context(OperationContext *context) throw(IPCException) = 0;
      
      virtual OperationContext *take_req_context(void) throw(IPCException) = 0;
      virtual OperationContext *take_proc_context(void) throw(IPCException) = 0;
      virtual OperationContext *take_completion_context(void) throw(IPCException) = 0;
      
      virtual void put_request(Message *request) throw(IPCException) = 0;
      virtual Message *take_request(void) throw(IPCException) = 0;
      
      virtual void put_response(Message *response) throw(IPCException) = 0;
      virtual Message *take_response(void) throw(IPCException) = 0;
      
      virtual void set_state_bits(Uint32 bits) throw(IPCException) = 0;
      virtual void clear_state_bits(Uint32 bits) throw(IPCException) = 0;
      virtual Uint32 get_state(void) throw(IPCException) = 0;
      virtual Boolean test_state_bit(Uint32 mask) throw(IPCException) = 0;
      
      virtual void set_flag_bits(Uint32 bits) throw(IPCException) = 0;
      virtual void clear_flag_bits(Uint32 bits) throw(IPCException) = 0;
      virtual Uint32 get_flag_bits(void) throw(IPCException) = 0;
      virtual Boolean test_flag_bit(Uint32 mask) throw(IPCException) = 0;
      
      virtual void set_lifetime(struct timeval *lifetime) throw(IPCException) = 0;
      virtual Boolean check_lifetime(void) const throw(IPCException) = 0;
            
      virtual void lock(void)  throw(IPCException) = 0;
      virtual void unlock(void) throw(IPCException) = 0;
      virtual void check_owner(void) throw(IPCException) = 0;
      virtual int get_rh_type(void) throw(IPCException) = 0;
      virtual void put_response_handler(void *rh, int type) throw(IPCException) = 0;
      virtual void *take_response_handler(void) throw(IPCException) = 0;
      virtual Boolean is_child(void) = 0;
      virtual Uint32 is_parent(void) = 0; 
      virtual Boolean is_my_child(AsyncOpNode *myself) throw(IPCException) = 0;
      virtual void make_orphan( AsyncOpNode *parent) throw(IPCException) = 0;

      virtual Uint32 get_total_operations(void) = 0;
      virtual Uint32 get_completed_operations(void) = 0;
      virtual ProviderHandle *take_provider_handle(void) throw(IPCException) = 0;
      virtual void put_provider_handle(ProviderHandle *handle) throw(IPCException) = 0;
      virtual int get_error_code(void) throw(IPCException) = 0;
      virtual String get_error_description(void) throw(IPCException) = 0 ;
      virtual void put_dispatch_async_struct(void *) throw (IPCException) = 0;
      virtual void *take_dispatch_async_struct(void) throw (IPCException) = 0;
};

 PEGASUS_NAMESPACE_END

#endif //Pegasus_AsyncOpNode_h
