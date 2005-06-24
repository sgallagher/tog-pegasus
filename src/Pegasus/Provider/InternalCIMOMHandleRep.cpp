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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//				Seema Gupta, (gseema@in.ibm.com for PEP135)
//              Amit K Arora, IBM (amita@in.ibm.com) for Bug#1090
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Tracer.h>

#ifdef PEGASUS_OS_OS400
#include <qycmutilu2.H>
#include "OS400ConvertChar.h"
#include "CIMOMHandleOS400UserState.h"
#include "CIMOMHandleOS400SystemState.h"
#endif

#include "InternalCIMOMHandleRep.h"

PEGASUS_NAMESPACE_BEGIN

// chuck 2.4
// l10n start
static void deleteContentLanguage(void* data)
{
   if (data != NULL)
   {
      ContentLanguages* cl = static_cast<ContentLanguages*>(data);
      delete cl;
   }
}
// l10n end


class CIMOMHandleDispatch
{
public:
    CIMOMHandleDispatch(
        Message* msg,
        Uint32 my_qid,
        Uint32 output_qid)
     : _msg(msg),
       _my_qid(my_qid),
       _out_qid(output_qid)
    {
    }

    ~CIMOMHandleDispatch()
    {
    }

    Message* _msg;
    Uint32 _my_qid;
    Uint32 _out_qid;
#ifdef PEGASUS_OS_OS400
    char os400PH[12];
#endif

private:
    CIMOMHandleDispatch();    // Unimplemented
};


InternalCIMOMHandleRep::InternalCIMOMHandleRep()
    : MessageQueue(PEGASUS_QUEUENAME_INTERNALCLIENT),
      _server_terminating(0),
      _msg_avail(0),
      _response(true, 0)
{
    // initialize the qids
    // output queue defaults to CIMOPRequestDispatcher
    MessageQueue* out = MessageQueue::lookup(PEGASUS_QUEUENAME_BINARY_HANDLER);
    if (out)
    {
        _output_qid = out->getQueueId();
    }
    else
    {
        _output_qid = _queueId;
    }

    // return queue defaults to myself
    _return_qid = _queueId;
}

InternalCIMOMHandleRep::InternalCIMOMHandleRep(Uint32 out_qid, Uint32 ret_qid)
    : MessageQueue(PEGASUS_QUEUENAME_INTERNALCLIENT),
      _output_qid(out_qid),
      _return_qid(ret_qid),
      _server_terminating(0),
      _msg_avail(0),
      _response(true, 0)
{
    if (0 == q_exists(_output_qid))
    {
        _output_qid = _queueId;
    }
    if (0 == q_exists(_return_qid))
    {
        _return_qid = _queueId;
    }
}

// Private, unimplemented copy constructor
InternalCIMOMHandleRep::InternalCIMOMHandleRep(
    const InternalCIMOMHandleRep& rep)
    : CIMOMHandleRep(),
      MessageQueue(PEGASUS_QUEUENAME_INTERNALCLIENT)
{
    PEGASUS_ASSERT(0);
}

// Private, unimplemented assignment operator
InternalCIMOMHandleRep& InternalCIMOMHandleRep::operator=(
    const InternalCIMOMHandleRep& rep)
{
    PEGASUS_ASSERT(0);
    return *this;
}

InternalCIMOMHandleRep::~InternalCIMOMHandleRep()
{
}

#ifdef PEGASUS_OS_OS400
InternalCIMOMHandleRep::InternalCIMOMHandleRep(Uint32 os400UserStateKey)
    : MessageQueue(PEGASUS_QUEUENAME_INTERNALCLIENT),
      _chOS400(os400UserStateKey)
{
}

void InternalCIMOMHandleRep::setOS400ProfileHandle(const char* profileHandle)
{
    memcpy(os400PH, profileHandle, 12);
}
#endif

Uint32 InternalCIMOMHandleRep::get_output_qid()
{
    try
    {
        AutoMutex autoMut(_qid_mutex);
        Uint32 qid = _output_qid;
        return qid;
    }
    catch (...)
    {
        return _queueId;
    }
}

