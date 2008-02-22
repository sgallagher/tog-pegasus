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
//=============================================================================
//
//%////////////////////////////////////////////////////////////////////////////

#include "CIMOperationRequestDispatcher.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlReader.h> // stringToValue(), stringArrayToValue()
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/AuditLogger.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Formatter.h>
#include <Pegasus/Server/reg_table.h>

#include <Pegasus/Server/QuerySupportRouter.h>

PEGASUS_NAMESPACE_BEGIN

//PEGASUS_USING_STD;
//#define CDEBUG(X) cout << "CIMOpReqDsptchr " << X << endl
#define CDEBUG(X)

// Test tool to limit enumerations to a single level.  This is not
// production and is used only to debug special problems in the requests
// that issue multiple provider operations.
//#define LIMIT_ENUM_TO_ONE_LEVEL

static DynamicRoutingTable _routing_table;

// Variable to control whether we do search or simply single provider for
// reference and associatior lookups.
// ATTN: KS 5 April 2003 This is to be removed ATTN:
// static bool singleProviderType = true;

// Local save for host name. save host name here.  NOTE: Problem if hostname
// changes.
// Set by object init. Used by aggregator.
String cimAggregationLocalHost;

// A helper function that resets the Propagated and ClassOrigin attributes on
// properties of CIMInstance and CIMClass objects. This is used during
// Create/Modify Instance and Create/Modify Class operations, where the
// Propagated and ClassOrigin attributes must be ignored.
template <class ObjectClass>
void removePropagatedAndOriginAttributes(ObjectClass& newObject);

// static counter for aggretation serial numbers.
// can be used to determine lost aggregations.
Uint64 CIMOperationRequestDispatcher::cimOperationAggregationSN = 0;

OperationAggregate::OperationAggregate(
    CIMRequestMessage* request,
    Uint32 msgRequestType,
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
      _request(request)
{
    _totalIssued = 0;
    _totalReceived = 0;
    _totalReceivedComplete = 0;
    _totalReceivedExpected = 0;
    _totalReceivedErrors = 0;
    _totalReceivedNotSupported = 0;
    _magicNumber = 12345;
    _aggregationSN = 0;
}

OperationAggregate::~OperationAggregate()
{
    _magicNumber = 0;
    delete _request;
    delete _query;
}

Boolean OperationAggregate::valid() const
{
    return (_magicNumber == 12345) ? true : false;
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
    //AutoMutex autoMut(_appendResponseMutex);
    Uint32 size =  _responseList.size();
    return size;
}

CIMRequestMessage* OperationAggregate::getRequest()
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

Uint32 OperationAggregate::getRequestType() const
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
    static const String func = "OperationAggregate::resequenceResponse: ";
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
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
            Logger::TRACE,
            String(func + "Response has error. "
                "Namespace: $0  Class name: $1 Response Sequence: $2"),
            _nameSpace.getString(),
            _className.getString(),
            _totalReceived));
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
            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                Formatter::format(
                    func + "message is complete. "
                        "total responses: $0, "
                        "total chunks: $1, "
                        "total errors: $2",
                    _totalReceivedComplete,
                    _totalReceived,
                    _totalReceivedErrors));
        }
        else
        {
            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                func + "All completed responses ($0) for current request "
                    "have been accounted for but expected count ($1) does "
                    "not match the received count ($2). error count ($3).  "
                    "Attempting to continue ...",
                _totalReceivedComplete,
                _totalReceivedExpected,
                _totalReceived,
                _totalReceivedErrors));
            PEG_TRACE((
                __FILE__, __LINE__, TRC_DISCARDED_DATA, Tracer::LEVEL2,
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
    // Check whether AssociationTraversal is supported.
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    _enableAssociationTraversal = ConfigManager::parseBooleanValue(
        configManager->getCurrentValue("enableAssociationTraversal"));

    _enableIndicationService = ConfigManager::parseBooleanValue(
        configManager->getCurrentValue("enableIndicationService"));

    //
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

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    _enableNormalization = ConfigManager::parseBooleanValue(
        configManager->getCurrentValue("enableNormalization"));

    if (_enableNormalization)
    {
        String moduleList =
            configManager->getCurrentValue("excludeModulesFromNormalization");

        for (Uint32 pos = moduleList.find(','); moduleList.size() != 0;
             pos = moduleList.find(','))
        {
            String moduleName = moduleList.subString(0, pos);

            _excludeModulesFromNormalization.append(moduleName);

            moduleList.remove(0, (pos == PEG_NOT_FOUND ? pos : pos + 1));
        }
    }
#endif

    _routing_table = DynamicRoutingTable::get_rw_routing_table();

    cimAggregationLocalHost = System::getHostName();

    PEG_METHOD_EXIT();
}

CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher");
    PEG_METHOD_EXIT();
}

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
        pos = provider.findProperty("Name");
        if (pos != PEG_NOT_FOUND)
        {
            provider.getProperty(pos).getValue().get(providerName);
        }
    }
}

void CIMOperationRequestDispatcher::_logOperation(
    const CIMRequestMessage* request,
    const CIMResponseMessage* response)
{
#ifndef PEGASUS_DISABLE_AUDIT_LOGGER

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
    static const char func[] =
        "CIMOperationRequestDispatcher::_enqueueResponse";
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

        switch(type)
        {
            case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE :
                handleEnumerateInstanceNamesResponseAggregation(poA);
                break;

            case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE :
                handleEnumerateInstancesResponseAggregation(poA);
                break;

            case CIM_ASSOCIATORS_REQUEST_MESSAGE :
                handleAssociatorsResponseAggregation(poA);
                break;

            case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE :
                handleAssociatorNamesResponseAggregation(poA);
                break;

            case CIM_REFERENCES_REQUEST_MESSAGE :
                handleReferencesResponseAggregation(poA);
                break;

            case CIM_REFERENCE_NAMES_REQUEST_MESSAGE :
                handleReferenceNamesResponseAggregation(poA);
                break;

            case CIM_EXEC_QUERY_REQUEST_MESSAGE :
                handleExecQueryResponseAggregation(poA);
                break;

            default:
                static const char failMsg[] =
                    "Invalid response type to aggregate: ";
                char typeP[11];
                sprintf(typeP,"%u", type);
                PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
                    Logger::TRACE,
                    String(func) + String(failMsg) + String(typeP)));
                PEGASUS_ASSERT(0);
                break;
        } // switch

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
            (strcmp(name, PEGASUS_QUEUENAME_BINARY_HANDLER) == 0 ||
             strcmp(name, PEGASUS_QUEUENAME_INTERNALCLIENT) == 0));

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
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
            Logger::TRACE, String(func) + String(failMsg)));

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
    if (req->getType() == async_messages::ASYNC_LEGACY_OP_START)
    {
        req->op->processing();

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

Boolean CIMOperationRequestDispatcher::_lookupInternalProvider(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    String& service,
    String& provider)
{
    static AtomicInt _initialized(0);
    static Mutex _monitor;

    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupInternalProvider");
    // Clear the strings since used as test later. Poor code but true now

    service = String::EMPTY;
    provider = String::EMPTY;
    CIMNamespaceName _wild;
    if (_initialized.get() == 0)
    {
        AutoMutex autoMut(_monitor);
        if (_initialized.get() == 0)
        {
            _routing_table.insert_record(
                PEGASUS_CLASSNAME_CONFIGSETTING,
                PEGASUS_NAMESPACENAME_CONFIG,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_CONFIGPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_AUTHORIZATION,
                PEGASUS_NAMESPACENAME_AUTHORIZATION,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_USERAUTHPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_USER,
                PEGASUS_NAMESPACENAME_USER,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_USERAUTHPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_SHUTDOWN,
                PEGASUS_NAMESPACENAME_SHUTDOWN,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_SHUTDOWNPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME___NAMESPACE,
                _wild,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_NAMESPACEPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

#ifdef PEGASUS_HAS_SSL
            _routing_table.insert_record(
                PEGASUS_CLASSNAME_CERTIFICATE,
                PEGASUS_NAMESPACENAME_CERTIFICATE,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_CERTIFICATEPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_CRL,
                PEGASUS_NAMESPACENAME_CERTIFICATE,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_CERTIFICATEPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);
#endif

#ifndef PEGASUS_DISABLE_PERFINST
            _routing_table.insert_record(
                PEGASUS_CLASSNAME_CIMOMSTATDATA,
                PEGASUS_NAMESPACENAME_CIMOMSTATDATA,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_CIMOMSTATDATAPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);
#endif

#ifndef PEGASUS_DISABLE_CQL
            _routing_table.insert_record(
                PEGASUS_CLASSNAME_CIMQUERYCAPABILITIES,
                //PEGASUS_NAMESPACENAME_CIMQUERYCAPABILITIES,
                _wild,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_CIMQUERYCAPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);
#endif

#if !defined(PEGASUS_DISABLE_PERFINST) || defined(PEGASUS_ENABLE_SLP)
            // InteropProvider ObjectManager Class
            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_OBJECTMANAGER,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            // CIM_Namespace - Implemented to assure that it does not
            // access the repository and to allow  access to
            // common class CIM_Namespace for namespace creation.
            _routing_table.insert_record(
                PEGASUS_CLASSNAME_CIMNAMESPACE,
                _wild,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            // PG_NAMESPACE - Implements subclass of CIM_Namespace managed by
            // InteropProvider.
            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PGNAMESPACE,
                _wild,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);
#endif

#ifdef PEGASUS_ENABLE_SLP
            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_CIMXMLCOMMUNICATIONMECHANISM,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_COMMMECHANISMFORMANAGER,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_COMPUTERSYSTEM,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_HOSTEDOBJECTMANAGER,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_HOSTEDACCESSPOINT,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_NAMESPACEINMANAGER,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_REFERENCEDPROFILE,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                 PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE,
                _wild,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                 PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE_RP_RP,
                _wild,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                 PEGASUS_CLASSNAME_PG_SUBPROFILEREQUIRESPROFILE,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_ELEMENTSOFTWAREIDENTITY,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PG_INSTALLEDSOFTWAREIDENTITY,
                PEGASUS_NAMESPACENAME_INTEROP,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);
#endif // PEGASUS_ENABLE_SLP

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PROVIDERMODULE,
                PEGASUS_NAMESPACENAME_PROVIDERREG,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_PROVREGPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PROVIDER,
                PEGASUS_NAMESPACENAME_PROVIDERREG,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_PROVREGPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_PROVIDERCAPABILITIES,
                PEGASUS_NAMESPACENAME_PROVIDERREG,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_PROVREGPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            _routing_table.insert_record(
                PEGASUS_CLASSNAME_CONSUMERCAPABILITIES,
                PEGASUS_NAMESPACENAME_PROVIDERREG,
                DynamicRoutingTable::INTERNAL,
                0,
                static_cast<MessageQueueService*>(
                    MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
                PEGASUS_MODULENAME_PROVREGPROVIDER,
                PEGASUS_QUEUENAME_CONTROLSERVICE);

            if (_enableIndicationService)
            {
                _routing_table.insert_record(
                    PEGASUS_CLASSNAME_INDSUBSCRIPTION,
                    _wild,
                    DynamicRoutingTable::INTERNAL,
                    0,
                    static_cast<MessageQueueService*>(MessageQueue::lookup(
                        PEGASUS_QUEUENAME_INDICATIONSERVICE)),
                    String::EMPTY,
                    PEGASUS_QUEUENAME_INDICATIONSERVICE);

                _routing_table.insert_record(
                    PEGASUS_CLASSNAME_FORMATTEDINDSUBSCRIPTION,
                    _wild,
                    DynamicRoutingTable::INTERNAL,
                    0,
                    static_cast<MessageQueueService*>(MessageQueue::lookup(
                        PEGASUS_QUEUENAME_INDICATIONSERVICE)),
                    String::EMPTY,
                    PEGASUS_QUEUENAME_INDICATIONSERVICE);

                _routing_table.insert_record(
                    PEGASUS_CLASSNAME_INDHANDLER,
                    _wild,
                    DynamicRoutingTable::INTERNAL,
                    0,
                    static_cast<MessageQueueService*>(MessageQueue::lookup(
                        PEGASUS_QUEUENAME_INDICATIONSERVICE)),
                    String::EMPTY,
                    PEGASUS_QUEUENAME_INDICATIONSERVICE);

                        _routing_table.insert_record(
                    PEGASUS_CLASSNAME_LSTNRDST_CIMXML,
                    _wild,
                    DynamicRoutingTable::INTERNAL,
                    0,
                    static_cast<MessageQueueService*>(MessageQueue::lookup(
                        PEGASUS_QUEUENAME_INDICATIONSERVICE)),
                    String::EMPTY,
                    PEGASUS_QUEUENAME_INDICATIONSERVICE);

                _routing_table.insert_record(
                    PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                    _wild,
                    DynamicRoutingTable::INTERNAL,
                    0,
                    static_cast<MessageQueueService*>(MessageQueue::lookup(
                        PEGASUS_QUEUENAME_INDICATIONSERVICE)),
                    String::EMPTY,
                    PEGASUS_QUEUENAME_INDICATIONSERVICE);

                _routing_table.insert_record(
                    PEGASUS_CLASSNAME_INDHANDLER_SNMP,
                    _wild,
                    DynamicRoutingTable::INTERNAL,
                    0,
                    static_cast<MessageQueueService*>(MessageQueue::lookup(
                        PEGASUS_QUEUENAME_INDICATIONSERVICE)),
                    String::EMPTY,
                    PEGASUS_QUEUENAME_INDICATIONSERVICE);

                _routing_table.insert_record(
                    PEGASUS_CLASSNAME_LSTNRDST_SYSTEM_LOG,
                    _wild,
                    DynamicRoutingTable::INTERNAL,
                    0,
                    static_cast<MessageQueueService*>(MessageQueue::lookup(
                        PEGASUS_QUEUENAME_INDICATIONSERVICE)),
                    String::EMPTY,
                    PEGASUS_QUEUENAME_INDICATIONSERVICE);

                _routing_table.insert_record(
                    PEGASUS_CLASSNAME_LSTNRDST_EMAIL,
                    _wild,
                    DynamicRoutingTable::INTERNAL,
                    0,
                    static_cast<MessageQueueService*>(MessageQueue::lookup(
                        PEGASUS_QUEUENAME_INDICATIONSERVICE)),
                    String::EMPTY,
                    PEGASUS_QUEUENAME_INDICATIONSERVICE);

                _routing_table.insert_record(
                    PEGASUS_CLASSNAME_INDFILTER,
                    _wild,
                    DynamicRoutingTable::INTERNAL,
                    0,
                    static_cast<MessageQueueService*>(MessageQueue::lookup(
                        PEGASUS_QUEUENAME_INDICATIONSERVICE)),
                    String::EMPTY,
                    PEGASUS_QUEUENAME_INDICATIONSERVICE);
            }
            _initialized = 1;
        }
    }

    MessageQueueService* router =
        _routing_table.get_routing(
            className,
            nameSpace,
            DynamicRoutingTable::INTERNAL,
            0,
            provider,
            service);
    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
        "Internal provider  Service = " + service + " provider " + provider +
            " found.");

    PEG_METHOD_EXIT();
    return (router != 0);
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
    //
    if (!className.equal (PEGASUS_CLASSNAME___NAMESPACE))
    {
        try
        {
            // Get the complete list of subclass names
            _repository->getSubClassNames(nameSpace,
                 className, true, subClassNames);
            PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
                Logger::TRACE,
                "CIMOperationRequestDispatcher::_getSubClassNames - "
                    "Namespace: $0  Class name: $1",
                nameSpace.getString(),
                className.getString()));
        }
        catch (...)
        {
            // getSubClassNames throws an exception if the class does not exist
            PEG_METHOD_EXIT();
            throw;
        }
    }
    // Prepend the array with the classname that formed the array.
    subClassNames.prepend(className);
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
// Provider Lookup Functions
///////////////////////////////////////////////////////////
/* _lookupAllInstanceProviders - Returns the list of all subclasses of this
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
   validated in the operation code.
*/
Array<ProviderInfo> CIMOperationRequestDispatcher::_lookupAllInstanceProviders(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Uint32& providerCount)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAllInstanceProviders");

    providerCount = 0;

    // NOTE: This function can generate an exception.
    Array<CIMName> classNames = _getSubClassNames(nameSpace, className);

    Array<ProviderInfo> providerInfoList;

    // Loop for all classNames found
    for (Uint32 i = 0, n = classNames.size(); i < n; i++)
    {
        // Lookup any instance providers and add to send list
        ProviderInfo providerInfo =
            _lookupNewInstanceProvider(
                nameSpace,
                classNames[i]);

        if (providerInfo.hasProvider)
        {
            providerCount++;

            CDEBUG("Found provider for class = " << classNames[i].getString());

            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "Provider found for class = " +
                    providerInfo.className.getString() +
                    " servicename = " + providerInfo.serviceName +
                    " controlProviderName = " +
                    providerInfo.controlProviderName);
        }
        else
        {
            CDEBUG("No provider for class = " << classNames[i].getString());
        }

        providerInfoList.append(providerInfo);
   }

   PEG_METHOD_EXIT();

   return providerInfoList;
}

