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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Tracer.h>

#include "ClientCIMOMHandleRep.h"

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


/**
    This class is used to prevent concurrent access to a non-reentrant
    CIMClient object.
 */
class ClientCIMOMHandleAccessController
{
public:
    ClientCIMOMHandleAccessController(Mutex& lock)
        : _lock(lock)
    {
        try
        {
            _lock.try_lock(pegasus_thread_self());
        }
        catch (AlreadyLocked &)
        {
            PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                "AlreadyLocked Exception, throwing CIMException");
            throw CIMException(CIM_ERR_ACCESS_DENIED, MessageLoaderParms(
                "Provider.CIMOMHandle.RECURSIVE_USE_CIMOMHANDLE",
                "Recursive Use of CIMOMHandle Attempted"));
        }
        catch (Exception& e)
        {
            PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                String("Unexpected Exception: ") + e.getMessage());
            throw;
        }
        catch (...)
        {
            PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                "Unexpected exception");
            throw;
        }
    }

    ~ClientCIMOMHandleAccessController()
    {
        // Must not throw an exception from a destructor
        try
        {
            _lock.unlock();
        }
        catch (Exception& e)
        {
            PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                String("Ignoring Exception: ") + e.getMessage());
        }
        catch (...)
        {
            PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                "Ignoring unknown exception");
        }
    }

private:
    // Unimplemented constructors and assignment operator
    ClientCIMOMHandleAccessController();
    ClientCIMOMHandleAccessController(const ClientCIMOMHandleAccessController&);
    ClientCIMOMHandleAccessController& operator=(
        const ClientCIMOMHandleAccessController&);

    Mutex& _lock;
};


/**
    The ClientCIMOMHandleSetup class encapsulates the logic to set up the
    CIMClient object state based on a specified OperationContext.  The
    original CIMClient state is restored by the destructor.  Only one
    ClientCIMOMHandleSetup object may operate on a given CIMClient object
    at a time.  Use of the ClientCIMOMHandleAccessController class is
    recommend to control access to CIMClient objects.
 */
class ClientCIMOMHandleSetup
{
public:
    ClientCIMOMHandleSetup(
        CIMClient*& client,
        const OperationContext& context)
    {
        //
        // Initialize the CIMClient object if necessary
        //
        if (client == 0)
        {
            PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                "Creating CIMClient connection");
            client = new CIMClient();
            client->connectLocal();
        }
        _client = client;

        //
        // If the caller specified a timeout value in the OperationContext,
        // set it in the CIMClient object.
        //
        _origTimeout = client->getTimeout();
        try
        {
            TimeoutContainer t_cntr = (TimeoutContainer)
                context.get(TimeoutContainer::NAME);
            client->setTimeout(t_cntr.getTimeOut());
        }
        catch (...)
        {
            // No TimeoutContainer in OperationContext; just use the default
        }

        //
        // If the caller specified an Accept-Language in the
        // OperationContext, set it in the CIMClient object.
        //
        _origAcceptLanguages = client->getRequestAcceptLanguages();
        try
        {
            AcceptLanguageListContainer al_cntr = (AcceptLanguageListContainer)
                context.get(AcceptLanguageListContainer::NAME);
            _client->setRequestAcceptLanguages(al_cntr.getLanguages());
        }
        catch (Exception &)
        {
            // No AcceptLanguageListContainer in OperationContext; try
            // getting the AcceptLanguages from the current thread
            AcceptLanguages* al = Thread::getLanguages();
            if (al != NULL)
            {
                _client->setRequestAcceptLanguages(*al);
            }
        }

        //
        // If the caller specified a Content-Language in the
        // OperationContext, set it in the CIMClient object.
        //
        _origContentLanguages = client->getRequestContentLanguages();
        try
        {
            ContentLanguageListContainer cl_cntr =
                (ContentLanguageListContainer)context.get(
                    ContentLanguageListContainer::NAME);
            _client->setRequestContentLanguages(cl_cntr.getLanguages());
        }
        catch (Exception &)
        {
            // No ContentLanguageListContainer in OperationContext; just
            // use the default
        }
    }

    ~ClientCIMOMHandleSetup()
    {
        // Must not throw an exception from a destructor
        try
        {
            //
            // If the response has a Content-Language then save it into
            // thread-specific storage
            //
            if (_client->getResponseContentLanguages().size() > 0)
            {
                 Thread* curThrd = Thread::getCurrent();
                 if (curThrd != NULL)
                 {
                     // deletes the old tsd and creates a new one
                     curThrd->put_tsd("cimomHandleContentLanguages",
                         deleteContentLanguage,
                         sizeof(ContentLanguages*),
                         new ContentLanguages(
                             _client->getResponseContentLanguages()));
                 }
            }

            //
            // Reset CIMClient timeout value and languages to original values
            //
            _client->setTimeout(_origTimeout);
            _client->setRequestAcceptLanguages(_origAcceptLanguages);
            _client->setRequestContentLanguages(_origContentLanguages);
        }
        catch (Exception& e)
        {
            PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                String("Ignoring Exception: ") + e.getMessage());
        }
        catch (...)
        {
            PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL2,
                "Ignoring unknown exception");
        }
    }

