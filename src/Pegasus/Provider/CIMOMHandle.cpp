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

#include "CIMOMHandle.h"



PEGASUS_NAMESPACE_BEGIN



struct CIMOMHandle::_cimom_handle_rep 
{

      class callback_data
      {
	 public:
	    Message *reply;
	    Semaphore client_sem;
	    CIMOMHandle & cimom_handle;
	    
	    callback_data(CIMOMHandle *handle)
	       : reply(0), client_sem(0), cimom_handle(*handle)
	    {
	    }
	    
	    ~callback_data()
	    {
	       delete reply;
	    }

	    Message *get_reply(void)
	    {
	       Message *ret = reply;
	       reply = NULL;
	       return(ret);
	    }

	    callback_data(void);
      };


      static void async_callback(Uint32 user_data, Message *reply, void *parm)
      {
	 callback_data *cb_data = reinterpret_cast<callback_data *>(parm);
	 cb_data->reply = reply;
	 cb_data->client_sem.signal();
      }


      Message * _controller_async(Message *rq)
      {
	 if(rq == 0)
	    throw UninitializedObjectException();
	    

	 // prevent recursive entry into the cimom handle !
	 try 
	 {
	    _recursion.try_lock(pegasus_thread_self());
	 }
	 catch(...)
	 {
	    PEGASUS_STD(cout) << " recursive use of CIMOMHandle " << PEGASUS_STD(endl);
	    return 0;
	 }

	 callback_data *cb_data = new callback_data(_container);
	 
	 // create request envelope
	 AsyncLegacyOperationStart * asyncRequest =
	    new AsyncLegacyOperationStart (
	       _provider_manager->get_next_xid(),
	       NULL,
	       _dispatcher_qid,
	       rq,
	       _dispatcher_qid);

	 if(false  == _controller->ClientSendAsync(*_client_handle,
						   0,
						   _dispatcher_qid,
						   asyncRequest,
						   async_callback,
						   (void *)cb_data))
	 {
	    delete asyncRequest;
	    delete cb_data;
	    // unlock the recursion mutex
	    _recursion.unlock();
	    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, String::EMPTY);
	 }
	 cb_data->client_sem.wait();
	 AsyncLegacyOperationResult * asyncReply = 
	    static_cast<AsyncLegacyOperationResult *>(cb_data->get_reply()) ;
	 AsyncOpNode *op = asyncRequest->op;
	 Message * response = asyncReply->get_result();
	 delete asyncRequest;
	 delete asyncReply;
	 delete cb_data;
	    
