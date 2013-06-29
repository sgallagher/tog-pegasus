//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%////////////////////////////////////////////////////////////////////////////

#include "CIMOperationRequestDispatcher.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/WsmServer/WsmConstants.h>
#include <Pegasus/Common/XmlReader.h> // stringToValue(), stringArrayToValue()
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/AuditLogger.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/ObjectNormalizer.h>
#include <Pegasus/Server/reg_table.h>
#include <Pegasus/General/VersionUtil.h>

#include <Pegasus/Server/QuerySupportRouter.h>

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

// A helper function that resets the Propagated and ClassOrigin attributes on
// properties of CIMInstance and CIMClass objects. This is used during
// Create/Modify Instance and Create/Modify Class operations, where the
// Propagated and ClassOrigin attributes must be ignored.
template <class ObjectClass>
void removePropagatedAndOriginAttributes(ObjectClass& newObject);

static const char* _getServiceName(Uint32 serviceId)
{
    MessageQueue *queue = MessageQueue::lookup(serviceId);

    return queue ? queue->getQueueName() : "none";
}

/****************************************************************************
**
**  Implementation of OperationAggregate Class
**
****************************************************************************/
OperationAggregate::OperationAggregate(
    CIMOperationRequestMessage* request,
    MessageType msgRequestType,
    String messageId,
    Uint32 dest,
    CIMName className,
    CIMNamespaceName nameSpace,
    QueryExpressionRep* query,
    String queryLanguage)
    : _messageId(messageId),
      _msgRequestType(msgRequestType),
      _dest(dest),
      _nameSpace(nameSpace),
      _className(className),
      _query(query),
      _queryLanguage(queryLanguage),
      _request(request),
      _totalIssued(0), _totalReceived(0), _totalReceivedComplete(0),
      _totalReceivedExpected(0), _totalReceivedErrors(0),
      _totalReceivedNotSupported(0),
      _magicNumber(0xc685255e)        // set to constant for use by valid()
{
}

OperationAggregate::~OperationAggregate()
{
    _magicNumber = 0;
    delete _request;
    delete _query;
}

Boolean OperationAggregate::valid() const
{
    // test if valid OperationAggregate object.
    return (_magicNumber == 0xc685255e) ? true : false;
}

void OperationAggregate::setTotalIssued(Uint32 i)
{
    _totalIssued = i;
}

Boolean OperationAggregate::appendResponse(CIMResponseMessage* response)
{
    AutoMutex autoMut(_appendResponseMutex);
    _responseList.append(response);
    Boolean returnValue = (_totalIssued == numberResponses());
    return returnValue;
}

Uint32 OperationAggregate::numberResponses() const
{
    //// AutoMutex autoMut(_appendResponseMutex);
    return _responseList.size();
}

CIMOperationRequestMessage* OperationAggregate::getRequest()
{
    return _request;
}

CIMResponseMessage* OperationAggregate::getResponse(const Uint32& pos)
{
    AutoMutex autoMut(_appendResponseMutex);
    CIMResponseMessage* tmp = _responseList[pos];
    return tmp;
}

CIMResponseMessage* OperationAggregate::removeResponse(const Uint32& pos)
{
    AutoMutex autoMut(_appendResponseMutex);
    CIMResponseMessage* tmp = _responseList[pos];
    _responseList.remove(pos);
    return tmp;
}

void OperationAggregate::deleteResponse(const Uint32&pos)
{
    AutoMutex autoMut(_appendResponseMutex);
    delete _responseList[pos];
    _responseList.remove(pos);
}

MessageType OperationAggregate::getRequestType() const
{
    return _msgRequestType;
}

// There are many response pieces (chunks) from potentially many
// threads funneling through this function in random order. This isolates a
// single response (by locking) from a given thread and "resequences" the
// response as part of one large response. It is crucial that the first
// response to come through here be sequenced (or indexed) as 0 and the last
// response from the last thread be marked as "isComplete"

// NOTE: for now this assumes no chunks can come AFTER a "isComplete" message
// of the LAST thread.

void OperationAggregate::resequenceResponse(CIMResponseMessage& response)
{
    CIMStatusCode error = response.cimException.getCode();
    bool notSupportedReceived = false;
    if (error != CIM_ERR_SUCCESS)
    {
        if (error == CIM_ERR_NOT_SUPPORTED)
        {
            notSupportedReceived = true;
            _totalReceivedNotSupported++;
        }
        _totalReceivedErrors++;
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "%s: Response has error.  Namespace: %s, Class name: %s, "
                "Response Sequence: %s",
            "OperationAggregate::resequenceResponse",
            CSTRING(_nameSpace.getString()),
            CSTRING(_className.getString()),
            (_totalReceived) ? "true" : "false"));
    }

    Boolean isComplete = response.isComplete();

    if (isComplete == true)
    {
        _totalReceivedComplete++;
        _totalReceivedExpected += response.getIndex() + 1;
    }

    response.setIndex(_totalReceived++);

    // set to incomplete until ALL completed messages have come in

    isComplete = false;

    // NOTE:
    // _totalReceivedExpected is calculated by adding up every response index
    // count WHEN the message is marked complete. This may differ from the
    // following reasons:
    // 1. An exception occurred in which the correct index could not be set.
    // 2. Somehow the completed response arrived before the other
    //    (non-completed) responses ? (shouldnt happen with the current
    //    synchronous code).
    // In either case, a message will be logged and attempt to continue

    if (_totalReceivedComplete == _totalIssued)
    {
        if (_totalReceivedExpected == _totalReceived)
        {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "%s: Message is complete.  Total responses: %u, "
                    "total chunks: %u, total errors: %u",
                "OperationAggregate::resequenceResponse",
                _totalReceivedComplete,
                _totalReceived,
                _totalReceivedErrors));
        }
        else
        {
            PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "All completed responses (%u) for current request "
                    "have been accounted for but expected count (%u) does "
                    "not match the received count (%u). error count (%u).",
                _totalReceivedComplete,
                _totalReceivedExpected,
                _totalReceived,
                _totalReceivedErrors));
        }

        // If all of the errors received were NOT_SUPPORTED and
        // all of the responses were errors, then keep the last
        // NOT_SUPPORTED error.
        // The condition below is the oposite of that. If there was an error
        // besides NOT_SUPPORTED, or a non-error response was received, and
        // the last response was a NOT_SUPPORTED error, then clear the error
        if ((_totalReceivedErrors != _totalReceivedNotSupported ||
                 _totalReceivedErrors != _totalReceived) &&
            notSupportedReceived)
        {
            response.cimException = CIMException();
        }

        isComplete = true;
        _totalReceivedComplete = 0;
        _totalReceivedExpected = 0;
        _totalReceivedErrors = 0;
        _totalReceivedNotSupported = 0;
        _totalReceived = 0;
    }
    else if (notSupportedReceived)
    {
        // Clear the NOT_SUPPORTED exception
        // We ignore it unless it's the only response received
        response.cimException = CIMException();
    }

    response.setComplete(isComplete);
}
/*************************************************************************
**
** Implementation of CIMOperationRequestDispatcher Class
**
***************************************************************************/

// Dispatcher Class Constructor. Setup Dispatcher configuration from
// compile and runtime options.  This includes setting up QueueIds,
// limit parameters, etc. that will be used for request operation processing
//
CIMOperationRequestDispatcher::CIMOperationRequestDispatcher(
    CIMRepository* repository,
    ProviderRegistrationManager* providerRegistrationManager)
    : Base(PEGASUS_QUEUENAME_OPREQDISPATCHER),
      _repository(repository),
      _providerRegistrationManager(providerRegistrationManager)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::CIMOperationRequestDispatcher");

    //
    // Setup enable AssociationTraversal parameter.  Determines whether
    // Association operations are allowed.
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    _enableAssociationTraversal = ConfigManager::parseBooleanValue(
        configManager->getCurrentValue("enableAssociationTraversal"));

    //  Set up maximum breadth of Enums parameter.  Determines maximum
    //  classes that can particpate in an enumeration.  Can be used to
    //  limit size of enumerations, associations, etc. to a maximum number
    //  of providers
    //  Get the maximum breadth of enum parameter from config if it exists.
    //
    // ATTN: KS 20030602 Bypass param acquistion until we get it into the
    // config manager
    // Config param removed for Pegasus 2.2 and until PEP 66 resolved.
    // In place, we simply allow anything through by setting the breadth too
    // large number.
// #define  MAXENUMBREADTHTESTENABLED
#ifdef MAXENUMBREADTHTESTENABLED
    String maxEnumBreadthOption;
    try
    {
        maxEnumBreadthOption =
            configManager->getCurrentValue("maximumEnumerationBreadth");
        if (maxEnumBreadthOption != String::EMPTY)
            _maximumEnumerateBreadth =
                atol(maxEnumBreadthOption.getCString());
    }
    catch (...)
    {
        _maximumEnumerateBreadth = 50;
    }
#else
    // As part of disabling the test for now, we set to very large number.
    _maximumEnumerateBreadth = 1000;
#endif

    //
    // Setup list of provider modules that will be excluded from normalization
    // List derived from runtime variable with format name, name, ....
    //
#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    String moduleList =
        configManager->getCurrentValue("excludeModulesFromNormalization");

    for (Uint32 pos = moduleList.find(','); moduleList.size() != 0;
        pos = moduleList.find(','))
    {
        String moduleName = moduleList.subString(0, pos);

        _excludeModulesFromNormalization.append(moduleName);

        moduleList.remove(0, (pos == PEG_NOT_FOUND ? pos : pos + 1));
    }
#endif

    // Get pointer to Internal Provider Routing Table. Note that table
    // probably not actually built at this point since it must be build
    // very late in initialization. See CIMServer.cpp
    _routing_table = DynamicRoutingTable::getRoutingTable();

    // Get ProviderManager QueueId (Used to forward requests
    // providerManager must be initialized before
    // Dispatcher so QueueId set.
    _providerManagerServiceId =
        lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)->getQueueId();

    PEG_METHOD_EXIT();
}

//
//  CIMOperationDispatcher object destructor
//
CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher");
    PEG_METHOD_EXIT();
}

//  Get Provider name and moduleName from OperationContext.
//  Used only by the AuditLog functions
void CIMOperationRequestDispatcher::_getProviderName(
    const OperationContext& context,
    String& moduleName,
    String& providerName)
{
    moduleName.clear();
    providerName.clear();

    if (context.contains(ProviderIdContainer::NAME))
    {
        const ProviderIdContainer pidc =
            (const ProviderIdContainer) context.get(ProviderIdContainer::NAME);

        CIMConstInstance module = pidc.getModule();
        Uint32 pos = module.findProperty(_PROPERTY_PROVIDERMODULE_NAME);
        if (pos != PEG_NOT_FOUND)
        {
            module.getProperty(pos).getValue().get(moduleName);
        }

        CIMConstInstance provider = pidc.getProvider();
        pos = provider.findProperty(PEGASUS_PROPERTYNAME_NAME);
        if (pos != PEG_NOT_FOUND)
        {
            provider.getProperty(pos).getValue().get(providerName);
        }
    }
}

void CIMOperationRequestDispatcher::_logOperation(
    const CIMOperationRequestMessage* request,
    const CIMResponseMessage* response)
{
#ifdef PEGASUS_ENABLE_AUDIT_LOGGER

    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_logOperation");

    String moduleName;
    String providerName;

    if (AuditLogger::isEnabled())
    {
        switch (request->getType())
        {
            case CIM_CREATE_CLASS_REQUEST_MESSAGE:
            {
                const CIMCreateClassRequestMessage* req =
                    reinterpret_cast<const CIMCreateClassRequestMessage*>(
                        request);
                AuditLogger::logUpdateClassOperation(
                    "CreateClass",
                    AuditLogger::EVENT_CREATE,
                    req->userName,
                    req->ipAddress,
                    req->nameSpace,
                    req->className,
                    response->cimException.getCode());
                break;
            }

            case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
            {
                const CIMModifyClassRequestMessage* req =
                    reinterpret_cast<const CIMModifyClassRequestMessage*>(
                        request);
                AuditLogger::logUpdateClassOperation(
                    "ModifyClass",
                    AuditLogger::EVENT_UPDATE,
                    req->userName,
                    req->ipAddress,
                    req->nameSpace,
                    req->className,
                    response->cimException.getCode());
                break;
            }

            case CIM_DELETE_CLASS_REQUEST_MESSAGE:
            {
                const CIMDeleteClassRequestMessage* req =
                    reinterpret_cast<const CIMDeleteClassRequestMessage*>(
                        request);
                AuditLogger::logUpdateClassOperation(
                    "DeleteClass",
                    AuditLogger::EVENT_DELETE,
                    req->userName,
                    req->ipAddress,
                    req->nameSpace,
                    req->className,
                    response->cimException.getCode());
                break;
            }

            case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
            {
                const CIMSetQualifierRequestMessage* req =
                    reinterpret_cast<const CIMSetQualifierRequestMessage*>(
                        request);
                AuditLogger::logUpdateQualifierOperation(
                    "SetQualifier",
                    AuditLogger::EVENT_UPDATE,
                    req->userName,
                    req->ipAddress,
                    req->nameSpace,
                    req->qualifierDeclaration.getName(),
                    response->cimException.getCode());
                break;
            }

            case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
            {
                const CIMDeleteQualifierRequestMessage* req =
                    reinterpret_cast<const CIMDeleteQualifierRequestMessage*>(
                        request);
                AuditLogger::logUpdateQualifierOperation(
                    "DeleteQualifier",
                    AuditLogger::EVENT_DELETE,
                    req->userName,
                    req->ipAddress,
                    req->nameSpace,
                    req->qualifierName,
                    response->cimException.getCode());
                break;
            }

            case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
            {
                const CIMCreateInstanceRequestMessage* req =
                    reinterpret_cast<const CIMCreateInstanceRequestMessage*>(
                        request);

                const CIMCreateInstanceResponseMessage* resp =
                    reinterpret_cast<const CIMCreateInstanceResponseMessage*>(
                        response);

                _getProviderName(
                    req->operationContext, moduleName, providerName);

                AuditLogger::logUpdateInstanceOperation(
                    "CreateInstance",
                    AuditLogger::EVENT_CREATE,
                    req->userName,
                    req->ipAddress,
                    req->nameSpace,
                    (response->cimException.getCode() == CIM_ERR_SUCCESS) ?
                        resp->instanceName :
                        CIMObjectPath(req->className.getString()),
                    moduleName,
                    providerName,
                    response->cimException.getCode());
                break;
            }

            case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
            {
                const CIMModifyInstanceRequestMessage* req =
                    reinterpret_cast<const CIMModifyInstanceRequestMessage*>(
                        request);

                _getProviderName(
                    req->operationContext, moduleName, providerName);

                AuditLogger::logUpdateInstanceOperation(
                    "ModifyInstance",
                    AuditLogger::EVENT_UPDATE,
                    req->userName,
                    req->ipAddress,
                    req->nameSpace,
                    req->modifiedInstance.getPath(),
                    moduleName,
                    providerName,
                    response->cimException.getCode());
                break;
            }

            case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
            {
                const CIMDeleteInstanceRequestMessage* req =
                    reinterpret_cast<const CIMDeleteInstanceRequestMessage*>(
                        request);

                _getProviderName(
                    req->operationContext, moduleName, providerName);

                AuditLogger::logUpdateInstanceOperation(
                    "DeleteInstance",
                    AuditLogger::EVENT_DELETE,
                    req->userName,
                    req->ipAddress,
                    req->nameSpace,
                    req->instanceName,
                    moduleName,
                    providerName,
                    response->cimException.getCode());
                break;
            }

            case CIM_SET_PROPERTY_REQUEST_MESSAGE:
            {
                const CIMSetPropertyRequestMessage* req =
                    reinterpret_cast<const CIMSetPropertyRequestMessage*>(
                        request);

                _getProviderName(
                    req->operationContext, moduleName, providerName);

                AuditLogger::logUpdateInstanceOperation(
                    "SetProperty",
                    AuditLogger::EVENT_UPDATE,
                    req->userName,
                    req->ipAddress,
                    req->nameSpace,
                    req->instanceName,
                    moduleName,
                    providerName,
                    response->cimException.getCode());
                break;
            }

            case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
            {
                const CIMInvokeMethodRequestMessage* req =
                    reinterpret_cast<const CIMInvokeMethodRequestMessage*>(
                        request);

                _getProviderName(
                    req->operationContext, moduleName, providerName);

                AuditLogger::logInvokeMethodOperation(
                    req->userName,
                    req->ipAddress,
                    req->nameSpace,
                    req->instanceName,
                    req->methodName,
                    moduleName,
                    providerName,
                    response->cimException.getCode());
                break;
            }

            default:
                // Other operations are not logged.
                break;
        }
    }

    PEG_METHOD_EXIT();

#endif
}

