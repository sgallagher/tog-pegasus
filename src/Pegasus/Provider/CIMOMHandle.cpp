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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/CimomMessage.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/peg_authorization.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/TraceComponents.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Sharable.h>

#include  "CIMOMHandle.h"
PEGASUS_NAMESPACE_BEGIN


const String TimeoutContainer::NAME = "TimeoutContainer";

TimeoutContainer::TimeoutContainer(const OperationContext::Container & container)
{
   const TimeoutContainer * p = dynamic_cast<const TimeoutContainer *>(&container);
   if(p == 0)
   {
      throw DynamicCastFailedException();
   }
   _value = p->_value;
}

TimeoutContainer::TimeoutContainer(Uint32 timeout)
{
   _value = timeout;
}

String TimeoutContainer::getName(void) const 
{
   return (NAME);
}

OperationContext::Container * TimeoutContainer::clone(void) const
{
   return (new TimeoutContainer(_value));
}

void TimeoutContainer::destroy(void)
{
   delete this;
}

Uint32 TimeoutContainer::getTimeOut(void) const
{
   return _value;
}



class CIMOMHandle;
class cimom_handle_op_semaphore;
class CIMOMHandle::_cimom_handle_rep : public MessageQueue, public Sharable
{
   private:
      Uint32 _output_qid;
      Uint32 _return_qid;
      AtomicInt _response_type;
      Mutex _recursion;
      Mutex _idle_mutex;
      Mutex _qid_mutex;
      
      AtomicInt _server_terminating;
      Semaphore _msg_avail;
      AsyncDQueue<Message> _response;
      Message *_request;
      AtomicInt _op_timeout;
      AtomicInt _pending_operation;
      AtomicInt _no_unload;
      struct timeval _idle_timeout;

   public: 
      typedef MessageQueue Base;
      
      _cimom_handle_rep(void);
      _cimom_handle_rep(Uint32 out_qid, Uint32 ret_qid);
      ~_cimom_handle_rep(void) {  }
   private:
      void get_idle_timer(struct timeval *);
      void update_idle_timer(void);
      Uint32 get_operation_timeout(void);
      void set_operation_timeout(Uint32);
      Boolean pending_operation(void);
      Boolean unload_ok(void);

      Uint32 get_output_qid(void);
      void set_output_qid(Uint32);
      Uint32 get_return_qid(void);
      void set_return_qid(Uint32);
      Uint32 get_qid(void);

      void protect(void);
      void unprotect(void);
      
      virtual void handleEnqueue(Message *);
      virtual void handleEnqueue(void);

      Message *do_request(Message *, Uint32 response_type, const OperationContext & context) 
	 throw();
      
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _dispatch(void *);
      MessageQueue * q_exists(Uint32 qid) const
      {
	 return MessageQueue::lookup(qid);
      }

      _cimom_handle_rep & operator = (const _cimom_handle_rep & );
      friend class CIMOMHandle;
      friend class cimom_handle_op_semaphore;
};

class cimom_handle_op_semaphore
{
   private:
      cimom_handle_op_semaphore(void)
      {
      }
      
   public:
      cimom_handle_op_semaphore(CIMOMHandle::_cimom_handle_rep *rep)
	 :_rep(rep)
      {
	 _rep->update_idle_timer();
	 (_rep->_pending_operation)++;
      }
      ~cimom_handle_op_semaphore(void)
      {
	 if(_rep)
	 {
	    (_rep->_pending_operation)--;
	 }
      }
   private:
      CIMOMHandle::_cimom_handle_rep *_rep;
};


class cimom_handle_dispatch
{
   private:
      cimom_handle_dispatch(void);
   public:
      cimom_handle_dispatch(Message *msg,
			    Uint32 my_qid,
			    Uint32 output_qid)
	 : _msg(msg),
	   _my_qid(my_qid),
	   _out_qid(output_qid)
      {
      }
      ~cimom_handle_dispatch(void)
      {
      }
      

      Message *_msg;
      Uint32 _my_qid;
      Uint32 _out_qid;
};



