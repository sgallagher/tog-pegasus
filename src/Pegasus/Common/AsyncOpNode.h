//%///////////-*-c++-*-//////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
//
// Permisyion is hereby granted, free of charge, to any person obtaining a copy
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
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/internal_dq.h>
#include <Pegasus/Common/IPC.h>

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

// we need a nak response message, with an error code and exception
// so that the dispatcher can receive a nak from the provider manager


class PEGASUS_EXPORT AsyncOpNode
{
   public:

      AsyncOpNode(void);
      ~AsyncOpNode(void);
            
      void reset(void) ;
      void reset(unlocked_dq<AsyncOpNode> *dst_q);
      Boolean  operator == (const void *key) const;
      Boolean operator == (const AsyncOpNode & node) const;
      Boolean timeout(void)  ;

      OperationContext & use_context(void) ;
      void release_context(void) ;
      
      void put_request(Message *request) ;
      Message *take_request(void) ;
      
      void put_response(Message *response) ;
      Message *take_response(void) ;
      
      void set_state_bits(Uint32 bits) ;
      void clear_state_bits(Uint32 bits) ;
      Uint32 get_state(void) ;
      Boolean test_state_bit(Uint32 mask) ;
      
      void set_flag_bits(Uint32 bits) ;
      void clear_flag_bits(Uint32 bits) ;
      Uint32 get_flag_bits(void) ;
      Boolean test_flag_bit(Uint32 mask) ;
      
      void set_lifetime(struct timeval *lifetime) ;
      Boolean check_lifetime(void) const ;

      void lock(void)  throw(IPCException);
      void unlock(void) throw(IPCException);

      inline Boolean is_child(void)   
      { 
	 if(_parent != NULL) 
	    return true; 
	 return false; 
      }
      inline Uint32 is_parent(void)
      { 
	 return _children.count();
      }

      Uint32 get_total_operations(void) { return _total_ops.value(); }
      Uint32 get_completed_operations(void) { return _completed_ops.value(); }

      void deliver(const Uint32 count) throw(IPCException);
      void reserve(const Uint32 size) throw(IPCException);
      void processing(void) throw(IPCException) ;
      void processing(OperationContext *context) throw(IPCException);
      void complete(void) throw(IPCException) ;
      void complete(OperationContext *context) throw(IPCException);
      
   private:
      Mutex _mut;
      Message *_request;
      Message *_response;
      OperationContext _operation_list;
      Uint32 _state;
      Uint32 _flags;
      AtomicInt _total_ops;
      AtomicInt _completed_ops;
      struct timeval _start;
      struct timeval _lifetime;
      struct timeval _updated;
      struct timeval _timeout_interval;

      AsyncOpNode *_parent;
      unlocked_dq<AsyncOpNode> _children;

      void _put_msg(Message **dst, Message *src) ;
      Message *_take_msg(Message **src) ;
      
      Boolean _is_my_child(const AsyncOpNode & caller) const;
      void _make_orphan( AsyncOpNode & parent) ;
      void _adopt_child(AsyncOpNode *child) ;
      void _disown_child(AsyncOpNode *child) ;
      void _set_bits(Uint32 *dst, Uint32 bits) ;
      void _clear_bits(Uint32 *dst, Uint32 bits) ;
      Boolean _test_bits(Uint32 *subject, Uint32 mask) ;
};


inline void AsyncOpNode::_put_msg(Message **dst, Message *src)
{
   delete *dst;
   *dst = src;
}
      
inline Message *AsyncOpNode::_take_msg(Message **src)
{
   Message *tmp = *src;
   *src = (Message *)0;
   return(tmp);
}
      
inline void AsyncOpNode::_set_bits(Uint32 *dst, Uint32 bits)
{
   *dst |= bits;
}
      
inline void AsyncOpNode::_clear_bits(Uint32 *dst, Uint32 bits)
{
   *dst &= ~bits;
}
      
inline Boolean AsyncOpNode::_test_bits(Uint32 *subject, Uint32 mask)
{
   return (Boolean((*subject & mask) != 0));
}


inline Boolean AsyncOpNode::operator == (const void *key) const
{
   if (key == (void *)this)
      return true;
   return false;
}

inline Boolean AsyncOpNode::operator == (const AsyncOpNode & node) const
{
   return AsyncOpNode::operator==((const void *)&node);
}

inline Boolean AsyncOpNode::timeout(void) 
{
   struct timeval now;
   gettimeofday(&now, NULL);
   
   if((_updated.tv_sec + _timeout_interval.tv_sec ) >= now.tv_sec)
      if((_updated.tv_usec + _timeout_interval.tv_usec ) >= now.tv_usec)
	 return true;
   return false;
}

inline  void AsyncOpNode::put_request(Message *request) 
{
   _put_msg(&_request, request);
}

inline  Message * AsyncOpNode::take_request(void) 
{
   return(_take_msg(&_request));
}

