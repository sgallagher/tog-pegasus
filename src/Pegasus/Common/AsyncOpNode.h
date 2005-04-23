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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: Amit K Arora, IBM (amita@in.ibm.com) for Bug#1188
//              Alagaraja Ramasubramanian (alags_raj@in.ibm.com) for Bug#1090
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_AsyncOpNode_h
#define Pegasus_AsyncOpNode_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/internal_dq.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

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
#define ASYNC_OPFLAGS_META_DISPATCHER   0x00000040
#define ASYNC_OPFLAGS_FIRE_AND_FORGET   0x00000080
#define ASYNC_OPFLAGS_SIMPLE_STATUS     0x00000100
#define ASYNC_OPFLAGS_CALLBACK          0x00000200
#define ASYNC_OPFLAGS_FORWARD           0x00000400
#define ASYNC_OPFLAGS_PSEUDO_CALLBACK   0x00000800
#define ASYNC_OPFLAGS_SAFE_CALLBACK     0x00001000

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
#define ASYNC_OPSTATE_ORPHANED          0x00001000
#define ASYNC_OPSTATE_RELEASED          0x00002000

class Cimom;
class Thread;

class PEGASUS_COMMON_LINKAGE AsyncOpNode
{

//     public:
//       static void * operator new(size_t );
//       static void operator delete( void *, size_t);
//    private:
//       static AsyncOpNode * _headOfFreeList;
//       static const int BLOCK_SIZE;
//       static Mutex _alloc_mut;
   public:

      AsyncOpNode(void);
      ~AsyncOpNode(void);

      Boolean  operator == (const void *key) const;
      Boolean operator == (const AsyncOpNode & node) const;

      void get_timeout_interval(struct timeval *buffer);
      void set_timeout_interval(const struct timeval *interval);

      Boolean timeout(void);

      OperationContext & get_context(void);

      void put_request(const Message *request);
      Message *get_request(void) ;

      void put_response(const Message *response);
      Message *get_response(void) ;

      Uint32 read_state(void);
      void write_state(Uint32);

      Uint32 read_flags(void);
      void write_flags(Uint32);

      /**
        @exception  IPCException    Indicates an IPC error occurred.
      */
      void lock(void);

      /**
        @exception  IPCException    Indicates an IPC error occurred.
      */
      void unlock(void);

      /**
        @exception  IPCException    Indicates an IPC error occurred.
      */
      void udpate(void);

      /**
        @exception  IPCException    Indicates an IPC error occurred.
      */
      void deliver(const Uint32 count);

      /**
        @exception  IPCException    Indicates an IPC error occurred.
      */
      void reserve(const Uint32 size);

      /**
        @exception  IPCException    Indicates an IPC error occurred.
      */
      void processing(void);

      /**
        @exception  IPCException    Indicates an IPC error occurred.
      */
      void processing(OperationContext *context);

      /**
        @exception  IPCException    Indicates an IPC error occurred.
      */
      void complete(void);

      /**
        @exception  IPCException    Indicates an IPC error occurred.
      */
      void complete(OperationContext *context);
      void release(void);
      void wait(void);


   private:
      Semaphore _client_sem;
      Mutex _mut;
      unlocked_dq<Message> _request;
      unlocked_dq<Message> _response;

      OperationContext _operation_list;
      Uint32 _state;
      Uint32 _flags;
      Uint32 _offered_count;
      Uint32 _total_ops;
      Uint32 _completed_ops;
      Uint32 _user_data;
      Uint32 _completion_code;
      MessageQueue *_op_dest;

      struct timeval _start;
      struct timeval _lifetime;
      struct timeval _updated;
      struct timeval _timeout_interval;

      AsyncOpNode *_parent;
      unlocked_dq<AsyncOpNode> _children;

      void _reset(unlocked_dq<AsyncOpNode> *dst_q);

      // the lifetime member is for cache management by the cimom
      void _set_lifetime(struct timeval *lifetime) ;
      Boolean _check_lifetime(void) ;

      Boolean _is_child(void) ;
      Uint32 _is_parent(void) ;
      Boolean _is_my_child(const AsyncOpNode & caller) const;
      void _make_orphan( AsyncOpNode & parent) ;
      void _adopt_child(AsyncOpNode *child) ;
      void _disown_child(AsyncOpNode *child) ;
      void (*_async_callback)(AsyncOpNode *,
                  MessageQueue *,
                  void *);
      void (*__async_callback)(Message *, void *, void *);
      // << Tue Mar 12 14:44:51 2002 mdd >>
      // pointers for async callbacks  - don't use
      AsyncOpNode *_callback_node;
      MessageQueue *_callback_response_q;
      void *_callback_ptr;
      void *_callback_parameter;
      void *_callback_handle;
      Condition *_callback_notify;

      MessageQueue *_callback_request_q;
      //      << Tue Mar 12 14:44:53 2002 mdd >>
      // pointers to help static class message handlers - don't use
      MessageQueue *_service_ptr;
      Thread *_thread_ptr;