CIMOMHandle::_cimom_handle_rep::_cimom_handle_rep(void)
   : Base(PEGASUS_QUEUENAME_INTERNALCLIENT),
     _server_terminating(0),
     _msg_avail(0),
     _response(true,0),
     _op_timeout(0),
     _pending_operation(0),
     _no_unload(0)
{
   // initialize the qids
   // output queue defaults to CIMOPRequestDispatcher
   MessageQueue *out = MessageQueue::lookup(PEGASUS_QUEUENAME_BINARY_HANDLER);
   if(out)
      _output_qid = out->getQueueId();
   else
      _output_qid = _queueId;
   
   // return queue defaults to myself
   _return_qid = _queueId;
   gettimeofday(&_idle_timeout, NULL);
   
}

CIMOMHandle::_cimom_handle_rep::_cimom_handle_rep(Uint32 out_qid, Uint32 ret_qid)
   : Base(PEGASUS_QUEUENAME_INTERNALCLIENT),
     _output_qid(out_qid), 
     _return_qid(ret_qid),
     _server_terminating(0),
     _msg_avail(0),
     _response(true,0),
     _op_timeout(0),
     _pending_operation(0),
     _no_unload(0)
{
   if(0 == q_exists(_output_qid) )
      _output_qid = _queueId;
   if(0 == q_exists(_return_qid) )
      _return_qid = _queueId;
   gettimeofday(&_idle_timeout, NULL);
}

void CIMOMHandle::_cimom_handle_rep::get_idle_timer(struct timeval *tv)
{
   if(tv == 0)
      return;
   
   try 
   {
      _idle_mutex.lock(pegasus_thread_self());
      memcpy(tv, &_idle_timeout, sizeof(struct timeval));
      _idle_mutex.unlock();
   }
   catch(...)
   {
      gettimeofday(tv, NULL);
   }
}

void CIMOMHandle::_cimom_handle_rep::update_idle_timer(void)
{
   try
   {
      _idle_mutex.lock(pegasus_thread_self());
      gettimeofday(&_idle_timeout, NULL);
      _idle_mutex.unlock();
   }
   catch(...)
   {
   }
}

Uint32 CIMOMHandle::_cimom_handle_rep::get_operation_timeout(void)
{
   return _op_timeout.value();
}

void CIMOMHandle::_cimom_handle_rep::set_operation_timeout(Uint32 t)
{
   _op_timeout = t;
}

Boolean CIMOMHandle::_cimom_handle_rep::pending_operation(void)
{
   if(_pending_operation.value())
      return true;
   return false;
}

Boolean CIMOMHandle::_cimom_handle_rep::unload_ok(void)
{
   if( _no_unload.value() || _pending_operation.value() )
      return false;
   return true;
}

Uint32 CIMOMHandle::_cimom_handle_rep::get_output_qid(void)
{
   try
   {
      _qid_mutex.lock(pegasus_thread_self());
      Uint32 qid = _output_qid;
      _qid_mutex.unlock();
      return qid;
   }
   catch(...)
   {
      return _queueId;
   }
}

void CIMOMHandle::_cimom_handle_rep::set_output_qid(Uint32 qid)
{
   try
   {
      _qid_mutex.lock(pegasus_thread_self());
      _output_qid = qid;
      _qid_mutex.unlock();
   }
   catch(...)
   {
   }
}

Uint32 CIMOMHandle::_cimom_handle_rep::get_return_qid(void)
{
   try
   {
      _qid_mutex.lock(pegasus_thread_self());
      Uint32 qid = _return_qid;
      _qid_mutex.unlock();
      return qid;
   }
   catch(...)
   {
      return _queueId;
   }
}

void CIMOMHandle::_cimom_handle_rep::set_return_qid(Uint32 qid)
{
   try
   {
      _qid_mutex.lock(pegasus_thread_self());
      _return_qid = qid;
      _qid_mutex.unlock();
   }
   catch(...)
   {
   }
}

Uint32 CIMOMHandle::_cimom_handle_rep::get_qid(void)
{
   return _queueId;
}

void CIMOMHandle::_cimom_handle_rep::protect(void)
{
   _no_unload++;
}

void CIMOMHandle::_cimom_handle_rep::unprotect(void)
{
   _no_unload--;
}

void CIMOMHandle::_cimom_handle_rep::handleEnqueue(void)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
                    "CIMOMHandle::_cimom_handle_rep::handleEnqueue(Message *)");

   Message *message = dequeue();

   if (!message)
   {
      PEG_METHOD_EXIT();
      return;
   }
   
   handleEnqueue(message);
   PEG_METHOD_EXIT();
}