	 // unlock the recursion mutex
	 _recursion.unlock();
	 return(response);
      }
      

      _cimom_handle_rep(void)
	 : _id(peg_credential_types::PROVIDER),
	   _provider_manager(0), 
	   _dispatcher(0),
	   _controller(0),
	   _client_handle(0), 
	   _dispatcher_qid(0),
	   _provider_manager_qid(0),
	   _initialized(false)
      {

      }
      
      _cimom_handle_rep(MessageQueueService *provider_manager)
	 : _id(peg_credential_types::PROVIDER),
	   _provider_manager(provider_manager), 
	   _dispatcher(0),
	   _controller(0),
	   _client_handle(0), 
	   _dispatcher_qid(0),
	   _provider_manager_qid(0),
	   _initialized(false)
      {
      }
      
      ~_cimom_handle_rep(void)
      {
	 if(_initialized == true && _controller != 0 && _client_handle != 0)
	 {
	    _controller->return_client_handle(_client_handle); 
	 }
      }

      _cimom_handle_rep(const _cimom_handle_rep & rep)
	 : _id(peg_credential_types::PROVIDER),
	   _provider_manager(rep._provider_manager), 
	   _dispatcher(rep._dispatcher),
	   _controller(rep._controller),
	   _client_handle(0), 
	   _dispatcher_qid(rep._dispatcher_qid),
	   _provider_manager_qid(rep._provider_manager_qid),
	   _initialized(false)
      {
      }


      _cimom_handle_rep & _cimom_handle_rep::operator=(const _cimom_handle_rep & rep)
      {
	 if(this != &rep)
	 {
	    if(_initialized == true && _controller != 0 && _client_handle != 0 )
	    {
	       _controller->return_client_handle(_client_handle);
	    }
	    
	    _id = rep._id;
	    _provider_manager = rep._provider_manager;
	    _dispatcher = rep._dispatcher;
	    _controller = rep._controller;
	    _client_handle = 0;
	    _dispatcher_qid = rep._dispatcher_qid;
	    _provider_manager_qid = rep._provider_manager_qid;
	    _initialized = false;
	 }
	 return *this;
      }
      
      void init(CIMOMHandle *container)
      {
	 if(_initialized == true && _controller != 0 && _client_handle != 0 )
	 {
	    return;
	 }

	 _container = container;
	 
	 if(_provider_manager == 0)
	 {
	    MessageQueue * queue =
	       MessageQueue::lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);
	    _provider_manager = dynamic_cast<MessageQueueService *>(queue);
	 }
	 MessageQueue * queue =
	    MessageQueue::lookup(PEGASUS_QUEUENAME_OPREQDISPATCHER);
	 _dispatcher = dynamic_cast<MessageQueueService *>(queue);
	 _controller = & ModuleController::get_client_handle(_id, &_client_handle);
	 
	 if(_controller && _dispatcher && _provider_manager && _client_handle)
	 {
	    _initialized = true;
	    _provider_manager_qid = _provider_manager->getQueueId();
	    _dispatcher_qid = _dispatcher->getQueueId();
	 }
	 else
	 {
	    throw UninitializedObjectException();
	 }
      }


      pegasus_internal_identity _id;
      MessageQueueService *_provider_manager;
      MessageQueueService *_dispatcher;

      ModuleController *_controller;
      ModuleController::client_handle  * _client_handle;
      CIMOMHandle *_container;
      Uint32 _dispatcher_qid;
      Uint32 _provider_manager_qid;
      Boolean _initialized;
      Mutex _recursion;

};

CIMOMHandle::CIMOMHandle(MessageQueueService *provider_manager)
{
   _rep = new _cimom_handle_rep(provider_manager);
}

CIMOMHandle::CIMOMHandle(void)
{
   _rep = new _cimom_handle_rep();
}


CIMOMHandle::~CIMOMHandle(void)
{
   delete _rep;
}

CIMOMHandle & CIMOMHandle::operator=(const CIMOMHandle & handle)
{
   if(this != &handle)
   {
      *_rep = *handle._rep;
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

   cout << " initializing cimom handle " << endl;
   
   _rep->init(this);
   cout << " encoding request  " << endl;
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
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMGetClassResponseMessage * response =
       static_cast<CIMGetClassResponseMessage *>(_rep->_controller_async(request));
    CIMClass cimClass;
   cout << " request complete  " << endl;

    if(response != 0)
    {
       cout << " response received " << endl;
       
       cimClass = response->cimClass;
    }

    delete response;

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
   _rep->init(this);

   CIMEnumerateClassesRequestMessage * request =
        new CIMEnumerateClassesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMEnumerateClassesResponseMessage *reply =
       static_cast<CIMEnumerateClassesResponseMessage *>(_rep->_controller_async(request));

    Array<CIMClass> cimClasses;
    if(reply != 0)
    {
       cimClasses = reply->cimClasses;
    }
    delete reply;
    return cimClasses;
}

Array<CIMName> CIMOMHandle::enumerateClassNames(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance)
{
   
   _rep->init(this);

    CIMEnumerateClassNamesRequestMessage * request =
        new CIMEnumerateClassNamesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        deepInheritance,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));


    CIMEnumerateClassNamesResponseMessage * reply =
       static_cast<CIMEnumerateClassNamesResponseMessage *>(_rep->_controller_async(request));

    Array<CIMName> classNames;
    if(reply != 0 )
    {
       classNames = reply->classNames;
    }

    delete reply;

    return(classNames);
}

void CIMOMHandle::createClass(
   const OperationContext & context,
   const CIMNamespaceName& nameSpace,
   const CIMClass& newClass)
{
      _rep->init(this);


    CIMCreateClassRequestMessage * request =
        new CIMCreateClassRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        newClass,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMCreateClassResponseMessage *reply =
       static_cast<CIMCreateClassResponseMessage *>(_rep->_controller_async(request));

    if(reply == 0)
    {
       throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    delete reply;
    return;
}

void CIMOMHandle::modifyClass(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass)
{
   _rep->init(this);

    CIMModifyClassRequestMessage * request =
        new CIMModifyClassRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        modifiedClass,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));


    CIMModifyClassResponseMessage *reply =
       static_cast<CIMModifyClassResponseMessage *>(_rep->_controller_async(request));

    if(reply == 0)
    {
       throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }
    delete reply;
    return;
}