/*
    send the given response synchronously using the given aggregation object.
    return whether the sent message was complete or not. The parameters are
    pointer references because they can be come invalid from external deletes
    if the message is complete after queueing. They can be zeroed in this
    function preventing the caller from referencing deleted pointers.
*/
Boolean CIMOperationRequestDispatcher::_enqueueResponse(
    OperationAggregate*& poA,
    CIMResponseMessage*& response)
{
    // Obtain the _enqueueResponseMutex mutex for this chunked request.
    // This mutex is used to serialize chunked responses from all incoming
    // provider threads. It is imperative that the sequencing done by the
    // resequenceResponse() method and the writing of the chunked response
    // to the connection socket (done as a synchronous enqueue at the end
    // of this method) are atomic to ensure that the chunk that is marked
    // as isComplete for the overall response is indeed the last chunk
    // returned to the client. See PEP 140 for details.
    // This mutex was moved into the OperationAggregate class as part of
    // bug 5157 because we only need to serialize on a per-request basis.
    // This prevents serializing independent requests on separate connections.
    AutoMutex autoMut(poA->_enqueueResponseMutex);
    Boolean isComplete = false;

    try
    {
        // get the completion status of the incoming response on this thread.
        isComplete = response->isComplete();

        poA->appendResponse(response);
        Uint32 type = poA->getRequestType();

        // there should never be more than one object in the list for async
        // queues these functions are called for their jobs other than
        // aggregating.

        // Operations which run through here are:
        // CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE
        // CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE
        // CIM_ASSOCIATORS_REQUEST_MESSAGE
        // CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE
        // CIM_REFERENCES_REQUEST_MESSAGE
        // CIM_REFERENCE_NAMES_REQUEST_MESSAGE
        // CIM_EXEC_QUERY_REQUEST_MESSAGE

        if (type != CIM_EXEC_QUERY_REQUEST_MESSAGE)
        {
            // Hostname completion required for the four assoc-like operations
            //
            handleOperationResponseAggregation(
                poA,
                ((CIM_ASSOCIATORS_REQUEST_MESSAGE == type) |
                 (CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE == type) |
                 (CIM_REFERENCES_REQUEST_MESSAGE == type) |
                 (CIM_REFERENCE_NAMES_REQUEST_MESSAGE == type)),
                (type == CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE));
        }
        else
        {
            handleExecQueryResponseAggregation(poA);
        }

        // now take the aggregated response. This is now the one we will
        // work with
        response = poA->removeResponse(Uint32(0));

        // reset the completion flag of the last response added to the list.
        // This only makes a difference when there was at least two on the list
        // to begin with before aggregation methods (above) were called.
        // Typically, there will be more than two on the list when a non-async
        // destination queue keeps appending the responses until the completion
        // of all threads/responses has occurred.

        response->setComplete(isComplete);

        Uint32 dest = poA->_dest;
        response->dest = dest;
        poA->resequenceResponse(*response);

        // now get the completion status of the response after it has been
        // resequenced. This will reset the completion status of the entire
        // message, not just this one thread coming through here.

        isComplete = response->isComplete();

        // can the destination service queue handle async responses ?
        // (i.e multiple responses from one request). Certain known ones
        // cannot handle it. Most notably, the internal client.

        MessageQueue* q = lookup(dest);
        const char* name = q ? q->getQueueName() : 0;
        Boolean isDestinationQueueAsync = !(name &&
            (strcmp(name, PEGASUS_QUEUENAME_INTERNALCLIENT) == 0 ||
             strcmp(name, PEGASUS_QUEUENAME_WSMPROCESSOR) == 0));

        // for non-async queues, we'll just keep appending until all responses
        // have come in

        if (isDestinationQueueAsync == false)
        {
            if (isComplete == false)
            {
                poA->appendResponse(response);
                return isComplete;
            }

            // need to reset the first response to complete if the
            // last one that came in was complete

            response->setComplete(true);
            response->setIndex(0);
        }

        if (isComplete)
        {
            _logOperation(poA->getRequest(), response);
        }

        // send it syncronously so that multiple responses will show up in the
        // receiving queue according to the order that we have set the response
        // index. If this was a single complete response, we could in theory
        // send it async (i.e SendForget), however, there is no need to make a
        // condition point based off this.

        if (q)
            q->enqueue(response);
        else throw UninitializedObjectException();
    }
    catch (...)
    {
        static const char failMsg[] =
            "Failed to resequence/aggregate/forward response";
        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "%s%s",
            "CIMOperationRequestDispatcher::_enqueueResponse",
            failMsg));

        if (response->cimException.getCode() != CIM_ERR_SUCCESS)
            response->cimException =
                CIMException(CIM_ERR_FAILED, String(failMsg));
    }

    // after sending, the response has been deleted externally
    response = 0;

    return isComplete;
}

void CIMOperationRequestDispatcher::_handle_async_request(AsyncRequest* req)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_handle_async_request");

    // pass legacy operations to handleEnqueue
    if (req->getType() == ASYNC_ASYNC_LEGACY_OP_START)
    {
        Message* message =
            static_cast<AsyncLegacyOperationStart*>(req)->get_action();

        handleEnqueue(message);

        PEG_METHOD_EXIT();
        return;
    }

    // pass all other operations to the default handler
    Base::_handle_async_request(req);
    PEG_METHOD_EXIT();
}

/*
    Get an internal provider that matches the namespace/classname pair
    @parameter nameSpace CIMNamespaceName of desired provider
    $param className CIMName input with className of desired provider
    @providerInfo ProviderInfo with registration information for registered
    provider if provider found
    @return True if internal provider found.  If found, registration
    information is in provider.  False if no internal provider found.
*/
Boolean CIMOperationRequestDispatcher::_lookupInternalProvider(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    ProviderInfo& providerInfo)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupInternalProvider");

    // Clear the parameters since used as test after getRouting call
    Uint32 serviceId = 0;
    String controlProviderName;

    providerInfo.hasProvider = _routing_table->getRouting(
        className,
        nameSpace,
        controlProviderName,
        serviceId);

    if (providerInfo.hasProvider)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "Internal provider Service = %s provider %s found.",
            _getServiceName(serviceId),
            CSTRING(controlProviderName) ));

        providerInfo.serviceId = serviceId;
        providerInfo.controlProviderName = controlProviderName;
    }

    PEG_METHOD_EXIT();
    return providerInfo.hasProvider;
}

/**
    _getSubClassNames - Gets the names of all subclasses of the defined
    class (including the class) and returns it in an array of strings. Uses a
    similar function in the repository class to get the names.  Note that
    this prepends the result with the input classname.
    @param namespace
    @param className
    @return Array of strings with class names.  Note that there should be at
    least one classname in the array (the input name)
    Note that there is a special exception to this function, the __namespace
    class which does not have any representation in the class repository.
    @exception CIMException(CIM_ERR_INVALID_CLASS)
*/
Array<CIMName> CIMOperationRequestDispatcher::_getSubClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_getSubClassNames");

    Array<CIMName> subClassNames;
    //
    // Get names of descendent classes:
    // Ignore if the special class __Namespace because it works in very
    // strange way.  NOTE: This class is also deprecated in DSP0200.
    //
    if (!className.equal (PEGASUS_CLASSNAME___NAMESPACE))
    {
        // Get the complete list of subclass names
        // getSubClassNames throws an exception if the class does not exist
        _repository->getSubClassNames(nameSpace,
             className, true, subClassNames);
    }

    // Prepend the array with the classname from the request so
    // return is this class and all subclasses
    subClassNames.prepend(className);

    PEG_METHOD_EXIT();
    return subClassNames;
}

///////////////////////////////////////////////////////////////////////////
//  PropertyList management functions
//////////////////////////////////////////////////////////////////////////

/* _showPropertyList is local support for displaying the propertylist
   For display only. Generates String with property list names
   or "empty" or "NULL" if that is the case.
   @param pl CIMPropertyList to convert
   @return String containing the list of properties comma separated
   or the keywords NULL or EMPTY.
 */
static String _showPropertyList(const CIMPropertyList& pl)
{
    String returnString;

    if (pl.isNull())
    {
        returnString = "NULL";
    }
    else if (pl.size() == 0)
    {
        returnString = "EMPTY";
    }
    else
    {
        for (Uint32 i = 0; i < pl.size(); i++)
        {
            if (i > 0)
            {
                returnString.append(", ");
            }
            returnString.append(pl[i].getString());
        }
    }

    return returnString;
}

///////////////////////////////////////////////////////////////////////////
//
// Provider Lookup Functions - Gets list of providers for operation types
//       (ex. associations, enumerate, get, invokeMethod
//
//////////////////////////////////////////////////////////////////////////
//
/* lookupAllInstanceProviders - Returns the list of all subclasses of this
   class along with information about whether it is an instance provider.
   @param nameSpace - Namespace for the lookup.
   @param className - className for which we are finding providers
   @param providerCount - Returns count of providers in the response array.
   This is because the array returned is all subclasses, not simply those
   with providers.
   @return Returns an array of ProviderInfo, one entry for each subclass.
   Each ProviderInfo instance defines whether a provider exists and the
   information on the provider so that the operation can be forwarded to the
   provider.
   @exception - Throws one exception if the className is in error.  Note that
   this should NEVER occur since the className should have already been
   validated in the operation validation code.
*/

ProviderInfoList CIMOperationRequestDispatcher::_lookupAllInstanceProviders(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAllInstanceProviders");

    ProviderInfoList providerList;

    Array<CIMName> classNames = _getSubClassNames(nameSpace, className);

    // Loop for all classNames found
    for (Uint32 i = 0, n = classNames.size(); i < n; i++)
    {
        // Lookup any instance providers and add to send list
        ProviderInfo providerInfo =
            _lookupInstanceProvider(
                nameSpace,
                classNames[i]);

        if (providerInfo.hasProvider)
        {
            providerList.providerCount++;

            PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL4,
                "Provider found for class = %s servicename = %s "
                "controlProviderName = %s",
                CSTRING(providerInfo.className.getString()),
                _getServiceName(providerInfo.serviceId),
                CSTRING(providerInfo.controlProviderName)));
        }

        // Always add providerInfo to be sure we include classes without
        // providers. This does not increment provider count
        providerList.append(providerInfo);
   }

    // Reject if the request is too broad
    _rejectEnumerateTooBroad(
        nameSpace, className, providerList.providerCount);

   PEG_METHOD_EXIT();

   return providerList;
}

/* _lookupInstanceProvider - Looks up the internal or registered instance
    provider for the classname and namespace.
    @return ProviderInfo struct with information about the provider found.

    If ProviderInfo.hasProvider returns false, no provider for this
    classname/namespace combination.

    This function calls both the _lookupInternalProvider function and if
    nothing found, the providerRegistration _lookupInstanceProvider function.

*/
ProviderInfo CIMOperationRequestDispatcher::_lookupInstanceProvider(
   const CIMNamespaceName& nameSpace,
   const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupInstanceProvider");

    ProviderInfo providerInfo(className);

    // Check for class provided by an internal provider
    Boolean hasControlProvider = _lookupInternalProvider(
        nameSpace,
        className,
        providerInfo);

    // If no internal provider, try registered providers
    if (!hasControlProvider)
    {
        CIMInstance pInstance;
        CIMInstance pmInstance;
        Boolean hasNoQuery = true;

        Boolean hasProvider =
            _providerRegistrationManager->lookupInstanceProvider(
                nameSpace,
                className,
                pInstance,
                pmInstance,
                false,
                &hasNoQuery);

        if (hasProvider)
        {
            providerInfo.addProviderInfo(_providerManagerServiceId, true,
                                         hasNoQuery);

           ProviderIdContainer* pi = _updateProviderContainer(
               nameSpace, pInstance, pmInstance);

           providerInfo.providerIdContainer.reset(pi);

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
            if (ObjectNormalizer::getEnableNormalization())
            {
                // normalization is enabled for all providers unless they
                // have an old interface version or are explicity excluded by
                // the excludeModulesFromNormalization configuration option.

                // check interface type and version

                String interfaceType;
                String interfaceVersion;
                Uint32 pos = PEG_NOT_FOUND;

                // get the provder interface type
                if ((pos = pmInstance.findProperty("InterfaceType")) !=
                        PEG_NOT_FOUND)
                {
                    pmInstance.getProperty(pos).getValue().get(interfaceType);
                }

                // get the provder interface version
                if ((pos = pmInstance.findProperty("InterfaceVersion")) !=
                        PEG_NOT_FOUND)
                {
                    pmInstance.getProperty(pos).
                        getValue().get(interfaceVersion);
                }

                // compare the interface type and vesion
                if (String::equalNoCase(interfaceType, "C++Default"))
                {
                    // version must be greater than 2.5.0
                    if (VersionUtil::isVersionGreaterOrEqual(
                        interfaceVersion, 2, 5, 0))
                    {
                        providerInfo.hasProviderNormalization = true;
                    }
                }
                else if (String::equalNoCase(interfaceType, "CMPI"))
                {
                    // version must be greater than 2.0.0
                    if (VersionUtil::isVersionGreaterOrEqual(
                        interfaceVersion, 2, 0, 0))
                    {
                        providerInfo.hasProviderNormalization = true;
                    }
                }
                else if (String::equalNoCase(interfaceType, "JMPI"))
                {
                    // version must be greater than 1.0.0
                    if (VersionUtil::isVersionGreaterOrEqual(
                        interfaceVersion, 1, 0, 0))
                    {
                        providerInfo.hasProviderNormalization = true;
                    }
                }

                // check for module exclusion

                String moduleName;

                // get the provider module name
                if ((pos = pmInstance.findProperty(
                    PEGASUS_PROPERTYNAME_NAME)) != PEG_NOT_FOUND)
                {
                    pmInstance.getProperty(pos).getValue().get(moduleName);
                }

                // check if module name is on excludeModulesFromNormalization
                // list
                for (Uint32 i = 0, n = _excludeModulesFromNormalization.size();
                     i < n; i++)
                {
                    if (String::equalNoCase(
                            moduleName, _excludeModulesFromNormalization[i]))
                    {
                        providerInfo.hasProviderNormalization = false;
                        break;
                    }
                }

                PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL4,
                    "Normalization for provider module %s is %s.",
                    CSTRING(moduleName),
                    (providerInfo.hasProviderNormalization ?
                         "enabled" : "disabled")));
            }
#endif

        }  // no provider or control provider
        else
        {
            PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL2,
                "Provider for %s not found.",
                CSTRING(className.getString()) ));
        }
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "CIMOperationRequestDispatcher::_lookupInstanceProvider - "
            "Namespace: %s  Class Name: %s  Service Name: %s  "
            "Provider Name: %s found. hasProvider = %s",
        CSTRING(nameSpace.getString()),
        CSTRING(className.getString()),
        _getServiceName(providerInfo.serviceId),
        CSTRING(providerInfo.controlProviderName),
        (providerInfo.hasProvider ? "true" : "false")));

    PEG_METHOD_EXIT();
    return providerInfo;
}