void CIMOMHandle::_cimom_handle_rep::handleEnqueue(Message *message)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
                    "CIMOMHandle::_cimom_handle_rep::handleEnqueue(Message *)");
   if (!message)
   {
      PEG_METHOD_EXIT();
      return;
   }

   switch(message->getType())
   {
      case CIM_GET_CLASS_RESPONSE_MESSAGE:
      case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
      case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
      case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
      case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
      case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
      case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
      case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
      case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
      case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
      case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
      case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
      case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
      case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
      case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
      case CIM_REFERENCES_RESPONSE_MESSAGE:
      case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
      case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
      case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
	 try
	 {
	    _response.insert_last_wait(message);
	    _msg_avail.signal();
	 }
	 catch(...)
	 {
	    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
			  "CIMOMHandle::_cimom_handle_rep::handleEnqueue(Message *) - IPC Exception");
	        delete message;
	 }
	 break;
	 
      default:
      {
	 PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
			  "CIMOMHandle::_cimom_handle_rep::handleEnqueue(Message *) - unexpected message");
	 delete message;
      }
   }
   PEG_METHOD_EXIT();
}

// run as a detached thread 
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL 
CIMOMHandle::_cimom_handle_rep::_dispatch(void *parm)
{
//   Thread *th_dp = reinterpret_cast<Thread *>(parm);
   
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
                    "CIMOMHandle::_cimom_handle_rep::_dispatch(void *)");

   cimom_handle_dispatch *dp  = 
     reinterpret_cast<cimom_handle_dispatch *>(parm);
   if(dp )
   {
      try 
      {
	 MessageQueue * target = MessageQueue::lookup(dp->_out_qid);
	 MessageQueue *me = MessageQueue::lookup(dp->_my_qid);
	 if(me && target && dp->_msg)
	 {
	    CIMOMHandle::_cimom_handle_rep *myself = 
	       static_cast<CIMOMHandle::_cimom_handle_rep *>(me);
	    target->enqueue(dp->_msg);
	 }
      }
      catch(...)
      {
      }
      delete dp;
   }
   
   PEG_METHOD_EXIT();
//   exit_thread((PEGASUS_THREAD_RETURN)1);
   return 0;
}

Message *CIMOMHandle::_cimom_handle_rep::do_request(Message *request, 
						    Uint32 response_type,
						    const OperationContext & context) 
   throw()
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
                    "CIMOMHandle::_cimom_handle_rep::do_request(Message *, Uint32, Uint32)");

   try 
   {
      _recursion.try_lock(pegasus_thread_self());
   }
   catch(AlreadyLocked & )
   {
      PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
		       "AlreadyLocked Exception, throwing Deadlock");
      throw Deadlock(pegasus_thread_self());
   }
   catch(Deadlock & )
   {
      PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
		       "Deadlock Exception");
      throw;
   }
   catch(...)
   {
      PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			     "Unexpected Exception");
      throw;
   }
   cimom_handle_dispatch *dp = 
      new cimom_handle_dispatch(request, get_qid(), get_output_qid());

   MessageQueueService::get_thread_pool()->allocate_and_awaken(dp, _dispatch);

   _request = request;

   CIMResponseMessage *response = 0;
   
    Uint32 timeout = 0;
    try 
    {
       const TimeoutContainer *p = dynamic_cast<const TimeoutContainer *>
	  ( &(context.get(TimeoutContainer::NAME)));
       if(p)
	  timeout = p->getTimeOut();
    }
    catch(Exception & )
    {
    }
    try 
    {
       if(timeout)
	  _msg_avail.time_wait(timeout);
       else
	  _msg_avail.wait();
    }
    catch(TimeOut)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"timeout waiting for response");
       _request = 0;
       _recursion.unlock();
       throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Empty CIM Response");
    }
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Unexpected Exception");
       _request = 0;
       _recursion.unlock();
       throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Empty CIM Response");
    }
    _request = 0;
    Message *temp = _response.remove_first();
    response = dynamic_cast<CIMResponseMessage *>(temp);
    if(response == 0 || (response->getType() != response_type) )
    {
       delete temp;
       try 
       {
	  _response.empty_list();
       }
       catch(...)
       {
       }
       _recursion.unlock();      
       throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Empty CIM Response");
    }

    if(response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
       CIMException ex(response->cimException);
       
       delete response;
       _recursion.unlock();      
       throw ex;
    }
    
    PEG_METHOD_EXIT();
    _recursion.unlock();      
    return response;
}

