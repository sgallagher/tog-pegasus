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
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/internal_dq.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

// REVIEW: missing "PEGASUS_" prefixes:

#define ASYNC_OPFLAGS_UNKNOWN           0x00000000
#define ASYNC_OPFLAGS_INTERVAL_REPEAT   0x00000010
#define ASYNC_OPFLAGS_INDICATION        0x00000020
#define ASYNC_OPFLAGS_REMOTE            0x00000040
#define ASYNC_OPFLAGS_LOCAL_OUT_OF_PROC 0x00000080
#define ASYNC_OPFLAGS_PHASED            0x00000001
#define ASYNC_OPFLAGS_PARTIAL           0x00000002
#define ASYNC_OPFLAGS_NORMAL            0x00000000
#define ASYNC_OPFLAGS_SINGLE            0x00000008
#define ASYNC_OPFLAGS_MULTIPLE          0x00000010
#define ASYNC_OPFLAGS_TOTAL             0x00000020

#define ASYNC_OPSTATE_UNKNOWN           0x00000000
#define ASYNC_OPSTATE_OFFERED           0x00000001
#define ASYNC_OPSTATE_DECLINED          0x00000002
#define ASYNC_OPSTATE_STARTED           0x00000004
#define ASYNC_OPSTATE_PROCESSING        0x00000008
#define ASYNC_OPSTATE_DELIVER           0x00000010 
#define ASYNC_OPSTATE_RESERVE           0x00000020
#define ASYNC_OPSTATE_COMPLETE          0x00000040
#define ASYNC_OPSTATE_TIMEOUT           0x00000080
#define ASYNC_OPSTATE_CANCELLED         0x00000100
#define ASYNC_OPSTATE_PAUSED            0x00000200
#define ASYNC_OPSTATE_SUSPENDED         0x00000400
#define ASYNC_OPSTATE_RESUMED           0x00000800
#define ASYNC_OPSTATE_ORPHANED          0X00001000



class PEGASUS_COMMON_LINKAGE AsyncOpNode
{
   public:

      AsyncOpNode(void);
      ~AsyncOpNode(void);
            
      Boolean  operator == (const void *key) const;
      Boolean operator == (const AsyncOpNode & node) const;
      Boolean timeout(void)  ;

      OperationContext & get_context(void) ;

      void put_request(const Message *request) ;
      const Message *get_request(void) ;
      
      void put_response(const Message *response) ;
      const Message *get_response(void) ;
      
      Uint32 read_state(void) ;
      void write_state(Uint32) ;

      Uint32 read_flags(void);
      void write_flags(Uint32);
      
      void lock(void)  throw(IPCException);
      void unlock(void) throw(IPCException);
      void udpate(void) throw(IPCException);
      void deliver(const Uint32 count) throw(IPCException);
      void reserve(const Uint32 size) throw(IPCException);
      void processing(void) throw(IPCException) ;
      void processing(OperationContext *context) throw(IPCException);
      void complete(void) throw(IPCException) ;
      void complete(OperationContext *context) throw(IPCException);
      
   private:
      Semaphore _client_sem;
      Mutex _mut;
      Message *_request;
      Message *_response;
      OperationContext _operation_list;
      Uint32 _state;
      Uint32 _flags;
      Uint32 _offered_count;
      Uint32 _total_ops;
      Uint32 _completed_ops;
      struct timeval _start;
      struct timeval _lifetime;
      struct timeval _updated;
      struct timeval _timeout_interval;

      AsyncOpNode *_parent;
      unlocked_dq<AsyncOpNode> _children;

      void _reset(unlocked_dq<AsyncOpNode> *dst_q);

      void _set_lifetime(struct timeval *lifetime) ;
      Boolean _check_lifetime(void) const ;

      Boolean _is_child(void) ;
      Uint32 _is_parent(void) ;

      Boolean _is_my_child(const AsyncOpNode & caller) const;
      void _make_orphan( AsyncOpNode & parent) ;
      void _adopt_child(AsyncOpNode *child) ;
      void _disown_child(AsyncOpNode *child) ;
};


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