private:
    // Unimplemented constructors and assignment operator
    ClientCIMOMHandleSetup();
    ClientCIMOMHandleSetup(const ClientCIMOMHandleSetup&);
    ClientCIMOMHandleSetup& operator=(const ClientCIMOMHandleSetup&);

    CIMClient* _client;
    Uint32 _origTimeout;
    AcceptLanguages _origAcceptLanguages;
    ContentLanguages _origContentLanguages;
};


//
// ClientCIMOMHandleRep
//

ClientCIMOMHandleRep::ClientCIMOMHandleRep()
    : _client(0)
{
}

ClientCIMOMHandleRep::~ClientCIMOMHandleRep()
{
    if (_client != 0)
    {
        // Must not throw an exception from a destructor
        try
        {
            _client->disconnect();
        }
        catch (...)
        {
            // Ignore disconnect exceptions
        }

        delete _client;
    }
}

// Private, unimplemented copy constructor
ClientCIMOMHandleRep::ClientCIMOMHandleRep(const ClientCIMOMHandleRep& rep)
  : CIMOMHandleRep()
{
    PEGASUS_ASSERT(0);
}

// Private, unimplemented assignment operator
ClientCIMOMHandleRep& ClientCIMOMHandleRep::operator=(
    const ClientCIMOMHandleRep& rep)
{
    PEGASUS_ASSERT(0);
    return *this;
}


//
// CIM Operations
//

CIMClass ClientCIMOMHandleRep::getClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::getClass");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->getClass(
        nameSpace,
        className,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMClass> ClientCIMOMHandleRep::enumerateClasses(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "ClientCIMOMHandleRep::enumerateClasses");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->enumerateClasses(
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin);
}

Array<CIMName> ClientCIMOMHandleRep::enumerateClassNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "ClientCIMOMHandleRep::enumerateClassNames");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->enumerateClassNames(
        nameSpace,
        className,
        deepInheritance);
}

void ClientCIMOMHandleRep::createClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::createClass");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    _client->createClass(
        nameSpace,
        newClass);

    PEG_METHOD_EXIT();
}

void ClientCIMOMHandleRep::modifyClass(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMClass& modifiedClass)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::modifyClass");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    _client->modifyClass(
        nameSpace,
        modifiedClass);

    PEG_METHOD_EXIT();
}

void ClientCIMOMHandleRep::deleteClass(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className)
{

    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::deleteClass");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    _client->deleteClass(
        nameSpace,
        className);

    PEG_METHOD_EXIT();
}

CIMInstance ClientCIMOMHandleRep::getInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::getInstance");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->getInstance(
        nameSpace,
        instanceName,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMInstance> ClientCIMOMHandleRep::enumerateInstances(
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
        "ClientCIMOMHandleRep::enumerateInstances");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->enumerateInstances(
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMObjectPath> ClientCIMOMHandleRep::enumerateInstanceNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "ClientCIMOMHandleRep::enumerateInstanceNames");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->enumerateInstanceNames(
        nameSpace,
        className);
}

CIMObjectPath ClientCIMOMHandleRep::createInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMInstance& newInstance)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "ClientCIMOMHandleRep::createInstance");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->createInstance(
        nameSpace,
        newInstance);
}

void ClientCIMOMHandleRep::modifyInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "ClientCIMOMHandleRep::modifyInstance");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    _client->modifyInstance(
        nameSpace,
        modifiedInstance,
        includeQualifiers,
        propertyList);

    PEG_METHOD_EXIT();
}

void ClientCIMOMHandleRep::deleteInstance(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "ClientCIMOMHandleRep::deleteInstance");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    _client->deleteInstance(
        nameSpace,
        instanceName);

    PEG_METHOD_EXIT();
}

Array<CIMObject> ClientCIMOMHandleRep::execQuery(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const String& queryLanguage,
    const String& query)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::execQuery");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->execQuery(
        nameSpace,
        queryLanguage,
        query);
}

Array<CIMObject> ClientCIMOMHandleRep::associators(
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
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::associators");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->associators(
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

Array<CIMObjectPath> ClientCIMOMHandleRep::associatorNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "ClientCIMOMHandleRep::associatorNames");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->associatorNames(
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole);
}

Array<CIMObject> ClientCIMOMHandleRep::references(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::references");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->references(
        nameSpace,
        objectName,
        resultClass,
        role,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMObjectPath> ClientCIMOMHandleRep::referenceNames(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "ClientCIMOMHandleRep::referenceNames");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->referenceNames(
        nameSpace,
        objectName,
        resultClass,
        role);
}

CIMValue ClientCIMOMHandleRep::getProperty(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::getProperty");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->getProperty(
        nameSpace,
        instanceName,
        propertyName);
}

void ClientCIMOMHandleRep::setProperty(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::setProperty");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    _client->setProperty(
        nameSpace,
        instanceName,
        propertyName,
        newValue);

    PEG_METHOD_EXIT();
}

CIMValue ClientCIMOMHandleRep::invokeMethod(
    const OperationContext & context,
    const CIMNamespaceName &nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters)
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE, "ClientCIMOMHandleRep::invokeMethod");

    CIMOMHandleOpSemaphore opsem(this);
    ClientCIMOMHandleAccessController access(_clientMutex);
    ClientCIMOMHandleSetup setup(_client, context);

    PEG_METHOD_EXIT();
    return _client->invokeMethod(
        nameSpace,
        instanceName,
        methodName,
        inParameters,
        outParameters);
}


//
// Other public methods
//

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
OperationContext ClientCIMOMHandleRep::getResponseContext()
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
