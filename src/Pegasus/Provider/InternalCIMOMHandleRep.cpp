//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Modified By:
//      Carol Ann Krug Graves, Hewlett-Packard Company (carolann_graves@hp.com)
//      Mike Day, IBM (mdday@us.ibm.com)
//      Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//		Seema Gupta, (gseema@in.ibm.com for PEP135)
//      Amit K Arora, IBM (amita@in.ibm.com) for Bug#1090
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/AutoPtr.h>

#ifdef PEGASUS_OS_OS400
#include <qycmutilu2.H>
#include "OS400ConvertChar.h"
#include "CIMOMHandleOS400UserState.h"
#include "CIMOMHandleOS400SystemState.h"
#endif

#include "InternalCIMOMHandleRep.h"

PEGASUS_NAMESPACE_BEGIN

InternalCIMOMHandleMessageQueue::InternalCIMOMHandleMessageQueue(void)
    : MessageQueue(PEGASUS_QUEUENAME_INTERNALCLIENT),
    _output_qid(0),
    _return_qid(0),
    _response(0)
{
    // output queue is the binary message handler
    MessageQueue* out = MessageQueue::lookup(PEGASUS_QUEUENAME_BINARY_HANDLER);

    PEGASUS_ASSERT(out != 0);

    _output_qid = out->getQueueId();

    // input queue is this
    _return_qid = getQueueId();
}

InternalCIMOMHandleMessageQueue::~InternalCIMOMHandleMessageQueue(void)
{
    try
    {
        // ATTN: release any unprocessed responses
        _response.lock(pegasus_thread_self());

        for(Uint32 i = 0, n = _response.count(); i < n; i++)
        {
            Message* message = _response.dequeue();

            delete message;
        }

        _response.unlock();
    }
    catch(...)
    {
    }
}

void InternalCIMOMHandleMessageQueue::handleEnqueue(void)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleMessageQueue::handleEnqueue");

    Message* message = dequeue();

    switch(message->getType())
    {
    /*
    case CIM_GET_CLASS_REQUEST_MESSAGE:
    case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
    case CIM_CREATE_CLASS_REQUEST_MESSAGE:
    case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
    case CIM_DELETE_CLASS_REQUEST_MESSAGE:
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
    case CIM_REFERENCES_REQUEST_MESSAGE:
    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
        sendRequest(message);

        break;
    */
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
        _response.enqueue(message);

        break;
    default:
        PEG_TRACE_STRING(
            TRC_DISCARDED_DATA,
            Tracer::LEVEL2,
            "Error: unexpected message type");

        delete message;

        break;
    }

    PEG_METHOD_EXIT();
}

CIMResponseMessage* InternalCIMOMHandleMessageQueue::sendRequest(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::sendRequest");

    AutoMutex autoMutex(_mutex);

    // update message to include routing information
    request->dest = _output_qid;
    request->queueIds.push(_return_qid);
    request->queueIds.push(_output_qid);

    // locate destination
    MessageQueueService* service =
        dynamic_cast<MessageQueueService *>(
            MessageQueue::lookup(_output_qid));

    PEGASUS_ASSERT(service != 0);

    // forward request
    if(service->SendForget(request) == false)
    {
        PEG_METHOD_EXIT();
        throw Exception("Failed to send message");
    }

    // wait for response
    CIMResponseMessage* response =
        dynamic_cast<CIMResponseMessage *>(_response.dequeue_wait());

    PEG_METHOD_EXIT();
    return(response);
}

static void _deleteContentLanguage(void* data)
{
   if(data != 0)
   {
       ContentLanguageList* cl = static_cast<ContentLanguageList*>(data);

       delete cl;
   }
}

