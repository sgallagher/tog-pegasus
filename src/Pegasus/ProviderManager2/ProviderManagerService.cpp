//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//              Karl Schopmeyer(k.schopmeyer@opengroup.org) - Fix associators.
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Adrian Schuur, IBM (schuur@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManagerService.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/ProviderManager/OperationResponseHandler.h>

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/ProviderManager2/ProviderManagerModule.h>
#include <Pegasus/ProviderManager2/ProviderManager.h>

PEGASUS_NAMESPACE_BEGIN

// BEGIN TEMP SECTION
class ProviderManagerContainer
{
public:
    ProviderManagerContainer(void) : _manager(0)
    {
    }

    ProviderManagerContainer(const ProviderManagerContainer & container) : _manager(0)
    {
        *this = container;
    }

    ProviderManagerContainer(const String & physicalName, const String & logicalName, const String & interfaceName) : _manager(0)
    {
        #if defined(PEGASUS_OS_TYPE_WINDOWS)
        _physicalName = physicalName + String(".dll");
        #elif defined(PEGASUS_OS_HPUX) && defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
        _physicalName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        _physicalName.append(String("/lib") + physicalName + String(".sl"));
        #elif defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
        _physicalName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        _physicalName.append(String("/lib") + physicalName + String(".so"));
        #elif defined(PEGASUS_OS_OS400)
        _physicalName = physicalName;
        #else
        _physicalName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        _physicalName.append(String("/lib") + physicalName + String(".so"));
        #endif

        _logicalName = logicalName;

        _interfaceName = interfaceName;

        _module = ProviderManagerModule(_physicalName);

        _module.load();

        _manager = _module.getProviderManager(_logicalName);

        PEGASUS_ASSERT(_manager != 0);
    }

    ~ProviderManagerContainer(void)
    {
        _module.unload();
    }

    ProviderManagerContainer & operator=(const ProviderManagerContainer & container)
    {
        if(this == &container)
        {
            return(*this);
        }

        _logicalName = container._logicalName;
        _physicalName = container._physicalName;
        _interfaceName = container._interfaceName;

        _module = container._module;
        _manager = container._manager;

        return(*this);
    }

    ProviderManager *getProviderManager(void)
    {
        return _manager;
    }

    const String & getPhysicalName(void) const
    {
        return(_physicalName);
    }

    const String & getLogicalName(void) const
    {
        return(_logicalName);
    }

    const String & getInterfaceName(void) const
    {
        return(_interfaceName);
    }

private:
    String _physicalName;
    String _logicalName;
    String _interfaceName;

    ProviderManagerModule _module;
    ProviderManager * _manager;

};

static Array<ProviderManagerContainer*> _providerManagers;
// END TEMP SECTION

inline Boolean _isSupportedRequestType(const Message * message)
{
    // ATTN: needs implementation

    // for now, assume all requests are valid

    return(true);
}

inline Boolean _isSupportedResponseType(const Message * message)
{
    // ATTN: needs implementation

    // for now, assume all responses are invalid

    return(false);
}

ProviderManagerService* ProviderManagerService::providerManagerService=NULL;

ProviderManagerService::ProviderManagerService(void)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)
{
    providerManagerService=this;
}