/* _lookupInstanceProvider - Looks up the instance provider for the
    classname and namespace.
    Returns the name of the provider.
    NOTE: This is the low level lookup for compatibility with
    the external lookup in provider registration manager.
    Added value is simply to get the name property from the returned
    Instance.

*/
ProviderInfo CIMOperationRequestDispatcher::_lookupInstanceProvider(
   const CIMNamespaceName& nameSpace,
   const CIMName& className)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupInstanceProvider");

    ProviderInfo providerInfo(className);

    String providerName;
    String serviceName;

    MessageQueueService* router =
       _routing_table.get_routing(
           className,
           nameSpace,
           DynamicRoutingTable::INSTANCE,
           0,
           providerName,
           serviceName);

    if (router)
    {
       PEG_TRACE_STRING(
           TRC_DISPATCHER,
           Tracer::LEVEL4,
           "providerName = " + providerName + " found.");

       providerInfo.serviceName = serviceName;
       providerInfo.controlProviderName = providerName;

       PEG_METHOD_EXIT();

       return providerInfo;
    }

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
        providerInfo.serviceName = PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP;

        providerInfo.hasProvider = true;
        providerInfo.hasNoQuery = hasNoQuery;

#ifdef PEGASUS_ENABLE_REMOTE_CMPI
        String remoteInformation;
        Boolean isRemote =
            _repository->isRemoteNameSpace(nameSpace, remoteInformation);

        if (isRemote)
        {
            providerInfo.providerIdContainer.reset(new ProviderIdContainer(
                pmInstance, pInstance, isRemote, remoteInformation));
        }
        else
        {
            providerInfo.providerIdContainer.reset(
                new ProviderIdContainer(pmInstance, pInstance));
        }
#else
        providerInfo.providerIdContainer.reset(
            new ProviderIdContainer(pmInstance, pInstance));
#endif

        Uint32 pos = PEG_NOT_FOUND;

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
        if (_enableNormalization)
        {
            // normalization is enabled for all providers unless they
            // have an old interface version or are explicity excluded by
            // the excludeModulesFromNormalization configuration option.

            // check interface type and version

            String interfaceType;
            String interfaceVersion;

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
                pmInstance.getProperty(pos).getValue().get(interfaceVersion);
            }

            // compare the interface type and vesion
            if (String::equalNoCase(interfaceType, "C++Default"))
            {
                // version must be greater than 2.5.0
                if (String::compare(interfaceVersion, "2.5.0") >= 0)
                {
                    providerInfo.hasProviderNormalization = true;
                }
            }
            else if (String::equalNoCase(interfaceType, "CMPI"))
            {
                // version must be greater than 2.0.0
                if (String::compare(interfaceVersion, "2.0.0") >= 0)
                {
                    providerInfo.hasProviderNormalization = true;
                }
            }
            else if (String::equalNoCase(interfaceType, "JMPI"))
            {
                // version must be greater than 1.0.0
                if (String::compare(interfaceVersion, "1.0.0") >= 0)
                {
                    providerInfo.hasProviderNormalization = true;
                }
            }

            // check for module exclusion

            String moduleName;

            // get the provder module name
            if ((pos = pmInstance.findProperty("Name")) != PEG_NOT_FOUND)
            {
                pmInstance.getProperty(pos).getValue().get(moduleName);
            }

            // check if module name is on the excludeModulesFromNormalization
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

            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "Normalization for provider module " + moduleName + " is " +
                    (providerInfo.hasProviderNormalization ?
                        "enabled" : "disabled") +
                    ".");
        }
#endif

        /*
        // get the provder name
        if ((pos = pInstance.findProperty("Name")) != PEG_NOT_FOUND)
        {
            pInstance.getProperty(pos).getValue().get(providerName);

            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "Provider for class " + className.getString() + " provider " +
                    providerName + " found.");

            PEG_METHOD_EXIT();

            return providerInfo;
        }
        */

        return providerInfo;
    }

    PEG_TRACE_STRING(
        TRC_DISPATCHER,
        Tracer::LEVEL4,
        "Provider for " + className.getString() + " not found.");

    PEG_METHOD_EXIT();

    return providerInfo;
}

/* _lookupNewInstanceProvider - Looks up the internal and/or instance provider
    for the defined namespace and class and returns the serviceName and
    control provider name if a provider is found.
    @return true if a service, control provider, or instance provider is found
    for the defined class and namespace.
    This should be combined with the lookupInstanceProvider code eventually but
    the goal now was to simplify the handlers.
*/
ProviderInfo CIMOperationRequestDispatcher::_lookupNewInstanceProvider(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupNewInstanceProvider");

    ProviderInfo providerInfo(className);

    String serviceName;
    String controlProviderName;

    // Check for class provided by an internal provider
    Boolean hasControlProvider =
        _lookupInternalProvider(
            nameSpace,
            className,
            serviceName,
            controlProviderName);

    if (hasControlProvider)
    {
        providerInfo.serviceName = serviceName;
        providerInfo.controlProviderName = controlProviderName;
        providerInfo.hasProvider = true;
    }
    else
    {
        // get provider for class
        providerInfo =
            _lookupInstanceProvider(
                nameSpace,
                className);
    }

    PEG_TRACE_STRING(
        TRC_DISPATCHER,
        Tracer::LEVEL4,
        "Lookup Provider = " + providerInfo.serviceName +
            " provider " + providerInfo.controlProviderName + " found."
            + " return = " + (providerInfo.hasProvider ? "true" : "false"));

    PEG_LOGGER_TRACE((
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "CIMOperationRequestDispatcher::_lookupNewInstanceProvider - "
            "Namespace: $0  Class Name: $1  Service Name: $2  "
            "Provider Name: $3",
        nameSpace.getString(),
        className.getString(),
        providerInfo.serviceName,
        providerInfo.controlProviderName));

    PEG_METHOD_EXIT();

    return providerInfo;
}

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
    String providerName;
    String serviceName;

    MessageQueueService* router =
        _routing_table.get_routing(className,
            nameSpace,
            DynamicRoutingTable::METHOD,
            0,
            providerName,
            serviceName);
    if (router)
    {
        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "providerName = " + providerName + " found.");
        PEG_METHOD_EXIT();
        return providerName;
    }


    if (_providerRegistrationManager->lookupMethodProvider(
            nameSpace, className, methodName, pInstance, pmInstance))
    {
        ProviderIdContainer* providercontainer;
#ifdef PEGASUS_ENABLE_REMOTE_CMPI
        String remoteInformation;
        Boolean isRemote = false;
        isRemote = _repository->isRemoteNameSpace(
            nameSpace,
            remoteInformation);
        if (isRemote)
             providercontainer = new ProviderIdContainer(
                pmInstance, pInstance, isRemote, remoteInformation);
        else
            providercontainer = new ProviderIdContainer(pmInstance, pInstance);
#else
        providercontainer = new ProviderIdContainer(pmInstance, pInstance);
#endif
        (*providerIdContainer) = providercontainer;
        // get the provder name
        Uint32 pos = pInstance.findProperty(CIMName ("Name"));

        if (pos != PEG_NOT_FOUND)
        {
            pInstance.getProperty(pos).getValue().get(providerName);

            PEG_METHOD_EXIT();
            return providerName;
        }
        else
        {
            PEG_METHOD_EXIT();
            return String::EMPTY;
        }
    }
    else
    {
        PEG_METHOD_EXIT();
        return String::EMPTY;
    }
}