CIMOMHandle::CIMOMHandle(void)
{
   _rep = new _cimom_handle_rep();

}

CIMOMHandle::CIMOMHandle(const CIMOMHandle & h)
{
   if(this != &h)
   {
      Inc(this->_rep = h._rep);
   }
   
}

CIMOMHandle::~CIMOMHandle(void)
{
   Dec(_rep);
}

CIMOMHandle & CIMOMHandle::operator=(const CIMOMHandle & handle)
{
   if(this != &handle)
   {
      Inc(_rep = handle._rep);
   }
   return *this;
}


CIMClass CIMOMHandle::getClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{

   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::getClass()");
   cimom_handle_op_semaphore opsem(_rep);

    // encode request
    CIMGetClassRequestMessage * request =
        new CIMGetClassRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));

    request->dest = _rep->get_output_qid();
    
    CIMGetClassResponseMessage * response;
    try 
    {
       response = 
	  static_cast<CIMGetClassResponseMessage *>(
	     _rep->do_request(request, 
			      CIM_GET_CLASS_RESPONSE_MESSAGE,
			      context));
    }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }
        
    CIMClass cimClass = response->cimClass;
    delete response;
    
    PEG_METHOD_EXIT();
    return cimClass;
}


Array<CIMClass> CIMOMHandle::enumerateClasses(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::enumerateClasses()");
   cimom_handle_op_semaphore opsem(_rep);

   CIMEnumerateClassesRequestMessage * request =
        new CIMEnumerateClassesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
	QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
   
   request->dest = _rep->get_output_qid();

   CIMEnumerateClassesResponseMessage *response;
   
   try 
   {
      response = 
	 static_cast<CIMEnumerateClassesResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }
   Array<CIMClass> cimClasses = response->cimClasses;
   delete response;
   PEG_METHOD_EXIT();
   return cimClasses;
}


Array<CIMName> CIMOMHandle::enumerateClassNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::enumerateClassNames()");
   cimom_handle_op_semaphore opsem(_rep);

    CIMEnumerateClassNamesRequestMessage * request =
        new CIMEnumerateClassNamesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        deepInheritance,
	QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
    
    request->dest = _rep->get_output_qid();

    CIMEnumerateClassNamesResponseMessage * response;
    try 
    {
       response = 
	  static_cast<CIMEnumerateClassNamesResponseMessage * >(
	     _rep->do_request(request, 
			      CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE,
			      context));
    }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }
    Array<CIMName> classNames = response->classNames;

    delete response;
    PEG_METHOD_EXIT();
    return(classNames);
}


void CIMOMHandle::createClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::createClass()");
   cimom_handle_op_semaphore opsem(_rep);
   
   
   CIMCreateClassRequestMessage * request =
      new CIMCreateClassRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 newClass,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));

   request->dest = _rep->get_output_qid();
   
   CIMCreateClassResponseMessage *response;
   
   try 
   {
      response = 
	 static_cast<CIMCreateClassResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_CREATE_CLASS_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }
   
   delete response;
   PEG_METHOD_EXIT();
   return;
}



void CIMOMHandle::modifyClass(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMClass& modifiedClass)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::modifyClass()");
   cimom_handle_op_semaphore opsem(_rep);
   
   CIMModifyClassRequestMessage * request =
      new CIMModifyClassRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 modifiedClass,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
   
   request->dest = _rep->get_output_qid();
   CIMModifyClassResponseMessage *response;
   try 
   {
      response = 
	 static_cast<CIMModifyClassResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_MODIFY_CLASS_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }
   
   delete response;
   PEG_METHOD_EXIT();
   return;
}