/*
    Lookup the method provider for the namespace, classname, and methodname
    provided.
    NOTE: Lookup of internal providers is handled by the method request handler
    today.  This should probably be fixed to provide exactly the
    same lookup for all types in the future.
*/
String CIMOperationRequestDispatcher::_lookupMethodProvider(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const CIMName& methodName,
    ProviderIdContainer** providerIdContainer)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupMethodProvider");

    CIMInstance pInstance;
    CIMInstance pmInstance;
    String providerName = String::EMPTY;

    // lookup the provider. Returns provider instance and provider
    // module instance.
    if (_providerRegistrationManager->lookupMethodProvider(
            nameSpace, className, methodName, pInstance, pmInstance))
    {
        (*providerIdContainer) = _updateProviderContainer(
            nameSpace, pInstance, pmInstance);

        // get the provder name
        Uint32 pos = pInstance.findProperty(PEGASUS_PROPERTYNAME_NAME);

        if (pos != PEG_NOT_FOUND)
        {
            pInstance.getProperty(pos).getValue().get(providerName);
        }
    }
    // return providerName. Empty if method provider not found.
    PEG_METHOD_EXIT();
    return providerName;
}

/*  _lookupAllAssociation Providers
    Returns all of the association providers that exist for the defined class
    name.  Uses the referencenames function to get list of classes for which
    providers required and then looks up the providers for each     class
    @param nameSpace
    @param objectName
    @param - assocClass referenced parameter that is modified in this function.
    @param role String defining role for this association
    @param providerCount count of actual providers found, not the count of
    classes.  This differs from the count of classes in that the providerInfo
    list is all classes including those that would go to the repository.
    @returns List of ProviderInfo
    @exception - Exceptions From the Repository
*/
ProviderInfoList CIMOperationRequestDispatcher::_lookupAllAssociationProviders(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const String& role)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAllAssociationProviders");

    ProviderInfoList providerInfoList;

    CIMName className = objectName.getClassName();
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "Association Class Lookup for Class %s  and assocClass %s",
        CSTRING(className.getString()),
        CSTRING(assocClass.getString()) ));

    // The association class is the basis for association registration.
    // When an association class request is received by the CIMOM the target
    // class is the endpoint class or instance.  Prevously we also called
    // providers registered against this class.  Thus, typically this would
    // be the same class as the instance class.  For example ClassA with
    // Assoc1.  We would register an instance and Association provider
    // against Class A.  With this change, we register the Instance class
    // as before, but register the association class with the association
    // itself, i.e. register it against the class Assoc1.
    // In this case, the list of potential classes for providers is the same
    // as the reference names, i.e the association classes that have
    // associations pointing back to the target class.

    // ATTN: KS 20030515. After we test and complete the move to using this
    // option, lets go back and change the call to avoid the double
    // conversion to and from CIM Object path.

    Array<CIMObjectPath> objPaths;

    try
    {
        // Note:  We use assocClass because this is the association function.
        // The Reference(Name)s calls must put the resultClass here.
        objPaths = _repository->referenceNames(
            nameSpace,
            CIMObjectPath(String::EMPTY, CIMNamespaceName(), className),
            assocClass,
            role);
    }
    catch (...)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::lookupAllAssociationProvider "
                "exception.  Namespace: %s  Object Name: %s  Assoc Class: %s",
            CSTRING(nameSpace.getString()),
            CSTRING(objectName.toString()),
            CSTRING(assocClass.getString())));
        throw;
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "Association Lookup: %u classes found", objPaths.size()));

    // lookup association providers for all classes returned by the
    // reference names lookup
    for (Uint32 i = 0; i < objPaths.size(); i++)
    {
        CIMName cn = objPaths[i].getClassName();

        // Create ProviderInfo object with default info and class name
        ProviderInfo pi(cn);

        // Use returned classname for the association classname
        // under the assumption that the registration is for the
        // association class, not the target class
        if (_lookupAssociationProvider(nameSpace, cn, pi))
        {
            providerInfoList.appendProvider(pi);
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Association append: class = %s to list. count = %u",
                CSTRING(objPaths[i].getClassName().getString()),
                providerInfoList.size() ));
        }
    }
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "providerCount = %u.", providerInfoList.providerCount));
    PEG_METHOD_EXIT();
    return providerInfoList;
}

/* _lookupAssociationProvider - Look up the internal and/or registered
    association provider for the defined namespace and class and return a
    providerInfo struct containing information about the provider if found.
    @param nameSpace
    @param assocClass
    @param providerInfo ProviderInfo& container holding information about
    the provider found including name, etc.
    @return true if a provider is found for the defined class and namespace.
*/
Boolean CIMOperationRequestDispatcher::_lookupAssociationProvider(
    const CIMNamespaceName& nameSpace,
    const CIMName& assocClass,
    ProviderInfo& providerInfo)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAssociationProvider");

    ProviderIdContainer* providerIdContainer=NULL;
    String providerName;

    // Check for class provided by an internal provider
    if (!_lookupInternalProvider(nameSpace, assocClass, providerInfo))
    {
        // get provider for class from registered providers. Note that we
        // reduce it from multiple possible class names to a single one here.
        // This limit is defined by ProviderRegistrationManager in that
        // it will only return one provider since we allow only a single
        // provider for a class. (See bug 9581)
        Array<String> assocProviderList =
            _lookupRegisteredAssociationProvider(
                nameSpace, assocClass, &providerIdContainer);

        /// Provider registration should NEVER return more than a single
        //  provider for this lookup by design of provider registration today.
        if (assocProviderList.size() > 0)
        {
            providerInfo.addProviderInfo(_providerManagerServiceId,true,false);
            providerName = assocProviderList[0];
        }
    }

    // Fill out providerInfo providerIdContainer
    if (providerInfo.hasProvider)
    {
        providerInfo.providerIdContainer.reset(providerIdContainer);
    }
    else
    {
        providerInfo.providerIdContainer.reset();
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "Association Provider %s for class=\"%s\"  in namespace \"%s\"."
            " servicename=\"%s\" provider = \"%s\" controlProvider = \"%s\"",
        (providerInfo.hasProvider? "found" : "NOT found"),
        CSTRING(assocClass.getString()),
        CSTRING(nameSpace.getString()),
        _getServiceName(providerInfo.serviceId),
        CSTRING(providerName),
        CSTRING(providerInfo.controlProviderName) ));

    PEG_METHOD_EXIT();
    return providerInfo.hasProvider;
}

// Lookup the registered Association provider(s) for this class and convert
// the result to an array of provider names.
// NOTE: The code allows for multiple providers but there is no reason
// to have this until we really support multiple providers per class.
// The multiple provider code was here because there was some code in to
// do the lookup in provider registration which was removed. Some day we
// will support multiple providers per class so it was logical to just leave
// the multiple provider code in place.
// NOTE: assocClass and resultClass are optional
//
Array<String>
   CIMOperationRequestDispatcher::_lookupRegisteredAssociationProvider(
   const CIMNamespaceName& nameSpace,
   const CIMName& assocClass,
   ProviderIdContainer** providerIdContainer)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupRegisteredAssociationProvider");

    // instances of the provider class and provider module class for the
    // response
    Array<CIMInstance> pInstances; // Providers
    Array<CIMInstance> pmInstances; // ProviderModules

    Array<String> providerNames;
    String providerName;

    //get list of registered association providers
    Boolean returnValue =
        _providerRegistrationManager->lookupAssociationProvider(
            nameSpace, assocClass, pInstances, pmInstances);

    if (returnValue)
    {
        for (Uint32 i = 0, n = pInstances.size(); i < n; i++)
        {
            // At present only one provider per class or per association is
            // supported and the same provider is stored in the
            // providerIdContainer. So the array will actually have only one
            // item. And hence with the same element providerIdContainer will
            // be created. When we start supporting multiple providers per
            // class or assoc, we need to change the code to make
            // providerIdContainer accordingly.
            if (i == 0)
            {
                // Call function that depends on the Remote CMPI flag
                // to update ProviderIdContainer
                (*providerIdContainer) = _updateProviderContainer(
                    nameSpace, pInstances[i], pmInstances[i]);
            }

            // get the provider name
            Uint32 pos = pInstances[i].findProperty(PEGASUS_PROPERTYNAME_NAME);

            if ( pos != PEG_NOT_FOUND )
            {
                pInstances[i].getProperty(pos).getValue().get(providerName);

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "Association providerName = %s found for Class %s",
                    CSTRING(providerName),
                    CSTRING(assocClass.getString())));
                    providerNames.append(providerName);
            }
        }
    }

    if (providerNames.size() == 0)
    {
        PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL3,
            "Association Provider NOT found for Class %s in nameSpace %s",
            CSTRING(assocClass.getString()), CSTRING(nameSpace.getString()) ));
    }
    PEG_METHOD_EXIT();
    return providerNames;
}

// Service function to build the ProviderIdContainer.
// If remote CMPI enabled and if this is a remote namespace, it must
// place the remote info into the container.  Otherwise it just builds
// a new container with pmInstance and pInstance and returns it

ProviderIdContainer* CIMOperationRequestDispatcher::_updateProviderContainer(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& pInstance,
    const CIMInstance& pmInstance)
{
        ProviderIdContainer* pc;
#ifdef PEGASUS_ENABLE_REMOTE_CMPI
        String remoteInformation;
        Boolean isRemote = false;
        isRemote = _repository->isRemoteNameSpace(
            nameSpace,
            remoteInformation);
        if (isRemote)
            pc = new ProviderIdContainer(
                pmInstance, pInstance, isRemote, remoteInformation);
        else
            pc = new ProviderIdContainer(pmInstance, pInstance);
#else
        pc = new ProviderIdContainer(pmInstance, pInstance);
#endif
        return pc;
}

/*****************************************************************************
**
** Dispatcher callback functions.  These functions are called by the
** service/providerManager generating responses to return response information
** to the Dispatcher Response handlers.  The reference to the correct handler
** is provided by the Dispatcher request handlers when requests are passed
** forward for services or provider managers, i.e. _forwardRequest* Functions)
**
******************************************************************************/
/*  Dispatcher callback for response aggregation
    The userParameter contains the OperationAggregate for this operation
*/
void CIMOperationRequestDispatcher::_forwardForAggregationCallback(
    AsyncOpNode* op,
    MessageQueue* q,
    void* userParameter)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardForAggregationCallback");

    CIMOperationRequestDispatcher* service =
        static_cast<CIMOperationRequestDispatcher*>(q);

    AsyncRequest* asyncRequest = static_cast<AsyncRequest*>(op->getRequest());
    AsyncReply* asyncReply = static_cast<AsyncReply*>(op->removeResponse());

    OperationAggregate* poA =
        reinterpret_cast<OperationAggregate*>(userParameter);

    PEGASUS_ASSERT(asyncRequest != 0);
    PEGASUS_ASSERT(asyncReply != 0);

    PEGASUS_ASSERT(poA != 0);
    PEGASUS_ASSERT(poA->valid());

    CIMResponseMessage* response = 0;

    MessageType msgType = asyncReply->getType();

    if (msgType == ASYNC_ASYNC_LEGACY_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage*>(
            (static_cast<AsyncLegacyOperationResult*>(asyncReply))->
                get_result());
    }
    else if (msgType == ASYNC_ASYNC_MODULE_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage*>(
            (static_cast<AsyncModuleOperationResult*>(asyncReply))->
                get_result());
    }
    else
    {
        // This should never happen.
        PEGASUS_UNREACHABLE(PEGASUS_ASSERT(false);)
    }

    PEGASUS_ASSERT(response != 0);
    PEGASUS_ASSERT(response->messageId == poA->_messageId);
    delete asyncReply;

    // Before resequencing, the isComplete() flag represents the completion
    // status of one provider's response, not the entire response

    Boolean thisResponseIsComplete = response->isComplete();
    if (thisResponseIsComplete)
    {
        // these are per provider instantiations
        op->removeRequest();
        delete asyncRequest;
        service->return_op(op);
    }

    // After resequencing, this flag represents the completion status of
    // the ENTIRE response to the request.

    Boolean entireResponseIsComplete = service->_enqueueResponse(poA, response);

    if (entireResponseIsComplete)
    {
        // delete OperationAggregation and attached request.
        delete poA;
        poA = 0;
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "Provider thisResponse = %s. Entire response = %s",
        (thisResponseIsComplete? "complete": "incomplete"),
        (entireResponseIsComplete? "complete": "incomplete")  ));

    PEG_METHOD_EXIT();
}


/*  Dispatcher Callback function for nonAggregation calls.
    The userParameter contains the request message for this operation
*/
void CIMOperationRequestDispatcher::_forwardRequestCallback(
    AsyncOpNode* op,
    MessageQueue* q,
    void* userParameter)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestCallback");

    CIMOperationRequestDispatcher* service =
        static_cast<CIMOperationRequestDispatcher*>(q);

    AsyncRequest* asyncRequest =
        static_cast<AsyncRequest*>(op->removeRequest());
    AsyncReply* asyncReply = static_cast<AsyncReply*>(op->removeResponse());

    CIMOperationRequestMessage* request =
        reinterpret_cast<CIMOperationRequestMessage*>(userParameter);
    PEGASUS_ASSERT(request != 0);

    CIMResponseMessage* response = 0;

    MessageType msgType = asyncReply->getType();

    if (msgType == ASYNC_ASYNC_LEGACY_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage*>(
            (static_cast<AsyncLegacyOperationResult*>(asyncReply))->
                get_result());
    }
    else if (msgType == ASYNC_ASYNC_MODULE_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage*>(
            (static_cast<AsyncModuleOperationResult*>(asyncReply))->
                get_result());
    }
    else
    {
        // Error
    }

    PEGASUS_ASSERT(response != 0);

    Boolean isComplete = response->isComplete();

    service->_enqueueResponse(request, response);

    if (isComplete == true)
    {
        delete request;
        delete asyncRequest;
        delete asyncReply;
        service->return_op(op);
    }

    PEG_METHOD_EXIT();
}

/***************************************************************************
**
**  The _forwardRequest* functions forward CIMOperation Requests to
**  other services for processing. Normally this will be either
**  Control Providers, services, or Provider Managers
**
***************************************************************************/