/* _lookupAllAssociation Providers for Class
    Returns all of the association providers that exist
    for the defined class name.  Used the referencenames
    function to get list of classes for which providers
    required and then looks up the providers for each
    class
    @param nameSpace
    @param objectName
    @param - referenced parameter that is modified in this funciton.  This
    is the count of actual providers found, not the count of classes.  This
    differs from the count of classes in that the providerInfo list is all
    classes including those that would go to the repository.
    @returns List of ProviderInfo
    @exception - Exceptions From the Repository
*/
Array<ProviderInfo>
    CIMOperationRequestDispatcher::_lookupAllAssociationProviders(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const String& role,
        Uint32& providerCount)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAllAssociationProviders");

    providerCount = 0;
    Array<ProviderInfo> providerInfoList;

    Array<CIMName> classNames;
    CIMName className = objectName.getClassName();
    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
        "Association Class Lookup for Class " + className.getString());

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

    CDEBUG("_LookupALLAssocProvider Calling referenceNames. class " <<
        className);

    Array<CIMObjectPath> tmp;
    CIMException cimException;

    try
    {
        // Note:  We use assocClass because this is the association function.
        // The Reference(Name)s calls must put the resultClass here.
        tmp = _repository->referenceNames(
            nameSpace,
            CIMObjectPath(String::EMPTY, CIMNamespaceName(), className),
            assocClass,
            role);
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

    // If exception from repository, write log message and throw the exception
    if (cimException.getCode() != CIM_ERR_SUCCESS)
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
            "CIMOperationRequestDispatcher::lookupAllAssociationProvider "
                "exception.  Namespace: $0  Object Name: $1  Assoc Class: $2",
            nameSpace.getString(),
            objectName.toString(),
            assocClass.getString());
        throw cimException;
    }


    // returns the list of possible association classes for this target.
    // Convert to classnames
    for (Uint32 i = 0; i < tmp.size(); i++)
    {
        classNames.append(tmp[i].getClassName());
        CDEBUG("Reference Lookup returnsclass " << tmp[i].getClassName());
    }

    CDEBUG("_lookup all assoc Classes Returned class list of size " <<
        classNames.size() << " className= " << className.getString() <<
        " assocClass= " << assocClass);

    // CV 20050211 Commented out debug loops
    //for (Uint32 i = 0; i < classNames.size(); i++)
    //{
    //    CDEBUG(" Count i " << i << "Class rtned " <<
    //        classNames[i].getString());
    //}

    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
        Formatter::format(" Association Lookup $0 classes found",
            classNames.size()));

    // ATTN: KS P2 20030420  What should we do with remote associations if
    // there are any
    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        String serviceName = String::EMPTY;
        String controlProviderName = String::EMPTY;
        ProviderInfo pi(classNames[i]);
        ProviderIdContainer* container=NULL;

        // We use the returned classname for the association classname
        // under the assumption that the registration is for the
        // association class, not the target class
        if (_lookupNewAssociationProvider(nameSpace, classNames[i],
            serviceName, controlProviderName,&container))
        {
            //CDEBUG("LookupNew return. Class = " <<   classNames[i]);
            pi.serviceName = serviceName;
            pi.controlProviderName = controlProviderName;
            pi.hasProvider = true;
            pi.providerIdContainer.reset(container);
            providerCount++;
            CDEBUG("Found Association Provider for class = " <<
                classNames[i].getString());

            // ATTN: Do not need this trace.  The _LookupNewAssoc Function
            // should handle it.
            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                "Provider found for Class = " + classNames[i].getString() +
                    " servicename = " + serviceName +
                    " controlProviderName = " +
                    ((controlProviderName.size()) ? controlProviderName
                                                  : String("None")));
            CDEBUG("Found Assoc wo Provider for class= " <<
                classNames[i].getString());
        }
        else
        {
            pi.hasProvider = false;
            pi.providerIdContainer.reset();
        }
        providerInfoList.append(pi);
    }
    CDEBUG("_lookupAllAssocProvider rtns count = " <<
        providerInfoList.size() << " providerCount= " << providerCount);
    PEG_METHOD_EXIT();
    return providerInfoList;
}

/* _lookupNewAssociationProvider - Looks up the internal and/or instance
    provider for the defined namespace and class and returns the serviceName
    and control provider name if a provider is found.
    @param nameSpace
    @param assocClass
    @param serviceName
    @param controlProviderName
    @return true if an service, control provider, or instance provider is found
    for the defined class and namespace.
    This should be combined with the lookupInstanceProvider code eventually but
    the goal now was to simplify the handlers.
*/
Boolean CIMOperationRequestDispatcher::_lookupNewAssociationProvider(
    const CIMNamespaceName& nameSpace,
    const CIMName& assocClass,
    String& serviceName,
    String& controlProviderName,
    ProviderIdContainer** providerIdContainer)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupNewAssociationProvider");
    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
        "assocClass = " + assocClass.getString());

    Boolean hasProvider = false;
    String providerName = String::EMPTY;
    // Check for class provided by an internal provider
    if (_lookupInternalProvider(
            nameSpace, assocClass, serviceName, controlProviderName))
        hasProvider = true;
    else
    {
        // get provider for class. Note that we reduce it from
        // Multiple possible class names to a single one here.
        // This is a hack.  Clean up later
        Array<String> tmp;
        CIMException cimException;
        try
        {
            tmp = _lookupAssociationProvider(
                nameSpace, assocClass,providerIdContainer);
        }
        catch (const CIMException&)
        {
            PEG_METHOD_EXIT();
            throw;
        }
        catch (const Exception& exception)
        {
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
        }
        catch (...)
        {
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        if (tmp.size() > 0)
        {
            providerName = tmp[0];
        }
    }

    if (providerName != String::EMPTY)
    {
        serviceName = PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP;
        hasProvider = true;
    }
    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
        "Lookup Provider = " + serviceName + " provider " + providerName +
            " found." + " return = " + (hasProvider? "true" : "false"));

    PEG_METHOD_EXIT();
    return hasProvider;
}

// Lookup the Association provider(s) for this class and convert the
// result to an array of providernames.
// NOTE: The code allows for multiple providers but there is no reason
// to have this until we really support multiple providers per class.
// The multiple provider code was here because there was some code in to
// do the lookup in provider registration which was removed. Some day we
// will support multiple providers per class so it was logical to just leave
// the multiple provider code in place.
// NOTE: assocClass and resultClass are optional
//
Array<String> CIMOperationRequestDispatcher::_lookupAssociationProvider(
   const CIMNamespaceName& nameSpace,
   const CIMName& assocClass,
   ProviderIdContainer** providerIdContainer)
{
    // instances of the provider class and provider module class for the
    // response
    Array<CIMInstance> pInstances; // Providers
    Array<CIMInstance> pmInstances; // ProviderModules

    Array<String> providerNames;
    String providerName;

    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAssociationProvider");
    //Isolate the provider names from the response and return list of providers
    Boolean returnValue = false;
    CIMException cimException;
    try
    {
        CDEBUG("_lookupAssociationProvider. assocClass= " <<
            assocClass.getString());
        returnValue = _providerRegistrationManager->lookupAssociationProvider(
                nameSpace, assocClass, pInstances, pmInstances);
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
                ProviderIdContainer* providercontainer;
#ifdef PEGASUS_ENABLE_REMOTE_CMPI
                String remoteInformation=String::EMPTY;
                Boolean isRemote=false;
                isRemote = _repository->isRemoteNameSpace(
                    nameSpace, remoteInformation);
                if (isRemote)
                    providercontainer = new ProviderIdContainer(
                        pmInstances[i],
                        pInstances[i],
                        isRemote,
                        remoteInformation);
                else
                    providercontainer = new ProviderIdContainer(
                        pmInstances[i],
                        pInstances[i]);
#else
                providercontainer = new ProviderIdContainer(
                    pmInstances[i],
                    pInstances[i]);
#endif
                (*providerIdContainer) = providercontainer;
            }
            // get the provider name
            Uint32 pos = pInstances[i].findProperty(CIMName ("Name"));

            if ( pos != PEG_NOT_FOUND )
            {
                pInstances[i].getProperty(pos).getValue().get(providerName);

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                    "Association providerName = " + providerName + " found." +
                        " for Class " + assocClass.getString());
                providerNames.append(providerName);
            }
        }
    }

    if (providerNames.size() == 0)
    {
        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            "Association Provider NOT found for Class " +
                assocClass.getString() +
                " in nameSpace " +
                nameSpace.getString());
    }
    PEG_METHOD_EXIT();
    return providerNames;
}

/*  Common Dispatcher callback for response aggregation.
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

    // Verify that the aggregator is valid.
    PEGASUS_ASSERT(poA->valid());
    //CDEBUG("_ForwardForAggregationCallback ");
    CIMResponseMessage* response = 0;

    Uint32 msgType = asyncReply->getType();

    if (msgType == async_messages::ASYNC_LEGACY_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage*>(
            (static_cast<AsyncLegacyOperationResult*>(asyncReply))->
                get_result());
    }
    else if (msgType == async_messages::ASYNC_MODULE_OP_RESULT)
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
    PEGASUS_ASSERT(response->messageId == poA->_messageId);
    delete asyncReply;

    // Before resequencing, the isComplete() flag represents the completion
    // status of one provider's response, not the entire response

    if (response->isComplete())
    {
        // these are per provider instantiations
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Provider response is complete.");
        op->removeRequest();
        delete asyncRequest;
        op->release();
        service->return_op(op);
    }
    else
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Provider response is incomplete.");
    }

    // After resequencing, this flag represents the completion status of
    // the ENTIRE response to the request.

    Boolean isComplete = service->_enqueueResponse(poA, response);

    if (isComplete)
    {
        // also deletes the copied request attached to it
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Entire response to a request is complete.");
        delete poA;
        poA = 0;
    }
    else
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "The response to a request is not complete.");
    }

    PEG_METHOD_EXIT();
}


/*  Common Dispatcher callback.
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

    CIMRequestMessage* request =
        reinterpret_cast<CIMRequestMessage*>(userParameter);
    PEGASUS_ASSERT(request != 0);

    CIMResponseMessage* response = 0;

    Uint32 msgType = asyncReply->getType();

    if (msgType == async_messages::ASYNC_LEGACY_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage*>(
            (static_cast<AsyncLegacyOperationResult*>(asyncReply))->
                get_result());
    }
    else if (msgType == async_messages::ASYNC_MODULE_OP_RESULT)
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
        op->release();
        service->return_op(op);
    }

    PEG_METHOD_EXIT();
}


void CIMOperationRequestDispatcher::_forwardRequestToService(
    const String& serviceName,
    CIMRequestMessage* request,
    CIMRequestMessage* requestCopy)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToService");

    Array<Uint32> serviceIds;
    find_services(serviceName, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    AsyncOpNode* op = this->get_op();

    AsyncLegacyOperationStart* asyncRequest =
        new AsyncLegacyOperationStart(
            op,
            serviceIds[0],
            request,
            this->getQueueId());

    asyncRequest->dest = serviceIds[0];

    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3,
        "Forwarding " + String(MessageTypeToString(request->getType())) +
            " to service " + serviceName + ". Response should go to queue " +
            ((MessageQueue::lookup(request->queueIds.top())) ?
                String(((MessageQueue::lookup(
                    request->queueIds.top()))->getQueueName()) ) :
                String("BAD queue name")));

    SendAsync(
        op,
        serviceIds[0],
        CIMOperationRequestDispatcher::_forwardRequestCallback,
        this,
        requestCopy);

    PEG_METHOD_EXIT();
}

/* This function simply decides based on the controlProviderNameField
    whether to forward to Service or ControlProvider.
    If controlProviderName String empty, ToService, else toControlProvider.
    This function specifically forwards requests for response aggregation.
*/
void CIMOperationRequestDispatcher::_forwardRequestForAggregation(
    const String& serviceName,
    const String& controlProviderName,
    CIMRequestMessage* request,
    OperationAggregate* poA,
    CIMResponseMessage* response)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestForAggregation");

    Array<Uint32> serviceIds;
    find_services(serviceName, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    //CDEBUG("ForwardRequestForAggregation");
    AsyncOpNode* op = this->get_op();

    // if a response is given, this means the caller wants to run only the
    // callback asynchronously
    if (response)
    {
        AsyncLegacyOperationResult* asyncResponse =
            new AsyncLegacyOperationResult(
                op, response);

        // By setting this to complete, this allows ONLY the callback to run
        // without going through the typical async request apparatus
        op->complete();
    }

    // If ControlProviderName empty, forward to service.
    if (controlProviderName == String::EMPTY)
    {
        AsyncLegacyOperationStart* asyncRequest =
            new AsyncLegacyOperationStart(
                op,
                serviceIds[0],
                request,
                this->getQueueId());

        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3,
            "Forwarding " + String(MessageTypeToString(request->getType())) +
                " to service " + serviceName +
                ". Response should go to queue " +
                ((MessageQueue::lookup(request->queueIds.top())) ?
                    String(((MessageQueue::lookup(
                        request->queueIds.top()))->getQueueName())) :
                    String("BAD queue name")));

        SendAsync(
            op,
            serviceIds[0],
            CIMOperationRequestDispatcher::_forwardForAggregationCallback,
            this,
            poA);
    }
    else
    {
       AsyncModuleOperationStart* moduleControllerRequest =
           new AsyncModuleOperationStart(
               op,
               serviceIds[0],
               this->getQueueId(),
               true,
               controlProviderName,
               request);

        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3,
            "Forwarding " + String(MessageTypeToString(request->getType())) +
                " to service " + serviceName + ", control provider " +
                controlProviderName + ". Response should go to queue " +
                ((MessageQueue::lookup(request->queueIds.top())) ?
                    String(((MessageQueue::lookup(
                        request->queueIds.top()))->getQueueName())) :
                String("BAD queue name")));

       SendAsync(
           op,
           serviceIds[0],
           CIMOperationRequestDispatcher::_forwardForAggregationCallback,
           this,
           poA);
    }

    PEG_METHOD_EXIT();
}