// context is now a locked list
inline OperationContext & AsyncOpNode::get_context(void)
{
   return _operation_list;
}


inline  void AsyncOpNode::put_request(const Message *request) 
{
   _mut.lock(pegasus_thread_self());
   _request = const_cast<Message *>(request);
   _mut.unlock();
   
}

inline const Message * AsyncOpNode::get_request(void) 
{
   Message *req = 0;
   _mut.lock(pegasus_thread_self());
   req = _request;
   _mut.unlock();
   return req;
}

inline void AsyncOpNode::put_response(const Message *response) 
{
   _mut.lock(pegasus_thread_self());
   _response = const_cast<Message *>(response);
   _mut.unlock();
   
}

inline const Message * AsyncOpNode::get_response(void) 
{
   Message *resp;
   _mut.lock(pegasus_thread_self());
   resp = _response;
   _mut.unlock();
   return resp;
}

inline Uint32 AsyncOpNode::read_state(void)
{
   return _state;
}

inline void AsyncOpNode::write_state(Uint32 state)
{
   _state = state;
}

inline Uint32 AsyncOpNode::read_flags(void)
{
   return _flags;
}

inline void AsyncOpNode::write_flags(Uint32 flags)
{
   _flags = flags;
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

inline void AsyncOpNode::udpate(void)
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   gettimeofday(&_updated, NULL);
   _mut.unlock();
   return;
}

inline void AsyncOpNode::deliver(const Uint32 count) 
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _completed_ops = count;
   _flags |= ASYNC_OPSTATE_DELIVER;
   gettimeofday(&_updated, NULL);
   _mut.unlock();
   return;
}

inline void AsyncOpNode::reserve(const Uint32 size)
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _total_ops = size;
   _flags |= ASYNC_OPSTATE_RESERVE;
   gettimeofday(&_updated, NULL);
   _mut.unlock();
   return;
}

inline void AsyncOpNode::processing(void) 
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _flags |= ASYNC_OPSTATE_PROCESSING;
   gettimeofday(&_updated, NULL);
   _mut.unlock();
   return;
}

// con will be empty upon return of this member function
inline void AsyncOpNode::processing(OperationContext *con) 
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _flags |= ASYNC_OPSTATE_PROCESSING;
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
   _flags |= ASYNC_OPSTATE_COMPLETE;
   gettimeofday(&_updated, NULL);
   _mut.unlock();
   return;
}

inline void AsyncOpNode::complete(OperationContext *con)
   throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
   _flags |= ASYNC_OPSTATE_COMPLETE;
   gettimeofday(&_updated, NULL);
   context *c = con->remove_context();
   while(c != 0)
   {
      _operation_list.add_context(c);
      c = con->remove_context();
   }
   _mut.unlock();
}

inline  void AsyncOpNode::_set_lifetime(struct timeval *lifetime) 
{
   _lifetime.tv_sec = lifetime->tv_sec;
   _lifetime.tv_usec = lifetime->tv_usec;
}

inline Boolean AsyncOpNode::_check_lifetime(void) const 
{
   struct timeval now;
   gettimeofday(&now, NULL);
   if((_start.tv_sec + _lifetime.tv_sec ) >= now.tv_sec)
      if((_start.tv_usec + _lifetime.tv_usec ) >= now.tv_usec)
	 return true;
   return false;
}

inline Boolean AsyncOpNode::_is_child(void)
{
   if (_parent != 0)
      return true;
   return false;
}

inline Uint32 AsyncOpNode::_is_parent(void)
{
   return _children.count();
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
   if(true == child->_is_child())
      throw Permission(pegasus_thread_self());
   child->_parent = this;
   _children.insert_last(child);
}
      
inline void AsyncOpNode::_disown_child(AsyncOpNode *child)
{
   if(child == NULL)
      throw NullPointer();
   if( false == child->_is_child() || false == child->_is_my_child( *this ))
      throw Permission(pegasus_thread_self());
   child->_make_orphan( *this );
   _children.remove(child);
} 



PEGASUS_NAMESPACE_END

#endif //Pegasus_AsyncOpNode_h