void CIMOMHandle::deleteClass(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className)
{
   
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::deleteClass()");
   cimom_handle_op_semaphore opsem(_rep);
   
   // encode request
   CIMDeleteClassRequestMessage * request =
      new CIMDeleteClassRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 className,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
   request->dest = _rep->get_output_qid();

   CIMDeleteClassResponseMessage * response;
   
   try 
   {
      response = 
	 static_cast<CIMDeleteClassResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_DELETE_CLASS_RESPONSE_MESSAGE,
			     context));
   }
   catch(CIMException &)
   {
      throw;
   }
   
   catch(...)
   {
      PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
		       "Exception caught in CIMOMHandle");
      throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
   }
   delete response;
   PEG_METHOD_EXIT();
   return;
}



CIMInstance CIMOMHandle::getInstance(
   const OperationContext & context,
   const CIMNamespaceName &nameSpace,
   const CIMObjectPath& instanceName,
   Boolean localOnly,
   Boolean includeQualifiers,
   Boolean includeClassOrigin,
   const CIMPropertyList& propertyList)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::getInstance()");
   cimom_handle_op_semaphore opsem(_rep);

   
   // encode request
   CIMGetInstanceRequestMessage * request =
      new CIMGetInstanceRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 instanceName,
	 localOnly,
	 includeQualifiers,
	 includeClassOrigin,
	 propertyList,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
   
   request->dest = _rep->get_output_qid();
   CIMGetInstanceResponseMessage * response;
   try 
   {
      response = 
	 static_cast<CIMGetInstanceResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_GET_INSTANCE_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

   CIMInstance cimInstance  = response->cimInstance;
    
   delete response;
   PEG_METHOD_EXIT();
   return(cimInstance);
}

Array<CIMInstance> CIMOMHandle::enumerateInstances(
   const OperationContext & context,
   const CIMNamespaceName &nameSpace,
   const CIMName& className,
   Boolean deepInheritance,
   Boolean localOnly,
   Boolean includeQualifiers,
   Boolean includeClassOrigin,
   const CIMPropertyList& propertyList)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::enumerateInstances()");
   cimom_handle_op_semaphore opsem(_rep);

   // encode request
   CIMEnumerateInstancesRequestMessage * request =
      new CIMEnumerateInstancesRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 className,
	 deepInheritance,
	 localOnly,
	 includeQualifiers,
	 includeClassOrigin,
	 propertyList,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));

   request->dest = _rep->get_output_qid();

   CIMEnumerateInstancesResponseMessage * response; 
   try 
   {
      response = 
	 static_cast<CIMEnumerateInstancesResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }
   
   Array<CIMInstance> cimInstances = response->cimNamedInstances;

   delete response;
   PEG_METHOD_EXIT();
   return(cimInstances);
}


Array<CIMObjectPath> CIMOMHandle::enumerateInstanceNames(
   const OperationContext & context,
   const CIMNamespaceName &nameSpace,
   const CIMName& className)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::enumerateInstanceNamess()");
   cimom_handle_op_semaphore opsem(_rep);

   // encode request
   CIMEnumerateInstanceNamesRequestMessage * request =
      new CIMEnumerateInstanceNamesRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 className,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));

   request->dest = _rep->get_output_qid();
   
   CIMEnumerateInstanceNamesResponseMessage * response;
   try 
   {
      response = 
	 static_cast<CIMEnumerateInstanceNamesResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

   Array<CIMObjectPath> cimReferences = response->instanceNames;

   delete response;
   PEG_METHOD_EXIT();
   return(cimReferences);
}

CIMObjectPath CIMOMHandle::createInstance(
   const OperationContext & context,
   const CIMNamespaceName &nameSpace,
   const CIMInstance& newInstance)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::createInstance()");
   cimom_handle_op_semaphore opsem(_rep);

   CIMCreateInstanceRequestMessage * request =
      new CIMCreateInstanceRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 newInstance,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
    
   request->dest = _rep->get_output_qid();

   CIMCreateInstanceResponseMessage *response;
   try 
   {
      response = 
	 static_cast<CIMCreateInstanceResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_CREATE_INSTANCE_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

   CIMObjectPath cimReference = response->instanceName;

   delete response;
   PEG_METHOD_EXIT();
   return(cimReference);
}