/** _forwardRequestToProviderManager
    This function decides based on the controlProviderNameField
    whether to forward to Service or ControlProvider.
    If controlProviderName String empty,
           ForwardToService,
       else
           ForwardtoControlProvider
    Convience coding to simplify other functions.
*/
void CIMOperationRequestDispatcher::_forwardRequestToProviderManager(
    const CIMName& className,        // only for diagnostic
    const String& serviceName,
    const String& controlProviderName,
    CIMRequestMessage* request,
    CIMRequestMessage* requestCopy)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToProviderManager");

    Array<Uint32> serviceIds;
    find_services(serviceName, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    AsyncOpNode* op = this->get_op();

    // If ControlProviderName empty, forward to service.
    if (controlProviderName == String::EMPTY)
    {
        AsyncLegacyOperationStart* asyncRequest =
            new AsyncLegacyOperationStart(
                op,
                serviceIds[0],
                request,
                this->getQueueId());

        asyncRequest->dest = serviceIds[0];

        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3,
            "Forwarding " + String(MessageTypeToString(request->getType())) +
                " on class " + className.getString() + " to service " +
                serviceName + ". Response should go to queue " +
                ((MessageQueue::lookup(request->queueIds.top())) ?
                    String(((MessageQueue::lookup(
                        request->queueIds.top()))->getQueueName())) :
                    String("BAD queue name")));

        SendAsync(
            op,
            serviceIds[0],
            CIMOperationRequestDispatcher::_forwardRequestCallback,
            this,
            requestCopy);
    }
    else
    {
        AsyncModuleOperationStart* moduleControllerRequest =
            new AsyncModuleOperationStart(
                op,
                serviceIds[0],
                this->getQueueId(),
                true,
                controlProviderName,
                request);

        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3,
            "Forwarding " + String(MessageTypeToString(request->getType())) +
                " on class " + className.getString() + " to service " +
                serviceName + ", control provider " + controlProviderName +
                ". Response should go to queue " +
                    ((MessageQueue::lookup(request->queueIds.top())) ?
                        String(((MessageQueue::lookup(
                            request->queueIds.top()))->getQueueName())) :
                        String("BAD queue name")));

       // Send to the Control provider
       SendAsync(
           op,
           serviceIds[0],
           CIMOperationRequestDispatcher::_forwardRequestCallback,
           this,
           requestCopy);
    }

    PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::_enqueueResponse(
   CIMRequestMessage* request,
   CIMResponseMessage* response)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_enqueueResponse");

    response->dest = request->queueIds.top();
    // Use the same key,setHTTPMethod and closeConnect as used in the request:
    response->syncAttributes(request);
    PEG_TRACE((
        TRC_HTTP,
        Tracer::LEVEL3,
        "_CIMOperationRequestDispatcher::_enqueueResponse - "
            "request->getCloseConnect() returned %d",
        request->getCloseConnect()));

    _logOperation(request, response);

    if (Base::_enqueueResponse(request, response))
    {
        PEG_METHOD_EXIT();
        return;
    }

    MessageQueue* queue = MessageQueue::lookup(request->queueIds.top());
    PEGASUS_ASSERT(queue != 0);

    queue->enqueue(response);

    PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnqueue(Message* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnqueue(Message* request)");
    if (!request)
    {
        PEG_METHOD_EXIT();
        return;
    }

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::handleEnqueue - Case: $0",
        request->getType()));

    // Set the client's requested language into this service thread.
    // This will allow functions in this service to return messages
    // in the correct language.
    CIMMessage* req = dynamic_cast<CIMMessage*>(request);
    if (req != NULL)
    {
        if (req->thread_changed())
        {
            Thread::setLanguages(new AcceptLanguageList(
                ((AcceptLanguageListContainer)req->operationContext.get(
                    AcceptLanguageListContainer::NAME)).getLanguages()));
        }
    }
    else
    {
         Thread::clearLanguages();
    }

    switch(request->getType())
    {
        case CIM_GET_CLASS_REQUEST_MESSAGE:
            handleGetClassRequest((CIMGetClassRequestMessage*)request);
            break;

        case CIM_GET_INSTANCE_REQUEST_MESSAGE:
            handleGetInstanceRequest((CIMGetInstanceRequestMessage*)request);
            break;

        case CIM_DELETE_CLASS_REQUEST_MESSAGE:
            handleDeleteClassRequest(
                (CIMDeleteClassRequestMessage*)request);
            break;

        case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
            handleDeleteInstanceRequest(
                (CIMDeleteInstanceRequestMessage*)request);
            break;

        case CIM_CREATE_CLASS_REQUEST_MESSAGE:
            handleCreateClassRequest((CIMCreateClassRequestMessage*)request);
            break;

        case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
            handleCreateInstanceRequest(
                (CIMCreateInstanceRequestMessage*)request);
            break;

        case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
            handleModifyClassRequest((CIMModifyClassRequestMessage*)request);
            break;

        case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
            handleModifyInstanceRequest(
                (CIMModifyInstanceRequestMessage*)request);
            break;

        case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
            handleEnumerateClassesRequest(
                (CIMEnumerateClassesRequestMessage*)request);
            break;

        case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
            handleEnumerateClassNamesRequest(
                (CIMEnumerateClassNamesRequestMessage*)request);
            break;

        case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
            handleEnumerateInstancesRequest(
                (CIMEnumerateInstancesRequestMessage*)request);
            break;

        case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
            handleEnumerateInstanceNamesRequest(
                (CIMEnumerateInstanceNamesRequestMessage*)request);
            break;

        case CIM_EXEC_QUERY_REQUEST_MESSAGE:
            handleExecQueryRequest(
                (CIMExecQueryRequestMessage*)request);
            break;

        case CIM_ASSOCIATORS_REQUEST_MESSAGE:
            handleAssociatorsRequest((CIMAssociatorsRequestMessage*)request);
            break;

        case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
            handleAssociatorNamesRequest(
                (CIMAssociatorNamesRequestMessage*)request);
            break;

        case CIM_REFERENCES_REQUEST_MESSAGE:
            handleReferencesRequest((CIMReferencesRequestMessage*)request);
            break;

        case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
            handleReferenceNamesRequest(
                (CIMReferenceNamesRequestMessage*)request);
            break;

        case CIM_GET_PROPERTY_REQUEST_MESSAGE:
            handleGetPropertyRequest(
                (CIMGetPropertyRequestMessage*)request);
            break;

        case CIM_SET_PROPERTY_REQUEST_MESSAGE:
            handleSetPropertyRequest(
                (CIMSetPropertyRequestMessage*)request);
            break;

        case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
            handleGetQualifierRequest((CIMGetQualifierRequestMessage*)request);
            break;

        case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
            handleSetQualifierRequest((CIMSetQualifierRequestMessage*)request);
            break;

        case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
            handleDeleteQualifierRequest(
                (CIMDeleteQualifierRequestMessage*)request);
            break;

        case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
            handleEnumerateQualifiersRequest(
                (CIMEnumerateQualifiersRequestMessage*)request);
            break;

        case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
            handleInvokeMethodRequest(
                (CIMInvokeMethodRequestMessage*)request);
            break;

       default:
           Logger::put_l(
               Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
               "Server.CIMOperationRequestDispatcher.HANDLE_ENQUEUE",
               "$0 - Case: $1 not valid",
               "CIMOperationRequestDispatcher::handleEnqueue",
               request->getType());
     }

     delete request;
     PEG_METHOD_EXIT();
}

// allocate a CIM Operation_async,  opnode, context, and response handler
// initialize with pointers to async top and async bottom
// link to the waiting q
void CIMOperationRequestDispatcher::handleEnqueue()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnqueue");

    Message* request = dequeue();

    if (request)
        handleEnqueue(request);

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleGetClassRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleGetClassRequest(
    CIMGetClassRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleGetClassRequest");

    AutoPtr<CIMGetClassResponseMessage> response(
        dynamic_cast<CIMGetClassResponseMessage*>(
            request->buildResponse()));

    // ATTN: Need code here to expand partial class!

    CIMException cimException;
    CIMClass cimClass;

    try
    {
        StatProviderTimeMeasurement providerTime(response.get());

        cimClass = _repository->getClass(
            request->nameSpace,
            request->className,
            request->localOnly,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList);

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationRequestDispatcher::handleGetClassRequest - "
                "Namespace: $0  Class name: $1",
            request->nameSpace.getString(),
            request->className.getString()));
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

    response->cimClass = cimClass;
    response->cimException = cimException;

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

    // ATTN: Need code here to expand partial instance!

    // get the class name
    CIMName className = request->instanceName.getClassName();
    CIMException checkClassException;

    CIMClass cimClass =
        _getClass(
            request->nameSpace,
            className,
            checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMOperationRequestDispatcher::handleGetInstanceRequest - "
                "CIM exist exception has occurred.  Namespace: $0  "
                "Class Name: $1",
            request->nameSpace.getString(),
            className.getString()));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    //String serviceName;
    //String controlProviderName;
    //ProviderIdContainer* providerIdContainer = 0;

    ProviderInfo providerInfo =
        _lookupNewInstanceProvider(
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

            //delete providerIdContainer;
            //providerIdContainer = 0;
        }

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
        if (_enableNormalization && providerInfo.hasProviderNormalization)
        {
            requestCopy->operationContext.insert(
                CachedClassDefinitionContainer(cimClass));
        }