/*  Forward requests for response aggregation( enums, etc. that
    call multiple providers) to other services (providerManager, etc).
    (i.e. requests where the callback is the function
    _forwardForAggregationCallback. These requests include an
    OperationAggregate structure that controls response handling.

    This function decides based on the controlProviderName Field
    whether to forward to Service or ControlProvider.
    If controlProviderName String empty, ToService, else toControlProvider.

    If a response is provided with the input, the caller wants to execute only
    the callback asychnonously but not call another service.
*/
void CIMOperationRequestDispatcher::_forwardRequestForAggregation(
    Uint32 serviceId,
    const String& controlProviderName,
    CIMOperationRequestMessage* request,
    OperationAggregate* poA,
    CIMResponseMessage* response)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestForAggregation");

    PEGASUS_ASSERT(serviceId);

    AsyncOpNode* op = this->get_op();

    // if a response is provided, execute only the asynchronous callback,
    // rather than forward to the provider.
    if (response)
    {
        // constructor of object is putting itself into a linked list
        // DO NOT remove the new operator
        new AsyncLegacyOperationResult(op, response);

        // Setting this to complete, allows ONLY the callback to run
        // without going through the typical async request apparatus
        op->complete();
    }

    // If ControlProviderName empty, forward to service.
    if (0 == controlProviderName.size())
    {
        // constructor of object is putting itself into a linked list
        // DO NOT remove the new operator
        new AsyncLegacyOperationStart(op,serviceId,request);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
            "Forwarding %s to service %s. Response should go to queue %s.",
            MessageTypeToString(request->getType()),
            _getServiceName(serviceId),
            ((MessageQueue::lookup(request->queueIds.top())) ?
            ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) :
                   "BAD queue name")));
    }
    else
    {
        // constructor of object is putting itself into a linked list
        // DO NOT remove the new operator
        new AsyncModuleOperationStart(
            op,
            serviceId,
            controlProviderName,
            request);

       PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
           "Forwarding %s to service %s, control provider %s. "
           "Response should go to queue %s.",
           MessageTypeToString(request->getType()),
           _getServiceName(serviceId),
           CSTRING(controlProviderName),
           ((MessageQueue::lookup(request->queueIds.top())) ?
           ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) :
                  "BAD queue name")));
    }

    SendAsync(
        op,
        serviceId,
        CIMOperationRequestDispatcher::_forwardForAggregationCallback,
        this,
        poA);

    PEG_METHOD_EXIT();
}

/** _forwardRequestToProvider
    This function forwards the request to a single provider,
    control provider or service. It decides based on
    the controlProviderName parameter whether to forward to
    Service/controlProvider or to the provider manager service.
    If controlProviderName String empty,
           ForwardToProviderManagerService,
       else
           ForwardtoControlProvider/service
    As part of that forwarding process in defines a callback
    function, _forwardRequestCallback(...) for responses to the
    request.
*/
void CIMOperationRequestDispatcher::_forwardRequestToProvider(
    const ProviderInfo& providerInfo,
    CIMOperationRequestMessage* request,
    CIMOperationRequestMessage* requestCopy)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToProvider");

    PEGASUS_ASSERT(providerInfo.serviceId);

    AsyncOpNode* op = this->get_op();

    // If ControlProviderName empty, forward to service defined on input
    if (providerInfo.controlProviderName == String::EMPTY)
    {
        AsyncLegacyOperationStart* asyncRequest =
            new AsyncLegacyOperationStart(
                op,
                providerInfo.serviceId,
                request);

        asyncRequest->dest = providerInfo.serviceId;
    }
    // control provider name exists.
    else
    {
        // constructor of object is putting itself into a linked list
        // DO NOT remove the new operator
        new AsyncModuleOperationStart(
            op,
            providerInfo.serviceId,
            providerInfo.controlProviderName,
            request);
    }
    // Forward the request asynchronously with call back to
    // _forwardRequestCallback()
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "Forwarding %s on class %s to service %s, control provider %s. "
        "Response to queue %s.",
        MessageTypeToString(request->getType()),
        CSTRING(providerInfo.className.getString()),
        _getServiceName(providerInfo.serviceId),
        (CSTRING(providerInfo.controlProviderName)),
        ((MessageQueue::lookup(request->queueIds.top())) ?
        ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) :
               "BAD queue name")));

    SendAsync(
       op,
       providerInfo.serviceId,
       CIMOperationRequestDispatcher::_forwardRequestCallback,
       this,
       requestCopy);

    PEG_METHOD_EXIT();
}

/*
    Enqueue an Exception response
    This is a helper function that creates a response message
    with the defined exception and queues it.
*/
void CIMOperationRequestDispatcher::_enqueueExceptionResponse(
   CIMOperationRequestMessage* request,
   CIMException& exception)
{
    CIMResponseMessage* response = request->buildResponse();
    response->cimException = exception;
    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::_enqueueExceptionResponse(
   CIMOperationRequestMessage* request,
   TraceableCIMException& exception)
{
    CIMResponseMessage* response = request->buildResponse();
    response->cimException = exception;
    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::_enqueueExceptionResponse(
    CIMOperationRequestMessage* request,
    CIMStatusCode code,
    const String& ExtraInfo)
{
    CIMException exception = PEGASUS_CIM_EXCEPTION(code, ExtraInfo);
    _enqueueExceptionResponse(request, exception);
}

/*
   Enqueue the response provided with the call
   Logs this operation, assures resquest and response
   attributes are syncd, gets queue from request,
   gets queue name from request,
   if internal client (queuename) does
       base::_enqueueResponse(request,response)
   else
      calls queue->enqueue(response)
*/
void CIMOperationRequestDispatcher::_enqueueResponse(
   CIMOperationRequestMessage* request,
   CIMResponseMessage* response)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_enqueueResponse");

    response->dest = request->queueIds.top();

    // Ensure the response message attributes are synchonized with the request
    PEGASUS_ASSERT(request->getMask() == response->getMask());
    PEGASUS_ASSERT(request->getHttpMethod() == response->getHttpMethod());
    PEGASUS_ASSERT(request->getCloseConnect() == response->getCloseConnect());

    PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
        "_CIMOperationRequestDispatcher::_enqueueResponse - "
            "request->getCloseConnect() returned %d",
        request->getCloseConnect()));

    _logOperation(request, response);

    MessageQueue* queue = MessageQueue::lookup(request->queueIds.top());
    PEGASUS_ASSERT(queue != 0);

    queue->enqueue(response);

    PEG_METHOD_EXIT();
}

/*
    handleEnqueue is the Request input processor to the Dispatcher.
    It processes all incoming request messages and distributes them to
    the appropriate request handler functions based on the operation type.
*/
void CIMOperationRequestDispatcher::handleEnqueue(Message* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnqueue(Message* request)");

    PEGASUS_ASSERT(request != 0);
    PEGASUS_DEBUG_ASSERT(request->magic);

    PEG_TRACE(( TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleEnqueue - Case: %u",
        request->getType() ));

    CIMOperationRequestMessage* opRequest =
        dynamic_cast<CIMOperationRequestMessage*>(request);

    if (!opRequest)
    {
        PEG_TRACE(( TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Ignored unexpected message of type %u in "
                "CIMOperationRequestDispatcher::handleEnqueue",
            request->getType() ));
        delete request;
        PEG_METHOD_EXIT();
        return;
    }

    //
    // This try/catch block ensures that any exception raised during the
    // processing of an operation is handled and translated into an operation
    // response.
    //

    CIMException cimException;

    try
    {
        // Set the client's requested language into this service thread.
        // This will allow functions in this service to return messages
        // in the correct language.
        opRequest->updateThreadLanguages();

        switch (opRequest->getType())
        {
        case CIM_GET_CLASS_REQUEST_MESSAGE:
            handleGetClassRequest((CIMGetClassRequestMessage*)opRequest);
            break;

        case CIM_GET_INSTANCE_REQUEST_MESSAGE:
            handleGetInstanceRequest((CIMGetInstanceRequestMessage*)opRequest);
            break;

        case CIM_DELETE_CLASS_REQUEST_MESSAGE:
            handleDeleteClassRequest(
                (CIMDeleteClassRequestMessage*)opRequest);
            break;

        case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
            handleDeleteInstanceRequest(
                (CIMDeleteInstanceRequestMessage*)opRequest);
            break;

        case CIM_CREATE_CLASS_REQUEST_MESSAGE:
            handleCreateClassRequest((CIMCreateClassRequestMessage*)opRequest);
            break;

        case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
            handleCreateInstanceRequest(
                (CIMCreateInstanceRequestMessage*)opRequest);
            break;

        case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
            handleModifyClassRequest((CIMModifyClassRequestMessage*)opRequest);
            break;

        case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
            handleModifyInstanceRequest(
                (CIMModifyInstanceRequestMessage*)opRequest);
            break;

        case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
            handleEnumerateClassesRequest(
                (CIMEnumerateClassesRequestMessage*)opRequest);
            break;

        case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
            handleEnumerateClassNamesRequest(
                (CIMEnumerateClassNamesRequestMessage*)opRequest);
            break;

        case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
            handleEnumerateInstancesRequest(
                (CIMEnumerateInstancesRequestMessage*)opRequest);
            break;

        case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
            handleEnumerateInstanceNamesRequest(
                (CIMEnumerateInstanceNamesRequestMessage*)opRequest);
            break;

        case CIM_EXEC_QUERY_REQUEST_MESSAGE:
            handleExecQueryRequest(
                (CIMExecQueryRequestMessage*)opRequest);
            break;

        case CIM_ASSOCIATORS_REQUEST_MESSAGE:
            handleAssociatorsRequest((CIMAssociatorsRequestMessage*)opRequest);
            break;

        case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
            handleAssociatorNamesRequest(
                (CIMAssociatorNamesRequestMessage*)opRequest);
            break;

        case CIM_REFERENCES_REQUEST_MESSAGE:
            handleReferencesRequest((CIMReferencesRequestMessage*)opRequest);
            break;

        case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
            handleReferenceNamesRequest(
                (CIMReferenceNamesRequestMessage*)opRequest);
            break;

        case CIM_GET_PROPERTY_REQUEST_MESSAGE:
            handleGetPropertyRequest(
                (CIMGetPropertyRequestMessage*)opRequest);
            break;

        case CIM_SET_PROPERTY_REQUEST_MESSAGE:
            handleSetPropertyRequest(
                (CIMSetPropertyRequestMessage*)opRequest);
            break;

        case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
            handleGetQualifierRequest(
                (CIMGetQualifierRequestMessage*)opRequest);
            break;

        case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
            handleSetQualifierRequest(
                (CIMSetQualifierRequestMessage*)opRequest);
            break;

        case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
            handleDeleteQualifierRequest(
                (CIMDeleteQualifierRequestMessage*)opRequest);
            break;

        case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
            handleEnumerateQualifiersRequest(
                (CIMEnumerateQualifiersRequestMessage*)opRequest);
            break;

        case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
            handleInvokeMethodRequest(
                (CIMInvokeMethodRequestMessage*)opRequest);
            break;

        default:
            PEGASUS_UNREACHABLE(PEGASUS_ASSERT(0);)
        }
    }
    catch (const CIMException& exception)
    {
        cimException = exception;
    }
    catch (const Exception& exception)
    {
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
    }
    catch (...)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    if (cimException.getCode() != CIM_ERR_SUCCESS)
    {
        AutoPtr<CIMResponseMessage> response(opRequest->buildResponse());
        response->cimException = cimException;
        _enqueueResponse(opRequest, response.release());
    }

    delete request;
    PEG_METHOD_EXIT();
}

/*
    CIMOperationDispatcher dequeue function to dequeue the
    next input operation request and send to the handle functions.
*/
void CIMOperationRequestDispatcher::handleEnqueue()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnqueue");

    Message* request = dequeue();

    if (request)
        handleEnqueue(request);

    PEG_METHOD_EXIT();
}

/****************************************************************************
**
**     Request Parameter Test and reject functions
**     Each function tests a particular possible reason for reject.
**     If the test fails, the reject message is generated and
**     a true response returned.  If the test passes, true returned.
**     Generally named with the prefix reject to indicate that they
**     reject and return true.
**     Each function should terminate the operation processing if true
**     is returned.
**
****************************************************************************/

// Test to determine if Association traversal is enabled.
// returns true if Not Enabled, false if enabled
Boolean CIMOperationRequestDispatcher::_rejectAssociationTraversalDisabled(
    CIMOperationRequestMessage* request,
    const String& opName)
{
    if (_enableAssociationTraversal)
    {
        // return enabled
        return false;
    }
    else
    {
        _enqueueExceptionResponse(request,
            CIM_ERR_NOT_SUPPORTED, opName);
        return true;
    }
}


/* Test the roleParameter to determine if it exists and is a valid CIMName.
   Generate error if it exists and is not a valid CIMName.
   @return true if invalid and false if valid
*/
Boolean CIMOperationRequestDispatcher::_rejectInvalidRoleParameter(
    CIMOperationRequestMessage* request,
    const String& roleParameter,
    const String& parameterName)
{
    if (roleParameter != String::EMPTY && (!CIMName::legal(roleParameter)))
    {
        _enqueueExceptionResponse(request,
            CIM_ERR_INVALID_PARAMETER, roleParameter);

        return true;
    }
    return false;
}


// _rejectEnumerateTooBroad.  Checks providerCount against parameter. Generates
// exception of providerCount to large.
// This limits the number of provider invocations, not the number
// of instances returned. It throws exception because request not available
// in function where called.
void CIMOperationRequestDispatcher::_rejectEnumerateTooBroad(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Uint32 providerCount)
{
    if (providerCount > _maximumEnumerateBreadth)
    {
        PEG_TRACE((TRC_DISPATCHER,  Tracer::LEVEL1,
            "Operation too broad for class %s.  "
                "  Namespace: %s  Limit = %u, providerCount = %u",
            CSTRING(className.getString()),
            CSTRING(nameSpace.getString()),
            _maximumEnumerateBreadth,
            providerCount));

        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,
            MessageLoaderParms(
                "Server.CIMOperationRequestDispatcher.ENUM_REQ_TOO_BROAD",
                "Enumerate request too Broad"));
    }
}

Boolean CIMOperationRequestDispatcher::_rejectInvalidClassParameter(
    CIMOperationRequestMessage* request,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    CIMConstClass& targetClass)
{
    CIMException checkClassException;

    targetClass = _getClass(
        nameSpace,
        className,
        checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "CIMOperationRequestDispatcher - "
                "CIM class exist exception has occurred.  Namespace: %s  "
                "Class Name: %s  Exception message: \"%s\"",
            CSTRING(nameSpace.getString()),
            CSTRING(className.getString()),
            CSTRING(checkClassException.getMessage())));

        _enqueueExceptionResponse(request,checkClassException);
        return true;
    }
    return false;
}

Boolean CIMOperationRequestDispatcher::_rejectNoProvidersOrRepository(
    CIMOperationRequestMessage* request, Uint32 providerCount,
    const CIMName& className)
{
    if ((providerCount == 0) &&
        !(_repository->isDefaultInstanceProvider()))
    {
        _enqueueExceptionResponse(request,
            CIM_ERR_NOT_SUPPORTED, String::EMPTY);
        return true;
    }
    else   // We have either providers or a repository
    {
        return false;
    }
}

/*
    Test the validity of the class name parameter and get the corresponding
    class. If not found, generates exception INVALID_PARAMETER response
    and returns true.
    @param request
    @param objectName which contains target className
    @param namespace CIMNamespaceName for this operation
    @return false if class found or true if class not found
*/

Boolean CIMOperationRequestDispatcher::_rejectInvalidClassParameter(
    CIMOperationRequestMessage* request,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName)
{
    if (!_checkExistenceOfClass(nameSpace, objectName.getClassName()))
    {
        _enqueueExceptionResponse(request,
            CIM_ERR_INVALID_PARAMETER,
            objectName.getClassName().getString());
        return true;
    }
    return false;
}

