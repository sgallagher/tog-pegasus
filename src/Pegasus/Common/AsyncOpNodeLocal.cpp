//%//////////////////////////////////////////////////////////////////////////
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

#include <Pegasus/Common/AsyncOpNodeLocal.h>
#include <Pegasus/Common/ResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN
namespace 
{
   
   void delete_rh(void *handler, ResponseHandlerType type)
      throw(TypeMismatch)
   {
      if(handler == NULL)
	 return;
      switch(type)
      {
	 case UNDEFINED:
	    break;
	 case CIM_CLASS:
	    delete static_cast<ResponseHandler<CIMClass> *>(handler);
	    break;
	 case CIM_INSTANCE:
	    delete static_cast<ResponseHandler<CIMInstance> *>(handler);
	    break;
	 case CIM_OBJECT:
	    delete static_cast<ResponseHandler<CIMObject> *>(handler);
	    break;
	 case CIM_OBJECT_WITH_PATH:
	    delete static_cast<ResponseHandler<CIMObjectWithPath> *>(handler);
	    break;
	 case CIM_VALUE:
	    delete static_cast<ResponseHandler<CIMValue> *>(handler);
	    break;
	 case CIM_INDICATION:
	    delete static_cast<ResponseHandler<CIMIndication> *>(handler);
	    break;
	 case CIM_REFERENCE:
	    delete static_cast<ResponseHandler<CIMReference> *>(handler);
	 default:
	    throw TypeMismatch();
      }
   }
}



AsyncOpNodeLocal::AsyncOpNodeLocal(void) 
   : _mut(), _request(0), _response(0), _req_ctx(0), 
     _proc_ctx(0), _comp_ctx(0), _state(0), _flags(0),
     _total_ops(0), _completed_ops(0), _responseHandler(0), 
     _parent(0), _children(true)
{
   memset(&_start, 0x00, sizeof(struct timeval));
   memset(&_lifetime, 0x00, sizeof(struct timeval));
   memset(&_updated, 0x00, sizeof(struct timeval));
   memset(&_timeout_interval, 0xff, sizeof(struct timeval));
}

AsyncOpNodeLocal::~AsyncOpNodeLocal(void)
{
   reset();
   delete_rh(_responseHandler, _rh_type);
}

void AsyncOpNodeLocal::reset(void) throw(IPCException)
{
   delete _request;
   delete _response;
   delete _req_ctx;
   delete _proc_ctx;
   delete _comp_ctx;
   _children.empty_list();
}

inline Boolean AsyncOpNodeLocal::operator == (const void *key) const
{
   if (key == (void *)this)
      return true;
   return false;
}

inline Boolean AsyncOpNodeLocal::operator == (const AsyncOpNode & node) const
{
   return AsyncOpNodeLocal::operator==((const void *)&node);
}


// notifications should only come from children 
 void AsyncOpNodeLocal::notify(const void *key, 
			       OperationContext *context,
			       Uint32 flag, 
			       Uint32 state,
			       ResponseHandlerType type)
   throw(IPCException)
{
   if( ! _children.count() )
      return;
   if(flag & AsyncOpFlags::COMPLETE)
   {
      ;
   }
   if(flag & AsyncOpFlags::INDICATION)
   {
      ;
   }
   
   if(flag & AsyncOpFlags::DELIVER)
   {
      ;
   }
   
   return;
}



inline  void AsyncOpNodeLocal::put_req_context(OperationContext *context) 
   throw(IPCException)
{
   _put_ctx(&_req_ctx, context);
}

inline  void AsyncOpNodeLocal::put_proc_context(OperationContext *context) 
   throw(IPCException)
{
   _put_ctx(&_proc_ctx, context);
}

inline  void AsyncOpNodeLocal::put_completion_context(OperationContext *context) 
   throw(IPCException)
{
   _put_ctx(&_comp_ctx, context);
}

inline  OperationContext * AsyncOpNodeLocal::take_req_context(void) 
   throw(IPCException)
{
   return(_take_ctx(&_req_ctx));
}
         
inline  OperationContext * AsyncOpNodeLocal::take_proc_context(void)
   throw(IPCException)
{
   return(_take_ctx(&_proc_ctx));
}