void InternalCIMOMHandleRep::set_output_qid(Uint32 qid)
{
    try
    {
        AutoMutex autoMut(_qid_mutex);
        _output_qid = qid;
    }
    catch (...)
    {
    }
}

Uint32 InternalCIMOMHandleRep::get_return_qid()
{
    try
    {
        AutoMutex autoMut(_qid_mutex);
        Uint32 qid = _return_qid;
        return qid;
    }
    catch (...)
    {
        return _queueId;
    }
}

void InternalCIMOMHandleRep::set_return_qid(Uint32 qid)
{
    try
    {
        AutoMutex autoMut(_qid_mutex);
        _return_qid = qid;
    }
    catch (...)
    {
    }
}

Uint32 InternalCIMOMHandleRep::get_qid()
{
    return _queueId;
}

void InternalCIMOMHandleRep::handleEnqueue()
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::handleEnqueue()");

    Message* message = dequeue();

    if (!message)
    {
        PEG_METHOD_EXIT();
        return;
    }

    handleEnqueue(message);
    PEG_METHOD_EXIT();
}

void InternalCIMOMHandleRep::handleEnqueue(Message* message)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::handleEnqueue(Message*)");

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
    case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
        try
        {
            _response.insert_last_wait(message);
            _msg_avail.signal();
        }
        catch (...)
        {
            PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2, "IPC Exception");
            delete message;
        }
        break;

    default:
        {
            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "Error: unexpected message type");
            delete message;
        }
    }
    PEG_METHOD_EXIT();
}

// run as a detached thread
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
InternalCIMOMHandleRep::_dispatch(void* parm)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
                    "InternalCIMOMHandleRep::_dispatch");

    CIMOMHandleDispatch* dp =
        reinterpret_cast<CIMOMHandleDispatch*>(parm);
    if (dp)
    {
        try
        {
#ifdef PEGASUS_OS_OS400
            // On OS/400, this code runs in a system state thread.  Swap the
            // thread profile to be able to access server resources.
            if (CIMOMHandleOS400SystemState::setProfileHandle(dp->os400PH) !=
                CIM_ERR_SUCCESS)
            {
                throw Exception("");
            }
#endif

            MessageQueue* target = MessageQueue::lookup(dp->_out_qid);
            MessageQueue* me = MessageQueue::lookup(dp->_my_qid);
            if (me && target && dp->_msg)
            {
                target->enqueue(dp->_msg);
            }
        }
        catch (...)
        {
        }
        delete dp;
    }

    PEG_METHOD_EXIT();
//    exit_thread((PEGASUS_THREAD_RETURN)1);
    return 0;
}