void CIMOMHandle::deleteClass(
   const OperationContext & context,
   const CIMNamespaceName& nameSpace,
   const CIMName& className)
{

   _rep->init(this);


    // encode request
    CIMDeleteClassRequestMessage * request =
        new CIMDeleteClassRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMDeleteClassResponseMessage * reply =
       static_cast<CIMDeleteClassResponseMessage * >(_rep->_controller_async(request));

    if(reply == 0)
    {
       throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }
    delete reply;

    return;
}

CIMInstance CIMOMHandle::getInstance(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList)
{

      _rep->init(this);


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
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));


    CIMGetInstanceResponseMessage * reply  =
       static_cast<CIMGetInstanceResponseMessage *>(_rep->_controller_async(request));

    CIMInstance cimInstance ;

    if(reply != 0)
    {
       cimInstance = reply->cimInstance;
    }

    delete reply;

    return(cimInstance);
}

Array<CIMInstance> CIMOMHandle::enumerateInstances(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList)
{
      _rep->init(this);

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
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMEnumerateInstancesResponseMessage * reply =
       static_cast<CIMEnumerateInstancesResponseMessage *>(_rep->_controller_async(request));


    Array<CIMInstance> cimInstances;
    if(reply != 0)
    {
       cimInstances = reply->cimNamedInstances;
    }
    delete reply;

    return(cimInstances);
}

Array<CIMObjectPath> CIMOMHandle::enumerateInstanceNames(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMName& className)
{

      _rep->init(this);

    // encode request
    CIMEnumerateInstanceNamesRequestMessage * request =
        new CIMEnumerateInstanceNamesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMEnumerateInstanceNamesResponseMessage * reply =
       static_cast<CIMEnumerateInstanceNamesResponseMessage *>(_rep->_controller_async(request));

    Array<CIMObjectPath> cimReferences;

    if(reply != 0)
    {
       cimReferences = reply->instanceNames;
    }
    delete reply;

    return(cimReferences);
}


CIMObjectPath CIMOMHandle::createInstance(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMInstance& newInstance)
{

      _rep->init(this);

    CIMCreateInstanceRequestMessage * request =
        new CIMCreateInstanceRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        newInstance,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMCreateInstanceResponseMessage *reply =
       static_cast<CIMCreateInstanceResponseMessage *>(_rep->_controller_async(request));


    CIMObjectPath cimReference;

    if(reply != 0)
    {
       cimReference = reply->instanceName;
    }
    delete reply;

    return(cimReference);
}

void CIMOMHandle::modifyInstance(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMInstance& modifiedInstance,
        Boolean includeQualifiers,
        const CIMPropertyList& propertyList)
{

      _rep->init(this);

    CIMModifyInstanceRequestMessage * request =
        new CIMModifyInstanceRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        CIMInstance(),
        includeQualifiers,
        propertyList,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMModifyInstanceResponseMessage *reply =
       static_cast<CIMModifyInstanceResponseMessage *>(_rep->_controller_async(request));

    if(reply == 0)
    {
       throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }
    delete reply;

    return;
}

void CIMOMHandle::deleteInstance(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName)
{
      _rep->init(this);

    CIMDeleteInstanceRequestMessage * request =
        new CIMDeleteInstanceRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        instanceName,
	QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMDeleteInstanceResponseMessage *reply =
       static_cast<CIMDeleteInstanceResponseMessage *>(_rep->_controller_async(request));

    if(reply == 0)
    {
       throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }
    delete reply;

    return;
}

Array<CIMObject> CIMOMHandle::execQuery(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const String& queryLanguage,
        const String& query)
{

   _rep->init(this);
   
   CIMExecQueryRequestMessage * request =
      new CIMExecQueryRequestMessage(
	 XmlWriter::getNextMessageId(),
	 nameSpace,
	 queryLanguage,
	 query,
	 QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));
   
   CIMExecQueryResponseMessage * reply =
      static_cast<CIMExecQueryResponseMessage *>(_rep->_controller_async(request));


    Array<CIMObject> cimObjects;
    if(reply != 0)
    {
       cimObjects = reply->cimObjects;
    }

    delete reply;

    return(cimObjects);
}