void CIMOMHandle::modifyInstance(
   const OperationContext & context,
   const CIMNamespaceName &nameSpace,
   const CIMInstance& modifiedInstance,
   Boolean includeQualifiers,
   const CIMPropertyList& propertyList)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::modifyInstance()");
   cimom_handle_op_semaphore opsem(_rep);
   
   CIMModifyInstanceRequestMessage * request =
      new CIMModifyInstanceRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 CIMInstance(),
	 includeQualifiers,
	 propertyList,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
    
   request->dest = _rep->get_output_qid();

   CIMModifyInstanceResponseMessage *response;
    
   try 
   {
      response = 
	 static_cast<CIMModifyInstanceResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE,
			     context));
   }
   catch(CIMException &)
   {
      throw;
   }
   
   catch(...)
   {
      PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
		       "Exception caught in CIMOMHandle");
      throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

   delete response;
   PEG_METHOD_EXIT();
   return;
}



void CIMOMHandle::deleteInstance(
   const OperationContext & context,
   const CIMNamespaceName &nameSpace,
   const CIMObjectPath& instanceName)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::deleteInstance()");
   cimom_handle_op_semaphore opsem(_rep);

   CIMDeleteInstanceRequestMessage * request =
      new CIMDeleteInstanceRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 instanceName,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
    
   request->dest = _rep->get_output_qid();
   CIMDeleteInstanceResponseMessage *response;
    
   try 
   {
      response = 
	 static_cast<CIMDeleteInstanceResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_DELETE_INSTANCE_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

   delete response;
   PEG_METHOD_EXIT();
   return;
}



Array<CIMObject> CIMOMHandle::execQuery(
   const OperationContext & context,
   const CIMNamespaceName &nameSpace,
   const String& queryLanguage,
   const String& query)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::exeQuery()");
   cimom_handle_op_semaphore opsem(_rep);
   
   CIMExecQueryRequestMessage * request =
      new CIMExecQueryRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 queryLanguage,
	 query,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
   
   request->dest = _rep->get_output_qid();
   
   CIMExecQueryResponseMessage * response;
   
   try 
   {
      response = 
	 static_cast<CIMExecQueryResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_EXEC_QUERY_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

   Array<CIMObject> cimObjects = response->cimObjects;

   delete response;
   PEG_METHOD_EXIT();
   return(cimObjects);
}


Array<CIMObject> CIMOMHandle::associators(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::associators()");
   cimom_handle_op_semaphore opsem(_rep);

    CIMAssociatorsRequestMessage * request =
        new CIMAssociatorsRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
	QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
    
    request->dest = _rep->get_output_qid();
    CIMAssociatorsResponseMessage *response;
    
   try 
   {
      response = 
	 static_cast<CIMAssociatorsResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_ASSOCIATORS_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

    Array<CIMObject> cimObjects = response->cimObjects;

    delete response;
    PEG_METHOD_EXIT();
    return(cimObjects);
}



Array<CIMObjectPath> CIMOMHandle::associatorNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::associatorNames()");
   cimom_handle_op_semaphore opsem(_rep);

   CIMAssociatorNamesRequestMessage * request =
      new CIMAssociatorNamesRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 objectName,
	 assocClass,
	 resultClass,
	 role,
	 resultRole,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
    
   request->dest = _rep->get_output_qid();

   CIMAssociatorNamesResponseMessage *response;
    
   try 
   {
      response = 
	 static_cast<CIMAssociatorNamesResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

   Array<CIMObjectPath> cimObjectPaths = response->objectNames;

   delete response;
   PEG_METHOD_EXIT();
   return(cimObjectPaths);
}



Array<CIMObject> CIMOMHandle::references(
   const OperationContext & context,
   const CIMNamespaceName &nameSpace,
   const CIMObjectPath& objectName,
   const CIMName& resultClass,
   const String& role,
   Boolean includeQualifiers,
   Boolean includeClassOrigin,
   const CIMPropertyList& propertyList)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::references()");
   cimom_handle_op_semaphore opsem(_rep);

   CIMReferencesRequestMessage * request =
      new CIMReferencesRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 objectName,
	 resultClass,
	 role,
	 includeQualifiers,
	 includeClassOrigin,
	 propertyList,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
    
   request->dest = _rep->get_output_qid();

   CIMReferencesResponseMessage *response;
    
   try 
   {
      response = 
	 static_cast<CIMReferencesResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_REFERENCES_RESPONSE_MESSAGE,
			     context));
   }
   catch(CIMException &)
   {
      throw;
   }
   
   catch(...)
   {
      PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
		       "Exception caught in CIMOMHandle");
      throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
   }

   Array<CIMObject> cimObjects = response->cimObjects;

   delete response;
   PEG_METHOD_EXIT();
   return(cimObjects);
}