Message* InternalCIMOMHandleRep::do_request(
    Message* request,
    Uint32 responseType,
    const OperationContext & context)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::do_request");

    try
    {
        _recursion.try_lock(pegasus_thread_self());
    }
    catch (IPCException &)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "IPC Exception, throwing CIMException");
        //l10n
        //throw CIMException(CIM_ERR_ACCESS_DENIED, "Recursive Use of CIMOMHandle Attempted");
        throw CIMException(CIM_ERR_ACCESS_DENIED, MessageLoaderParms(
            "Provider.CIMOMHandle.RECURSIVE_USE_CIMOMHANDLE",
            "Recursive Use of CIMOMHandle Attempted"));
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Unexpected Exception");
        throw;
    }

    // Set an AcceptLanguages and ContentLanguages into the request based
    // on the languages in the OperationContext from the caller,
    // or the caller's thread.
    CIMMessage* cimmsg = dynamic_cast<CIMMessage*>(request);
    if (cimmsg != NULL)
    {
       try
        {
		#ifdef PEGASUS_OS_OS400
			// On OS/400, do not allow the provider set the user name for the request.
			// Get the user name from the current thread.
		char os400UserName[11];
		if (ycmGetCurrentUser(os400UserName) == 0)
		{
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Common.CIMOMHandleOS400UserState.UNKNOWN_ERROR",
            "An internal error occurred during the processing of the CIMOM handle"));
		}
		EtoA(os400UserName);
		cimmsg->operationContext.insert(IdentityContainer(String(os400UserName)));
		#else
			IdentityContainer identity_cntr = (IdentityContainer)context.get(IdentityContainer::NAME);
            cimmsg->operationContext.insert(IdentityContainer(identity_cntr.getUserName()));
		#endif 			
        }
        catch (Exception &)
        {
			// for OS400 don't proceed if there is an exception, otherwise create an empty identity container.
			#ifdef PEGASUS_OS_OS400
				throw ;
			#else
			cimmsg->operationContext.insert(IdentityContainer(String::EMPTY));
			#endif
	    }
		// chuck 2.4
        // If the caller specified an Accept-Language or Content-Language
        // in the OperationContext, then use those on the request msg.
        try
        {
            AcceptLanguageListContainer al_cntr = (AcceptLanguageListContainer)
                context.get(AcceptLanguageListContainer::NAME);
			cimmsg->operationContext.set(AcceptLanguageListContainer(al_cntr.getLanguages())); 
        }
        catch (Exception &)
        {
            // If the container is not found then try to use the
            // AcceptLanguages from the current thread
            AcceptLanguages* pal = Thread::getLanguages();
            if (pal != NULL)
            {
				cimmsg->operationContext.set(AcceptLanguageListContainer(*pal)); 
            }
        }

        try
        {
            ContentLanguageListContainer cl_cntr =
                (ContentLanguageListContainer)context.get(
                    ContentLanguageListContainer::NAME);
			cimmsg->operationContext.set(ContentLanguageListContainer(cl_cntr.getLanguages())); 
        }
        catch (Exception &)
        {
			cimmsg->operationContext.set(ContentLanguageListContainer(ContentLanguages::EMPTY)); 
            // ignore the container not found error
        }

        // end chuck 2.4
    }
    else
    {
        // ignore if not a CIMMessage
    }

    // Make sure to delete the dispatch  if it does not get processed by _dispatch
    CIMOMHandleDispatch* dp =
        new CIMOMHandleDispatch(request, get_qid(), get_output_qid());

#ifdef PEGASUS_OS_OS400
    memcpy(dp->os400PH, os400PH, 12);
#endif
    ThreadStatus rtn = MessageQueueService::get_thread_pool()->allocate_and_awaken(dp, _dispatch);
    if (rtn != PEGASUS_THREAD_OK) 
    {
	 Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		"Not enough threads to create a worker to dispatch a request. ");
 
	 Tracer::trace(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
			"Could not allocate thread for %s to dispatch a request.");
	delete dp;
    	PEG_METHOD_EXIT();
	throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CANNOT_ALLOCATE_THREAD",
	     "Could not create a worker for the dispatch request."));
    }
    _request = request;

    CIMResponseMessage* response = 0;

    Uint32 timeout = 0;
    try
    {
        const TimeoutContainer* p = dynamic_cast<const TimeoutContainer*>(
            &(context.get(TimeoutContainer::NAME)));
        if (p)
        {
            timeout = p->getTimeOut();
        }
    }
    catch (Exception &)
    {
    }

    try
    {
        if (timeout)
        {
            _msg_avail.time_wait(timeout);
        }
        else
        {
            _msg_avail.wait();
        }
    }
    catch (TimeOut&)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                        "timeout waiting for response");
        _request = 0;
        _recursion.unlock();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Empty CIM Response");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.EMPTY_CIM_RESPONSE",
            "Empty CIM Response"));
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Unexpected Exception");
        _request = 0;
        _recursion.unlock();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Empty CIM Response");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.EMPTY_CIM_RESPONSE",
            "Empty CIM Response"));
    }
    _request = 0;
    Message* temp = _response.remove_first();
    response = dynamic_cast<CIMResponseMessage*>(temp);
    if (response == 0 || (response->getType() != responseType))
    {
        delete temp;
        try
        {
            _response.empty_list();
        }
        catch (...)
        {
        }
        _recursion.unlock();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Empty CIM Response");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.EMPTY_CIM_RESPONSE",
            "Empty CIM Response"));
    }

    if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMException ex(response->cimException);

        delete response;
        _recursion.unlock();
        throw ex;
    }

    // chuck 2.4
    // If the response has a Content-Language then save it into thread-specific
    // storage
	ContentLanguageListContainer  cnt_lang_cntr = response->operationContext.get(ContentLanguageListContainer::NAME);
	if((cnt_lang_cntr.getLanguages()).size()>0)
    {
         Thread* curThrd = Thread::getCurrent();
         if (curThrd != NULL)
         {
             // deletes the old tsd and creates a new one
             curThrd->put_tsd("cimomHandleContentLanguages",
                 deleteContentLanguage,
                 sizeof(ContentLanguages*),
                 new ContentLanguages(cnt_lang_cntr.getLanguages()));
         }
    }

    PEG_METHOD_EXIT();
    _recursion.unlock();
    return response;
}