#endif

        CIMGetInstanceRequestMessage* requestCallbackCopy =
            new CIMGetInstanceRequestMessage(*requestCopy);

        _forwardRequestToProviderManager(
            providerInfo.className,
            providerInfo.serviceName,
            providerInfo.controlProviderName,
            requestCopy,
            requestCallbackCopy);

         PEG_METHOD_EXIT();
         return;
    }

    // not internal or found provider, go to default
    AutoPtr<CIMGetInstanceResponseMessage> response(
        dynamic_cast<CIMGetInstanceResponseMessage*>(
            request->buildResponse()));

    if (_repository->isDefaultInstanceProvider())
    {
        CIMException cimException;
        CIMInstance cimInstance;

        try
        {
            StatProviderTimeMeasurement providerTime(response.get());

            cimInstance =
                _repository->getInstance(
                    request->nameSpace,
                    request->instanceName,
                    request->localOnly,
                    request->includeQualifiers,
                    request->includeClassOrigin,
                    request->propertyList);
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
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        response->cimInstance = cimInstance;
        response->cimException = cimException;

        _enqueueResponse(request, response.release());
    }
    else // No provider is registered and the repository isn't the default
    {
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),

       _enqueueResponse(request, response.release());
    }

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleDeleteClassRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleDeleteClassRequest(
    CIMDeleteClassRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleDeleteClassRequest");

    AutoPtr<CIMDeleteClassResponseMessage> response(
        dynamic_cast<CIMDeleteClassResponseMessage*>(
            request->buildResponse()));

    CIMException cimException;

    try
    {
        StatProviderTimeMeasurement providerTime(response.get());

        _repository->deleteClass(
            request->nameSpace,
            request->className);

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationRequestDispatcher::handleDeleteClassRequest - "
                "Namespace: $0  Class Name: $1",
            request->nameSpace.getString(),
            request->className.getString()));
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
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    response->cimException = cimException;

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleDeleteInstanceRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleDeleteInstanceRequest(
    CIMDeleteInstanceRequestMessage* request)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleDeleteInstanceRequest");

    // get the class name
    CIMName className = request->instanceName.getClassName();

    CIMException checkClassException;

    _checkExistenceOfClass(request->nameSpace, className, checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMOperationRequestDispatcher::handleDeleteInstanceRequest - "
                "CIM exist exception has occurred.  Namespace: $0  "
                "Class Name: $1",
            request->nameSpace.getString(),
            className.getString()));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    //String serviceName;
    //String controlProviderName;
    //ProviderIdContainer* providerIdContainer = 0;

    ProviderInfo providerInfo =
        _lookupNewInstanceProvider(
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

        _forwardRequestToProviderManager(
            providerInfo.className,
            providerInfo.serviceName,
            providerInfo.controlProviderName,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        return;
    }
    else if (_repository->isDefaultInstanceProvider())
    {
        AutoPtr<CIMDeleteInstanceResponseMessage> response(
            dynamic_cast<CIMDeleteInstanceResponseMessage*>(
                request->buildResponse()));

        CIMException cimException;

        try
        {
            StatProviderTimeMeasurement providerTime(response.get());

            _repository->deleteInstance(
                request->nameSpace,
                request->instanceName);

            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG,
                System::CIMSERVER,
                Logger::TRACE,
                "CIMOperationRequestDispatcher::handleDeleteInstanceRequest - "
                    "Namespace: $0  Instance Name: $1",
                request->nameSpace.getString(),
                request->instanceName.toString()));
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
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        response->cimException = cimException;

        _enqueueResponse(request, response.release());
    }
    else // No provider is registered and the repository isn't the default
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),

        _enqueueResponse(request, response);
    }

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleCreateClassRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleCreateClassRequest(
    CIMCreateClassRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleCreateClassRequest");

    AutoPtr<CIMCreateClassResponseMessage> response(
        dynamic_cast<CIMCreateClassResponseMessage*>(
            request->buildResponse()));

    CIMException cimException;

    try
    {
        removePropagatedAndOriginAttributes(request->newClass);

        StatProviderTimeMeasurement providerTime(response.get());

        _repository->createClass(
            request->nameSpace,
            request->newClass,
            ((ContentLanguageListContainer)request->operationContext.get(
                ContentLanguageListContainer::NAME)).getLanguages());

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationRequestDispatcher::handleCreateClassRequest - "
                "Namespace: $0  Class Name: $1",
            request->nameSpace.getString(),
            request->newClass.getClassName().getString()));
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
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    response->cimException = cimException;

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

    CIMException checkClassException;

    _checkExistenceOfClass(request->nameSpace, className, checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMOperationRequestDispatcher::handleCreateInstanceRequest - "
                "CIM exist exception has occurred.  Namespace: $0  "
                "Class Name: $1",
            request->nameSpace.getString(),
            className.getString()));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    //String serviceName;
    //String controlProviderName;
    //ProviderIdContainer* providerIdContainer = 0;

    ProviderInfo providerInfo =
        _lookupNewInstanceProvider(
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

            //delete providerIdContainer;
            //providerIdContainer = 0;
        }

        CIMCreateInstanceRequestMessage* requestCallbackCopy =
            new CIMCreateInstanceRequestMessage(*requestCopy);

        _forwardRequestToProviderManager(
            providerInfo.className,
            providerInfo.serviceName,
            providerInfo.controlProviderName,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        return;
    }
    else if (_repository->isDefaultInstanceProvider())
    {
        AutoPtr<CIMCreateInstanceResponseMessage> response(
            dynamic_cast<CIMCreateInstanceResponseMessage*>(
                request->buildResponse()));

        CIMException cimException;
        CIMObjectPath instanceName;

        try
        {
            removePropagatedAndOriginAttributes(request->newInstance);

            StatProviderTimeMeasurement providerTime(response.get());

            instanceName = _repository->createInstance(
                request->nameSpace,
                request->newInstance,
                ((ContentLanguageListContainer)request->operationContext.get(
                    ContentLanguageListContainer::NAME)).getLanguages());

            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG,
                System::CIMSERVER,
                Logger::TRACE,
                "CIMOperationRequestDispatcher::handleCreateInstanceRequest - "
                    "Namespace: $0  Instance name: $1",
                request->nameSpace.getString(),
                request->newInstance.getClassName().getString()));
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
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        response->instanceName = instanceName;
        response->cimException = cimException;

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
    return;
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

    AutoPtr<CIMModifyClassResponseMessage> response(
        dynamic_cast<CIMModifyClassResponseMessage*>(
            request->buildResponse()));

    CIMException cimException;

    try
    {
        removePropagatedAndOriginAttributes(request->modifiedClass);

        StatProviderTimeMeasurement providerTime(response.get());

        _repository->modifyClass(
            request->nameSpace,
            request->modifiedClass,
            ((ContentLanguageListContainer)request->operationContext.get(
                ContentLanguageListContainer::NAME)).getLanguages());
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

    response->cimException = cimException;

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleModifyInstanceRequest
    ATTN: FIX LOOKUP
**********************************************************/

void CIMOperationRequestDispatcher::handleModifyInstanceRequest(
    CIMModifyInstanceRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleModifyInstanceRequest");

    // ATTN: Who makes sure the instance name and the instance match?
    // ATTN: KS May 28. Change following to reflect new instancelookup
    // get the class name
    CIMName className = request->modifiedInstance.getClassName();

    CIMException checkClassException;

    _checkExistenceOfClass(request->nameSpace, className, checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMOperationRequestDispatcher::handleModifyInstanceRequest - "
                "CIM exist exception has occurred.  Namespace: $0  "
                "Class Name: $1",
            request->nameSpace.getString(),
            className.getString()));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    //String serviceName;
    //String controlProviderName;
    //ProviderIdContainer* providerIdContainer = 0;

    ProviderInfo providerInfo =
        _lookupNewInstanceProvider(
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

            //delete providerIdContainer;
            //providerIdContainer = 0;
        }

        CIMModifyInstanceRequestMessage* requestCallbackCopy =
            new CIMModifyInstanceRequestMessage(*requestCopy);

        _forwardRequestToProviderManager(
            providerInfo.className,
            providerInfo.serviceName,
            providerInfo.controlProviderName,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        return;
    }
    else if (_repository->isDefaultInstanceProvider())
    {
        // translate and forward request to repository
        CIMException cimException;

        AutoPtr<CIMModifyInstanceResponseMessage> response(
            dynamic_cast<CIMModifyInstanceResponseMessage*>(
                request->buildResponse()));

        try
        {
            removePropagatedAndOriginAttributes(request->modifiedInstance);

            StatProviderTimeMeasurement providerTime(response.get());

            _repository->modifyInstance(
                request->nameSpace,
                request->modifiedInstance,
                request->includeQualifiers,request->propertyList,
                ((ContentLanguageListContainer)request->operationContext.get(
                    ContentLanguageListContainer::NAME)).getLanguages());

            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG,
                System::CIMSERVER,
                Logger::TRACE,
                "CIMOperationRequestDispatcher::handleModifyInstanceRequest - "
                    "Namespace: $0  Instance name: $1",
                request->nameSpace.getString(),
                request->modifiedInstance.getClassName().getString()));
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
           cimException =
               PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
       }

       response->cimException = cimException;

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
    return;
}

/**$*******************************************************
    handleEnumerateClassesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleEnumerateClassesRequest(
    CIMEnumerateClassesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
       "CIMOperationRequestDispatcher::handleEnumerateClassesRequest");

    AutoPtr<CIMEnumerateClassesResponseMessage> response(
        dynamic_cast<CIMEnumerateClassesResponseMessage*>(
            request->buildResponse()));

    Array<CIMClass> cimClasses;
    CIMException cimException;

    try
    {
        StatProviderTimeMeasurement providerTime(response.get());

        cimClasses = _repository->enumerateClasses(
            request->nameSpace,
            request->className,
            request->deepInheritance,
            request->localOnly,
            request->includeQualifiers,
            request->includeClassOrigin);

        PEG_LOGGER_TRACE((
           Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
           "CIMOperationRequestDispatcher::handleEnumerateClassesRequest - "
               "Namespace: $0  Class name: $1",
           request->nameSpace.getString(),
           request->className.getString()));
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
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    response->cimClasses = cimClasses;
    response->cimException = cimException;

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleEnumerateClassNamesRequest
    Passed directly to the CIMRepository
**********************************************************/
void CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest(
    CIMEnumerateClassNamesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
       "CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest");

    AutoPtr<CIMEnumerateClassNamesResponseMessage> response(
        dynamic_cast<CIMEnumerateClassNamesResponseMessage*>(
            request->buildResponse()));

    Array<CIMName> classNames;
    CIMException cimException;

    try
    {
        StatProviderTimeMeasurement providerTime(response.get());

        classNames = _repository->enumerateClassNames(
            request->nameSpace,
            request->className,
            request->deepInheritance);

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest"
                " - Namespace: $0  Class name: $1",
            request->nameSpace.getString(),
            request->className.getString()));
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
        cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }

    response->classNames = classNames;
    response->cimException = cimException;

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

    // get the class name
    CIMName className = request->className;
    CIMException checkClassException;

    CIMClass cimClass = _getClass(
        request->nameSpace,
        className,
        checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    CDEBUG("CIMOP ei client propertyList = " <<
        _showPropertyList(request->propertyList));

    // If DeepInheritance==false and no PropertyList was specified by the
    // client, the provider PropertyList should contain all the properties
    // in the specified class.
    if (!request->deepInheritance && request->propertyList.isNull())
    {
        Array<CIMName> propertyNameArray;
        Uint32 numProperties = cimClass.getPropertyCount();
        for (Uint32 i = 0; i < numProperties; i++)
        {
            propertyNameArray.append(cimClass.getProperty(i).getName());
        }

        request->propertyList.set(propertyNameArray);
    }

    CDEBUG("CIMOP ei provider propertyList = " <<
        _showPropertyList(request->propertyList));

    //
    // Get names of descendent classes:
    //
    CIMException cimException;
    Array<ProviderInfo> providerInfos;

    Uint32 providerCount;

    // Get list of providers.
    try
    {
        CDEBUG("Looking up Instance Providers");
        providerInfos =
            _lookupAllInstanceProviders(
                request->nameSpace,
                className,
                providerCount);
    }
    catch (const CIMException& exception)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = exception;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    Uint32 toIssueCount = providerInfos.size();

    // Test for "enumerate too Broad" and if so, execute exception.
    // This limits the number of provider invocations, not the number
    // of instances returned.
    if (providerCount > _maximumEnumerateBreadth)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "Request-too-broad exception.  Namespace: $0  Class Name: $1  "
            "Limit: $2  ProviderCount: $3",
            request->nameSpace.getString(),
            request->className.getString(),
            _maximumEnumerateBreadth, providerCount));

        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            Formatter::format(
                "ERROR Enumerate too broad for class $0. Limit = $1, "
                    "Request = $2",
                request->className.getString(),
                _maximumEnumerateBreadth,
                providerCount));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED, MessageLoaderParms(
                "Server.CIMOperationRequestDispatcher.ENUM_REQ_TOO_BROAD",
                "Enumerate request too Broad")),

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED

    if ((providerCount == 0) && !(_repository->isDefaultInstanceProvider()))
    {
        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            "CIM_ERROR_NOT_SUPPORTED for " + request->className.getString());

        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    //
    // Get names of descendent classes:
    //
    //CIMException cimException;
    //Array<ProviderInfo> providerInfos;

    // We have instances for Providers and possibly repository.
    // Set up an aggregate object and save a copy of the original request.
    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstancesRequestMessage(*request),
        request->getType(),
        request->messageId,
        request->queueIds.top(),
        request->className);

    poA->_aggregationSN = cimOperationAggregationSN++;
    Uint32 numClasses = providerInfos.size();

    // gather up the repository responses and send it to out as one response
    // with many instances
    if (_repository->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        for (Uint32 i = 0; i < numClasses; i++)
        {
            ProviderInfo& providerInfo = providerInfos[i];

            // this class is registered to a provider - skip
            if (providerInfo.hasProvider)
                continue;

            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                Formatter::format(
                    "EnumerateInstances Req. class $0 to repository, "
                        "No $1 of $2, SN $3",
                    providerInfo.className.getString(),
                    i,
                    numClasses,
                    poA->_aggregationSN));

            AutoPtr<CIMEnumerateInstancesResponseMessage> response(
                dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
                    request->buildResponse()));

            CIMException cimException;
            Array<CIMInstance> cimNamedInstances;

            try
            {
                StatProviderTimeMeasurement providerTime(response.get());

                // Enumerate instances only for this class
                cimNamedInstances =
                    _repository->enumerateInstancesForClass(
                        request->nameSpace,
                        providerInfo.className,
                        request->localOnly,
                        request->includeQualifiers,
                        request->includeClassOrigin,
                        request->propertyList);
            }
            catch (const CIMException& exception)
            {
                cimException = exception;
            }
            catch (const Exception& exception)
            {
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                    exception.getMessage());
            }
            catch (...)
            {
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                    String::EMPTY);
            }

            response->cimNamedInstances = cimNamedInstances;
            response->cimException = cimException;

            poA->appendResponse(response.release());
        } // for all classes and derived classes

        Uint32 numberResponses = poA->numberResponses();
        Uint32 totalIssued = providerCount + (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        if (numberResponses > 0)
        {
            handleEnumerateInstancesResponseAggregation(poA);

            CIMResponseMessage* response = poA->removeResponse(0);

            _forwardRequestForAggregation(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMEnumerateInstancesRequestMessage(*request),
                poA,
                response);
        }
    } // if isDefaultInstanceProvider
    else
    {
        // Set the number of expected responses in the OperationAggregate
        poA->setTotalIssued(providerCount);
    }

    CDEBUG("Before Loop to send requests. numClasses = " << numClasses);

    // Loop through providerInfos, forwarding requests to providers
    for (Uint32 i = 0; i < numClasses; i++)
    {
        ProviderInfo& providerInfo = providerInfos[i];

        // this class is NOT registered to a provider - skip
        if (! providerInfo.hasProvider)
            continue;

        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            Formatter::format(
                "EnumerateInstances Req. class $0 to svc \"$1\" for control "
                    "provider \"$2\", No $3 of $4, SN $5",
                providerInfo.className.getString(),
                providerInfo.serviceName,
                providerInfo.controlProviderName,
                i, numClasses, poA->_aggregationSN));

        CIMEnumerateInstancesRequestMessage* requestCopy =
            new CIMEnumerateInstancesRequestMessage(*request);

        requestCopy->className = providerInfo.className;

        CIMException checkClassException;

        CIMClass cimClass = _getClass(
            request->nameSpace,
            providerInfo.className,
            checkClassException);

        // The following is not correct. Need better way to terminate.
        if (checkClassException.getCode() != CIM_ERR_SUCCESS)
        {
            CIMResponseMessage* response = request->buildResponse();

            _forwardRequestForAggregation(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMEnumerateInstancesRequestMessage(*request),
                poA,
                response);
        }

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *(providerInfo.providerIdContainer.get()));
        }

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
        if (_enableNormalization && providerInfo.hasProviderNormalization)
        {
            requestCopy->operationContext.insert(
                CachedClassDefinitionContainer(cimClass));
        }