inline  void AsyncOpNode::put_response(Message *response) 
{
   _put_msg(&_response, response);
}

inline  Message * AsyncOpNode::take_response(void) 
{
   return(_take_msg(&_response));
}

inline  void AsyncOpNode::set_state_bits(Uint32 bits) 
{
   _set_bits(&_state, bits);
}

inline  void AsyncOpNode::clear_state_bits(Uint32 bits) 
{
   _clear_bits(&_state, bits);
}


inline  Uint32 AsyncOpNode::get_state(void) 
{
   return _state;
}

inline  Boolean AsyncOpNode::test_state_bit(Uint32 mask) 
{
   return(_test_bits(&_state, mask));
}

inline  void AsyncOpNode::set_flag_bits(Uint32 bits) 
{
   _set_bits(&_flags, bits);
}

inline  void AsyncOpNode::clear_flag_bits(Uint32 bits) 
{
   _clear_bits(&_flags, bits);
}

inline  Uint32 AsyncOpNode::get_flag_bits(void) 
{
   return _flags;
}

inline  Boolean AsyncOpNode::test_flag_bit(Uint32 mask) 
{
   return(_test_bits(&_flags, mask));
}
      
inline  void AsyncOpNode::set_lifetime(struct timeval *lifetime) 
{
   _lifetime.tv_sec = lifetime->tv_sec;
   _lifetime.tv_usec = lifetime->tv_usec;
}

inline Boolean AsyncOpNode::check_lifetime(void) const 
{
   struct timeval now;
   gettimeofday(&now, NULL);
   if((_start.tv_sec + _lifetime.tv_sec ) >= now.tv_sec)
      if((_start.tv_usec + _lifetime.tv_usec ) >= now.tv_usec)
	 return true;
   return false;
}

inline  void AsyncOpNode::lock(void)  
   throw(IPCException) 
{
   _mut.lock(pegasus_thread_self());
}

inline void AsyncOpNode::unlock(void) 
   throw(IPCException) 
{
   _mut.unlock();
}

inline Boolean AsyncOpNode::_is_my_child(const AsyncOpNode & caller) const
{ 
   if ( _parent == &caller )
      return true;
   return false;
}

inline void AsyncOpNode::_make_orphan( AsyncOpNode & parent) 
{
   if( _parent == &parent )
   {
      _parent = NULL;
      parent._children.remove(this);
   }
   else
      throw Permission(pegasus_thread_self());
}

inline void AsyncOpNode::_adopt_child(AsyncOpNode *child) 
{
   if(child == NULL)
      throw NullPointer();
   if(true == child->is_child())
      throw Permission(pegasus_thread_self());
   child->_parent = this;
   _children.insert_last(child);
}
      
inline void AsyncOpNode::_disown_child(AsyncOpNode *child)
{
   if(child == NULL)
      throw NullPointer();
   if( false == child->is_child() || false == child->_is_my_child( *this ))
      throw Permission(pegasus_thread_self());
   child->_make_orphan( *this );
   _children.remove(child);
} 


inline void AsyncOpNode::deliver(const Uint32 count) 
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _completed_ops = count;
   _flags |= ASYNC_OPFLAGS_DELIVER;
   gettimeofday(&_updated, NULL);
   _mut.unlock();
   return;
}

inline void AsyncOpNode::reserve(const Uint32 size)
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _total_ops = size;
   _flags |= ASYNC_OPFLAGS_RESERVE;
   gettimeofday(&_updated, NULL);
   _mut.unlock();
   return;
}

inline void AsyncOpNode::processing(void) 
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _flags |= ASYNC_OPFLAGS_PROCESSING;
   gettimeofday(&_updated, NULL);
   _mut.unlock();
   return;
}

// con will be empty upon return of this member function
inline void AsyncOpNode::processing(OperationContext *con) 
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _flags |= ASYNC_OPFLAGS_PROCESSING;
   gettimeofday(&_updated, NULL);
   
   context *c = con->remove_context();
   while(c != 0)
   {
      _operation_list.add_context(c);
      c = con->remove_context();
   }
   _mut.unlock();
   return;
}

inline void AsyncOpNode::complete(void) 
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _flags |= ASYNC_OPFLAGS_COMPLETE;
   gettimeofday(&_updated, NULL);
   _mut.unlock();
   return;
}

inline void AsyncOpNode::complete(OperationContext *con)
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _flags |= ASYNC_OPFLAGS_COMPLETE;
   gettimeofday(&_updated, NULL);
   context *c = con->remove_context();
   while(c != 0)
   {
      _operation_list.add_context(c);
      c = con->remove_context();
   }
   _mut.unlock();
}

// need to have a handle to the manager; i.e., provider manager, 
// service manager, repository manager. 
// handle must support the cancellation of aysnchronous operations.
// manager->cancel_operation();
// manager->suspend_operation();
// manager->resume_operation();

PEGASUS_NAMESPACE_END

#endif //Pegasus_AsyncOpNode_h