Array<CIMObject> CIMOMHandle::associators(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const CIMName& resultClass,
        const String& role,
        const String& resultRole,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList)
{
      _rep->init(this);

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
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMAssociatorsResponseMessage *reply =
       static_cast<CIMAssociatorsResponseMessage *>(_rep->_controller_async(request));

    Array<CIMObject> cimObjects;

    if(reply != 0)
    {
       cimObjects = reply->cimObjects;
    }
    delete reply;

    return(cimObjects);
}

Array<CIMObjectPath> CIMOMHandle::associatorNames(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const CIMName& resultClass,
        const String& role,
        const String& resultRole)
{
      _rep->init(this);

    CIMAssociatorNamesRequestMessage * request =
        new CIMAssociatorNamesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMAssociatorNamesResponseMessage *reply =
       static_cast<CIMAssociatorNamesResponseMessage *>(_rep->_controller_async(request));

    Array<CIMObjectPath> cimObjectPaths;

    if(reply != 0)
    {
       cimObjectPaths = reply->objectNames;
    }
    delete reply;

    return(cimObjectPaths);
}

Array<CIMObject> CIMOMHandle::references(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass,
        const String& role,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList)
{
      _rep->init(this);

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
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMReferencesResponseMessage *reply =
       static_cast<CIMReferencesResponseMessage *>(_rep->_controller_async(request));

    Array<CIMObject> cimObjects;

    if(reply != 0)
    {
       cimObjects = reply->cimObjects;
    }
    delete reply;

    return(cimObjects);
}

Array<CIMObjectPath> CIMOMHandle::referenceNames(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass,
        const String& role)
{
      _rep->init(this);

    CIMReferenceNamesRequestMessage * request =
        new CIMReferenceNamesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        objectName,
        resultClass,
        role,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMReferenceNamesResponseMessage * reply =
       static_cast<CIMReferenceNamesResponseMessage *>(_rep->_controller_async(request));

    Array<CIMObjectPath> cimObjectPaths;
    if(reply != 0)
    {
       cimObjectPaths = reply->objectNames;
    }
    delete reply;

    return(cimObjectPaths);
}

CIMValue CIMOMHandle::getProperty(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName)
{
      _rep->init(this);

    CIMGetPropertyRequestMessage * request =
        new CIMGetPropertyRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        instanceName,
        propertyName,
        QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMGetPropertyResponseMessage *reply =
       static_cast<CIMGetPropertyResponseMessage *>(_rep->_controller_async(request));

    CIMValue cimValue;
    if(reply != 0)
    {
       cimValue = reply->value;
    }

    delete reply;

    return(cimValue);
}

void CIMOMHandle::setProperty(
        const OperationContext & context,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName,
        const CIMValue& newValue)
{
      _rep->init(this);

    CIMSetPropertyRequestMessage * request =
       new CIMSetPropertyRequestMessage(
	  XmlWriter::getNextMessageId(),
	  nameSpace,
	  instanceName,
	  propertyName,
	  newValue,
	  QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));

    CIMSetPropertyResponseMessage *reply  =
       static_cast<CIMSetPropertyResponseMessage *>(_rep->_controller_async(request));
    if(reply == 0)
    {
       throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }
    delete reply;

    return;
}

CIMValue CIMOMHandle::invokeMethod(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters)
{
      _rep->init(this);

    Message* request = new CIMInvokeMethodRequestMessage(
    XmlWriter::getNextMessageId(),
    nameSpace,
    instanceName,
    methodName,
    inParameters,
    QueueIdStack(_rep->_dispatcher_qid, _rep->_provider_manager_qid));
    
    CIMInvokeMethodResponseMessage *reply  =
       static_cast<CIMInvokeMethodResponseMessage *>(_rep->_controller_async(request));

    CIMValue cimValue;
    if(reply != 0)
    {
       cimValue = reply->retValue;
    }
    else
    {
       throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }
    delete reply;

    return cimValue;
}


PEGASUS_NAMESPACE_END