#endif

        if (checkClassException.getCode() == CIM_ERR_SUCCESS)
        {
            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                Formatter::format(
                    "EnumerateInstances Req. Fwd class $0 to svc \"$1\" for "
                        "control provider \"$2\", PropertyList= $3",
                    providerInfo.className.getString(),
                    providerInfo.serviceName,
                    providerInfo.controlProviderName,
                    _showPropertyList(requestCopy->propertyList)));

            _forwardRequestForAggregation(
                providerInfo.serviceName,
                providerInfo.controlProviderName,
                requestCopy,
                poA);
        }
    } // for all classes and dervied classes

    PEG_METHOD_EXIT();
    return;
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

    CIMName className = request->className;

    CIMException checkClassException;

    CIMClass cimClass = _getClass(
        request->nameSpace,
        className,
        checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    //
    // Get names of descendent classes:
    //
    CIMException cimException;
    Array<ProviderInfo> providerInfos;

    // This gets set by _lookupAllInstanceProviders()
    Uint32 providerCount;

    try
    {
        providerInfos = _lookupAllInstanceProviders(
            request->nameSpace,
            request->className,
            providerCount);
    }
    catch (const CIMException& exception)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = exception;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    // Test for "enumerate too Broad" and if so, execute exception.
    // This limits the number of provider invocations, not the number
    // of instances returned.
    if (providerCount > _maximumEnumerateBreadth)
    {
        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "Request-too-broad exception.  Namespace: $0  "
                "Class Name: $1 Limit: $2  ProviderCount: $3",
            request->nameSpace.getString(),
            request->className.getString(),
            _maximumEnumerateBreadth, providerCount));

        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            Formatter::format(
                "ERROR Enumerate too broad for class $0. "
                    "Limit = $1, Request = $2",
                request->className.getString(),
                _maximumEnumerateBreadth,
                providerCount));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,
            MessageLoaderParms(
                "Server.CIMOperationRequestDispatcher.ENUM_REQ_TOO_BROAD",
                "Enumerate request too Broad")),

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED
    if ((providerCount == 0) && !(_repository->isDefaultInstanceProvider()))
    {
        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            "CIM_ERROR_NOT_SUPPORTED for " + request->className.getString());

        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        _enqueueResponse(request, response);

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

    poA->_aggregationSN = cimOperationAggregationSN++;
    Uint32 numClasses = providerInfos.size();

    if (_repository->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        for (Uint32 i = 0; i < numClasses; i++)
        {
            ProviderInfo& providerInfo = providerInfos[i];

            // this class is registered to a provider - skip
            if (providerInfo.hasProvider)
                continue;

            // If this class does not have a provider

            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                Formatter::format(
                    "EnumerateInstanceNames Req. class $0 to repository, "
                    "No $1 of $2, SN $3",
                    providerInfo.className.getString(),
                    i, numClasses, poA->_aggregationSN));

            AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response(
                dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
                    request->buildResponse()));

            try
            {
                StatProviderTimeMeasurement providerTime(response.get());

                // Enumerate instances only for this class
                response->instanceNames =
                    _repository->enumerateInstanceNamesForClass(
                        request->nameSpace,
                        providerInfo.className);
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
        Uint32 totalIssued = providerCount + (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        if (numberResponses > 0)
        {
            handleEnumerateInstanceNamesResponseAggregation(poA);

            CIMResponseMessage* response = poA->removeResponse(0);

            _forwardRequestForAggregation(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMEnumerateInstanceNamesRequestMessage(*request),
                poA,
                response);
        }
    } // if isDefaultInstanceProvider
    else
    {
        // Set the number of expected responses in the OperationAggregate
        poA->setTotalIssued(providerCount);
    }

    // Loop through providerInfos, forwarding requests to providers
    for (Uint32 i = 0; i < numClasses; i++)
    {
        ProviderInfo& providerInfo = providerInfos[i];

        // this class is NOT registered to a provider - skip
        if (! providerInfo.hasProvider)
            continue;

        PEG_TRACE_STRING(
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            Formatter::format(
                "EnumerateInstanceNames Req. class $0 to svc \"$1\" for "
                    "control provider \"$2\", No $3 of $4, SN $5",
                providerInfo.className.getString(),
                providerInfo.serviceName,
                providerInfo.controlProviderName,
                i,
                numClasses,
                poA->_aggregationSN));

        CIMEnumerateInstanceNamesRequestMessage* requestCopy =
            new CIMEnumerateInstanceNamesRequestMessage(*request);

        requestCopy->className = providerInfo.className;

        CIMException checkClassException;

        CIMClass cimClass =
            _getClass(
                request->nameSpace,
                providerInfo.className,
                checkClassException);

        // The following is not correct. Need better way to terminate.
        if (checkClassException.getCode() != CIM_ERR_SUCCESS)
        {
            CIMResponseMessage* response = request->buildResponse();

            _forwardRequestForAggregation(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMEnumerateInstanceNamesRequestMessage(*request),
                poA,
                response);
        }

        if (providerInfo.providerIdContainer.get() != 0)
        {
            requestCopy->operationContext.insert(
                *(providerInfo.providerIdContainer.get()));
        }

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
        if (_enableNormalization && providerInfo.hasProviderNormalization)
        {
            requestCopy->operationContext.insert(
                CachedClassDefinitionContainer(cimClass));
        }
#endif

        if (checkClassException.getCode() == CIM_ERR_SUCCESS)
        {
            _forwardRequestForAggregation(
                providerInfo.serviceName,
                providerInfo.controlProviderName,
                requestCopy,
                poA);
        }
    } // for all classes and derived classes

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleAssociatorsRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleAssociatorsRequest(
    CIMAssociatorsRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleAssociatorsRequest");

    if (!_enableAssociationTraversal)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "Associators");

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    // Validate role parameter syntax
    if ((request->role != String::EMPTY) && (!CIMName::legal(request->role)))
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_PARAMETER, request->role);

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    // Validate resultRole parameter syntax
    if ((request->resultRole != String::EMPTY) &&
        (!CIMName::legal(request->resultRole)))
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER, request->resultRole);

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    CIMException checkClassException;
    _checkExistenceOfClass(request->nameSpace,
                           request->objectName.getClassName(),
                           checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
        {
            checkClassException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER, request->objectName.toString());
        }

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::handleAssociators - "
            "Namespace: $0  Class name: $1",
        request->nameSpace.getString(),
        request->objectName.toString()));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
                      "Associators executing Class request");

        AutoPtr<CIMAssociatorsResponseMessage> response(
            dynamic_cast<CIMAssociatorsResponseMessage*>(
                request->buildResponse()));

        try
        {
            StatProviderTimeMeasurement providerTime(response.get());

            response->cimObjects = _repository->associators(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                request->resultClass,
                request->role,
                request->resultRole,
                request->includeQualifiers,
                request->includeClassOrigin,
                request->propertyList);
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

        _enqueueResponse(request, response.release());
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfos;
        Uint32 providerCount;
        try
        {
            providerInfos = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                String::EMPTY,
                providerCount);
        }
        catch (const CIMException& cimException)
        {
            CIMResponseMessage* response = request->buildResponse();
            response->cimException = cimException;

            _enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return;
        }

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount));

        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if ((providerCount == 0) && !_repository->isDefaultInstanceProvider())
        {
            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "CIM_ERR_NOT_SUPPORTED for " + request->className.getString());

            CIMResponseMessage* response = request->buildResponse();
            response->cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

            _enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return;
        }

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

            try
            {
                StatProviderTimeMeasurement providerTime(response.get());

                response->cimObjects = _repository->associators(
                    request->nameSpace,
                    request->objectName,
                    request->assocClass,
                    request->resultClass,
                    request->role,
                    request->resultRole,
                    request->includeQualifiers,
                    request->includeClassOrigin,
                    request->propertyList);
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

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Associators repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    response->cimObjects.size()));
        }

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
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

        poA->_aggregationSN = cimOperationAggregationSN++;

        // Include the repository response in the aggregation, if applicable
        if (response.get() != 0)
        {
            poA->setTotalIssued(providerCount+1);
            // send the repository's results
            _forwardRequestForAggregation(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMAssociatorsRequestMessage(*request),
                poA,
                response.release());
        }
        else
        {
            poA->setTotalIssued(providerCount);
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
                    requestCopy->operationContext.insert(
                        *(providerInfos[i].providerIdContainer.get()));

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfos[i].className.getString());
                _forwardRequestForAggregation(providerInfos[i].serviceName,
                    providerInfos[i].controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleAssociatorNamesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleAssociatorNamesRequest(
    CIMAssociatorNamesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleAssociatorNamesRequest");

    if (!_enableAssociationTraversal)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "AssociatorNames");

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    // Validate role parameter syntax
    if ((request->role != String::EMPTY) && (!CIMName::legal(request->role)))
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_PARAMETER, request->role);

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    // Validate resultRole parameter syntax
    if ((request->resultRole != String::EMPTY) &&
        (!CIMName::legal(request->resultRole)))
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER, request->resultRole);

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    CIMException checkClassException;
    _checkExistenceOfClass(request->nameSpace,
                           request->objectName.getClassName(),
                           checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
        {
            checkClassException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER, request->objectName.toString());
        }

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::handleAssociatorNames - "
            "Namespace: $0  Class name: $1",
        request->nameSpace.getString(),
        request->objectName.toString()));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
                      "AssociatorNames executing Class request");

        AutoPtr<CIMAssociatorNamesResponseMessage> response(
            dynamic_cast<CIMAssociatorNamesResponseMessage*>(
                request->buildResponse()));

        try
        {
            StatProviderTimeMeasurement providerTime(response.get());

            response->objectNames = _repository->associatorNames(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                request->resultClass,
                request->role,
                request->resultRole);
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

        _enqueueResponse(request, response.release());
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfos;
        Uint32 providerCount;
        try
        {
            providerInfos = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                String::EMPTY,
                providerCount);
        }
        catch (const CIMException& cimException)
        {
            CIMResponseMessage* response = request->buildResponse();
            response->cimException = cimException;

            _enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return;
        }

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount));

        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if ((providerCount == 0) && !_repository->isDefaultInstanceProvider())
        {
            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "CIM_ERR_NOT_SUPPORTED for " + request->className.getString());

            CIMResponseMessage* response = request->buildResponse();
            response->cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

            _enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return;
        }

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

            try
            {
                StatProviderTimeMeasurement providerTime(response.get());

                response->objectNames = _repository->associatorNames(
                    request->nameSpace,
                    request->objectName,
                    request->assocClass,
                    request->resultClass,
                    request->role,
                    request->resultRole);
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

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "AssociatorNames repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    response->objectNames.size()));
        }

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
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

        poA->_aggregationSN = cimOperationAggregationSN++;

        // Include the repository response in the aggregation, if applicable
        if (response.get() != 0)
        {
            poA->setTotalIssued(providerCount+1);
            // send the repository's results
            _forwardRequestForAggregation(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMAssociatorNamesRequestMessage(*request),
                poA,
                response.release());
        }
        else
        {
            poA->setTotalIssued(providerCount);
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

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfos[i].className.getString());
                _forwardRequestForAggregation(providerInfos[i].serviceName,
                    providerInfos[i].controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleReferencesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleReferencesRequest(
    CIMReferencesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferencesRequest");

    if (!_enableAssociationTraversal)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "References");

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    // Validate role parameter syntax
    if ((request->role != String::EMPTY) && (!CIMName::legal(request->role)))
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_PARAMETER, request->role);

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    CIMException checkClassException;
    _checkExistenceOfClass(
        request->nameSpace,
        request->objectName.getClassName(),
        checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
        {
            checkClassException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER, request->objectName.toString());
        }

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::handleReferences - "
            "Namespace: $0  Class name: $1",
        request->nameSpace.getString(),
        request->objectName.toString()));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
                      "References executing Class request");

        AutoPtr<CIMReferencesResponseMessage> response(
            dynamic_cast<CIMReferencesResponseMessage*>(
                request->buildResponse()));

        try
        {
            StatProviderTimeMeasurement providerTime(response.get());

            response->cimObjects = _repository->references(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                request->role,
                request->includeQualifiers,
                request->includeClassOrigin,
                request->propertyList);
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

        _enqueueResponse(request, response.release());
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfos;
        Uint32 providerCount;
        try
        {
            providerInfos = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                String::EMPTY,
                providerCount);
        }
        catch (const CIMException& cimException)
        {
            CIMResponseMessage* response = request->buildResponse();
            response->cimException = cimException;

            _enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return;
        }

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount));

        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if ((providerCount == 0) && !_repository->isDefaultInstanceProvider())
        {
            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "CIM_ERR_NOT_SUPPORTED for " + request->className.getString());

            CIMResponseMessage* response = request->buildResponse();
            response->cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

            _enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return;
        }

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

            try
            {
                StatProviderTimeMeasurement providerTime(response.get());

                response->cimObjects = _repository->references(
                    request->nameSpace,
                    request->objectName,
                    request->resultClass,
                    request->role,
                    request->includeQualifiers,
                    request->includeClassOrigin,
                    request->propertyList);
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

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "References repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    response->cimObjects.size()));
        }

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
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

        poA->_aggregationSN = cimOperationAggregationSN++;

        // Include the repository response in the aggregation, if applicable
        if (response.get() != 0)
        {
            poA->setTotalIssued(providerCount+1);
            // send the repository's results
            _forwardRequestForAggregation(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMReferencesRequestMessage(*request),
                poA,
                response.release());
        }
        else
        {
            poA->setTotalIssued(providerCount);
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

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfos[i].className.getString());
                _forwardRequestForAggregation(providerInfos[i].serviceName,
                    providerInfos[i].controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleReferenceNamesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleReferenceNamesRequest(
    CIMReferenceNamesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferenceNamesRequest");

    if (!_enableAssociationTraversal)
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "ReferenceNames");

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    // Validate role parameter syntax
    if ((request->role != String::EMPTY) && (!CIMName::legal(request->role)))
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_PARAMETER, request->role);

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    CIMException checkClassException;
    _checkExistenceOfClass(request->nameSpace,
                           request->objectName.getClassName(),
                           checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
        {
            checkClassException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER, request->objectName.toString());
        }

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::handleReferenceNames - "
            "Namespace: $0  Class name: $1",
        request->nameSpace.getString(),
        request->objectName.toString()));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
                      "ReferenceNames executing Class request");

        AutoPtr<CIMReferenceNamesResponseMessage> response(
            dynamic_cast<CIMReferenceNamesResponseMessage*>(
                request->buildResponse()));

        try
        {
            StatProviderTimeMeasurement providerTime(response.get());

            response->objectNames = _repository->referenceNames(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                request->role);
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

        _enqueueResponse(request, response.release());
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfos;
        Uint32 providerCount;
        try
        {
            providerInfos = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                String::EMPTY,
                providerCount);
        }
        catch (const CIMException& cimException)
        {
            CIMResponseMessage* response = request->buildResponse();
            response->cimException = cimException;

            _enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return;
        }

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount));

        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if ((providerCount == 0) && !_repository->isDefaultInstanceProvider())
        {
            PEG_TRACE_STRING(
                TRC_DISPATCHER,
                Tracer::LEVEL4,
                "CIM_ERR_NOT_SUPPORTED for " + request->className.getString());

            CIMResponseMessage* response = request->buildResponse();
            response->cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

            _enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return;
        }

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

            try
            {
                StatProviderTimeMeasurement providerTime(response.get());

                response->objectNames = _repository->referenceNames(
                    request->nameSpace,
                    request->objectName,
                    request->resultClass,
                    request->role);
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

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "ReferenceNames repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    response->objectNames.size()));
        }

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
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

        poA->_aggregationSN = cimOperationAggregationSN++;

        // Include the repository response in the aggregation, if applicable
        if (response.get() != 0)
        {
            poA->setTotalIssued(providerCount+1);
            _forwardRequestForAggregation(
                String(PEGASUS_QUEUENAME_OPREQDISPATCHER),
                String(),
                new CIMReferenceNamesRequestMessage(*request),
                poA,
                response.release());
        }
        else
        {
            poA->setTotalIssued(providerCount);
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

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfos[i].className.getString());
                _forwardRequestForAggregation(providerInfos[i].serviceName,
                    providerInfos[i].controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleGetPropertyRequest
    ATTN: FIX LOOKUP
**********************************************************/

void CIMOperationRequestDispatcher::handleGetPropertyRequest(
   CIMGetPropertyRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetPropertyRequest");

   CIMName className = request->instanceName.getClassName();

   // check the class name for an "external provider"
   // Assumption here is that there are no "internal" property requests.
   // teATTN: KS 20030402 - This needs cleanup along with the setproperty.

   ProviderInfo providerInfo =
       _lookupInstanceProvider(
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

       _forwardRequestToService(
           PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP,
           requestCopy,
           requestCallbackCopy);

       PEG_METHOD_EXIT();
       return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
      AutoPtr<CIMGetPropertyResponseMessage> response(
          dynamic_cast<CIMGetPropertyResponseMessage*>(
              request->buildResponse()));

      try
      {
          StatProviderTimeMeasurement providerTime(response.get());

          response->value = _repository->getProperty(
              request->nameSpace,
              request->instanceName,
              request->propertyName);
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
    ATTN: FIX LOOKUP
**********************************************************/

void CIMOperationRequestDispatcher::handleSetPropertyRequest(
   CIMSetPropertyRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleSetPropertyRequest");

    {
        CIMException cimException;
        try
        {
            _fixSetPropertyValueType(request);
        }
        catch (CIMException& exception)
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
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        if (cimException.getCode() != CIM_ERR_SUCCESS)
        {
            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                "CIMOperationRequestDispatcher::handleSetPropertyRequest - "
                    "CIM exception has occurred."));

            CIMResponseMessage* response = request->buildResponse();
            response->cimException = cimException;

            _enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return;
        }
    }

    CIMName className = request->instanceName.getClassName();

    // check the class name for an "external provider"
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

        _forwardRequestToService(
            PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        return;
    }
    else if (_repository->isDefaultInstanceProvider())
    {
        AutoPtr<CIMSetPropertyResponseMessage> response(
            dynamic_cast<CIMSetPropertyResponseMessage*>(
                request->buildResponse()));

        try
        {
            StatProviderTimeMeasurement providerTime(response.get());

            _repository->setProperty(
                request->nameSpace,
                request->instanceName,
                request->propertyName,
                request->newValue,
                ((ContentLanguageListContainer)request->operationContext.get(
                    ContentLanguageListContainer::NAME)).getLanguages());

            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                "CIMOperationRequestDispatcher::handleSetPropertyRequest - "
                    "Namespace: $0  Instance Name: $1  Property Name: $2  New "
                    "Value: $3",
                request->nameSpace.getString(),
                request->instanceName.getClassName().getString(),
                request->propertyName.getString(),
                request->newValue.toString()));
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

    AutoPtr<CIMGetQualifierResponseMessage> response(
        dynamic_cast<CIMGetQualifierResponseMessage*>(
            request->buildResponse()));

    try
    {
        StatProviderTimeMeasurement providerTime(response.get());

        response->cimQualifierDecl = _repository->getQualifier(
            request->nameSpace,
            request->qualifierName);

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationRequestDispatcher::handleGetQualifierRequest - "
                "Namespace: $0  Qualifier Name: $1",
            request->nameSpace.getString(),
            request->qualifierName.getString()));
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

    AutoPtr<CIMSetQualifierResponseMessage> response(
        dynamic_cast<CIMSetQualifierResponseMessage*>(
            request->buildResponse()));

    try
    {
        StatProviderTimeMeasurement providerTime(response.get());

        _repository->setQualifier(
            request->nameSpace,
            request->qualifierDeclaration,
            ((ContentLanguageListContainer)request->operationContext.get(
                ContentLanguageListContainer::NAME)).getLanguages());

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::handleSetQualifierRequest - "
            "Namespace: $0  Qualifier Name: $1",
        request->nameSpace.getString(),
        request->qualifierDeclaration.getName().getString()));
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

    AutoPtr<CIMDeleteQualifierResponseMessage> response(
        dynamic_cast<CIMDeleteQualifierResponseMessage*>(
            request->buildResponse()));

    try
    {
        StatProviderTimeMeasurement providerTime(response.get());

        _repository->deleteQualifier(
            request->nameSpace,
            request->qualifierName);

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationRequestDispatcher::handleDeleteQualifierRequest - "
                "Namespace: $0  Qualifier Name: $1",
            request->nameSpace.getString(),
            request->qualifierName.getString()));
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

    AutoPtr<CIMEnumerateQualifiersResponseMessage> response(
        dynamic_cast<CIMEnumerateQualifiersResponseMessage*>(
            request->buildResponse()));

    try
    {
        StatProviderTimeMeasurement providerTime(response.get());

        response->qualifierDeclarations = _repository->enumerateQualifiers(
            request->nameSpace);

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest"
                " - Namespace: $0",
            request->nameSpace.getString()));
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

    {
        CIMException cimException;
        try
        {
            _fixInvokeMethodParameterTypes(request);
        }
        catch (CIMException& exception)
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
            cimException =
                PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
        }

        if (cimException.getCode() != CIM_ERR_SUCCESS)
        {
            PEG_LOGGER_TRACE((
                Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                "CIMOperationRequestDispatcher::handleInvokeMethodRequest - "
                    "CIM exception has occurred."));

            CIMResponseMessage* response = request->buildResponse();
            response->cimException = cimException;

            _enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return;
        }
    }

    CIMName className = request->instanceName.getClassName();

    CIMException checkClassException;
    _checkExistenceOfClass(request->nameSpace, className, checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        // map CIM_ERR_INVALID_CLASS to CIM_ERR_NOT_FOUND
        if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
        {
            checkClassException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_FOUND, className.getString());
        }

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                "CIMOperationRequestDispatcher::handleInvokeMethodRequest - "
                    "CIM exist exception has occurred.  Namespace: $0  "
                    "Class Name: $1",
                request->nameSpace.getString(),
                className.getString()));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);
        PEG_METHOD_EXIT();
        return;
    }

    String serviceName;
    String controlProviderName;

    // Check for class provided by an internal provider
    if (_lookupInternalProvider(
            request->nameSpace, className, serviceName, controlProviderName))
    {
        CIMInvokeMethodRequestMessage* requestCopy =
            new CIMInvokeMethodRequestMessage(*request);

        CIMInvokeMethodRequestMessage* requestCallbackCopy =
            new CIMInvokeMethodRequestMessage(*requestCopy);

        _forwardRequestToProviderManager(
            className,
            serviceName,
            controlProviderName,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        return;
    }

    // check the class name for an "external provider"
    ProviderIdContainer* providerIdContainer=NULL;

    String providerName = _lookupMethodProvider(
        request->nameSpace,
        className,
        request->methodName,
        &providerIdContainer);

    if (providerName.size() != 0)
    {
        CIMInvokeMethodRequestMessage* requestCopy =
            new CIMInvokeMethodRequestMessage(*request);

        if (providerIdContainer!=NULL)
        {
            requestCopy->operationContext.insert(*providerIdContainer);
            delete providerIdContainer;
            providerIdContainer = NULL;
        }

        CIMInvokeMethodRequestMessage* requestCallbackCopy =
            new CIMInvokeMethodRequestMessage(*requestCopy);

        _forwardRequestToService(
            PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP,
            requestCopy,
            requestCallbackCopy);

        PEG_METHOD_EXIT();
        return;
    }

    CIMResponseMessage* response = request->buildResponse();
    response->cimException =
        PEGASUS_CIM_EXCEPTION(CIM_ERR_METHOD_NOT_AVAILABLE,
            request->methodName.getString());

    _enqueueResponse(request, response);

    PEG_METHOD_EXIT();
}