inline  OperationContext * AsyncOpNodeLocal::take_completion_context(void) 
   throw(IPCException)
{
   return(_take_ctx(&_comp_ctx));
}

      
inline  void AsyncOpNodeLocal::put_request(Message *request) 
   throw(IPCException)
{
   _put_msg(&_request, request);
}

inline  Message * AsyncOpNodeLocal::take_request(void) 
   throw(IPCException)
{
   return(_take_msg(&_request));
}

inline  void AsyncOpNodeLocal::put_response(Message *response) 
   throw(IPCException)
{
   _put_msg(&_response, response);
}

inline  Message * AsyncOpNodeLocal::take_response(void) 
   throw(IPCException)
{
   return(_take_msg(&_response));
}

inline  void AsyncOpNodeLocal::set_state_bits(Uint32 bits) 
   throw(IPCException)
{
   _set_bits(&_state, bits);
}

inline  void AsyncOpNodeLocal::clear_state_bits(Uint32 bits) 
   throw(IPCException)
{
   _clear_bits(&_state, bits);
}


inline  Uint32 AsyncOpNodeLocal::get_state(void) 
   throw(IPCException)
{
   check_owner();
   return _state;
}

inline  Boolean AsyncOpNodeLocal::test_state_bit(Uint32 mask) 
   throw(IPCException)
{
   return(_test_bits(&_state, mask));
}

inline  void AsyncOpNodeLocal::set_flag_bits(Uint32 bits) 
   throw(IPCException)
{
   _set_bits(&_flags, bits);
}

inline  void AsyncOpNodeLocal::clear_flag_bits(Uint32 bits) 
   throw(IPCException)
{
   _clear_bits(&_flags, bits);
}

inline  Uint32 AsyncOpNodeLocal::get_flag_bits(void) 
   throw(IPCException)
{
   check_owner();
   return _flags;
}

inline  Boolean AsyncOpNodeLocal::test_flag_bit(Uint32 mask) 
   throw(IPCException)
{
   return(_test_bits(&_flags, mask));
}
      
inline  void AsyncOpNodeLocal::set_lifetime(struct timeval *lifetime) 
   throw(IPCException)
{
   check_owner();
   _lifetime.tv_sec = lifetime->tv_sec;
   _lifetime.tv_usec = lifetime->tv_usec;
}

Boolean AsyncOpNodeLocal::check_lifetime(struct timeval *dst)
   throw(IPCException)
{
   check_owner();
   struct timeval now;
   gettimeofday(&now, NULL);

   dst->tv_sec = now.tv_sec - _start.tv_sec;
   dst->tv_usec = now.tv_usec - _start.tv_usec;
   
   if(dst->tv_sec <= _lifetime.tv_sec)
      if(dst->tv_usec <= _lifetime.tv_usec)
	 return false;
   return true;
}

inline  void AsyncOpNodeLocal::lock(void)  
   throw(IPCException) 
{
   _mut.lock(pegasus_thread_self());
}

inline  void AsyncOpNodeLocal::unlock(void) 
   throw(IPCException) 
{
   _mut.unlock();
}

inline  void AsyncOpNodeLocal::check_owner(void) throw(IPCException)
{
   if(_mut.get_owner() != pegasus_thread_self())
      throw Permission(pegasus_thread_self());
   return;
}

inline ResponseHandlerType AsyncOpNodeLocal::get_rh_type(void) 
   throw(IPCException)
{
   check_owner();
   return _rh_type;
}


inline void AsyncOpNodeLocal::_adopt_child(AsyncOpNodeLocal *child) 
   throw(IPCException)
{
   if(child == NULL)
      throw NullPointer();
   if(true == child->is_child())
      throw Permission(pegasus_thread_self());
   child->_parent = this;
   _children.insert_last(child);
}

      
inline void AsyncOpNodeLocal::_disown_child(AsyncOpNodeLocal *child)
   throw(IPCException)
{
   if(child == NULL)
      throw NullPointer();
   if( false == child->is_child() || false == child->is_my_child(this))
      throw Permission(pegasus_thread_self());
   child->make_orphan(this);
   _children.remove(child);
}


PEGASUS_NAMESPACE_END