/*  Build a property list of the properties in the class provided as an
    argument.
    @param class CIMClass with the properties to be put in list
    @return propertyList containing the properties in the class.
*/
void _buildPropertyListFromClass(CIMConstClass& thisClass,
                                 CIMPropertyList& propertyList)
{
    Array<String> pla;
    Uint32 numProperties = thisClass.getPropertyCount();
    for (Uint32 i = 0; i < numProperties; i++)
    {
        pla.append(thisClass.getProperty(i).getName().getString());
    }
    propertyList.append(pla);
}

/****************************************************************************
**
** CIMOperationDispatcher request handlers.  There is a handler for each
** operation request type. These handlers process the requsts including:
**     - Validation of parameters (may generate error responses)
**     - Passage to the correct processor (service, control providers,
**       providers, repository) depending on message type)
**     - Generation of responses for those handlers that are synchronous.
**
****************************************************************************/

/**$*******************************************************
    handleGetClassRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleGetClassRequest(
    CIMGetClassRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleGetClassRequest");

    CIMClass cimClass =
        _repository->getClass(
            request->nameSpace,
            request->className,
            request->localOnly,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleGetClassRequest - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->className.getString())));

    AutoPtr<CIMGetClassResponseMessage> response(
        dynamic_cast<CIMGetClassResponseMessage*>(
            request->buildResponse()));
    response->cimClass = cimClass;

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleGetInstanceRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleGetInstanceRequest(
    CIMGetInstanceRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleGetInstanceRequest");

    // get the class name
    CIMName className = request->instanceName.getClassName();

    // Validate the class in the request. Returns class if not rejected
    CIMConstClass cimClass;
    if (_rejectInvalidClassParameter(request,request->nameSpace,
        request->className, cimClass))
    {
        PEG_METHOD_EXIT();
        return;
    }

    ProviderInfo providerInfo = _lookupInstanceProvider(
        request->nameSpace,
        className);

    if (providerInfo.hasProvider)
    {
        CIMGetInstanceRequestMessage* requestCopy =
            new CIMGetInstanceRequestMessage(*request);

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
        }

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
        if (providerInfo.hasProviderNormalization)
        {
            requestCopy->operationContext.insert(
                CachedClassDefinitionContainer(cimClass));
        }
#endif

        CIMGetInstanceRequestMessage* requestCallbackCopy =
            new CIMGetInstanceRequestMessage(*requestCopy);

        _forwardRequestToProvider(
            providerInfo,
            requestCopy,
            requestCallbackCopy);

         PEG_METHOD_EXIT();
         return;
    }

    // not internal or found provider, go to default

    if (_repository->isDefaultInstanceProvider())
    {
        CIMInstance cimInstance =
            _repository->getInstance(
                request->nameSpace,
                request->instanceName,
                request->includeQualifiers,
                request->includeClassOrigin,
                request->propertyList);

        AutoPtr<CIMGetInstanceResponseMessage> response(
            dynamic_cast<CIMGetInstanceResponseMessage*>(
                request->buildResponse()));
        response->getResponseData().setInstance(cimInstance);

        _enqueueResponse(request, response.release());
    }
    else // No provider is registered and the repository isn't the default
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        _enqueueResponse(request, response);
    }

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleDeleteClassRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleDeleteClassRequest(
    CIMDeleteClassRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleDeleteClassRequest");

    _repository->deleteClass(
        request->nameSpace,
        request->className);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleDeleteClassRequest - "
            "Namespace: %s  Class Name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->className.getString())));

    AutoPtr<CIMDeleteClassResponseMessage> response(
        dynamic_cast<CIMDeleteClassResponseMessage*>(
            request->buildResponse()));

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleDeleteInstanceRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleDeleteInstanceRequest(
    CIMDeleteInstanceRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleDeleteInstanceRequest");

    // get the class name
    CIMName className = request->instanceName.getClassName();

    if (!_checkExistenceOfClass(request->nameSpace, className))
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::handleDeleteInstanceRequest - "
                "CIM class does not exist exception has occurred.  "
                "Namespace: %s  Class Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(className.getString())));

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_CLASS, className.getString());
    }

    ProviderInfo providerInfo = _lookupInstanceProvider(
        request->nameSpace,
        className);

    if (providerInfo.hasProvider)
    {
        CIMDeleteInstanceRequestMessage* requestCopy =
            new CIMDeleteInstanceRequestMessage(*request);

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());

            //delete providerIdContainer;
            //providerIdContainer = 0;
        }

        CIMDeleteInstanceRequestMessage* requestCallbackCopy =
            new CIMDeleteInstanceRequestMessage(*requestCopy);

        _forwardRequestToProvider(
            providerInfo,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        return;
    }
    else if (_repository->isDefaultInstanceProvider())
    {
        _repository->deleteInstance(
            request->nameSpace,
            request->instanceName);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
            "CIMOperationRequestDispatcher::handleDeleteInstanceRequest - "
                "Namespace: %s  Instance Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(request->instanceName.toString())));

        AutoPtr<CIMDeleteInstanceResponseMessage> response(
            dynamic_cast<CIMDeleteInstanceResponseMessage*>(
                request->buildResponse()));

        _enqueueResponse(request, response.release());
    }
    else // No provider is registered and the repository isn't the default
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        _enqueueResponse(request, response);
    }

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleCreateClassRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleCreateClassRequest(
    CIMCreateClassRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleCreateClassRequest");

    removePropagatedAndOriginAttributes(request->newClass);

    _repository->createClass(request->nameSpace, request->newClass);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleCreateClassRequest - "
            "Namespace: %s  Class Name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->className.getString())));

    AutoPtr<CIMCreateClassResponseMessage> response(
        dynamic_cast<CIMCreateClassResponseMessage*>(
            request->buildResponse()));

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleCreateInstanceRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleCreateInstanceRequest(
    CIMCreateInstanceRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleCreateInstanceRequest()");

    // get the class name
    CIMName className = request->newInstance.getClassName();

    if (!_checkExistenceOfClass(request->nameSpace, className))
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::handleCreateInstanceRequest - "
                "CIM class does not exist exception has occurred.  "
                "Namespace: %s  Class Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(className.getString())));

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_CLASS, className.getString());
    }

    ProviderInfo providerInfo = _lookupInstanceProvider(
        request->nameSpace,
        className);

    if (providerInfo.hasProvider)
    {
        CIMCreateInstanceRequestMessage* requestCopy =
            new CIMCreateInstanceRequestMessage(*request);

        removePropagatedAndOriginAttributes(requestCopy->newInstance);

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
        }

        CIMCreateInstanceRequestMessage* requestCallbackCopy =
            new CIMCreateInstanceRequestMessage(*requestCopy);

        _forwardRequestToProvider(
            providerInfo,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        return;
    }
    else if (_repository->isDefaultInstanceProvider())
    {
        removePropagatedAndOriginAttributes(request->newInstance);

        CIMObjectPath instanceName = _repository->createInstance(
            request->nameSpace,
            request->newInstance);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
            "CIMOperationRequestDispatcher::handleCreateInstanceRequest - "
                "Namespace: %s  Instance Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(request->newInstance.getClassName().getString())));

        AutoPtr<CIMCreateInstanceResponseMessage> response(
            dynamic_cast<CIMCreateInstanceResponseMessage*>(
                request->buildResponse()));
        response->instanceName = instanceName;

        _enqueueResponse(request, response.release());
    }
    else // No provider is registered and the repository isn't the default
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        _enqueueResponse(request, response);
    }

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleModifyClassRequest
    This request is analyzed and if correct, is passed directly to the
    repository
**********************************************************/
void CIMOperationRequestDispatcher::handleModifyClassRequest(
    CIMModifyClassRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
       "CIMOperationRequestDispatcher::handleModifyClassRequest");

    removePropagatedAndOriginAttributes(request->modifiedClass);

    _repository->modifyClass(request->nameSpace, request->modifiedClass);

    AutoPtr<CIMModifyClassResponseMessage> response(
        dynamic_cast<CIMModifyClassResponseMessage*>(
            request->buildResponse()));

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleModifyInstanceRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleModifyInstanceRequest(
    CIMModifyInstanceRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleModifyInstanceRequest");

    CIMName className = request->modifiedInstance.getClassName();

    if (!_checkExistenceOfClass(request->nameSpace, className))
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::handleModifyInstanceRequest - "
                "CIM class does not exist exception has occurred.  "
                "Namespace: %s  Class Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(className.getString())));

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_CLASS, className.getString());
    }

    ProviderInfo providerInfo = _lookupInstanceProvider(
        request->nameSpace,
        className);

    if (providerInfo.hasProvider)
    {
        CIMModifyInstanceRequestMessage* requestCopy =
            new CIMModifyInstanceRequestMessage(*request);
        removePropagatedAndOriginAttributes(requestCopy->modifiedInstance);
        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
        }

        CIMModifyInstanceRequestMessage* requestCallbackCopy =
            new CIMModifyInstanceRequestMessage(*requestCopy);

        _forwardRequestToProvider(
            providerInfo,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        return;
    }
    else if (_repository->isDefaultInstanceProvider())
    {
        removePropagatedAndOriginAttributes(request->modifiedInstance);

        _repository->modifyInstance(
            request->nameSpace,
            request->modifiedInstance,
            request->includeQualifiers,request->propertyList);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
            "CIMOperationRequestDispatcher::handleModifyInstanceRequest - "
                "Namespace: %s  Instance Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(request->modifiedInstance.getClassName().getString())));

        AutoPtr<CIMModifyInstanceResponseMessage> response(
            dynamic_cast<CIMModifyInstanceResponseMessage*>(
                request->buildResponse()));

        _enqueueResponse(request, response.release());
    }
    else // No provider is registered and the repository isn't the default
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

       _enqueueResponse(request, response);
    }

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleEnumerateClassesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleEnumerateClassesRequest(
    CIMEnumerateClassesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnumerateClassesRequest");

    Array<CIMClass> cimClasses =
        _repository->enumerateClasses(
            request->nameSpace,
            request->className,
            request->deepInheritance,
            request->localOnly,
            request->includeQualifiers,
            request->includeClassOrigin);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleEnumerateClassesRequest - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->className.getString())));

    AutoPtr<CIMEnumerateClassesResponseMessage> response(
        dynamic_cast<CIMEnumerateClassesResponseMessage*>(
            request->buildResponse()));
    response->cimClasses = cimClasses;

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleEnumerateClassNamesRequest
**********************************************************/
void CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest(
    CIMEnumerateClassNamesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest");

    Array<CIMName> classNames =
        _repository->enumerateClassNames(
            request->nameSpace,
            request->className,
            request->deepInheritance);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->className.getString())));

    AutoPtr<CIMEnumerateClassNamesResponseMessage> response(
        dynamic_cast<CIMEnumerateClassNamesResponseMessage*>(
            request->buildResponse()));
    response->classNames = classNames;

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleEnumerateInstancesRequest

    if !validClassName
        generate exception response
        return
    get all subclasses to target class
    for all classes
        get Provider for Class
    if (number of providers > BreadthLimit)
        generate exception
        return
    if (no providers found) and !(repository is default provider)
        generate CIM_ERR_NOT_SUPPORTED response
        return
    for all targetclass and subclasses
        if (class has a provider)
            copy request
            substitute current class name
            forward request to provider
    if (repository is default provider)
        for all targetclass and subclasses
            if !(class has a provider)
                issue request for this class to repository
                put response on aggregate list
**********************************************************/

void CIMOperationRequestDispatcher::handleEnumerateInstancesRequest(
    CIMEnumerateInstancesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnumerateInstancesRequest");

    //
    // Validate the class in the request and get the target class to
    // be used later in the operation.
    //
    CIMConstClass cimClass;
    if (_rejectInvalidClassParameter(request,request->nameSpace,
            request->className,
        cimClass))
        {
            PEG_METHOD_EXIT();
            return;
        }

    // Set the propertylist to be forwarded in accord with the propertyList
    // parameter. The property list forwarded to providers
    // is constructed from properties in the top level class  when
    // !deepInheritance && propertyList.isNull() to assure
    // that providers will return only the properties of the requested classpr
    // NOTE - The request class pl should really be null.  Subclasses should
    // get the propertyList. But we do not have a record today of the
    // original class in the providerInfo list so everybody gets the
    // list.

        if (!request->deepInheritance && request->propertyList.isNull())
        {
            _buildPropertyListFromClass(cimClass, request->propertyList);
        }

    //
    // Get names of descendent classes and their providers
    //
    ProviderInfoList providerInfos = _lookupAllInstanceProviders(
        request->nameSpace,
        request->className);

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED

    if (_rejectNoProvidersOrRepository(request, providerInfos.providerCount,
                                       request->className))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Set up an aggregate object with a copy of the original request.
    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstancesRequestMessage(*request),
        request->getType(),
        request->messageId,
        request->queueIds.top(),
        request->className);

    Uint32 numClasses = providerInfos.size();

    // Gather the repository responses and send as one response
    // with many instances
    if (_repository->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        for (Uint32 i = 0; i < numClasses; i++)
        {
            ProviderInfo& providerInfo = providerInfos[i];

            // this class is registered to a provider - skip
            if (providerInfo.hasProvider)
            {
                continue;
            }

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Routing EnumerateInstances request for class %s to the "
                    "repository.  Class # %u of %u",
                CSTRING(providerInfo.className.getString()),
                (i + 1),
                numClasses ));

            AutoPtr<CIMEnumerateInstancesResponseMessage> response(
                dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
                    request->buildResponse()));

            try
            {
                // Enumerate instances only for this class
                response->getResponseData().setInstances(
                    _repository->enumerateInstancesForClass(
                        request->nameSpace,
                        providerInfo.className,
                        request->includeQualifiers,
                        request->includeClassOrigin,
                        request->propertyList));
            }
            catch (const CIMException& exception)
            {
                response->cimException = exception;
            }
            catch (const Exception& exception)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                    exception.getMessage());
            }
            catch (...)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                    String::EMPTY);
            }

            poA->appendResponse(response.release());
        } // for all classes in ProviderInfoList

        Uint32 numberResponses = poA->numberResponses();
        Uint32 totalIssued = providerInfos.providerCount
            + (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        if (numberResponses > 0)
        {
            handleOperationResponseAggregation(poA,false,true);

            CIMResponseMessage* response = poA->removeResponse(0);

            _forwardRequestForAggregation(
                getQueueId(),
                String(),
                new CIMEnumerateInstancesRequestMessage(*request),
                poA,
                response);
        }
    } // if isDefaultInstanceProvider
    else
    {
        // Set the number of expected responses in the OperationAggregate
        poA->setTotalIssued(providerInfos.providerCount);
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "Forward requests to a subset of %u classes.", numClasses));

    // Loop through providerInfos, forwarding requests to providers
    for (Uint32 i = 0; i < numClasses; i++)
    {
        ProviderInfo& providerInfo = providerInfos[i];

        // this class is NOT registered to a provider - skip
        if (providerInfo.hasProvider)
        {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Routing EnumerateInstances request for class %s to "
                    "service \"%s\" for control provider \"%s\".  "
                    "Class # %u of %u",
                CSTRING(providerInfo.className.getString()),
                _getServiceName(providerInfo.serviceId),
                CSTRING(providerInfo.controlProviderName),
                i + 1,
                numClasses ));

            CIMEnumerateInstancesRequestMessage* requestCopy =
                new CIMEnumerateInstancesRequestMessage(*request);

            requestCopy->className = providerInfo.className;

            // Get the class to confirm it exists and for
            // normalization if required.
            CIMException checkClassException;

            CIMConstClass cimClass = _getClass(
                request->nameSpace,
                providerInfo.className,
                checkClassException);

            if (checkClassException.getCode() != CIM_ERR_SUCCESS)
            {
                CIMResponseMessage* response = request->buildResponse();

                _forwardRequestForAggregation(
                    getQueueId(),
                    String(),
                    new CIMEnumerateInstancesRequestMessage(*request),
                    poA,
                    response);
            }
            else
            {
                if (providerInfo.providerIdContainer.get() != 0)
                {
                    requestCopy->operationContext.insert(
                        *(providerInfo.providerIdContainer.get()));
                }

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
                if (providerInfo.hasProviderNormalization)
                {
                    requestCopy->operationContext.insert(
                        CachedClassDefinitionContainer(cimClass));
                }
#endif
                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,"PropertyList = %s",
                    CSTRING(_showPropertyList(requestCopy->propertyList)) ));

                _forwardRequestForAggregation(
                    providerInfo.serviceId,
                    providerInfo.controlProviderName,
                    requestCopy,
                    poA);
            }
        }
    } // for all classes and dervied classes

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleEnumerateInstanceNamesRequest

    if !validClassName
        generate exception response
        return
    get all subclasses to target class
    for all classes
        get Provider for Class
    if (number of providers > BreadthLimit)
        generate exception
        return
    if (no providers found) and !(repository is default provider)
        generate CIM_ERR_NOT_SUPPORTED response
        return
    for all targetclass and subclasses
        if (class has a provider)
            copy request
            substitute current class name
            forward request to provider
    if (repository is default provider)
        for all targetclass and subclasses
            if !(class has a provider)
                issue request for this class to repository
                put response on aggregate list