Array<CIMObjectPath> CIMOMHandle::referenceNames(
   const OperationContext & context,
   const CIMNamespaceName &nameSpace,
   const CIMObjectPath& objectName,
   const CIMName& resultClass,
   const String& role)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::()referenceNames");
   cimom_handle_op_semaphore opsem(_rep);

   CIMReferenceNamesRequestMessage * request =
      new CIMReferenceNamesRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 objectName,
	 resultClass,
	 role,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
    
   request->dest = _rep->get_output_qid();


   CIMReferenceNamesResponseMessage * response;
    
   try 
   {
      response = 
	 static_cast<CIMReferenceNamesResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_REFERENCE_NAMES_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

   Array<CIMObjectPath> cimObjectPaths = response->objectNames;

   delete response;
   PEG_METHOD_EXIT();
   return(cimObjectPaths);
}


CIMValue CIMOMHandle::getProperty(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::getProperty()");
   cimom_handle_op_semaphore opsem(_rep);

    CIMGetPropertyRequestMessage * request =
        new CIMGetPropertyRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        instanceName,
        propertyName,
	QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
    
    request->dest = _rep->get_output_qid();

    CIMGetPropertyResponseMessage *response;
    
    try 
    {
       response = 
	  static_cast<CIMGetPropertyResponseMessage *>(
	     _rep->do_request(request, 
			      CIM_GET_PROPERTY_RESPONSE_MESSAGE,
			      context));
    }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

    CIMValue cimValue = response->value;

    delete response;
    PEG_METHOD_EXIT();
    return(cimValue);
}



void CIMOMHandle::setProperty(
   const OperationContext & context,
   const CIMNamespaceName &nameSpace,
   const CIMObjectPath& instanceName,
   const CIMName& propertyName,
   const CIMValue& newValue)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::setProperty()");
   cimom_handle_op_semaphore opsem(_rep);

   CIMSetPropertyRequestMessage * request =
      new CIMSetPropertyRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 instanceName,
	 propertyName,
	 newValue,
	 QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
    
    
   request->dest = _rep->get_output_qid();

   CIMSetPropertyResponseMessage *response;
	  
   try 
   {
      response = 
	 static_cast<CIMSetPropertyResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_SET_PROPERTY_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }

   delete response;
   PEG_METHOD_EXIT();
   return;
}



CIMValue CIMOMHandle::invokeMethod(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters)
{
   PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "CIMOMHandle::invokeMethod()");
   cimom_handle_op_semaphore opsem(_rep);
      
   Message* request = new CIMInvokeMethodRequestMessage(
      XmlWriter::getNextMessageId(),
      nameSpace,
      instanceName,
      methodName,
      inParameters,
      QueueIdStack(_rep->get_qid(), _rep->get_output_qid()));
   
   CIMInvokeMethodResponseMessage *response;
   
   try 
   {
      response = 
	 static_cast<CIMInvokeMethodResponseMessage *>(
	    _rep->do_request(request, 
			     CIM_INVOKE_METHOD_RESPONSE_MESSAGE,
			     context));
   }
    catch(CIMException &)
    {
       throw;
    }
    
    catch(...)
    {
       PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL4,
			"Exception caught in CIMOMHandle");
       throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
    }
   
   CIMValue value = response->retValue;
   
   delete response;
   PEG_METHOD_EXIT();
   return value;
}



void CIMOMHandle::get_idle_timer(struct timeval *tv)
{
   _rep->get_idle_timer(tv);
}

void CIMOMHandle::update_idle_timer(void)
{
   _rep->update_idle_timer();
}

Boolean CIMOMHandle::pending_operation(void)
{
   if(_rep->_pending_operation.value())
      return true;
   return false;
}

Boolean CIMOMHandle::unload_ok(void)
{
   return _rep->unload_ok();
}

void CIMOMHandle::protect(void)
{
   _rep->protect();
}

void CIMOMHandle::unprotect(void)
{
   _rep->unprotect();
}


PEGASUS_NAMESPACE_END