/*********************************************************************/
//
//   Return Aggregated responses back to the Correct Aggregator
//   ATTN: This was temporary to isolate the aggregation processing.
//   We need to combine this with the other callbacks to create a single
//   set of functions
//
//   The aggregator includes an aggregation object that is used to
//   accumulate responses.  It is attached to each request sent and
//   received back as part of the response call back in the "parm"
//   Responses are aggregated until the count reaches the sent count and
//   then the aggregation code is called to create a single response from
//   the accumulated responses.
//
/*********************************************************************/

// Aggregate the responses for reference names into a single response
//
void CIMOperationRequestDispatcher::handleAssociatorNamesResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::"
            "handleAssociatorNamesResponseAggregation");
    CIMAssociatorNamesResponseMessage* toResponse =
        (CIMAssociatorNamesResponseMessage*) poA->getResponse(0);
    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::AssociatorNames Response - "
            "Namespace: $0  Class name: $1 Response Count: $2",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses()));

    // This double loop has 2 purposes:
    // 1. To work backward and delete each response off the end of the array
    //    and append each instance to the list of instances of the first
    //    element in the array.
    // 2. fill in host, namespace on all instances on all elements of array
    //    if they have been left out. This is required because XML reader
    //    will fail without them populated

    Uint32 i = poA->numberResponses() - 1;
    do
    {
        CIMAssociatorNamesResponseMessage* fromResponse =
            (CIMAssociatorNamesResponseMessage*)poA->getResponse(i);

        for (Uint32 j = 0, n = fromResponse->objectNames.size(); j < n; j++)
        {
            CIMObjectPath& p = fromResponse->objectNames[j];

            if (p.getHost().size() == 0)
                p.setHost(cimAggregationLocalHost);

            if (p.getNameSpace().isNull())
                p.setNameSpace(poA->_nameSpace);

            // only append if we are not the first response
            if (i > 0)
                toResponse->objectNames.append(p);
        }

        // only delete if we are not the first response, else we're done
        if (i == 0)
            break;
        else poA->deleteResponse(i--);

    } while (true);

    PEG_METHOD_EXIT();
}