      friend class cimom;
      friend class MessageQueueService;
      friend class ProviderManagerService;
      friend class BinaryMessageHandler;
 public:
      // << Tue Jun  4 16:44:09 2002 mdd >>
      // debug artifact
      Uint32 _source_queue;
      // << Fri Jul 19 08:41:45 2002 mdd >>
      // debugging utility
      // careful - wipes out current value of *buf
      void print_to_buffer(char **buf);
      String print_to_string(void);
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


inline void AsyncOpNode::get_timeout_interval(struct timeval *buffer)
{
   if(buffer != 0)
   {
      AutoMutex autoMut(_mut);
      buffer->tv_sec = _timeout_interval.tv_sec;
      buffer->tv_usec = _timeout_interval.tv_usec;
   } // mutex unlocks here
}

inline void AsyncOpNode::set_timeout_interval(const struct timeval *interval)
{
   if(interval != 0)
   {
      AutoMutex autoMut(_mut);
      _timeout_interval.tv_sec = interval->tv_sec;
      _timeout_interval.tv_usec = interval->tv_usec;
      gettimeofday(&_updated, NULL);
   } // mutex unlocks here
}


inline Boolean AsyncOpNode::timeout(void)
{
   struct timeval now;
   gettimeofday(&now, NULL);
   Boolean ret = false;

   AutoMutex autoMut(_mut);
   if((_updated.tv_sec + _timeout_interval.tv_sec ) < now.tv_sec)
       if((_updated.tv_usec + _timeout_interval.tv_usec ) < now.tv_usec)
    ret =  true;

   return ret;
}

// context is now a locked list
inline OperationContext & AsyncOpNode::get_context(void)
{
   gettimeofday(&_updated, NULL);
   return _operation_list;
}


inline  void AsyncOpNode::put_request(const Message *request)
{
   AutoMutex autoMut(_mut);
   gettimeofday(&_updated, NULL);
   if( false == _request.exists(reinterpret_cast<void *>(const_cast<Message *>(request))) )
   _request.insert_last( const_cast<Message *>(request) ) ;

//   _request = const_cast<Message *>(request);
}

inline Message * AsyncOpNode::get_request(void)
{
   Message *ret;
   AutoMutex autoMut(_mut);
   gettimeofday(&_updated, NULL);
   ret = _request.remove_first() ;

   return ret;
}

inline void AsyncOpNode::put_response(const Message *response)
{
   AutoMutex autoMut(_mut);
   gettimeofday(&_updated, NULL);
   if (false == _response.exists(reinterpret_cast<void *>(const_cast<Message *>(response))))
   _response.insert_last( const_cast<Message *>(response) );
}

inline Message * AsyncOpNode::get_response(void)
{
   Message *ret;

   AutoMutex autoMut(_mut);
//   gettimeofday(&_updated, NULL);
   ret = _response.remove_first();
//   ret = _response;

   return ret;
}

inline Uint32 AsyncOpNode::read_state(void)
{
   AutoMutex autoMut(_mut);
   gettimeofday(&_updated, NULL);
   Uint32 ret = _state;

   return ret;

}

inline void AsyncOpNode::write_state(Uint32 state)
{
   AutoMutex autoMut(_mut);
   gettimeofday(&_updated, NULL);
   _state = state;
}

inline Uint32 AsyncOpNode::read_flags(void)
{
   AutoMutex autoMut(_mut);
   gettimeofday(&_updated, NULL);
   Uint32 ret = _flags;

   return ret;
}

inline void AsyncOpNode::write_flags(Uint32 flags)
{
   AutoMutex autoMut(_mut);
   gettimeofday(&_updated, NULL);
   _flags = flags;
}


inline  void AsyncOpNode::lock(void)
{
   _mut.lock(pegasus_thread_self());
}

inline void AsyncOpNode::unlock(void)
{
   _mut.unlock();
}

inline void AsyncOpNode::udpate(void)
{
   AutoMutex autoMut(_mut);
   gettimeofday(&_updated, NULL);
}

inline void AsyncOpNode::deliver(const Uint32 count)
{
   AutoMutex autoMut(_mut);
   _completed_ops = count;
   _state |= ASYNC_OPSTATE_DELIVER;
   gettimeofday(&_updated, NULL);
}

inline void AsyncOpNode::reserve(const Uint32 size)
{
   AutoMutex autoMut(_mut);
   _total_ops = size;
   _state |= ASYNC_OPSTATE_RESERVE;
   gettimeofday(&_updated, NULL);
}

inline void AsyncOpNode::processing(void)
{
   AutoMutex autoMut(_mut);
   _state |= ASYNC_OPSTATE_PROCESSING;
   gettimeofday(&_updated, NULL);
}

// con will be empty upon return of this member function
inline void AsyncOpNode::processing(OperationContext *con)
{
   AutoMutex autoMut(_mut);
   _state |= ASYNC_OPSTATE_PROCESSING;
   gettimeofday(&_updated, NULL);

   /*
   context *c = con->remove_context();
   while(c != 0)
   {
      _operation_list.add_context(c);
      c = con->remove_context();
   }
   */
}

inline void AsyncOpNode::complete(void)
{
   AutoMutex autoMut(_mut);
   _state |= ASYNC_OPSTATE_COMPLETE;
   gettimeofday(&_updated, NULL);
}

inline void AsyncOpNode::complete(OperationContext *con)
{
   AutoMutex autoMut(_mut);
   _state |= ASYNC_OPSTATE_COMPLETE;
   gettimeofday(&_updated, NULL);
   /*
   context *c = con->remove_context();
   while(c != 0)
   {
      _operation_list.add_context(c);
      c = con->remove_context();
   }
   */
}

inline void AsyncOpNode::wait(void)
{
   _client_sem.wait();
}

inline void AsyncOpNode::release(void)
{
   AutoMutex autoMut(_mut);
   _state |= ASYNC_OPSTATE_RELEASED;
}

inline  void AsyncOpNode::_set_lifetime(struct timeval *lifetime)
{
   AutoMutex autoMut(_mut);
   _lifetime.tv_sec = lifetime->tv_sec;
   _lifetime.tv_usec = lifetime->tv_usec;

}

inline Boolean AsyncOpNode::_check_lifetime(void)
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
