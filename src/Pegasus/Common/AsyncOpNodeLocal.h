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


#ifndef Pegasus_AsyncOpNodeLocal_h
#define Pegasus_AsyncOpNodeLocal_h

#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMIndication.h>


PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE AsyncOpNodeLocal : public AsyncOpNode
{
   public:
      AsyncOpNodeLocal(void);
      ~AsyncOpNodeLocal(void);
            
      virtual void reset(void) throw(IPCException);
      virtual Boolean  operator == (const void *key) const;
      virtual Boolean operator == (const AsyncOpNode & node) const;

      virtual void notify(const void *key,
			  OperationContext *context,
			  Uint32 flag,
			  Uint32 state,
			  ResponseHandlerType type) throw(IPCException);

      virtual void put_req_context(OperationContext *context) throw(IPCException);
      virtual void put_proc_context(OperationContext *context) throw(IPCException);
      virtual void put_completion_context(OperationContext *context) throw(IPCException);
      
      virtual OperationContext *take_req_context(void) throw(IPCException);
      virtual OperationContext *take_proc_context(void) throw(IPCException);
      virtual OperationContext *take_completion_context(void) throw(IPCException);
      
      virtual void put_request(Message *request) throw(IPCException);
      virtual Message *take_request(void) throw(IPCException);
      
      virtual void put_response(Message *response) throw(IPCException);
      virtual Message *take_response(void) throw(IPCException);
      
      virtual void set_state_bits(Uint32 bits) throw(IPCException);
      virtual void clear_state_bits(Uint32 bits) throw(IPCException);
      virtual Uint32 get_state(void) throw(IPCException);
      virtual Boolean test_state_bit(Uint32 mask) throw(IPCException);
      
      virtual void set_flag_bits(Uint32 bits) throw(IPCException);
      virtual void clear_flag_bits(Uint32 bits) throw(IPCException);
      virtual Uint32 get_flag_bits(void) throw(IPCException);
      virtual Boolean test_flag_bit(Uint32 mask) throw(IPCException);
      
      virtual void set_lifetime(struct timeval *lifetime) throw(IPCException);
      virtual Boolean check_lifetime(struct timeval *dst) throw(IPCException);
      virtual void lock(void)  throw(IPCException);
      virtual void unlock(void) throw(IPCException);
      virtual void check_owner(void) throw(IPCException);
      virtual ResponseHandlerType get_rh_type(void) throw(IPCException);
      virtual Boolean is_child(void) { if(_parent != NULL) return true; return false; }
      virtual Uint32 is_parent(void) { return _children.count();}
      virtual Boolean is_my_child(AsyncOpNode *myself) 
      {
	 if ((AsyncOpNode *)_parent == myself)
	    return true;
	 return false;
      }
      virtual void make_orphan( AsyncOpNode *parent)
      {
	 if(parent == (AsyncOpNode *) _parent)
	    _parent = NULL;
	 else
	    throw Permission(pegasus_thread_self());
      }
      
      virtual Uint32 get_total_operations(void) { return _total_ops; }
      
      virtual Uint32 get_completed_operations(void) { return _completed_ops; }

   private:
      Mutex _mut;
      Message *_request;
      Message *_response;
      OperationContext *_req_ctx;
      OperationContext *_proc_ctx;
      OperationContext *_comp_ctx;
      Uint32 _state;
      Uint32 _flags;
      Uint32 _total_ops;
      Uint32 _completed_ops;
      struct timeval _start;
      struct timeval _lifetime;
      struct timeval _updated;
      struct timeval _timeout_interval;
      ResponseHandlerType _rh_type;
      void *_responseHandler;

      AsyncOpNode *_parent;
      DQueue<AsyncOpNode> _children;

      void _adopt_child(AsyncOpNodeLocal *child) throw(IPCException) ;
      void _disown_child(AsyncOpNodeLocal *child) throw(IPCException);
      inline void _put_ctx(OperationContext **dst, OperationContext *src) 
	 throw(IPCException)
      {
	 check_owner();
	 delete *dst;
	 *dst = src;
      }
      
      inline OperationContext * _take_ctx(OperationContext **src)
	 throw(IPCException)
      {
	 check_owner();
	 OperationContext *tmp = *src;
	 *src = (OperationContext *)0;
	 return(tmp);
      }
      
      inline void _put_msg(Message **dst, Message *src)
	 throw(IPCException)
      {
	 check_owner();
	 delete *dst;
	 *dst = src;
      }
      
      inline Message *_take_msg(Message **src)
	 throw(IPCException)
      {
	 check_owner();
	 Message *tmp = *src;
	 *src = (Message *)0;
	 return(tmp);
      }
      
      inline void _set_bits(Uint32 *dst, Uint32 bits)
	 throw(IPCException)
      {
	 check_owner();
	 *dst |= bits;
      }
      
      inline void _clear_bits(Uint32 *dst, Uint32 bits)
	 throw(IPCException)
      {
	 check_owner();
	 *dst &= ~bits;
      }
      
      inline Boolean _test_bits(Uint32 *subject, Uint32 mask)
	 throw(IPCException)
      {
	 check_owner();
	 return ((Boolean)(*subject & mask));
      }

};

PEGASUS_NAMESPACE_END


#endif // Pegasus_AsyncOpNodeLocal_h