// Aggregate the responses for Associators into a single response

void CIMOperationRequestDispatcher::handleAssociatorsResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleAssociatorsResponseAggregation");

    CIMAssociatorsResponseMessage* toResponse =
    (CIMAssociatorsResponseMessage*) poA->getResponse(0);

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::Associators Response - "
            "Namespace: $0  Class name: $1 Response Count: $2",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses()));

    // This double loop has 2 purposes:
    // 1. To work backward and delete each response off the end of the array
    //    and append each instance to the list of instances of the first
    //    element in the array.
    // 2. fill in host, namespace on all instances on all elements of array
    //    if they have been left out. This is required because XML reader
    //    will fail without them populated

    Uint32 i = poA->numberResponses() - 1;
    do
    {
        CIMAssociatorsResponseMessage* fromResponse =
            (CIMAssociatorsResponseMessage*)poA->getResponse(i);

        for (Uint32 j = 0, n = fromResponse->cimObjects.size(); j < n; j++)
        {
            const CIMObject& object = fromResponse->cimObjects[j];
            CIMObjectPath& p = const_cast<CIMObjectPath&>(object.getPath());

            if (p.getHost().size() ==0)
                p.setHost(cimAggregationLocalHost);

            if (p.getNameSpace().isNull())
                p.setNameSpace(poA->_nameSpace);

            // only append if we are not the first response
            if (i > 0)
                toResponse->cimObjects.append(object);
        }

        // only delete if we are not the first response, else we're done
        if (i == 0)
            break;
        else poA->deleteResponse(i--);

    } while (true);

    PEG_METHOD_EXIT();
}

// Aggregate the responses for References into a single response

void CIMOperationRequestDispatcher::handleReferencesResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferencesResponseAggregation");

    CIMReferencesResponseMessage* toResponse =
        (CIMReferencesResponseMessage*) poA->getResponse(0);

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::References Response - "
            "Namespace: $0  Class name: $1 Response Count: $2",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses()));

    // This double loop has 2 purposes:
    // 1. To work backward and delete each response off the end of the array
    //    and append each instance to the list of instances of the first
    //    element in the array.
    // 2. fill in host, namespace on all instances on all elements of array
    //    if they have been left out. This is required because XML reader
    //    will fail without them populated

    Uint32 i = poA->numberResponses() - 1;
    do
    {
        CIMReferencesResponseMessage* fromResponse =
            (CIMReferencesResponseMessage*)poA->getResponse(i);

        for (Uint32 j = 0, n = fromResponse->cimObjects.size(); j < n; j++)
        {
            // Test and complete path if necessary. Required because
            // XML not correct without full path.
            const CIMObject& object = fromResponse->cimObjects[j];
            CIMObjectPath& p = const_cast<CIMObjectPath&>(object.getPath());

            if (p.getHost().size() == 0)
                p.setHost(cimAggregationLocalHost);

            if (p.getNameSpace().isNull())
                p.setNameSpace(poA->_nameSpace);

            // only append if we are not the first response
            if (i > 0)
                toResponse->cimObjects.append(object);
        }

        // only delete if we are not the first response, else we're done
        if (i == 0)
            break;
        else poA->deleteResponse(i--);

    } while (true);

    PEG_METHOD_EXIT();
}

// Aggregate the responses for reference names into a single response
//
void CIMOperationRequestDispatcher::handleReferenceNamesResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::"
            "handleReferenceNamesResponseAggregation");
    CIMReferenceNamesResponseMessage* toResponse =
    (CIMReferenceNamesResponseMessage*) poA->getResponse(0);

    // Work backward and delete each response off the end of the array
    // adding it to the toResponse, which is really the first response.
    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::ReferenceNames Response - "
            "Namespace: $0  Class name: $1 Response Count: $2",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses()));

    // This double loop has 2 purposes:
    // 1. To work backward and delete each response off the end of the array
    //    and append each instance to the list of instances of the first
    //    element in the array.
    // 2. fill in host, namespace on all instances on all elements of array
    //    if they have been left out. This is required because XML reader
    //    will fail without them populated

    Uint32 i = poA->numberResponses() - 1;
    do
    {
        CIMReferenceNamesResponseMessage* fromResponse =
            (CIMReferenceNamesResponseMessage*)poA->getResponse(i);

        for (Uint32 j = 0, n = fromResponse->objectNames.size(); j < n; j++)
        {
            CIMObjectPath& p = fromResponse->objectNames[j];

            if (p.getHost().size() == 0)
                p.setHost(cimAggregationLocalHost);

            if (p.getNameSpace().isNull())
                p.setNameSpace(poA->_nameSpace);

            // only append if we are not the first response
            if (i > 0)
                toResponse->objectNames.append(p);
        }

        // only delete if we are not the first response, else we're done
        if (i == 0)
            break;
        else poA->deleteResponse(i--);

    } while (true);

    PEG_METHOD_EXIT();
}

/* aggregate the responses for enumerateinstancenames into a single response
*/
void CIMOperationRequestDispatcher::
    handleEnumerateInstanceNamesResponseAggregation(
        OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::"
            "handleEnumerateInstanceNamesResponseAggregation");
    CIMEnumerateInstanceNamesResponseMessage* toResponse =
        (CIMEnumerateInstanceNamesResponseMessage*) poA->getResponse(0);

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::EnumerateInstanceNames Response - "
            "Namespace: $0  Class name: $1 Response Count: $2",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses()));

    // Work backward and delete each response off the end of the array
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMEnumerateInstanceNamesResponseMessage* fromResponse =
            (CIMEnumerateInstanceNamesResponseMessage*)poA->getResponse(i);

        for (Uint32 j = 0; j < fromResponse->instanceNames.size(); j++)
        {
            // Duplicate test goes here if we decide to eliminate dups in
            // the future.
            toResponse->instanceNames.append(fromResponse->instanceNames[j]);
        }
        poA->deleteResponse(i);
    }
    PEG_METHOD_EXIT();
}

/* The function aggregates individual EnumerateInstance Responses into a
   single response
   for return to the client. It aggregates the responses into the
   first response (0).
   ATTN: KS 28 May 2002 - At this time we do not do the following:
   1. eliminate duplicates.
   2. prune the properties if localOnly or deepInheritance are set.
   This function does not send any responses.
*/
void CIMOperationRequestDispatcher::
    handleEnumerateInstancesResponseAggregation(
        OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnumerateInstancesResponse");

    CIMEnumerateInstancesResponseMessage* toResponse =
        (CIMEnumerateInstancesResponseMessage*)poA->getResponse(0);

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::EnumerateInstancesResponseAggregation"
            "- Namespace: $0 Class name: $1 Response Count: $2",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses()));

    CIMEnumerateInstancesRequestMessage* request =
        (CIMEnumerateInstancesRequestMessage*)poA->getRequest();

    // Work backward and delete each response off the end of the array
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMEnumerateInstancesResponseMessage* fromResponse =
            (CIMEnumerateInstancesResponseMessage*)poA->getResponse(i);

        for (Uint32 j = 0; j < fromResponse->cimNamedInstances.size(); j++)
        {
            toResponse->cimNamedInstances.append(
                fromResponse->cimNamedInstances[j]);
        }

        poA->deleteResponse(i);
    }

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::"
        "EnumerateInstancesResponseAggregation - "
        "Local Only: $0 Include Qualifiers: $1 Include Class Origin: $2",
        (request->localOnly == true ? "true" : "false"),
        (request->includeQualifiers == true ? "true" : "false"),
        (request->includeClassOrigin == true ? "true" : "false")));

    PEG_METHOD_EXIT();
}


void CIMOperationRequestDispatcher::handleExecQueryResponseAggregation(
    OperationAggregate* poA)
{
    QuerySupportRouter::routeHandleExecQueryResponseAggregation(this, poA);
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

                    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
                        Logger::TRACE,
                        "CIMOperationRequestDispatcher::"
                            "_fixInvokeMethodParameterTypes - "
                            "Namespace: $0  Class Name: $1",
                        request->nameSpace.getString(),
                        request->instanceName.getClassName().getString()));
                }
                catch (CIMException&)
                {
                    PEG_METHOD_EXIT();
                    throw;
                }
                catch (Exception& e)
                {
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_FAILED,
                        e.getMessage());
                }
                catch (...)
                {
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
                }

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

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationRequestDispatcher::_fixSetPropertyValueType - "
                "Namespace: $0  Class Name: $1",
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString()));
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
        else
        {
            PEG_METHOD_EXIT();
            throw;
        }
    }
    catch (Exception&)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch (...)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
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

void CIMOperationRequestDispatcher::_checkExistenceOfClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    CIMException& cimException)
{
    if (className.equal (CIMName (PEGASUS_CLASSNAME___NAMESPACE)))
    {
        return;
    }

    CIMClass cimClass;

    try
    {
        cimClass = _repository->getClass(
            nameSpace,
            className,
            true,
            false,
            false,
            CIMPropertyList());

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationRequestDispatcher::_checkExistenceOfClass - "
                "Namespace: $0  Class Name: $1",
            nameSpace.getString(),
            className.getString()));
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
}

CIMClass CIMOperationRequestDispatcher::_getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    CIMException& cimException)
{
    if (className.equal (CIMName (PEGASUS_CLASSNAME___NAMESPACE)))
    {
        CIMClass __namespaceClass(PEGASUS_CLASSNAME___NAMESPACE);
        // ATTN: Qualifiers not added here, but they shouldn't be needed
        __namespaceClass.addProperty(
            CIMProperty(CIMName("Name"), String::EMPTY));
        return __namespaceClass;
    }

    CIMClass cimClass;

    // get the complete class, specifically not local only
    try
    {
        cimClass = _repository->getClass(
            nameSpace,
            className,
            false,
            true,
            true,
            CIMPropertyList());

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CIMOperationRequestDispatcher::_getClass - "
                "Namespace: $0  Class Name: $1",
            nameSpace.getString(),
            className.getString()));
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