**********************************************************/

void CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest(
    CIMEnumerateInstanceNamesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest");

    //
    // Validate the class name
    //
    // Validate the class in the request
    CIMConstClass cimClass;
    if (_rejectInvalidClassParameter(request,request->nameSpace,
        request->className, cimClass))
    {
            PEG_METHOD_EXIT();
            return;
        }

    //
    // Get names of descendent classes and list of providers
    //
    ProviderInfoList providerInfos = _lookupAllInstanceProviders(
        request->nameSpace,
        request->className);

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED

    if (_rejectNoProvidersOrRepository(request,
                                       providerInfos.providerCount,
                                       request->className))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // We have instances for Providers and possibly repository.
    // Set up an aggregate object and save a copy of the original request.
    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstanceNamesRequestMessage(*request),
        request->getType(),
        request->messageId,
        request->queueIds.top(),
        request->className);

    Uint32 numClasses = providerInfos.size();

    if (_repository->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        for (Uint32 i = 0; i < numClasses; i++)
        {
            ProviderInfo& providerInfo = providerInfos[i];

            // this class is registered to a provider - skip
            if (providerInfo.hasProvider)
            {
                continue;
            }

            // If this class does not have a provider

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Routing EnumerateInstanceNames request for class %s to the "
                    "repository.  Class # %u of %u",
                CSTRING(providerInfo.className.getString()),
                i + 1,
                numClasses ));

            AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response(
                dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
                    request->buildResponse()));

            try
            {
                // Enumerate instances names only for this class
                const Array<CIMObjectPath>& cops =
                    _repository->enumerateInstanceNamesForClass(
                        request->nameSpace,
                        providerInfo.className);
                response->getResponseData().setInstanceNames(cops);
            }
            catch (const CIMException& exception)
            {
                response->cimException = exception;
            }
            catch (const Exception& exception)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED, exception.getMessage());
            }
            catch (...)
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED, String::EMPTY);
            }

            poA->appendResponse(response.release());
        } // for all classes and derived classes

        Uint32 numberResponses = poA->numberResponses();
        Uint32 totalIssued = providerInfos.providerCount
            + (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        if (numberResponses > 0)
        {
            handleOperationResponseAggregation(poA,false,false);

            CIMResponseMessage* response = poA->removeResponse(0);

            _forwardRequestForAggregation(
                getQueueId(),
                String(),
                new CIMEnumerateInstanceNamesRequestMessage(*request),
                poA,
                response);
        }
    } // if isDefaultInstanceProvider
    else
    {
        // Set the number of expected responses in the OperationAggregate
        poA->setTotalIssued(providerInfos.providerCount);
    }

    // Loop through providerInfos, forwarding requests to providers
    for (Uint32 i = 0; i < numClasses; i++)
    {
        ProviderInfo& providerInfo = providerInfos[i];

        // this class is NOT registered to a provider - skip
        if (providerInfo.hasProvider)
        {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Routing EnumerateInstanceNames request for class %s to "
                    "service \"%s\" for control provider \"%s\".  "
                    "Class # %u of %u",
                CSTRING(providerInfo.className.getString()),
                _getServiceName(providerInfo.serviceId),
                CSTRING(providerInfo.controlProviderName),
                i + 1,
                numClasses ));

            CIMEnumerateInstanceNamesRequestMessage* requestCopy =
                new CIMEnumerateInstanceNamesRequestMessage(*request);

            requestCopy->className = providerInfo.className;

            CIMException checkClassException;

            CIMConstClass cimClass = _getClass(
                request->nameSpace,
                providerInfo.className,
                checkClassException);

            // The following is not correct. Need better way to terminate.
            // This builds an error response message in the middle of the
            // normal response but lets rest of operation continue.
            if (checkClassException.getCode() != CIM_ERR_SUCCESS)
            {
                CIMResponseMessage* response = request->buildResponse();

                _forwardRequestForAggregation(
                    getQueueId(),
                    String(),
                    new CIMEnumerateInstanceNamesRequestMessage(*request),
                    poA,
                    response);
            }

            else
            {
                if (providerInfo.providerIdContainer.get() != 0)
                {
                    requestCopy->operationContext.insert(
                        *(providerInfo.providerIdContainer.get()));
                }

    #ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
                if (providerInfo.hasProviderNormalization)
                {
                    requestCopy->operationContext.insert(
                        CachedClassDefinitionContainer(cimClass));
                }
    #endif
                _forwardRequestForAggregation(
                    providerInfo.serviceId,
                    providerInfo.controlProviderName,
                    requestCopy,
                    poA);
            }
        }
    } // for all classes and derived classes

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleAssociatorsRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleAssociatorsRequest(
    CIMAssociatorsRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleAssociatorsRequest");

    if (_rejectAssociationTraversalDisabled(request, "Associators"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Validate role parameters syntax

    if (_rejectInvalidRoleParameter(request, request->role, "role"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidRoleParameter(request, request->resultRole, "resultrole"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidClassParameter(request, request->nameSpace,
                                request->objectName))
    {
        PEG_METHOD_EXIT();
        return;
    }


    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleAssociators - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    // The entity that creates the CIMMessage determines if this is a class
    // or instance operation.

    if (request->isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL3,
            "Associators executing Class request");

        Array<CIMObject> cimObjects =
            _repository->associators(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                request->resultClass,
                request->role,
                request->resultRole,
                request->includeQualifiers,
                request->includeClassOrigin,
                request->propertyList);

        AutoPtr<CIMAssociatorsResponseMessage> response(
            dynamic_cast<CIMAssociatorsResponseMessage*>(
                request->buildResponse()));

        // Flag to indicate that these are class objects is passed
        // from request in buildResponse above

        response->getResponseData().setObjects(cimObjects);

        _enqueueResponse(request, response.release());
    }
    else
    {
        //
        // Determine list of classes and providers for this request
        //
        ProviderInfoList providerInfos =
           _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                String::EMPTY);

        //
        // Get the instances from the repository, as necessary
        //

        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        AutoPtr<CIMAssociatorsResponseMessage> response;

        if (_repository->isDefaultInstanceProvider())
        {
            response.reset(dynamic_cast<CIMAssociatorsResponseMessage*>(
                request->buildResponse()));

            const Array<CIMObject>& cimObjects = _repository->associators(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                request->resultClass,
                request->role,
                request->resultRole,
                request->includeQualifiers,
                request->includeClassOrigin,
                request->propertyList);

            response->getResponseData().setObjects(cimObjects);

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Associators repository access: class = %s, count = %u.",
                    CSTRING(request->objectName.toString()),
                    response->getResponseData().getObjects().size()));
        }

        if (providerInfos.providerCount == 0)
        {
            //
            // We have no providers to call.  Just return what we have.
            //

            if (!response.get())
            {
                // No provider is registered and the repository isn't the
                // default.  Return CIM_ERR_NOT_SUPPORTED.

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
                    "CIM_ERR_NOT_SUPPORTED for %s",
                    CSTRING(request->className.getString()) ));

                response.reset(dynamic_cast<CIMAssociatorsResponseMessage*>(
                    request->buildResponse()));
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_NOT_SUPPORTED,
                    String::EMPTY);
            }

            _enqueueResponse(request, response.release());
            PEG_METHOD_EXIT();
            return;
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate* poA = new OperationAggregate(
            new CIMAssociatorsRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top(),
            request->objectName.getClassName(),
            request->nameSpace);

        // Include the repository response in the aggregation, if applicable
        if (response.get())
        {
            poA->setTotalIssued(providerInfos.providerCount+1);
            // send the repository's results
            _forwardRequestForAggregation(
                getQueueId(),
                String(),
                new CIMAssociatorsRequestMessage(*request),
                poA,
                response.release());
        }
        else
        {
            poA->setTotalIssued(providerInfos.providerCount);
        }

        for (Uint32 i = 0; i < providerInfos.size(); i++)
        {
            if (providerInfos[i].hasProvider)
            {
                CIMAssociatorsRequestMessage* requestCopy =
                    new CIMAssociatorsRequestMessage(*request);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->assocClass = providerInfos[i].className;

                if (providerInfos[i].providerIdContainer.get() != 0)
                {
                    requestCopy->operationContext.insert(
                        *(providerInfos[i].providerIdContainer.get()));
                }

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class %s",
                    CSTRING(providerInfos[i].className.getString()) ));

                _forwardRequestForAggregation(providerInfos[i].serviceId,
                    providerInfos[i].controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleAssociatorNamesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleAssociatorNamesRequest(
    CIMAssociatorNamesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleAssociatorNamesRequest");

    if (_rejectAssociationTraversalDisabled(request,"AssociatorNames"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Validate role parameters syntax

    if (_rejectInvalidRoleParameter(request, request->role, "role"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidRoleParameter(request, request->resultRole, "resultrole"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidClassParameter(request, request->nameSpace,
                                request->objectName))
    {
        PEG_METHOD_EXIT();
        return;
    }

    PEG_TRACE(( TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleAssociatorNames - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    if (request->isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL3,
            "AssociatorNames executing Class request");

        Array<CIMObjectPath> objectNames =
            _repository->associatorNames(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                request->resultClass,
                request->role,
                request->resultRole);

        AutoPtr<CIMAssociatorNamesResponseMessage> response(
            dynamic_cast<CIMAssociatorNamesResponseMessage*>(
                request->buildResponse()));

        // Flag to indicate that these are class objects is passed
        // from request in buildResponse above.

        response->getResponseData().setInstanceNames(objectNames);

        _enqueueResponse(request, response.release());
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        // Determine list of providers for this request
        //

        ProviderInfoList providerInfos = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->assocClass,
            String::EMPTY);
        //
        // Get the instances from the repository, as necessary
        //

        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        AutoPtr<CIMAssociatorNamesResponseMessage> response;

        if (_repository->isDefaultInstanceProvider())
        {
            response.reset(dynamic_cast<CIMAssociatorNamesResponseMessage*>(
                request->buildResponse()));

            Array<CIMObjectPath> objectNames = _repository->associatorNames(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                request->resultClass,
                request->role,
                request->resultRole);
            response->getResponseData().setInstanceNames(objectNames);

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "AssociatorNames repository access: class = %s, count = %u.",
                CSTRING(request->objectName.toString()),
                objectNames.size()));
        }

        if (providerInfos.providerCount == 0)
        {
            //
            // We have no providers to call.  Just return what we have.
            //

            if (!response.get())
            {
                // No provider is registered and the repository isn't the
                // default.  Return CIM_ERR_NOT_SUPPORTED.

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
                    "CIM_ERR_NOT_SUPPORTED for %s",
                    CSTRING(request->className.getString())));

                response.reset(
                    dynamic_cast<CIMAssociatorNamesResponseMessage*>(
                        request->buildResponse()));
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_NOT_SUPPORTED,
                    String::EMPTY);
            }

            _enqueueResponse(request, response.release());
            PEG_METHOD_EXIT();
            return;
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate* poA = new OperationAggregate(
            new CIMAssociatorNamesRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top(),
            request->objectName.getClassName(),
            request->nameSpace);

        // Include the repository response in the aggregation, if applicable
        if (response.get() != 0)
        {
            poA->setTotalIssued(providerInfos.providerCount+1);
            // send the repository's results
            _forwardRequestForAggregation(
                getQueueId(),
                String(),
                new CIMAssociatorNamesRequestMessage(*request),
                poA,
                response.release());
        }
        else
        {
            poA->setTotalIssued(providerInfos.providerCount);
        }

        for (Uint32 i = 0; i < providerInfos.size(); i++)
        {
            if (providerInfos[i].hasProvider)
            {
                CIMAssociatorNamesRequestMessage* requestCopy =
                    new CIMAssociatorNamesRequestMessage(*request);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->assocClass = providerInfos[i].className;

                if (providerInfos[i].providerIdContainer.get() != 0)
                    requestCopy->operationContext.insert(
                        *(providerInfos[i].providerIdContainer.get()));

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class %s",
                    CSTRING(providerInfos[i].className.getString())));

                _forwardRequestForAggregation(providerInfos[i].serviceId,
                    providerInfos[i].controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleReferencesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleReferencesRequest(
    CIMReferencesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferencesRequest");

    if (_rejectAssociationTraversalDisabled(request,"References"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Validate role parameter syntax
    if (_rejectInvalidRoleParameter(request, request->role, "role"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidClassParameter(request, request->nameSpace,
                                request->objectName))
    {
        PEG_METHOD_EXIT();
        return;
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleReferences - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    if (request->isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "References executing Class request");

        Array<CIMObject> cimObjects =
            _repository->references(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                request->role,
                request->includeQualifiers,
                request->includeClassOrigin,
                request->propertyList);

        AutoPtr<CIMReferencesResponseMessage> response(
            dynamic_cast<CIMReferencesResponseMessage*>(
                request->buildResponse()));

        // Flag to indicate that these are class objects is passed
        // from request in buildResponse above

        response->getResponseData().setObjects(cimObjects);

        _enqueueResponse(request, response.release());
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of Classes and providers for this request
        //
        ProviderInfoList providerInfos = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                String::EMPTY);

        //
        // Get the instances from the repository, as necessary
        //

        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        AutoPtr<CIMReferencesResponseMessage> response;

        if (_repository->isDefaultInstanceProvider())
        {
            response.reset(dynamic_cast<CIMReferencesResponseMessage*>(
                request->buildResponse()));

            Array<CIMObject> cimObjects = _repository->references(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                request->role,
                request->includeQualifiers,
                request->includeClassOrigin,
                request->propertyList);
            response->getResponseData().setObjects(cimObjects);

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "References repository access: class = %s, count = %u.",
                    CSTRING(request->objectName.toString()),
                    cimObjects.size()));
        }

        if (providerInfos.providerCount == 0)
        {
            //
            // We have no providers to call.  Just return what we have.
            //

            if (!response.get())
            {
                // No provider is registered and the repository isn't the
                // default.  Return CIM_ERR_NOT_SUPPORTED.

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
                    "CIM_ERR_NOT_SUPPORTED for %s",
                    CSTRING(request->className.getString()) ));

                response.reset(dynamic_cast<CIMReferencesResponseMessage*>(
                    request->buildResponse()));
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_NOT_SUPPORTED,
                    String::EMPTY);
            }

            _enqueueResponse(request, response.release());
            PEG_METHOD_EXIT();
            return;
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate* poA = new OperationAggregate(
            new CIMReferencesRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top(),
            request->objectName.getClassName(),
            request->nameSpace);

        // Include the repository response in the aggregation, if applicable
        if (response.get() != 0)
        {
            poA->setTotalIssued(providerInfos.providerCount+1);
            // send the repository's results
            _forwardRequestForAggregation(
                getQueueId(),
                String(),
                new CIMReferencesRequestMessage(*request),
                poA,
                response.release());
        }
        else
        {
            poA->setTotalIssued(providerInfos.providerCount);
        }

        for (Uint32 i = 0; i < providerInfos.size(); i++)
        {
            if (providerInfos[i].hasProvider)
            {
                CIMReferencesRequestMessage* requestCopy =
                    new CIMReferencesRequestMessage(*request);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->resultClass = providerInfos[i].className;

                if (providerInfos[i].providerIdContainer.get() != 0)
                    requestCopy->operationContext.insert(
                        *(providerInfos[i].providerIdContainer.get()));

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class %s",
                    CSTRING(providerInfos[i].className.getString()) ));

                _forwardRequestForAggregation(providerInfos[i].serviceId,
                    providerInfos[i].controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleReferenceNamesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleReferenceNamesRequest(
    CIMReferenceNamesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferenceNamesRequest");

    if (_rejectAssociationTraversalDisabled(request,"ReferenceNames"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Validate role parameter syntax
    if (_rejectInvalidRoleParameter(request, request->role, "role"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidClassParameter(request, request->nameSpace,
                                request->objectName))
    {
        PEG_METHOD_EXIT();
        return;
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleReferenceNames - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    if (request->isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "ReferenceNames executing Class request");

        Array<CIMObjectPath> objectNames =
            _repository->referenceNames(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                request->role);

        AutoPtr<CIMReferenceNamesResponseMessage> response(
            dynamic_cast<CIMReferenceNamesResponseMessage*>(
                request->buildResponse()));

        // Flag to indicate that these are class objects is passed
        // from request in buildResponse above.

        response->getResponseData().setInstanceNames(objectNames);

        _enqueueResponse(request, response.release());
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        // Determine list of providers for this request
        //
        ProviderInfoList providerInfos = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->resultClass,
            String::EMPTY);

        //
        // Get the instances from the repository, as necessary
        //

        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        AutoPtr<CIMReferenceNamesResponseMessage> response;

        if (_repository->isDefaultInstanceProvider())
        {
            response.reset(dynamic_cast<CIMReferenceNamesResponseMessage*>(
                request->buildResponse()));

            Array<CIMObjectPath> objectNames = _repository->referenceNames(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                request->role);
            response->getResponseData().setInstanceNames(objectNames);

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "ReferenceNames repository access: class = %s, count = %u.",
                CSTRING(request->objectName.toString()),
                objectNames.size()));
        }

        if (providerInfos.providerCount == 0)
        {
            //
            // We have no providers to call.  Just return what we have.
            //

            if (!response.get())
            {
                // No provider is registered and the repository isn't the
                // default.  Return CIM_ERR_NOT_SUPPORTED.

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
                    "CIM_ERR_NOT_SUPPORTED for %s",
                    CSTRING(request->className.getString()) ));

                response.reset(dynamic_cast<CIMReferenceNamesResponseMessage*>(
                    request->buildResponse()));
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_NOT_SUPPORTED,
                    String::EMPTY);
            }

            _enqueueResponse(request, response.release());
            PEG_METHOD_EXIT();
            return;
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate* poA = new OperationAggregate(
            new CIMReferenceNamesRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top(),
            request->objectName.getClassName(),
            request->nameSpace);

        // Include the repository response in the aggregation, if applicable
        if (response.get() != 0)
        {
            poA->setTotalIssued(providerInfos.providerCount+1);
            _forwardRequestForAggregation(
                getQueueId(),
                String(),
                new CIMReferenceNamesRequestMessage(*request),
                poA,
                response.release());
        }
        else
        {
            poA->setTotalIssued(providerInfos.providerCount);
        }

        for (Uint32 i = 0; i < providerInfos.size(); i++)
        {
            if (providerInfos[i].hasProvider)
            {
                CIMReferenceNamesRequestMessage* requestCopy =
                    new CIMReferenceNamesRequestMessage(*request);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->resultClass = providerInfos[i].className;

                if (providerInfos[i].providerIdContainer.get() != 0)
                    requestCopy->operationContext.insert(
                        *(providerInfos[i].providerIdContainer.get()));

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class %s",
                    CSTRING(providerInfos[i].className.getString())));

                _forwardRequestForAggregation(providerInfos[i].serviceId,
                    providerInfos[i].controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleGetPropertyRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleGetPropertyRequest(
    CIMGetPropertyRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleGetPropertyRequest");

    CIMName className = request->instanceName.getClassName();

    ProviderInfo providerInfo = _lookupInstanceProvider(
        request->nameSpace,
        className);

    if (providerInfo.hasProvider)
    {
        CIMGetPropertyRequestMessage* requestCopy =
            new CIMGetPropertyRequestMessage(*request);

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
        }

        CIMGetPropertyRequestMessage* requestCallbackCopy =
            new CIMGetPropertyRequestMessage(*requestCopy);

        _forwardRequestToProvider(
            providerInfo,
            requestCopy,
            requestCallbackCopy);
    }
    else if (_repository->isDefaultInstanceProvider())
    {
        CIMValue value =
            _repository->getProperty(
                request->nameSpace,
                request->instanceName,
                request->propertyName);

        AutoPtr<CIMGetPropertyResponseMessage> response(
            dynamic_cast<CIMGetPropertyResponseMessage*>(
                request->buildResponse()));
        response->value = value;

        _enqueueResponse(request, response.release());
    }
    else // No provider is registered and the repository isn't the default
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        _enqueueResponse(request, response);
    }
    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleSetPropertyRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleSetPropertyRequest(
    CIMSetPropertyRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleSetPropertyRequest");

    _fixSetPropertyValueType(request);

    CIMName className = request->instanceName.getClassName();

    ProviderInfo providerInfo = _lookupInstanceProvider(
        request->nameSpace,
        className);

    if (providerInfo.hasProvider)
    {
        CIMSetPropertyRequestMessage* requestCopy =
            new CIMSetPropertyRequestMessage(*request);

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *providerInfo.providerIdContainer.get());
        }

        CIMSetPropertyRequestMessage* requestCallbackCopy =
            new CIMSetPropertyRequestMessage(*requestCopy);

        _forwardRequestToProvider(
            providerInfo,
            requestCopy,
            requestCallbackCopy);
    }
    else if (_repository->isDefaultInstanceProvider())
    {
        _repository->setProperty(
            request->nameSpace,
            request->instanceName,
            request->propertyName,
            request->newValue);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
            "CIMOperationRequestDispatcher::handleSetPropertyRequest - "
                "Namespace: %s  Instance Name: %s  Property Name: %s  New "
                "Value: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(request->instanceName.getClassName().getString()),
            CSTRING(request->propertyName.getString()),
            CSTRING(request->newValue.toString())));

        AutoPtr<CIMSetPropertyResponseMessage> response(
            dynamic_cast<CIMSetPropertyResponseMessage*>(
                request->buildResponse()));

        _enqueueResponse(request, response.release());
    }
    else // No provider is registered and the repository isn't the default
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        _enqueueResponse(request, response);
    }

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleGetQualifierRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleGetQualifierRequest(
    CIMGetQualifierRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleGetQualifierRequest");

    CIMQualifierDecl cimQualifierDecl =
        _repository->getQualifier(
            request->nameSpace,
            request->qualifierName);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleGetQualifierRequest - "
            "Namespace: %s  Qualifier Name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->qualifierName.getString())));

    AutoPtr<CIMGetQualifierResponseMessage> response(
        dynamic_cast<CIMGetQualifierResponseMessage*>(
            request->buildResponse()));
    response->cimQualifierDecl = cimQualifierDecl;

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleSetQualifierRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleSetQualifierRequest(
    CIMSetQualifierRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleSetQualifierRequest");

    _repository->setQualifier(
        request->nameSpace,
        request->qualifierDeclaration);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleSetQualifierRequest - "
            "Namespace: %s  Qualifier Name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->qualifierDeclaration.getName().getString())));

    AutoPtr<CIMSetQualifierResponseMessage> response(
        dynamic_cast<CIMSetQualifierResponseMessage*>(
            request->buildResponse()));

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleDeleteQualifierRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleDeleteQualifierRequest(
    CIMDeleteQualifierRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleDeleteQualifierRequest");

    _repository->deleteQualifier(
        request->nameSpace,
        request->qualifierName);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleDeleteQualifierRequest - "
            "Namespace: %s  Qualifier Name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->qualifierName.getString())));

    AutoPtr<CIMDeleteQualifierResponseMessage> response(
        dynamic_cast<CIMDeleteQualifierResponseMessage*>(
            request->buildResponse()));

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleEnumerateQualifiersRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest(
    CIMEnumerateQualifiersRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest");

    Array<CIMQualifierDecl> qualifierDeclarations =
        _repository->enumerateQualifiers(request->nameSpace);

    PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest - "
            "Namespace: %s",
        CSTRING(request->nameSpace.getString())));

    AutoPtr<CIMEnumerateQualifiersResponseMessage> response(
        dynamic_cast<CIMEnumerateQualifiersResponseMessage*>(
            request->buildResponse()));
    response->qualifierDeclarations = qualifierDeclarations;

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleExecQueryRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleExecQueryRequest(
    CIMExecQueryRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleExecQueryRequest");

    AutoPtr<CIMExecQueryResponseMessage> response(
        dynamic_cast<CIMExecQueryResponseMessage*>(
            request->buildResponse()));

    Boolean exception = false;

#ifdef PEGASUS_DISABLE_EXECQUERY
    response->cimException =
        PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
    exception=true;
#else
    if (QuerySupportRouter::routeHandleExecQueryRequest(this,request)==false)
    {
        if (request->operationContext.contains(
                SubscriptionFilterConditionContainer::NAME))
        {
            SubscriptionFilterConditionContainer sub_cntr =
                request->operationContext.get(
                    SubscriptionFilterConditionContainer::NAME);
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED,
                sub_cntr.getQueryLanguage());
        }
        else
        {
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED, request->queryLanguage);
        }

        exception = true;
    }
#endif

    if (exception)
    {
        _enqueueResponse(request, response.release());
        PEG_METHOD_EXIT();
        return;
    }

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleInvokeMethodRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleInvokeMethodRequest(
    CIMInvokeMethodRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleInvokeMethodRequest");

    _fixInvokeMethodParameterTypes(request);

    CIMName className = request->instanceName.getClassName();

    if (!_checkExistenceOfClass(request->nameSpace, className))
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::handleInvokeMethodRequest - "
                "CIM class does not exist exception has occurred.  "
                "Namespace: %s  Class Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(className.getString()) ));

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, className.getString());
    }

    // Find Provider for this method
    ProviderIdContainer* providerIdContainer=NULL;
    ProviderInfo providerInfo(className);

    // First determine if there is an internal provider. If not, try
    // for a registered provider.
    if (!_lookupInternalProvider(request->nameSpace, className, providerInfo))
    {
        // check the class name for a registered provider for this namespace
        // classname and methodname. There can be only one.
        String providerName = _lookupMethodProvider(
            request->nameSpace,
            className,
            request->methodName,
            &providerIdContainer);

        // if provider name returned, set the provider info to
        // go to providerManagerService
        if (providerName.size() != 0)
        {
            providerInfo.addProviderInfo(
                _providerManagerServiceId,
                true,
                false);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::invokeMethodProvider - "
                "Namespace: %s  Class Name: %s"
                " Provider Name: %s found. ",
            CSTRING(request->nameSpace.getString()),
            CSTRING(className.getString()),
            CSTRING(providerName) ));
        }
        else
        {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
                "CIMOperationRequestDispatcher::invokeMethodProvider %s - "
                    "Namespace: %s  Class Name: %s  Service Name: %s  "
                    "Control Provider Name %s"
                    "Provider Name: %s found. ",
                ((providerInfo.controlProviderName.size() ==0)?
                    "Intern": "Extern"),
                CSTRING(request->nameSpace.getString()),
                CSTRING(className.getString()),
                _getServiceName(_providerManagerServiceId),
                CSTRING(providerInfo.controlProviderName),
                CSTRING(providerName) ));
        }
    }

    if (providerInfo.hasProvider)
    {
        CIMInvokeMethodRequestMessage* requestCopy =
            new CIMInvokeMethodRequestMessage(*request);

        CIMInvokeMethodRequestMessage* requestCallbackCopy =
            new CIMInvokeMethodRequestMessage(*requestCopy);

        if (providerIdContainer != NULL)
        {
            requestCopy->operationContext.insert(*providerIdContainer);
            delete providerIdContainer;
            providerIdContainer = NULL;
        }

        _forwardRequestToProvider(
            providerInfo,
            requestCopy,
            requestCallbackCopy);
    }

    else // No provider for this method.
    {
       // No provider or control provider.  Generate exception
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_METHOD_NOT_AVAILABLE,
                request->methodName.getString());

        _enqueueResponse(request, response);
    }

    PEG_METHOD_EXIT();
}