static OperationContext _filterOperationContext(const OperationContext& context)
{
    OperationContext temp;

    #ifdef PEGASUS_OS_OS400
    // on OS/400, do not allow the provider set the user name for the request
    // get the user name from the current thread.
    char os400UserName[11];

    if(ycmGetCurrentUser(os400UserName) == 0)
    {
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Common.CIMOMHandleOS400UserState.UNKNOWN_ERROR",
                "An internal error occurred during the processing of the CIMOM handle"));
    }

    EtoA(os400UserName);
    temp.insert(IdentityContainer(String(os400UserName)));
    #else
    try
    {
        // propagate the identity container if it exists (get() with throw
        // an exception if it does not)
        temp.insert(context.get(IdentityContainer::NAME));
    }
    catch(Exception &)
    {
        temp.insert(IdentityContainer(String::EMPTY));
    }
    #endif

    try
    {
        // propagate the accept languages container if it exists (get() with throw
        // an exception if it does not exist)
        temp.insert(context.get(AcceptLanguageListContainer::NAME));
    }
    catch(Exception &)
    {
        // If the container is not found then try to use the
        // AcceptLanguageList from the current thread
        AcceptLanguageList* pal = Thread::getLanguages();

        if(pal != 0)
        {
            temp.insert(AcceptLanguageListContainer(*pal));
        }
        else
        {
            temp.insert(AcceptLanguageListContainer(AcceptLanguageList()));
        }
    }

    try
    {
        // propagate the accept languages container if it exists (get() with throw
        // an exception if it does not)
        temp.insert(context.get(ContentLanguageListContainer::NAME));
    }
    catch(Exception &)
    {
        temp.insert(ContentLanguageListContainer(ContentLanguageList()));
    }

    return(temp);
}

InternalCIMOMHandleRep::InternalCIMOMHandleRep()
{
}

InternalCIMOMHandleRep::~InternalCIMOMHandleRep()
{
}

#ifdef PEGASUS_OS_OS400
InternalCIMOMHandleRep::InternalCIMOMHandleRep(Uint32 os400UserStateKey)
    : _chOS400(os400UserStateKey)
{
}

void InternalCIMOMHandleRep::setOS400ProfileHandle(const char* profileHandle)
{
    memcpy(os400PH, profileHandle, 12);
}
#endif

CIMResponseMessage* InternalCIMOMHandleRep::do_request(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "InternalCIMOMHandleRep::do_request");

    #ifdef PEGASUS_OS_OS400
    // On OS/400, this code runs in a system state thread.  Swap the
    // thread profile to be able to access server resources.
    if(CIMOMHandleOS400SystemState::setProfileHandle(os400PH) != CIM_ERR_SUCCESS)
    {
        PEG_METHOD_EXIT();
        throw Exception("Could not set profile handle");
    }
    #endif

    /*
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
    catch(Exception &)
    {
    }

    try
    {
        if(timeout)
        {
            _msg_avail.time_wait(timeout);
        }
        else
        {
            _msg_avail.wait();
        }
    }
    catch(TimeOut&)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                        "timeout waiting for response");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.EMPTY_CIM_RESPONSE",
            "Empty CIM Response"));
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
            "Unexpected Exception");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
            "Provider.CIMOMHandle.EMPTY_CIM_RESPONSE",
            "Empty CIM Response"));
    }
    */

    Message* temp = _queue.sendRequest(request);

    CIMResponseMessage* response = dynamic_cast<CIMResponseMessage*>(temp);

    if(response == 0)
    {
        delete response;

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.EMPTY_CIM_RESPONSE",
                "Empty CIM Response"));
    }

    if(response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMException e(response->cimException);

        delete response;

        PEG_METHOD_EXIT();
        throw e;
    }

    try
    {
        // If the response has a Content-Language then save it into thread-specific storage
        ContentLanguageListContainer container =
            response->operationContext.get(ContentLanguageListContainer::NAME);

        if(container.getLanguages().size() > 0)
        {
            Thread* currentThread = Thread::getCurrent();

            if(currentThread != 0)
            {
                // deletes the old tsd and creates a new one
                currentThread->put_tsd(
                    "cimomHandleContentLanguages",
                    _deleteContentLanguage,
                    sizeof(ContentLanguageList*),
                    new ContentLanguageList(container.getLanguages()));
            }
        }
    }
    catch(Exception &)
    {
    }

    PEG_METHOD_EXIT();
    return(response);
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
        CIMClass cimClass =
            _chOS400.getClass(
                context,
                nameSpace,
                className,
                localOnly,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

        PEG_METHOD_EXIT();
        return(cimClass);
    }
    #endif

    CIMOMHandleOpSemaphore opsem(this);

    // encode request
    CIMGetClassRequestMessage* request =
        new CIMGetClassRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);