ProviderManagerService::ProviderManagerService(ProviderRegistrationManager * providerRegistrationManager)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)
{
    providerManagerService=this;
    SetProviderRegistrationManager(providerRegistrationManager);

    // ATTN: this section is a temporary solution to populate the list of enabled
    // provider managers for a given distribution. it includes another temporary
    // solution for converting a generic file name into a file name useable by
    // each platform.

    // BEGIN TEMP SECTION
    //#if defined(PEGASUS_OS_OS400)
    //_providerManagers.append(ProviderManagerContainer("QSYS/??????????", "INTERNAL", "INTERNAL"));
    //#else
    //_providerManager.append(ProviderManagerContainer("InternalProviderManager", "DEFAULT", "INTERNAL"));
    //#endif

    #if defined(ENABLE_DEFAULT_PROVIDER_MANAGER)
    #if defined(PEGASUS_OS_OS400)
    _providerManagers.append(
       new ProviderManagerContainer("QSYS/QYCMDFTPVM", "DEFAULT", "C++Default"));
    #else
    _providerManagers.append(
       new ProviderManagerContainer("DefaultProviderManager", "DEFAULT", "C++Default"));
    #endif
    #endif

    #if defined(ENABLE_CMPI_PROVIDER_MANAGER)
    #if defined(PEGASUS_OS_OS400)
    _providerManagers.append(
       new ProviderManagerContainer("QSYS/QYCMCMPIPM", "CMPI", "CMPI"));
    #else
    _providerManagers.append(
       new ProviderManagerContainer("CMPIProviderManager", "CMPI", "CMPI"));
    #endif
    #endif
    // END TEMP SECTION
}

ProviderManagerService::~ProviderManagerService(void)
{
    providerManagerService=NULL;
}

Boolean ProviderManagerService::messageOK(const Message * message)
{
    PEGASUS_ASSERT(message != 0);

    if(_isSupportedRequestType(message))
    {
        return(MessageQueueService::messageOK(message));
    }

    return(false);
}

void ProviderManagerService::handleEnqueue(void)
{
    Message * message = dequeue();

    handleEnqueue(message);
}

void ProviderManagerService::handleEnqueue(Message * message)
{
    PEGASUS_ASSERT(message != 0);

    AsyncLegacyOperationStart * asyncRequest;

    if(message->_async != NULL)
    {
        asyncRequest = static_cast<AsyncLegacyOperationStart *>(message->_async);
    }
    else
    {
        asyncRequest = new AsyncLegacyOperationStart(
            get_next_xid(),
            0,
            this->getQueueId(),
            message,
            this->getQueueId());
    }

    _handle_async_request(asyncRequest);
}

void ProviderManagerService::_handle_async_request(AsyncRequest * request)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManagerService::_handle_async_request");

    PEGASUS_ASSERT((request != 0) && (request->op != 0));

    if(request->getType() == async_messages::ASYNC_LEGACY_OP_START)
    {
        request->op->processing();

        _incomingQueue.enqueue(request->op);

        _thread_pool->allocate_and_awaken((void *)this, ProviderManagerService::handleCimOperation);
    }
    else
    {
        // pass all other operations to the default handler
        MessageQueueService::_handle_async_request(request);
    }

    PEG_METHOD_EXIT();

    return;
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleCimOperation(void * arg) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleCimOperation");

    if(arg == 0)
    {
        // thread started with invalid argument.
        return(PEGASUS_THREAD_RETURN(1));
    }

    // get the service from argument
    ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

    if(service->_incomingQueue.size() == 0)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "ProviderManagerService::handleCimOperation() called with no op node in queue" );

        PEG_METHOD_EXIT();

        // thread started with no message in queue.
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncOpNode * op = service->_incomingQueue.dequeue();

    if((op == 0) || (op->_request.count() == 0))
    {
        MessageQueue * queue = MessageQueue::lookup(op->_source_queue);

        PEGASUS_ASSERT(queue != 0);

        PEG_METHOD_EXIT();

        // no request in op node
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncRequest * request = static_cast<AsyncRequest *>(op->_request.next(0));

    if((request == 0) || (request->getType() != async_messages::ASYNC_LEGACY_OP_START))
    {
        // reply with NAK

        PEG_METHOD_EXIT();

        return(PEGASUS_THREAD_RETURN(0));
    }

    try
    {
        Message * legacy = static_cast<AsyncLegacyOperationStart *>(request)->get_action();

        if(_isSupportedRequestType(legacy))
        {
            Destroyer<Message> xmessage(legacy);

            // Set the client's requested language into this service thread.
            // This will allow functions in this service to return messages
            // in the correct language.
            CIMMessage * msg = dynamic_cast<CIMMessage *>(legacy);

            if(msg != 0)
            {
                AcceptLanguages * langs = new AcceptLanguages(msg->acceptLanguages);

                Thread::setLanguages(langs);
            }
            else
            {
                Thread::clearLanguages();
            }

            service->handleCimRequest(op, legacy);
        }
    }
    catch(...)
    {
        // ATTN: log error
    }

    PEG_METHOD_EXIT();

    return(PEGASUS_THREAD_RETURN(0));
}