/*********************************************************************/
//
//   Return Aggregated responses back to the Correct Aggregator
//
//   The aggregator includes an aggregation object that is used to
//   accumulate responses.  It is attached to each request sent and
//   received back as part of the response call back in the "parm"
//   Responses are aggregated until the count reaches the sent count and
//   then the aggregation code is called to create a single response from
//   the accumulated responses.
//
/*********************************************************************/


void CIMOperationRequestDispatcher::handleOperationResponseAggregation(
    OperationAggregate* poA,
    bool requiresHostnameCompletion,
    bool hasPropList)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::"
            "handleOperationResponseAggregation");
    CIMResponseDataMessage* toResponse =
        (CIMResponseDataMessage*) poA->getResponse(0);
    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
        "CIMOperationRequestDispatcher Response - "
            "Namespace: %s  Class name: %s Response Count: %u",
        CSTRING(poA->_nameSpace.getString()),
        CSTRING(poA->_className.getString()),
        poA->numberResponses()));

    CIMResponseData & to = toResponse->getResponseData();

    // Re-add the property list as stored from request after deepInheritance fix
    // since on OOP on the response message the property list gets lost
    // This is only done for EnumerateInstances type requests
    if (hasPropList)
    {
        CIMEnumerateInstancesRequestMessage* request =
            (CIMEnumerateInstancesRequestMessage*)poA->getRequest();
        to.setPropertyList(request->propertyList);
    }

    // Work backward and delete each response off the end of the array
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMResponseDataMessage* fromResponse =
            (CIMResponseDataMessage*)poA->getResponse(i);
        CIMResponseData & from = fromResponse->getResponseData();
        to.appendResponseData(from);
        poA->deleteResponse(i);
    }

    if (requiresHostnameCompletion)
    {
        // fill in host, namespace on all instances on all elements of array
        // if they have been left out. This is required because XML reader
        // will fail without them populated
        to.completeHostNameAndNamespace(System::getHostName(),poA->_nameSpace);
    }
    PEG_METHOD_EXIT();
}