//  request->operationContext.get(AcceptLanguageListContainer::NAME);

    AutoPtr<CIMGetClassResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMGetClassResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    CIMClass cimClass = response->cimClass;

    PEG_METHOD_EXIT();
    return(cimClass);
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
        Array<CIMClass> cimClasses =
            _chOS400.enumerateClasses(
                context,
                nameSpace,
                className,
                deepInheritance,
                localOnly,
                includeQualifiers,
                includeClassOrigin);

        PEG_METHOD_EXIT();
        return(cimClasses);
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMEnumerateClassesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMEnumerateClassesResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMClass> cimClasses = response->cimClasses;

    PEG_METHOD_EXIT();
    return(cimClasses);
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
        Array<CIMName> cimClassNames =
            _chOS400.enumerateClassNames(
                context,
                nameSpace,
                className,
                deepInheritance);

        PEG_METHOD_EXIT();
        return(cimClassNames);
    }
    #endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMEnumerateClassNamesRequestMessage* request =
        new CIMEnumerateClassNamesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            deepInheritance,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMEnumerateClassNamesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMEnumerateClassNamesResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMName> cimClassNames = response->classNames;

    PEG_METHOD_EXIT();
    return(cimClassNames);
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

        PEG_METHOD_EXIT();
        return;
    }
    #endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMCreateClassRequestMessage* request =
        new CIMCreateClassRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            newClass,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMCreateClassResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMCreateClassResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

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

        PEG_METHOD_EXIT();
        return;
    }
    #endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMModifyClassRequestMessage* request =
        new CIMModifyClassRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            modifiedClass,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMModifyClassResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMModifyClassResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

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

        PEG_METHOD_EXIT();
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMDeleteClassResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMDeleteClassResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

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
        CIMInstance cimInstance =
            _chOS400.getInstance(
                context,
                nameSpace,
                instanceName,
                localOnly,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

        PEG_METHOD_EXIT();
        return(cimInstance);
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMGetInstanceResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMGetInstanceResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    CIMInstance cimInstance = response->cimInstance;

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
        Array<CIMInstance> cimInstances =
            _chOS400.enumerateInstances(
                context,
                nameSpace,
                className,
                deepInheritance,
                localOnly,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

        PEG_METHOD_EXIT();
        return(cimInstances);
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMEnumerateInstancesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMInstance> cimInstances = response->cimNamedInstances;

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
        Array<CIMObjectPath> cimObjectPaths =
            _chOS400.enumerateInstanceNames(
                context,
                nameSpace,
                className);

        PEG_METHOD_EXIT();
        return(cimObjectPaths);
    }
    #endif

    CIMOMHandleOpSemaphore opsem(this);

    // encode request
    CIMEnumerateInstanceNamesRequestMessage* request =
        new CIMEnumerateInstanceNamesRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            className,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObjectPath> cimObjectPaths = response->instanceNames;

    PEG_METHOD_EXIT();
    return(cimObjectPaths);
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
        CIMObjectPath cimReference =
            _chOS400.createInstance(
                context,
                nameSpace,
                newInstance);

        PEG_METHOD_EXIT();
        return(cimReference);
    }
    #endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMCreateInstanceRequestMessage* request =
        new CIMCreateInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            newInstance,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMCreateInstanceResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMCreateInstanceResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    CIMObjectPath cimReference = response->instanceName;

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

        PEG_METHOD_EXIT();
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMModifyInstanceResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMModifyInstanceResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

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

        PEG_METHOD_EXIT();
        return;
    }
    #endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMDeleteInstanceRequestMessage* request =
        new CIMDeleteInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMDeleteInstanceResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMDeleteInstanceResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

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
        Array<CIMObject> cimObjects =
            _chOS400.execQuery(
                context,
                nameSpace,
                queryLanguage,
                query);

        PEG_METHOD_EXIT();
        return(cimObjects);
    }
    #endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMExecQueryRequestMessage* request =
        new CIMExecQueryRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            queryLanguage,
            query,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMExecQueryResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMExecQueryResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObject> cimObjects = response->cimObjects;

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
        Array<CIMObject> cimObjects =
            _chOS400.associators(
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

        PEG_METHOD_EXIT();
        return(cimObjects);
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMAssociatorsResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMAssociatorsResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObject> cimObjects = response->cimObjects;

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
        Array<CIMObjectPath> cimObjectPaths =
            _chOS400.associatorNames(
                context,
                nameSpace,
                objectName,
                assocClass,
                resultClass,
                role,
                resultRole);

        PEG_METHOD_EXIT();
        return(cimObjectPaths);
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMAssociatorNamesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMAssociatorNamesResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObjectPath> cimObjectPaths = response->objectNames;

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
        Array<CIMObject> cimObjects =
            _chOS400.references(
                context,
                nameSpace,
                objectName,
                resultClass,
                role,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

        PEG_METHOD_EXIT();
        return(cimObjects);
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMReferencesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMReferencesResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObject> cimObjects = response->cimObjects;

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
        Array<CIMObjectPath> cimObjectPaths =
            _chOS400.referenceNames(
                context,
                nameSpace,
                objectName,
                resultClass,
                role);

        PEG_METHOD_EXIT();
        return(cimObjectPaths);
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMReferenceNamesResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMReferenceNamesResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    Array<CIMObjectPath> cimObjectPaths = response->objectNames;

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
        CIMValue cimValue =
            _chOS400.getProperty(
                context,
                nameSpace,
                instanceName,
                propertyName);

        PEG_METHOD_EXIT();
        return(cimValue);
    }
    #endif

    CIMOMHandleOpSemaphore opsem(this);

    CIMGetPropertyRequestMessage* request =
        new CIMGetPropertyRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            propertyName,
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMGetPropertyResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMGetPropertyResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    CIMValue cimValue = response->value;

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

        PEG_METHOD_EXIT();
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMSetPropertyResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMSetPropertyResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

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
        CIMValue cimValue =
            _chOS400.invokeMethod(
                context,
                nameSpace,
                instanceName,
                methodName,
                inParameters,
                outParameters);

        PEG_METHOD_EXIT();
        return(cimValue);
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
            QueueIdStack());

    // copy and adjust, as needed, the operation context
    request->operationContext = _filterOperationContext(context);

    AutoPtr<CIMInvokeMethodResponseMessage> response;

    try
    {
        response.reset(dynamic_cast<CIMInvokeMethodResponseMessage*>(
            do_request(request)));

        if(response.get() == 0)
        {
            PEG_TRACE_STRING(
                TRC_CIMOM_HANDLE,
                Tracer::LEVEL2,
                "Incorrect response type in CIMOMHandle");

            throw CIMException(CIM_ERR_FAILED);
        }
    }
    catch(CIMException &)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...)
    {
        PEG_TRACE_STRING(
            TRC_CIMOM_HANDLE,
            Tracer::LEVEL2,
            "Exception caught in CIMOMHandle");

        PEG_METHOD_EXIT();
        throw CIMException(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "Provider.CIMOMHandle.CAUGHT_EXCEPTION",
                "Exception caught in CIMOMHandle"));
    }

    CIMValue cimValue = response->retValue;
    outParameters = response->outParameters;

    PEG_METHOD_EXIT();
    return(cimValue);
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
        ctx.insert(ContentLanguageListContainer(ContentLanguageList()));
    }
    else
    {
        ContentLanguageList* contentLangs = (ContentLanguageList*)
            curThrd->reference_tsd("cimomHandleContentLanguages");
        curThrd->dereference_tsd();

        if (contentLangs == NULL)
        {
            ctx.insert(ContentLanguageListContainer(ContentLanguageList()));
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