//
// CIM Operations
//

CIMClass InternalCIMOMHandleRep::getClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::getClass");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.getClass(
            context,
            nameSpace,
            className,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    // encode request
    CIMGetClassRequestMessage* request = new CIMGetClassRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMGetClassResponseMessage* response;
    try
    {
        response = static_cast<CIMGetClassResponseMessage*>(
            do_request(
                request,
                CIM_GET_CLASS_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    CIMClass cimClass = response->cimClass;
    delete response;

    PEG_METHOD_EXIT();
    return cimClass;
}


Array<CIMClass> InternalCIMOMHandleRep::enumerateClasses(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::enumerateClasses");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.enumerateClasses(
            context,
            nameSpace,
            className,
            deepInheritance,
            localOnly,
            includeQualifiers,
            includeClassOrigin);
   }
#endif

   CIMOMHandleOpSemaphore opsem(this);

    CIMEnumerateClassesRequestMessage* request =
        new CIMEnumerateClassesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            deepInheritance,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMEnumerateClassesResponseMessage* response;

    try
    {
        response = static_cast<CIMEnumerateClassesResponseMessage*>(
            do_request(
                request,
                CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }
    Array<CIMClass> cimClasses = response->cimClasses;
    delete response;
    PEG_METHOD_EXIT();
    return cimClasses;
}


Array<CIMName> InternalCIMOMHandleRep::enumerateClassNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::enumerateClassNames");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.enumerateClassNames(
            context,
            nameSpace,
            className,
            deepInheritance);
   }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMEnumerateClassNamesRequestMessage* request =
        new CIMEnumerateClassNamesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        deepInheritance,
        QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMEnumerateClassNamesResponseMessage* response;
    try
    {
        response = static_cast<CIMEnumerateClassNamesResponseMessage*>(
            do_request(
                request,
                CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }
    Array<CIMName> classNames = response->classNames;

    delete response;
    PEG_METHOD_EXIT();
    return(classNames);
}


void InternalCIMOMHandleRep::createClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::createClass");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        _chOS400.createClass(
            context,
            nameSpace,
            newClass);
        return;
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);


    CIMCreateClassRequestMessage* request =
        new CIMCreateClassRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            newClass,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMCreateClassResponseMessage* response;

    try
    {
        response = static_cast<CIMCreateClassResponseMessage*>(
            do_request(
                request,
                CIM_CREATE_CLASS_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    delete response;
    PEG_METHOD_EXIT();
    return;
}


void InternalCIMOMHandleRep::modifyClass(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMClass& modifiedClass)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::modifyClass");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        _chOS400.modifyClass(
            context,
            nameSpace,
            modifiedClass);
        return;
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMModifyClassRequestMessage* request =
        new CIMModifyClassRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            modifiedClass,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMModifyClassResponseMessage* response;
    try
    {
        response = static_cast<CIMModifyClassResponseMessage*>(
            do_request(
                request,
                CIM_MODIFY_CLASS_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    delete response;
    PEG_METHOD_EXIT();
    return;
}


void InternalCIMOMHandleRep::deleteClass(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className)
{

    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::deleteClass");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        _chOS400.deleteClass(
            context,
            nameSpace,
            className);
        return;
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    // encode request
    CIMDeleteClassRequestMessage* request =
        new CIMDeleteClassRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            QueueIdStack(get_qid(), get_output_qid()));
    request->dest = get_output_qid();

    CIMDeleteClassResponseMessage* response;

    try
    {
        response = static_cast<CIMDeleteClassResponseMessage*>(
            do_request(
                request,
                CIM_DELETE_CLASS_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }
    delete response;
    PEG_METHOD_EXIT();
    return;
}


CIMInstance InternalCIMOMHandleRep::getInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::getInstance");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.getInstance(
            context,
            nameSpace,
            instanceName,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);


    // encode request
    CIMGetInstanceRequestMessage* request =
        new CIMGetInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMGetInstanceResponseMessage* response;
    try
    {
        response = static_cast<CIMGetInstanceResponseMessage*>(
            do_request(
                request,
                CIM_GET_INSTANCE_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    CIMInstance cimInstance = response->cimInstance;

    delete response;
    PEG_METHOD_EXIT();
    return(cimInstance);
}

Array<CIMInstance> InternalCIMOMHandleRep::enumerateInstances(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::enumerateInstances");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.enumerateInstances(
            context,
            nameSpace,
            className,
            deepInheritance,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    // encode request
    CIMEnumerateInstancesRequestMessage* request =
        new CIMEnumerateInstancesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            deepInheritance,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMEnumerateInstancesResponseMessage* response;
    try
    {
        response = static_cast<CIMEnumerateInstancesResponseMessage*>(
            do_request(
                request,
                CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    Array<CIMInstance> cimInstances = response->cimNamedInstances;

    delete response;
    PEG_METHOD_EXIT();
    return(cimInstances);
}


Array<CIMObjectPath> InternalCIMOMHandleRep::enumerateInstanceNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::enumerateInstanceNames");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.enumerateInstanceNames(
            context,
            nameSpace,
            className);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    // encode request
    CIMEnumerateInstanceNamesRequestMessage* request =
        new CIMEnumerateInstanceNamesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMEnumerateInstanceNamesResponseMessage* response;
    try
    {
        response = static_cast<CIMEnumerateInstanceNamesResponseMessage*>(
            do_request(
                request,
                CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    Array<CIMObjectPath> cimReferences = response->instanceNames;

    delete response;
    PEG_METHOD_EXIT();
    return(cimReferences);
}

CIMObjectPath InternalCIMOMHandleRep::createInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMInstance& newInstance)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::createInstance");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.createInstance(
            context,
            nameSpace,
            newInstance);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMCreateInstanceRequestMessage* request =
        new CIMCreateInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            newInstance,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMCreateInstanceResponseMessage* response;
    try
    {
        response = static_cast<CIMCreateInstanceResponseMessage*>(
            do_request(
                request,
                CIM_CREATE_INSTANCE_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    CIMObjectPath cimReference = response->instanceName;

    delete response;
    PEG_METHOD_EXIT();
    return(cimReference);
}


void InternalCIMOMHandleRep::modifyInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::modifyInstance");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        _chOS400.modifyInstance(
            context,
            nameSpace,
            modifiedInstance,
            includeQualifiers,
            propertyList);
        return;
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMModifyInstanceRequestMessage* request =
        new CIMModifyInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            modifiedInstance,
            includeQualifiers,
            propertyList,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMModifyInstanceResponseMessage* response;

    try
    {
        response = static_cast<CIMModifyInstanceResponseMessage*>(
            do_request(
                request,
                CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    delete response;
    PEG_METHOD_EXIT();
    return;
}


void InternalCIMOMHandleRep::deleteInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::deleteInstance");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        _chOS400.deleteInstance(
            context,
            nameSpace,
            instanceName);
        return;
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMDeleteInstanceRequestMessage* request =
        new CIMDeleteInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMDeleteInstanceResponseMessage* response;

    try
    {
        response = static_cast<CIMDeleteInstanceResponseMessage*>(
            do_request(
                request,
                CIM_DELETE_INSTANCE_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    delete response;
    PEG_METHOD_EXIT();
    return;
}


Array<CIMObject> InternalCIMOMHandleRep::execQuery(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const String& queryLanguage,
    const String& query)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::execQuery");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.execQuery(
            context,
            nameSpace,
            queryLanguage,
            query);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMExecQueryRequestMessage* request =
        new CIMExecQueryRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            queryLanguage,
            query,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMExecQueryResponseMessage* response;

    try
    {
        response = static_cast<CIMExecQueryResponseMessage*>(
            do_request(
                request,
                CIM_EXEC_QUERY_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    Array<CIMObject> cimObjects = response->cimObjects;

    delete response;
    PEG_METHOD_EXIT();
    return(cimObjects);
}


Array<CIMObject> InternalCIMOMHandleRep::associators(
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
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::associators");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.associators(
            context,
            nameSpace,
            objectName,
            assocClass,
            resultClass,
            role,
            resultRole,
            includeQualifiers,
            includeClassOrigin,
            propertyList);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMAssociatorsRequestMessage* request =
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
        QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMAssociatorsResponseMessage* response;

    try
    {
        response = static_cast<CIMAssociatorsResponseMessage*>(
            do_request(
                request,
                CIM_ASSOCIATORS_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    Array<CIMObject> cimObjects = response->cimObjects;

    delete response;
    PEG_METHOD_EXIT();
    return(cimObjects);
}


Array<CIMObjectPath> InternalCIMOMHandleRep::associatorNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::associatorNames");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.associatorNames(
            context,
            nameSpace,
            objectName,
            assocClass,
            resultClass,
            role,
            resultRole);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMAssociatorNamesRequestMessage* request =
        new CIMAssociatorNamesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            objectName,
            assocClass,
            resultClass,
            role,
            resultRole,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMAssociatorNamesResponseMessage* response;

    try
    {
        response = static_cast<CIMAssociatorNamesResponseMessage*>(
            do_request(
                request,
                CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    Array<CIMObjectPath> cimObjectPaths = response->objectNames;

    delete response;
    PEG_METHOD_EXIT();
    return(cimObjectPaths);
}


Array<CIMObject> InternalCIMOMHandleRep::references(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::references");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.references(
            context,
            nameSpace,
            objectName,
            resultClass,
            role,
            includeQualifiers,
            includeClassOrigin,
            propertyList);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMReferencesRequestMessage* request =
        new CIMReferencesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            objectName,
            resultClass,
            role,
            includeQualifiers,
            includeClassOrigin,
            propertyList,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMReferencesResponseMessage* response;

    try
    {
        response = static_cast<CIMReferencesResponseMessage*>(
            do_request(
                request,
                CIM_REFERENCES_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    Array<CIMObject> cimObjects = response->cimObjects;

    delete response;
    PEG_METHOD_EXIT();
    return(cimObjects);
}


Array<CIMObjectPath> InternalCIMOMHandleRep::referenceNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "InternalCIMOMHandleRep::referenceNames");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.referenceNames(
            context,
            nameSpace,
            objectName,
            resultClass,
            role);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMReferenceNamesRequestMessage* request =
        new CIMReferenceNamesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            objectName,
            resultClass,
            role,
            QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMReferenceNamesResponseMessage* response;

    try
    {
        response = static_cast<CIMReferenceNamesResponseMessage*>(
            do_request(
                request,
                CIM_REFERENCE_NAMES_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    Array<CIMObjectPath> cimObjectPaths = response->objectNames;

    delete response;
    PEG_METHOD_EXIT();
    return(cimObjectPaths);
}


CIMValue InternalCIMOMHandleRep::getProperty(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::getProperty");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.getProperty(
            context,
            nameSpace,
            instanceName,
            propertyName);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMGetPropertyRequestMessage* request =
        new CIMGetPropertyRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        instanceName,
        propertyName,
        QueueIdStack(get_qid(), get_output_qid()));

    request->dest = get_output_qid();

    CIMGetPropertyResponseMessage* response;

    try
    {
        response = static_cast<CIMGetPropertyResponseMessage*>(
            do_request(
                request,
                CIM_GET_PROPERTY_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    CIMValue cimValue = response->value;

    delete response;
    PEG_METHOD_EXIT();
    return(cimValue);
}


void InternalCIMOMHandleRep::setProperty(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::setProperty");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        _chOS400.setProperty(
            context,
            nameSpace,
            instanceName,
            propertyName,
            newValue);
        return;
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMSetPropertyRequestMessage* request =
        new CIMSetPropertyRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            propertyName,
            newValue,
            QueueIdStack(get_qid(), get_output_qid()));


    request->dest = get_output_qid();

    CIMSetPropertyResponseMessage* response;

    try
    {
        response = static_cast<CIMSetPropertyResponseMessage*>(
            do_request(
                request,
                CIM_SET_PROPERTY_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    delete response;
    PEG_METHOD_EXIT();
    return;
}


CIMValue InternalCIMOMHandleRep::invokeMethod(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::invokeMethod");

#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        return _chOS400.invokeMethod(
            context,
            nameSpace,
            instanceName,
            methodName,
            inParameters,
            outParameters);
    }
#endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMInvokeMethodRequestMessage* request =
        new CIMInvokeMethodRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            methodName,
            inParameters,
            QueueIdStack(get_qid(), get_output_qid()));

    CIMInvokeMethodResponseMessage* response;

    try
    {
        response = static_cast<CIMInvokeMethodResponseMessage*>(
            do_request(
                request,
                CIM_INVOKE_METHOD_RESPONSE_MESSAGE,
                context));
    }
    catch (CIMException &)
    {
        throw;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");
        //l10n
        //throw CIMException(CIM_ERR_FAILED, "Exception caught in CIMOMHandle");
        throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
            "Exception caught in CIMOMHandle"));
    }

    CIMValue value = response->retValue;
    outParameters = response->outParameters;

    delete response;
    PEG_METHOD_EXIT();
    return value;
}


//
// Public CIMOMHandle Methods
//

void InternalCIMOMHandleRep::disallowProviderUnload()
{
#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        _chOS400.disallowProviderUnload();
        return;
    }
#endif

    CIMOMHandleRep::disallowProviderUnload();
}

void InternalCIMOMHandleRep::allowProviderUnload()
{
#ifdef PEGASUS_OS_OS400
    // If this is running in user-state, then run the request
    // through the user-state layer
    if (_chOS400.hasKey())
    {
        _chOS400.allowProviderUnload();
        return;
    }
#endif

    CIMOMHandleRep::allowProviderUnload();
}

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
OperationContext InternalCIMOMHandleRep::getResponseContext()
{
    OperationContext ctx;

    Thread* curThrd = Thread::getCurrent();
    if (curThrd == NULL)
    {
        ctx.insert(ContentLanguageListContainer(ContentLanguages::EMPTY));
    }
    else
    {
        ContentLanguages* contentLangs = (ContentLanguages*)
            curThrd->reference_tsd("cimomHandleContentLanguages");
        curThrd->dereference_tsd();
 
        if (contentLangs == NULL)
        {
            ctx.insert(ContentLanguageListContainer(ContentLanguages::EMPTY));
        }
        else
        {
            ctx.insert(ContentLanguageListContainer(*contentLangs));
            // delete the old tsd to free the memory
            curThrd->delete_tsd("cimomHandleContentLanguages");
        }
    }

    return ctx;
}
#endif

PEGASUS_NAMESPACE_END