// Aggregator for execQuery Response Aggregation.
// Aggregates responses into a single response and maps the responses back
// to ExecQuery responses.
void CIMOperationRequestDispatcher::handleExecQueryResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleExecQueryResponseAggregation");

    Uint32 numberResponses = poA->numberResponses();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "CIMOperationRequestDispatcher::ExecQuery Response - "
            "Name Space: %s  Class name: %s Response Count: %u",
        CSTRING(poA->_nameSpace.getString()),
        CSTRING(poA->_className.getString()),
        numberResponses));

    if (numberResponses != 0)
    {
        CIMResponseMessage* response = poA->getResponse(0);
        CIMExecQueryResponseMessage* toResponse = 0;
        Uint32 startIndex = 0;
        Uint32 endIndex = numberResponses - 1;
        Boolean manyResponses = true;

        // Define pointer to the applyQueryToEnumeration function appropriate
        // for the query language defined.
        // NOTE: typedef applyQueryToEnumerationPtr is defined in
        // QuerySupportRouter.
        applyQueryFunctionPtr applyQueryToEnumeration =  NULL;
        applyQueryToEnumeration =
            QuerySupportRouter::getFunctPtr(this, poA->_query);

        if (response->getType() == CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE)
        {
            // Create an ExecQuery response from an EnumerateInstances request
            CIMOperationRequestMessage* request = poA->getRequest();
            AutoPtr<CIMExecQueryResponseMessage> query(
                new CIMExecQueryResponseMessage(
                    request->messageId,
                    CIMException(),
                    request->queueIds.copyAndPop()));
            query->syncAttributes(request);
            toResponse = query.release();
        }
        else
        {
            toResponse = (CIMExecQueryResponseMessage*) response;
            manyResponses = false;
        }

        // Work backward and delete each response off the end of the array
        for (Uint32 i = endIndex; i >= startIndex; i--)
        {
            if (manyResponses)
            {
                response = poA->getResponse(i);
            }

            if (response->getType() == CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE)
            {
                // convert enumerate instances response to exec query response
                // This is an indirect call to the appropriate function in
                // the WQL..., CQL, ... etc Class
                applyQueryToEnumeration(response, poA->_query);
                CIMEnumerateInstancesResponseMessage* fromResponse =
                    (CIMEnumerateInstancesResponseMessage*) response;
                CIMClass cimClass;

                Boolean clsRead=false;
                Array<CIMInstance>& a =
                    fromResponse->getResponseData().getInstances();

                for (Uint32 j = 0, m = a.size(); j < m; j++)
                {
                    CIMObject co=CIMObject(a[j]);
                    CIMObjectPath op=co.getPath();
                    const Array<CIMKeyBinding>& kbs=op.getKeyBindings();

                    if (kbs.size() == 0)
                    {     // no path set why ?
                        if (clsRead == false)
                        {
                            cimClass = _repository->getClass(
                                poA->_nameSpace, op.getClassName(),
                                false,true,false, CIMPropertyList());
                            clsRead=true;
                        }
                        op = a[j].buildPath(cimClass);
                    }
                    op.setNameSpace(poA->_nameSpace);
                    op.setHost(System::getHostName());
                    co.setPath(op);
                    if (manyResponses)
                    {
                        toResponse->getResponseData().appendObject(co);
                    }
                }
            }
            else
            {
                CIMExecQueryResponseMessage* fromResponse =
                    (CIMExecQueryResponseMessage*) response;

                CIMResponseData & from = fromResponse->getResponseData();
                from.completeHostNameAndNamespace(
                    System::getHostName(),
                    poA->_nameSpace);

                if (manyResponses)
                {
                    toResponse->getResponseData().appendResponseData(from);
                }
            }
            if (manyResponses)
            {
                poA->deleteResponse(i);
            }

            if (i == 0)
            {
                break;
            }
        } // for all responses in response list

        // If we started with an enumerateInstances repsonse, then add it
        // to overall
        if ((startIndex == 0) && manyResponses)
        {
            poA->appendResponse(toResponse);
        }
    }
    PEG_METHOD_EXIT();
}
/*******End of the functions for aggregation***************************/

/**
    Convert the specified CIMValue to the specified type, and return it in
    a new CIMValue.
*/
CIMValue CIMOperationRequestDispatcher::_convertValueType(
    const CIMValue& value,
    CIMType type)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_convertValueType");

    CIMValue newValue;

    if (value.isArray())
    {
        Array<String> stringArray;
        Array<char*> charPtrArray;
        Array<const char*> constCharPtrArray;

        //
        // Convert the value to Array<const char*> to send to conversion method
        //
        // ATTN-RK-P3-20020221: Deal with TypeMismatchException
        // (Shouldn't really ever get that exception)
        value.get(stringArray);

        for (Uint32 k=0; k<stringArray.size(); k++)
        {
            // Need to build an Array<const char*> to send to the conversion
            // routine, but also need to keep track of them pointers as char*
            // because Windows won't let me delete a const char*.
            char* charPtr = strdup(stringArray[k].getCString());
            charPtrArray.append(charPtr);
            constCharPtrArray.append(charPtr);
        }

        //
        // Convert the value to the specified type
        //
        try
        {
            newValue =
                XmlReader::stringArrayToValue(0, constCharPtrArray, type);
        }
        catch (XmlSemanticError&)
        {
            for (Uint32 k=0; k<charPtrArray.size(); k++)
            {
                free(charPtrArray[k]);
            }

            PEG_METHOD_EXIT();

            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_INVALID_PARAMETER,
                MessageLoaderParms(
                    "Server.CIMOperationRequestDispatcher."
                        "CIM_ERR_INVALID_PARAMETER",
                    "Malformed $0 value",
                    cimTypeToString(type)));
        }
        catch (...)
        {
            for (Uint32 k=0; k<charPtrArray.size(); k++)
            {
                free(charPtrArray[k]);
            }

            PEG_METHOD_EXIT();
            throw;
        }

        for (Uint32 k = 0; k < charPtrArray.size(); k++)
        {
            free(charPtrArray[k]);
        }
    }
    else
    {
        String stringValue;

        // ATTN-RK-P3-20020221: Deal with TypeMismatchException
        // (Shouldn't really ever get that exception)
        value.get(stringValue);

        try
        {
            newValue =
                XmlReader::stringToValue(0, stringValue.getCString(), type);
        }
        catch (XmlSemanticError&)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_INVALID_PARAMETER,
                MessageLoaderParms(
                    "Server.CIMOperationRequestDispatcher."
                        "CIM_ERR_INVALID_PARAMETER",
                    "Malformed $0 value",
                    cimTypeToString(type)));
        }
    }

    PEG_METHOD_EXIT();
    return newValue;
}

/**
   Find the CIMParamValues in the InvokeMethod request whose types were
   not specified in the XML encoding, and convert them to the types
   specified in the method schema.
*/
void CIMOperationRequestDispatcher::_fixInvokeMethodParameterTypes(
    CIMInvokeMethodRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_fixInvokeMethodParameterTypes");

    Boolean gotMethodDefinition = false;
    CIMMethod method;

    //
    // Cycle through the input parameters, converting the untyped ones.
    //
    Array<CIMParamValue> inParameters = request->inParameters;
    Uint32 numInParamValues = inParameters.size();
    for (Uint32 i=0; i<numInParamValues; i++)
    {
        if (!inParameters[i].isTyped())
        {
            //
            // Retrieve the method definition, if we haven't already done so
            // (only look up the method if we have an untyped parameter value)
            //
            if (!gotMethodDefinition)
            {
                //
                // Get the class definition for this method
                //
                CIMClass cimClass =
                    _repository->getClass(
                        request->nameSpace,
                        request->instanceName.getClassName(),
                        false, //localOnly,
                        false, //includeQualifiers,
                        false, //includeClassOrigin,
                        CIMPropertyList());

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "CIMOperationRequestDispatcher::"
                        "_fixInvokeMethodParameterTypes - "
                        "Namespace: %s  Class Name: %s",
                    CSTRING(request->nameSpace.getString()),
                    CSTRING(request->instanceName.getClassName().getString())));

                //
                // Get the method definition from the class
                //
                Uint32 methodPos = cimClass.findMethod(request->methodName);
                if (methodPos == PEG_NOT_FOUND)
                {
                    throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_METHOD_NOT_FOUND, String::EMPTY);
                }
                method = cimClass.getMethod(methodPos);

                gotMethodDefinition = true;
            }

            //
            // Find the parameter definition for this input parameter
            //
            CIMName paramName = inParameters[i].getParameterName();
            Uint32 numParams = method.getParameterCount();
            for (Uint32 j=0; j<numParams; j++)
            {
                CIMParameter param = method.getParameter(j);
                if (paramName == param.getName())
                {
                    //
                    // Retype the input parameter value according to the
                    // type defined in the class/method schema
                    //
                    CIMType paramType = param.getType();
                    CIMValue newValue;

                    if (inParameters[i].getValue().isNull())
                    {
                        newValue.setNullValue(
                            param.getType(),
                            param.isArray());
                    }
                    else if (inParameters[i].getValue().isArray() !=
                                 param.isArray())
                    {
                        // ATTN-RK-P1-20020222: Who catches this?  They aren't.
                        PEG_METHOD_EXIT();
                        throw PEGASUS_CIM_EXCEPTION(
                            CIM_ERR_TYPE_MISMATCH, String::EMPTY);
                    }
                    else
                    {
                        newValue = _convertValueType(
                            inParameters[i].getValue(),
                            paramType);
                    }

                    inParameters[i].setValue(newValue);
                    inParameters[i].setIsTyped(true);
                    break;
                }
            }
        }
    }

    PEG_METHOD_EXIT();
}

/**
    Convert the CIMValue given in a SetProperty request to the correct
    type according to the schema, because it is not possible to specify
    the property type in the XML encoding.
*/
void CIMOperationRequestDispatcher::_fixSetPropertyValueType(
    CIMSetPropertyRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_fixSetPropertyValueType");

    CIMValue inValue = request->newValue;

    //
    // Only do the conversion if the type is not already set
    //
    if ((inValue.getType() != CIMTYPE_STRING))
    {
        PEG_METHOD_EXIT();
        return;
    }

    //
    // Get the class definition for this property
    //
    CIMClass cimClass;
    try
    {
        cimClass = _repository->getClass(
            request->nameSpace,
            request->instanceName.getClassName(),
            false, //localOnly,
            false, //includeQualifiers,
            false, //includeClassOrigin,
            CIMPropertyList());

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "CIMOperationRequestDispatcher::_fixSetPropertyValueType - "
                "Namespace: %s  Class Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(request->instanceName.getClassName().getString())));
    }
    catch (CIMException& exception)
    {
        // map CIM_ERR_NOT_FOUND to CIM_ERR_INVALID_CLASS
        if (exception.getCode() == CIM_ERR_NOT_FOUND)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS,
                request->instanceName.getClassName().getString());
        }

        PEG_METHOD_EXIT();
        throw;
    }

    //
    // Get the property definition from the class
    //
    Uint32 propertyPos = cimClass.findProperty(request->propertyName);
    if (propertyPos == PEG_NOT_FOUND)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NO_SUCH_PROPERTY, String::EMPTY);
    }
    CIMProperty property = cimClass.getProperty(propertyPos);

    //
    // Retype the input property value according to the
    // type defined in the schema
    //
    CIMValue newValue;

    if (inValue.isNull())
    {
        newValue.setNullValue(property.getType(), property.isArray());
    }
    else if (inValue.isArray() != property.isArray())
    {
        // ATTN-RK-P1-20020222: Who catches this?  They aren't.
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_TYPE_MISMATCH, String::EMPTY);
    }
    else
    {
        newValue = _convertValueType(inValue, property.getType());
    }

    //
    // Put the retyped value back into the message
    //
    request->newValue = newValue;

    PEG_METHOD_EXIT();
}

/*
    Check the existence of a class matching a classname.
    Returns true if class found in repository.
    Note that this code checks for the special classname
    PEGASUS_CLASSNAME___NAMESPACE and returns true.

    If the repository returns exception this function returns false
    if NOT CIM_ERR_NOT_FOUND. Else, rethrows the exception.
*/
Boolean CIMOperationRequestDispatcher::_checkExistenceOfClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_checkExistenceOfClass");

    if (className.equal(CIMName(PEGASUS_CLASSNAME___NAMESPACE)))
    {
        PEG_METHOD_EXIT();
        return true;
    }

    try
    {
        CIMClass cimClass = _repository->getClass(
                nameSpace,
                className,
                true,
                false,
                false,
                CIMPropertyList());
    }
    catch (const CIMException& exception)
    {
        // CIM_ERR_NOT_FOUND indicates that the class is not defined
        if (exception.getCode() == CIM_ERR_NOT_FOUND)
        {
            PEG_METHOD_EXIT();
            return false;
        }

        PEG_METHOD_EXIT();
        throw;
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "CIMOperationRequestDispatcher::_checkExistenceOfClass - "
            "Namespace: %s  Class Name: %s found.",
        CSTRING(nameSpace.getString()),
        CSTRING(className.getString())));

    PEG_METHOD_EXIT();
    return true;
}

CIMConstClass CIMOperationRequestDispatcher::_getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    CIMException& cimException)
{
    if (className.equal(CIMName(PEGASUS_CLASSNAME___NAMESPACE)))
    {
        CIMClass __namespaceClass(PEGASUS_CLASSNAME___NAMESPACE);
        // ATTN: Qualifiers not added here, but they shouldn't be needed
        __namespaceClass.addProperty(
            CIMProperty(PEGASUS_PROPERTYNAME_NAME, String::EMPTY));
        return __namespaceClass;
    }

    CIMConstClass cimClass;

    // get the complete class, specifically not local only
    try
    {
        cimClass = _repository->getFullConstClass(
            nameSpace,
            className);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
            "CIMOperationRequestDispatcher::_getClass - "
                "Namespace: %s  Class Name: %s",
            CSTRING(nameSpace.getString()),
            CSTRING(className.getString())));
    }
    catch (const CIMException& exception)
    {
        // map CIM_ERR_NOT_FOUND to CIM_ERR_INVALID_CLASS
        if (exception.getCode() == CIM_ERR_NOT_FOUND)
        {
            cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_CLASS, className.getString());
        }
        else
        {
            cimException = exception;
        }
    }
    catch (const Exception& exception)
    {
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
    }
    catch (...)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    return cimClass;
}


template <class ObjectClass>
void removePropagatedAndOriginAttributes(ObjectClass& newObject)
{
    Uint32 numProperties = newObject.getPropertyCount();
    for (Uint32 i = 0; i < numProperties; i++)
    {
        CIMProperty currentProperty = newObject.getProperty(i);
        if (currentProperty.getPropagated() == true ||
            currentProperty.getClassOrigin().getString().size() > 0)
        {
            newObject.removeProperty(i);
            currentProperty.setPropagated(false);
            currentProperty.setClassOrigin(CIMName());
            newObject.addProperty(currentProperty);
            --i;
        }
    }
}

PEGASUS_NAMESPACE_END