void ProviderManagerService::handleCimRequest(AsyncOpNode * op, const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleCimRequest");

    // ATTN: ensure message is a request???
    CIMMessage * request = dynamic_cast<CIMMessage *>(const_cast<Message *>(message));

    // get request from op node
    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT((request != 0) && (async != 0));

    Message * response = 0;
    String ifc;

    // get the responsible provider Manager
    ProviderManager * pm = locateProviderManager(message,ifc);
    if(pm) {
        response = pm->processMessage(request);
    }
    else
    {
       if (request->getType()==CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE) {
          for (Uint32 i = 0, n = _providerManagers.size(); i < n; i++) {
              ProviderManagerContainer *pmc=_providerManagers[i];
// ---- next instruction is disabled because of a bug
//             response=pmc->getProviderManager()->processMessage(request);
// ---- this block is an workaround
	    CIMStopAllProvidersResponseMessage * resp =
              new CIMStopAllProvidersResponseMessage(
                 request->messageId,
                 CIMException(),
                 ((CIMStopAllProvidersRequestMessage*)request)->queueIds.copyAndPop());
             resp->setKey(request->getKey());
             resp->setHttpMethod (request->getHttpMethod ());
	     response=resp;
	     break;
// ---- end of  workaround
           }
       }
       else {
          CIMRequestMessage * req =
              dynamic_cast<CIMRequestMessage *>(const_cast<Message *>(message));
          CIMResponseMessage  *resp=new CIMResponseMessage(0,req->messageId,CIMException(),
             req->queueIds.copyAndPop());
          response=resp;
          resp->synch_response(req);
          OperationResponseHandler handler(req, resp);
          handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
       }
    }

    // preserve message key
    response->setKey(request->getKey());

    // set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    AsyncLegacyOperationResult * async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);

    PEG_METHOD_EXIT();
}

ProviderManager* ProviderManagerService::locateProviderManager(const Message *message,
             String & it)
{
    String nameSpace;
    String className;

    const CIMOperationRequestMessage * p =
       dynamic_cast<const CIMOperationRequestMessage *>(message);

    if (p) {
       nameSpace=p->nameSpace;
       if (p->providerType==ProviderType::ASSOCIATION)
          className=((CIMAssociatorsRequestMessage*)p)->assocClass;
       else className=p->className;

       ProviderName name(
           CIMObjectPath(String::EMPTY, nameSpace, className).toString(),
           String::EMPTY,
           String::EMPTY,
           String::EMPTY,
           p->providerType);
       // find provider manager
       name = ProviderRegistrar().findProvider(name);
       it=name.getInterfaceName();
    }

    else {
       const CIMIndicationRequestMessage * p =
          dynamic_cast<const CIMIndicationRequestMessage *>(message);
       if (p) {
          CIMIndicationRequestMessage *m=(CIMIndicationRequestMessage*)message;
          it=m->providerModule.getProperty (m->providerModule.findProperty
                ("InterfaceType")).getValue ().toString ();
       }
       else {
          it=String::EMPTY;
	  return NULL;
       }
    }

    // find provider manager for provider interface
    for(Uint32 i = 0, n = _providerManagers.size(); i < n; i++)
    {
        if (String::equalNoCase(it,_providerManagers[i]->getInterfaceName())) {
           ProviderManagerContainer *pmc=_providerManagers[i];
	   return pmc->getProviderManager();
        }
    }
    ProviderManagerContainer *pmc=_providerManagers[0];
    return pmc->getProviderManager();
}

void ProviderManagerService::unload_idle_providers(void)
{
}

PEGASUS_NAMESPACE_END

