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
#include <ctime>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Common/Threads.h>

#include <Pegasus/Server/QuerySupportRouter.h>

#include <Pegasus/Server/EnumerationContext.h>
#include <Pegasus/Server/EnumerationTable.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>


PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

#define CSTRING(ARG) (const char*) ARG.getCString()

// EXP_PULL_TEMP
//
const char * _toCharP(Boolean x)
{
    return (x? "true" : "false");
}

String _toString(const CIMPropertyList& pl)
{
    String rtn;
    //Array<CIMName> pls = pl.getPropertyNameArray();
    if (pl.isNull())
        return("NULL");

    if (pl.size() == 0)
        return("EMPTY");

    for (Uint32 i = 0 ; i < pl.size() ; i++)
    {
        if (i != 0)
            rtn.append(",");
        rtn.append(pl[i].getString());
    }
    return(rtn);
}

/******************************************************************************
**
**  Static variables outside of object context
**
******************************************************************************/

// Define the variable that controls the default pull operation timeout
// when NULL is received with a request. This sets the time in seconds between
// the completion of one operation of an enumeration sequence and the
// recipt of another.  The server must maintain the context for at least
// the time defined in this value.
#ifdef PEGASUS_PULL_OPERATION_DEFAULT_TIMEOUT
    Uint32 _pullOperationDefaultTimeout =
        PEGASUS_PULL_OPERATION_DEFAULT_TIMEOUT;
#else
    Uint32 _pullOperationDefaultTimeout = 15;
#endif

// Sets the default maximum size for the response cache in each
// enumerationContext.  As responses are returned from providers this is the
// maximum number that can be placed in the CIMResponseData cache waiting
// for pull operations to move send them as responses before responses
// start backing up the providers (i.e. delaying return from the provider
// deliver calls.
// FUTURE: As we develop more flexible resource management this value should
// be modified for each context creation in terms of the object sizes expected
// and the memory usage of the CIMServer.  Thus, it would be logical to
// allow caching many more path responses than instance responses because
// they are probably much smaller.
// This variableis not externalized to a #define because we are not sure
// pif that is logical.
Uint32 responseCacheDefaultMaximumSize = 1000;
//
// Define the table that will contain enumeration contexts for Open, Pull,
// Close, and countEnumeration operaitons.  The default interoperation
// timeout is set as part of creating the table.
//
static EnumerationTable enumerationTable(_pullOperationDefaultTimeout,
                                         responseCacheDefaultMaximumSize);

// Local save for host name. save host name here.  NOTE: Problem if hostname
// changes. Set by object init. Used by aggregator.
String cimAggregationLocalHost;

// static counter for aggretation serial numbers.
// can be used to determine lost aggregations.
// KS_TODO - This is really of no value any more. Get Rid of it????
Uint64 CIMOperationRequestDispatcher::cimOperationAggregationSN = 0;

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
**  Implementation of OperationAggregate
**
******************************************************************************/

OperationAggregate::OperationAggregate(
    CIMRequestMessage* request,
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
      _request(request)
{
    _totalIssued = 0;
    _totalReceived = 0;
    _totalReceivedComplete = 0;
    _totalReceivedExpected = 0;
    _totalReceivedErrors = 0;
    _totalReceivedNotSupported = 0;
    _aggregationSN = _operationAggregationSNAccumulator++;
    _magicNumber = 12345;
    _objectCount = 0;
    _pullOperation = false;
    _enumerationFinished = false;
    _enumerationContext = 0;
}
// static counter for aggretation serial numbers.
// can be used to determine lost aggregations.
Uint64 OperationAggregate::_operationAggregationSNAccumulator = 0;

OperationAggregate::~OperationAggregate()
{
    delete _request;
    delete _query;
}

Boolean OperationAggregate::valid() const
{
    return _magic;
}

void OperationAggregate::setTotalIssued(Uint32 i)
{
    _totalIssued = i;
}

Uint64 OperationAggregate::getAggregationSN()
{
    return _aggregationSN;
}

void OperationAggregate::incObjectCount()
{
    _objectCount++;
}

void OperationAggregate::decObjectCount()
{
    _objectCount--;
}

/*  Add one response to the responseList and
    return true if the total issued equals the number in the list.
    This return is no longer of any real value since we are dynamically
    adding to and removing from this list.
    EXP_PULL_TBD - Remove this return
*/
Boolean OperationAggregate::appendResponse(CIMResponseMessage* response)
{
    PEGASUS_ASSERT(valid());   // KS_TEMP;
    AutoMutex autoMut(_appendResponseMutex);
    Uint32 tmp = _responseList.size();   /// KS_TEMP
    PEGASUS_ASSERT(response != 0);       /// KS_TEMP
    _responseList.append(response);

    Boolean returnValue = (_totalIssued == numberResponses());


    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
        "numberResponses == %u append. Rtns %s size before = %u SN = %lu",
                (Uint32)_responseList.size(),
                _toCharP(returnValue),
                tmp,
                (unsigned long)_aggregationSN ));
    return returnValue;
}


/* Returns the count of the current number of responses in the
   responseList.  Note:this is not the Total number of responses,
   just the number currently available for aggregation.
*/
Uint32 OperationAggregate::numberResponses()
{
    PEGASUS_ASSERT(valid());   // KS_TEMP;
    AutoMutex autoMut(_appendResponseMutex);
    Uint32 size =  _responseList.size();
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
        "numberResponses == %u read SN = %lu", size,
               (unsigned long)_aggregationSN ));
    return size;
}

CIMRequestMessage* OperationAggregate::getRequest()
{
    return _request;
}

CIMResponseMessage* OperationAggregate::getResponse(const Uint32& pos)
{
    PEGASUS_ASSERT(valid());   // KS_TEMP;
    AutoMutex autoMut(_appendResponseMutex);
    CIMResponseMessage* tmp = _responseList[pos];

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
        "numberResponses == %u get # %u SN = %lu", _responseList.size(),
                pos, (unsigned long)_aggregationSN
        ));

    return tmp;
}

CIMResponseMessage* OperationAggregate::removeResponse(const Uint32& pos)
{
    PEGASUS_ASSERT(valid());   // KS_TEMP;
    AutoMutex autoMut(_appendResponseMutex);
    Uint32 tmpsize = _responseList.size();
    CIMResponseMessage* tmp = _responseList[pos];
    _responseList.remove(pos);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
        "numberResponses == %u remove # %u SN = %lu size before %u",
        _responseList.size(),
        pos, (unsigned long)_aggregationSN, tmpsize));
    return tmp;
}

void OperationAggregate::setRequest(CIMRequestMessage* request)
{
    _request = request;
}

void OperationAggregate::deleteResponse(const Uint32&pos)
{
    PEGASUS_ASSERT(valid());   // KS_TEMP;
    AutoMutex autoMut(_appendResponseMutex);
    delete _responseList[pos];
    _responseList.remove(pos);
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
        "numberResponses == %u delete # %u SN = %lu", _responseList.size(),
                pos,  (long unsigned int)_aggregationSN
        ));
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
    static const char* func = "OperationAggregate::resequenceResponse";
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
        "%s response MsgType = %s StatusCode = %s", func,
        MessageTypeToString(response.getType()),
        cimStatusCodeToString(response.cimException.getCode())
        ));

    CIMStatusCode error = response.cimException.getCode();
    bool notSupportedReceived = false;

    // if NOT_SUPPORTED Error, increment NotSupported counter
    if (error != CIM_ERR_SUCCESS)
    {
        if (error == CIM_ERR_NOT_SUPPORTED)
        {
            notSupportedReceived = true;
            _totalReceivedNotSupported++;
        }
        _totalReceivedErrors++;
        // KS_PULL_TBD - why true false below
        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL1,
            "%s: Response has error.  Namespace: %s, Class name: %s, "
                "Response Sequence: %s",
            func,
            CSTRING(_nameSpace.getString()),
            CSTRING(_className.getString()),
            (_totalReceived) ? "true" : "false"));
    }

    Boolean isComplete = response.isComplete();
// KS_PULLOP_TEMP TRACE
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
        "%s resequenceinfo index %u"
        "totalResponsesReceivedComplete = %u. _totalReceivedExpected = %u "
        "provider isComplete %s"
        " _totalIssued %u"
        "response.getIndex() %u",
        func,response.getIndex(),
        _totalReceivedComplete,
        _totalReceivedExpected,
        _toCharP(isComplete),
        _totalIssued,
        response.getIndex()
         ));

    // if this provider is complete increment totalReceivedComplete, etc.
    if (isComplete == true)
    {
        _totalReceivedComplete++;
        _totalReceivedExpected += response.getIndex() + 1;

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
            "%s resequence isComplete."
            "totalResponsesReceivedComplete = %u. _totalReceivedExpected = %u",
            func, _totalReceivedComplete, _totalReceivedExpected ));
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
                func,
                _totalReceivedComplete,
                _totalReceived,
                _totalReceivedErrors));
        }
        else
        {
            PEG_TRACE((
                TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "All completed responses (%u) for current request "
                    "have been accounted for but expected count (%u) does "
                    "not match the received count (%u). error count (%u).",
                (unsigned int)_totalReceivedComplete,
                (unsigned int)_totalReceivedExpected,
                (unsigned int)_totalReceived,
                (unsigned int)_totalReceivedErrors));
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
//      if (!_pullOperation)
//      {
            _totalReceivedComplete = 0;
            _totalReceivedExpected = 0;
            _totalReceivedErrors = 0;
            _totalReceivedNotSupported = 0;
            _totalReceived = 0;
//      }
    }
    else if (notSupportedReceived)
    {
        // Clear the NOT_SUPPORTED exception
        // We ignore it unless it's the only response received
        response.cimException = CIMException();
    }


    response.setComplete(isComplete);
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
        "%s: return status.  isComplete: %s Total responses: %u, "
            "total chunks: %u, total errors: %u totalIssued: %u",
        func, _toCharP(isComplete),
        _totalReceivedComplete,
        _totalReceived,
        _totalReceivedErrors,
        _totalIssued));
}
//EXP_PULL_BEGIN

/* setPullOperation sets variables in the Operation Aggregate
    upon the Open... operations for use by subsequent pull
    operations.

*/
void OperationAggregate::setPullOperation(const void* enContext,
    const String& contextString,
    const CIMNamespaceName& nameSpace )
{
    _pullOperation = true;
    _enumerationContext = (void *) enContext;
    _enumerationContextName = contextString;
    _nameSpace = nameSpace;
}

/*
*
*   Common functions use by the request Handlers
*/
/*
    build an array of CIMNames with all of the properties in the
    class.
    @param class CIMClass with the properties
    @return Array<CIMName> containing the names of all the properties
    in the class.
    KS_TBD - Should we move this one to the CIMClass file as standard
             function.
*/
Array<CIMName> _buildPropertyList(CIMConstClass& thisClass)
{
    Array<CIMName> propertyNameArray;
    Uint32 numProperties = thisClass.getPropertyCount();

    for (Uint32 i = 0; i < numProperties; i++)
    {
        propertyNameArray.append(thisClass.getProperty(i).getName());
    }
    return propertyNameArray;
}
//EXP_PULL_END

//***************************************************************************
//
// CIMOperationRequestDispatcher Class Implementation
//
//***************************************************************************

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

    // Define the maximum number of objects that the server will return for a
    // single pull... or open... operation. (Objects can be instances or
    // CIMObjectPaths  depending on the operation.
#ifdef PEGASUS_PULL_OPERATION_MAXIMUM_OBJECT_COUNT
    _systemMaxPullOperationObjectCount =
        PEGASUS_PULL_OPERATION_MAXIMUM_OBJECT_COUNT;
#else
    // Default setting if nothing supplied externally
    _systemMaxPullOperationObjectCount = 1000;
#endif

    // Define system maximum pull interoperation timeout value.  This defines
    // the maximum value for operationTimeout that will be accepted by
    // Pegasus. Anything larger than this will be rejected with the
    // error CIM_ERR_INVALID_OPERATION_TIMEOUT.

    _systemMaxOperationTimeout = 15;

    // define the variable that controls whether we allow 0 as a pull
    // interoperation timeout value.  Since the behavior for a zero value is
    // that the server maintains no timer for the context, it may be the
    // decision of some implementors to not allow this value.
    // Define the maximum number of objects that the server will return for a
    // single pull... or open... operation. (Objects can be instances or
    // CIMObjectPaths  depending on the operation.
#ifdef PEGASUS_PULL_OPERATION_REJECT_ZERO_TIMEOUT_VALUE
    _rejectZeroOperationTimeoutValue = true
#else
    // Default setting if nothing supplied externally
    _rejectZeroOperationTimeoutValue = false;
#endif

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

    _routing_table = DynamicRoutingTable::getRoutingTable();

    _providerManagerServiceId =
        lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)->getQueueId();

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
        pos = provider.findProperty(PEGASUS_PROPERTYNAME_NAME);
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
Boolean CIMOperationRequestDispatcher::_enqueueAggregateResponse(
    OperationAggregate*& poA,
    CIMResponseMessage*& response)
{
    static const char func[] =
        "CIMOperationRequestDispatcher::_enqueueAggregateResponse";
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
                handleEnumerateInstancesResponseAggregation(poA,true);
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
                PEG_TRACE((
                    TRC_DISCARDED_DATA,
                    Tracer::LEVEL1,
                    "%s%s%u",
                    func,
                    failMsg,
                    type));
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

        if (poA->_pullOperation)
        {
            // pull operation. Put CIMResponseData into Enum Context unless
            // enum context closed.
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "before call to EnumerationContextPutCache iscomplete = %s",
                _toCharP(isComplete)));

            EnumerationContext* en =
                (EnumerationContext*)poA->_enumerationContext;

            PEGASUS_ASSERT(en);          // KS_TEMP
            PEGASUS_ASSERT(en->valid()); // KS_TEMP
            enumerationTable.valid();    // KS_TEMP
            en->trace();                 // KS_TEMP

            enumerationTable.tableValidate();

            EnumerationContext* enTest = enumerationTable.find(
                en->getContextName());

            if (enTest == 0)
            {
                cout << "Error, EnumContext not found "
                    << en->getContextName() << endl;
                en->trace();
                PEGASUS_ASSERT(false);
            }

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,   // KS_TEMP
                "before call to EnumerationContextPutCache context = %s "
                "iscomplete = %s",
                (const char *)en->getContextName().getCString()
                       , _toCharP(isComplete)));

            // If this is an exception set the error in EnumerationContext
            if (response->cimException.getCode())
            {
                en->setErrorState(response->cimException);
            }

            // Send to the EnumerationContext cache along with the
            // isComplete indicator. Note that this may remove the
            // enumerationContext.

            en->putCache(poA->getRequestType(), response, isComplete);

            delete response;
        }
        else
        {
            if (q)
            {
                q->enqueue(response);
            }
            else
            {
                throw UninitializedObjectException();
            }
        }
    }
    catch (...)
    {
        static const char failMsg[] =
            "Failed to resequence/aggregate/forward response";
        PEG_TRACE((
            TRC_DISCARDED_DATA,
            Tracer::LEVEL1,
            "%s%s",
            func,
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

Boolean CIMOperationRequestDispatcher::_lookupInternalProvider(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Uint32 &serviceId,
    String& provider)
{
    static AtomicInt _initialized(0);
    static Mutex _monitor;

    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupInternalProvider");
    // Clear the strings since used as test later. Poor code but true now

    serviceId = 0;
    provider = String::EMPTY;
    CIMNamespaceName _wild;
    if (_initialized.get() == 0)
    {
        AutoMutex autoMut(_monitor);
        if (_initialized.get() == 0)
        {
            Uint32 controlServiceId = lookup(
                PEGASUS_QUEUENAME_CONTROLSERVICE)->getQueueId();

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_CONFIGSETTING,
                PEGASUS_NAMESPACENAME_CONFIG,
                PEGASUS_MODULENAME_CONFIGPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_AUTHORIZATION,
                PEGASUS_NAMESPACENAME_AUTHORIZATION,
                PEGASUS_MODULENAME_USERAUTHPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_USER,
                PEGASUS_NAMESPACENAME_USER,
                PEGASUS_MODULENAME_USERAUTHPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_SHUTDOWN,
                PEGASUS_NAMESPACENAME_SHUTDOWN,
                PEGASUS_MODULENAME_SHUTDOWNPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME___NAMESPACE,
                _wild,
                PEGASUS_MODULENAME_NAMESPACEPROVIDER,
                controlServiceId);

#ifdef PEGASUS_HAS_SSL
            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_CERTIFICATE,
                PEGASUS_NAMESPACENAME_CERTIFICATE,
                PEGASUS_MODULENAME_CERTIFICATEPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_CRL,
                PEGASUS_NAMESPACENAME_CERTIFICATE,
                PEGASUS_MODULENAME_CERTIFICATEPROVIDER,
                controlServiceId);
#endif

#ifndef PEGASUS_DISABLE_PERFINST
            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_CIMOMSTATDATA,
                PEGASUS_NAMESPACENAME_CIMOMSTATDATA,
                PEGASUS_MODULENAME_CIMOMSTATDATAPROVIDER,
                controlServiceId);
#endif

#ifdef PEGASUS_ENABLE_CQL
            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_CIMQUERYCAPABILITIES,
                //PEGASUS_NAMESPACENAME_CIMQUERYCAPABILITIES,
                _wild,
                PEGASUS_MODULENAME_CIMQUERYCAPPROVIDER,
                controlServiceId);
#endif

#if defined PEGASUS_ENABLE_INTEROP_PROVIDER
            // InteropProvider ObjectManager Class
            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_OBJECTMANAGER,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            // CIM_Namespace - Implemented to assure that it does not
            // access the repository and to allow  access to
            // common class CIM_Namespace for namespace creation.
            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_CIMNAMESPACE,
                _wild,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            // PG_NAMESPACE - Implements subclass of CIM_Namespace managed by
            // InteropProvider.
            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PGNAMESPACE,
                _wild,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_CIMXMLCOMMUNICATIONMECHANISM,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_COMMMECHANISMFORMANAGER,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_COMPUTERSYSTEM,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_HOSTEDOBJECTMANAGER,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_HOSTEDACCESSPOINT,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_NAMESPACEINMANAGER,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_REFERENCEDPROFILE,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                 PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE,
                _wild,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                 PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE_RP_RP,
                _wild,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                 PEGASUS_CLASSNAME_PG_PROVIDERPROFILECAPABILITIES,
                _wild,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                 PEGASUS_CLASSNAME_PG_SUBPROFILEREQUIRESPROFILE,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_ELEMENTSOFTWAREIDENTITY,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_INSTALLEDSOFTWAREIDENTITY,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);
#endif  // PEGASUS_ENABLE_INTEROP_PROVIDER

#ifdef PEGASUS_ENABLE_DMTF_INDICATION_PROFILE_SUPPORT
            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_ELEMENTCAPABILITIES,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_HOSTEDINDICATIONSERVICE,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_SERVICEAFFECTSELEMENT,
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_MODULENAME_INTEROPPROVIDER,
                controlServiceId);
#endif

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PROVIDERMODULE,
                PEGASUS_NAMESPACENAME_PROVIDERREG,
                PEGASUS_MODULENAME_PROVREGPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PROVIDER,
                PEGASUS_NAMESPACENAME_PROVIDERREG,
                PEGASUS_MODULENAME_PROVREGPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PROVIDERCAPABILITIES,
                PEGASUS_NAMESPACENAME_PROVIDERREG,
                PEGASUS_MODULENAME_PROVREGPROVIDER,
                controlServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_CONSUMERCAPABILITIES,
                PEGASUS_NAMESPACENAME_PROVIDERREG,
                PEGASUS_MODULENAME_PROVREGPROVIDER,
                controlServiceId);

            Uint32 indicationServiceId = lookup(
                PEGASUS_QUEUENAME_INDICATIONSERVICE)->getQueueId();

#ifdef PEGASUS_ENABLE_DMTF_INDICATION_PROFILE_SUPPORT
            Uint32 handlerServiceId = lookup(
                PEGASUS_QUEUENAME_INDHANDLERMANAGER)->getQueueId();

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PG_LSTNRDSTQUEUE,
                PEGASUS_NAMESPACENAME_INTERNAL,
                String::EMPTY,
                handlerServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_CIM_INDICATIONSERVICE,
                PEGASUS_NAMESPACENAME_INTEROP,
                String::EMPTY,
                indicationServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_CIM_INDICATIONSERVICECAPABILITIES,
                PEGASUS_NAMESPACENAME_INTEROP,
                String::EMPTY,
                indicationServiceId);
#endif
            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_INDSUBSCRIPTION,
                _wild,
                String::EMPTY,
                indicationServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_FORMATTEDINDSUBSCRIPTION,
                _wild,
                String::EMPTY,
                indicationServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_INDHANDLER,
                _wild,
                String::EMPTY,
                indicationServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_LSTNRDST_CIMXML,
                _wild,
                String::EMPTY,
                indicationServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                _wild,
                String::EMPTY,
                indicationServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_INDHANDLER_SNMP,
                _wild,
                String::EMPTY,
                indicationServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_LSTNRDST_SYSTEM_LOG,
                _wild,
                String::EMPTY,
                indicationServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_LSTNRDST_EMAIL,
                _wild,
                String::EMPTY,
                indicationServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_INDFILTER,
                _wild,
                String::EMPTY,
                indicationServiceId);

#ifdef PEGASUS_ENABLE_INDICATION_COUNT
            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_PROVIDERINDDATA,
                PEGASUS_NAMESPACENAME_INTERNAL,
                String::EMPTY,
                indicationServiceId);

            _routing_table->insertRecord(
                PEGASUS_CLASSNAME_SUBSCRIPTIONINDDATA,
                PEGASUS_NAMESPACENAME_INTERNAL,
                String::EMPTY,
                indicationServiceId);
#endif
            _initialized = 1;
        }
    }

    Boolean gotRouting =
        _routing_table->getRouting(
            className,
            nameSpace,
            provider,
            serviceId);

    if (gotRouting)
    {
        PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL4,
        "Internal provider Service = %s provider %s found.",
        _getServiceName(serviceId),
        (const char*)provider.getCString()));
    }

    PEG_METHOD_EXIT();
    return gotRouting;
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
        // Get the complete list of subclass names
        // getSubClassNames throws an exception if the class does not exist
        _repository->getSubClassNames(nameSpace,
             className, true, subClassNames);
        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL4,
            "CIMOperationRequestDispatcher::_getSubClassNames - "
                "Namespace: %s  Class name: %s",
            CSTRING(nameSpace.getString()),
            CSTRING(className.getString())));
    }
    // Prepend the array with the classname that formed the array.
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
// Provider Lookup Functions
//
///////////////////////////////////////////////////////////////////////////

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
ProviderInfoList CIMOperationRequestDispatcher::_lookupAllInstanceProviders(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAllInstanceProviders");

    ProviderInfoList providerList;
    //list.providerCount = 0;

    Array<CIMName> classNames = _getSubClassNames(nameSpace, className);

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
            providerList.providerCount++;

            PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL4,
                "Provider found for class = %s servicename = %s "
                "controlProviderName = %s",
                (const char*)providerInfo.className.getString().getCString(),
                _getServiceName(providerInfo.serviceId),
                (const char*)providerInfo.controlProviderName.getCString()));
        }

        providerList.append(providerInfo);
   }

   PEG_METHOD_EXIT();

   return providerList;
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
        providerInfo.serviceId = _providerManagerServiceId;
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
                pmInstance.getProperty(pos).getValue().get(interfaceVersion);
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
            if ((pos = pmInstance.findProperty(PEGASUS_PROPERTYNAME_NAME)) !=
                    PEG_NOT_FOUND)
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

            PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL4,
                "Normalization for provider module %s is %s.",
                (const char*)moduleName.getCString(),
                (providerInfo.hasProviderNormalization ?
                     "enabled" : "disabled")));
        }
#endif
        PEG_METHOD_EXIT();
        return providerInfo;
    }

    PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL2,
        "Provider for %s not found.",
        (const char*)className.getString().getCString()));

    PEG_METHOD_EXIT();
    return providerInfo;
}

/* _lookupNewInstanceProvider - Looks up the internal and/or instance provider
    for the defined namespace and class and returns the serviceId and
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

    Uint32 serviceId;
    String controlProviderName;

    // Check for class provided by an internal provider
    Boolean hasControlProvider =
        _lookupInternalProvider(
            nameSpace,
            className,
            serviceId,
            controlProviderName);

    if (hasControlProvider)
    {
        providerInfo.serviceId = serviceId;
        providerInfo.controlProviderName = controlProviderName;
        providerInfo.hasProvider = true;
    }
    else
    {
        // get provider for class
        providerInfo = _lookupInstanceProvider(
                nameSpace,
                className);
    }

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL4,
        "CIMOperationRequestDispatcher::_lookupNewInstanceProvider - "
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
        Uint32 pos = pInstance.findProperty(PEGASUS_PROPERTYNAME_NAME);

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
        (const char*)className.getString().getCString(),
        (const char*)assocClass.getString().getCString()));

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

    Array<CIMObjectPath> tmp;

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
    catch (...)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::lookupAllAssociationProvider "
                "exception.  Namespace: %s  Object Name: %s  Assoc Class: %s",
            (const char*)nameSpace.getString().getCString(),
            (const char*)objectName.toString().getCString(),
            (const char*)assocClass.getString().getCString()));
        throw;
    }

    // returns the list of possible association classes for this target.
    // Convert to classnames

    Array<CIMName> classNames;
    for (Uint32 i = 0; i < tmp.size(); i++)
    {
        classNames.append(tmp[i].getClassName());
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "Association Lookup: %u classes found",
        classNames.size()));

    // ATTN: KS P2 20030420  What should we do with remote associations if
    // there are any
    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        Uint32 serviceId;
        String controlProviderName;
        ProviderInfo pi(classNames[i]);
        ProviderIdContainer* container=NULL;

        // We use the returned classname for the association classname
        // under the assumption that the registration is for the
        // association class, not the target class
        if (_lookupNewAssociationProvider(nameSpace, classNames[i],
            serviceId, controlProviderName,&container))
        {
            pi.serviceId = serviceId;
            pi.controlProviderName = controlProviderName;
            pi.hasProvider = true;
            pi.providerIdContainer.reset(container);
            providerInfoList.providerCount++;
        }
        else
        {
            pi.hasProvider = false;
            pi.providerIdContainer.reset();
        }
        providerInfoList.append(pi);
    }

    PEG_METHOD_EXIT();
    return providerInfoList;
}

/* _lookupNewAssociationProvider - Looks up the internal and/or instance
    provider for the defined namespace and class and returns the serviceId
    and control provider name if a provider is found.
    @param nameSpace
    @param assocClass
    @param serviceId
    @param controlProviderName
    @return true if an service, control provider, or instance provider is found
    for the defined class and namespace.
    This should be combined with the lookupInstanceProvider code eventually but
    the goal now was to simplify the handlers.
*/
Boolean CIMOperationRequestDispatcher::_lookupNewAssociationProvider(
    const CIMNamespaceName& nameSpace,
    const CIMName& assocClass,
    Uint32 &serviceId,
    String& controlProviderName,
    ProviderIdContainer** providerIdContainer)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupNewAssociationProvider");

    Boolean hasProvider = false;
    String providerName;
    // Check for class provided by an internal provider
    if (_lookupInternalProvider(
            nameSpace, assocClass, serviceId, controlProviderName))
    {
        hasProvider = true;
    }
    else
    {
        // get provider for class. Note that we reduce it from
        // Multiple possible class names to a single one here.
        // This is a hack.  Clean up later
        Array<String> tmp =
            _lookupAssociationProvider(
                nameSpace, assocClass, providerIdContainer);

        if (tmp.size() > 0)
        {
            providerName = tmp[0];
        }
    }

    if (0 != providerName.size())
    {
        serviceId = _providerManagerServiceId;

        hasProvider = true;
    }

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL4,
        "Provider %s for class=\"%s\"  in namespace \"%s\"."
            " servicename=\"%s\" provider = \"%s\" controlProvider = \"%s\"",
        (hasProvider? "found" : "NOT found"),
        CSTRING(assocClass.getString()),
        CSTRING(nameSpace.getString()),
        _getServiceName(serviceId),
        (providerName.size() ? CSTRING(providerName) : "none"),
        (controlProviderName.size() ? CSTRING(controlProviderName) : "none")));


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
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAssociationProvider");

    // instances of the provider class and provider module class for the
    // response
    Array<CIMInstance> pInstances; // Providers
    Array<CIMInstance> pmInstances; // ProviderModules

    Array<String> providerNames;
    String providerName;

    //Isolate the provider names from the response and return list of providers
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
                ProviderIdContainer* providercontainer;
#ifdef PEGASUS_ENABLE_REMOTE_CMPI
                String remoteInformation;
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
            Uint32 pos = pInstances[i].findProperty(PEGASUS_PROPERTYNAME_NAME);

            if ( pos != PEG_NOT_FOUND )
            {
                pInstances[i].getProperty(pos).getValue().get(providerName);

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "Association providerName = %s found for Class %s",
                    (const char*)providerName.getCString(),
                    (const char*)assocClass.getString().getCString()));
                providerNames.append(providerName);
            }
        }
    }

    if (providerNames.size() == 0)
    {
        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL3,
            "Association Provider NOT found for Class %s in nameSpace %s",
            (const char*)assocClass.getString().getCString(),
            (const char*)nameSpace.getString().getCString()));
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
        service->return_op(op);
    }
    else
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
        "Provider response is incomplete.");
    }

    // After resequencing, this flag represents the completion status of
    // the ENTIRE response to the request.
    Boolean providersComplete;
    PEGASUS_ASSERT(poA->valid());           // KS_TEMP
    if (poA->_pullOperation)                // KS_TEMP
    {
        PEGASUS_ASSERT(poA->_enumerationContext); //KS_TEMP
//      providersComplete = service->_enqueuePullAggregateResponse(
//          poA,response);
    }
//  else
        providersComplete = service->_enqueueAggregateResponse(poA, response);

    // If providers complete, delete the OperationAggregate object
    if (providersComplete)
    {
        // also deletes the copied request attached to it
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "Entire provider response is complete. isPull %s",
            _toCharP(poA->_pullOperation)));
        delete poA;
        poA = 0;
    }
    else
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
        "Entire provider response to a request is not complete.");
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


void CIMOperationRequestDispatcher::_forwardRequestToService(
    Uint32 serviceId,
    CIMRequestMessage* request,
    CIMRequestMessage* requestCopy)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToService");

    PEGASUS_ASSERT(serviceId);

    AsyncOpNode* op = this->get_op();

    AsyncLegacyOperationStart* asyncRequest =
        new AsyncLegacyOperationStart(
            op,
            serviceId,
            request);

    asyncRequest->dest = serviceId;

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "Forwarding %s to service %s. Response should go to queue %s.",
        MessageTypeToString(request->getType()),
        _getServiceName(serviceId),
        ((MessageQueue::lookup(request->queueIds.top())) ?
        ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) :
               "BAD queue name")));

    SendAsync(
        op,
        serviceId,
        CIMOperationRequestDispatcher::_forwardRequestCallback,
        this,
        requestCopy);

    PEG_METHOD_EXIT();
}

/* This function decides based on the controlProviderNameField
    whether to forward to Service or ControlProvider.
    If controlProviderName String empty, ToService, else toControlProvider.
    This function specifically forwards requests for response aggregation
    by the CIMOperationRequestDispatcher::_forwardForAggregationCallback().
    If a response is included it sets the operation complete so the async
    functions call the callback function instead of the provider/control
    provider.
*/
void CIMOperationRequestDispatcher::_forwardRequestForAggregation(
    Uint32 serviceId,
    const String& controlProviderName,
    CIMRequestMessage* request,
    OperationAggregate* poA,
    CIMResponseMessage* response)
{
    PEG_METHOD_ENTER(
        TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestForAggregation");

    PEGASUS_ASSERT(poA->getRequestType() !=
        CIM_OPEN_ASSOCIATOR_INSTANCES_REQUEST_MESSAGE);
    PEGASUS_ASSERT(serviceId);

    AsyncOpNode* op = this->get_op();

    // if a response is given, this means the caller wants to run only the
    // callback asynchronously
    if (response)
    {
        AsyncLegacyOperationResult* asyncResponse =
            new AsyncLegacyOperationResult(
                op, response);

        // By setting this to complete, this allows ONLY the callback to run
        // without going through the async request apparatus
        op->complete();
    }

    // If ControlProviderName empty, forward to service.
    if (controlProviderName == String::EMPTY)
    {
        AsyncLegacyOperationStart* asyncRequest =
            new AsyncLegacyOperationStart(
                op,
                serviceId,
                request);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
            "Forwarding %s to service %s. Response should go to queue %s.",
            MessageTypeToString(request->getType()),
            _getServiceName(serviceId),
            ((MessageQueue::lookup(request->queueIds.top())) ?
            ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) :
                   "BAD queue name")));

        SendAsync(
            op,
            serviceId,
            CIMOperationRequestDispatcher::_forwardForAggregationCallback,
            this,
            poA);
    }
    else
    {
       AsyncModuleOperationStart* moduleControllerRequest =
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
           (const char*)controlProviderName.getCString(),
           ((MessageQueue::lookup(request->queueIds.top())) ?
           ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) :
                  "BAD queue name")));

       SendAsync(
           op,
           serviceId,
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
    Uint32 serviceId,
    const String& controlProviderName,
    CIMRequestMessage* request,
    CIMRequestMessage* requestCopy)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToProviderManager");

    PEGASUS_ASSERT(serviceId);

    AsyncOpNode* op = this->get_op();

    // If ControlProviderName empty, forward to service.
    if (controlProviderName == String::EMPTY)
    {
        AsyncLegacyOperationStart* asyncRequest =
            new AsyncLegacyOperationStart(
                op,
                serviceId,
                request);

        asyncRequest->dest = serviceId;

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
            "Forwarding %s on class %s to service %s. "
            "Response should go to queue %s.",
            MessageTypeToString(request->getType()),
            (const char*)className.getString().getCString(),
            _getServiceName(serviceId),
            ((MessageQueue::lookup(request->queueIds.top())) ?
            ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) :
                   "BAD queue name")));

        SendAsync(
            op,
            serviceId,
            CIMOperationRequestDispatcher::_forwardRequestCallback,
            this,
            requestCopy);
    }
    else
    {
        AsyncModuleOperationStart* moduleControllerRequest =
            new AsyncModuleOperationStart(
                op,
                serviceId,
                controlProviderName,
                request);


        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
            "Forwarding %s on class %s to service %s, control provider %s. "
            "Response should go to queue %s.",
            MessageTypeToString(request->getType()),
            (const char*)className.getString().getCString(),
            _getServiceName(serviceId),
            (const char*)controlProviderName.getCString(),
            ((MessageQueue::lookup(request->queueIds.top())) ?
            ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) :
                   "BAD queue name")));

       // Send to the Control provider
       SendAsync(
           op,
           serviceId,
           CIMOperationRequestDispatcher::_forwardRequestCallback,
           this,
           requestCopy);
    }

    PEG_METHOD_EXIT();
}

/*
    Enqueue an Exception response
    This is a helper function that creates a response message
    with the defined exception and queues it.
*/
void CIMOperationRequestDispatcher::_enqueueExceptionResponse(
   CIMRequestMessage* request,
   CIMException& exception)
{
    CIMResponseMessage* response = request->buildResponse();
    response->cimException = exception;
    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::_enqueueExceptionResponse(
   CIMRequestMessage* request,
   TraceableCIMException& exception)
{
    CIMResponseMessage* response = request->buildResponse();
    response->cimException = exception;
    _enqueueResponse(request, response);
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
   CIMRequestMessage* request,
   CIMResponseMessage* response)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_enqueueResponse");

    response->dest = request->queueIds.top();

    // Ensure the response message attributes are synchonized with the request
    PEGASUS_ASSERT(request->getMask() == response->getMask());
    PEGASUS_ASSERT(request->getHttpMethod() == response->getHttpMethod());
    PEGASUS_ASSERT(request->getCloseConnect() == response->getCloseConnect());

    PEG_TRACE((
        TRC_HTTP,
        Tracer::LEVEL4,
        "_CIMOperationRequestDispatcher::_enqueueResponse - "
            "request->getCloseConnect() returned %d",
        request->getCloseConnect()));

    _logOperation(request, response);

    // ATTN: Internal client does not have async capabilities, call enqueue()
    // for handling legacy messages directly.
    MessageQueue* queue = MessageQueue::lookup(request->queueIds.top());
    PEGASUS_ASSERT(queue != 0);

    queue->enqueue(response);

    PEG_METHOD_EXIT();
}

/*
    handle the incoming operation request by calling the proper
    handler function for the CIMMessageType. This function includes
    a common exception try/catch to account for exceptions in any of the
    individual operation request handlers. The function deletes the
    incoming request upon completion of the handler execution.
*/
void CIMOperationRequestDispatcher::handleEnqueue(Message* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnqueue(Message* request)");

    PEGASUS_ASSERT(request != 0);
    PEGASUS_DEBUG_ASSERT(request->magic);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleEnqueue - Case: %u",
        request->getType()));

    CIMOperationRequestMessage* opRequest =
        dynamic_cast<CIMOperationRequestMessage*>(request);

    if (!opRequest)
    {
        PEG_TRACE((
            TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Ignored unexpected message of type %u in "
                "CIMOperationRequestDispatcher::handleEnqueue",
            (unsigned int) request->getType()));
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

// KS_PULL_BEGIN
        case CIM_OPEN_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
            handleOpenEnumerateInstancesRequest(
                (CIMOpenEnumerateInstancesRequestMessage*)opRequest);
            break;

        case CIM_OPEN_ENUMERATE_INSTANCE_PATHS_REQUEST_MESSAGE:
            handleOpenEnumerateInstancePathsRequest(
                (CIMOpenEnumerateInstancePathsRequestMessage*)opRequest);
            break;

        case CIM_OPEN_REFERENCE_INSTANCES_REQUEST_MESSAGE:
            handleOpenReferenceInstancesRequest(
                (CIMOpenReferenceInstancesRequestMessage*)opRequest);
            break;

        case CIM_OPEN_REFERENCE_INSTANCE_PATHS_REQUEST_MESSAGE:
            handleOpenReferenceInstancePathsRequest(
                (CIMOpenReferenceInstancePathsRequestMessage*)opRequest);
            break;

        case CIM_OPEN_ASSOCIATOR_INSTANCES_REQUEST_MESSAGE:
            handleOpenAssociatorInstancesRequest(
                (CIMOpenAssociatorInstancesRequestMessage*)opRequest);
            break;

        case CIM_OPEN_ASSOCIATOR_INSTANCE_PATHS_REQUEST_MESSAGE:
            handleOpenAssociatorInstancePathsRequest(
                (CIMOpenAssociatorInstancePathsRequestMessage*)opRequest);
            break;
        case CIM_PULL_INSTANCES_WITH_PATH_REQUEST_MESSAGE:
            handlePullInstancesWithPath(
                (CIMPullInstancesWithPathRequestMessage*) opRequest);
            break;

        case CIM_PULL_INSTANCE_PATHS_REQUEST_MESSAGE:
            handlePullInstancePaths(
                (CIMPullInstancePathsRequestMessage*) opRequest);
            break;

        case CIM_CLOSE_ENUMERATION_REQUEST_MESSAGE:
            handleCloseEnumeration(
                (CIMCloseEnumerationRequestMessage*) opRequest);
            break;

        case CIM_ENUMERATION_COUNT_REQUEST_MESSAGE:
            handleEnumerationCount(
                (CIMEnumerationCountRequestMessage*) opRequest);
            break;

//KS_PULL_END

        default:
            PEGASUS_ASSERT(0);
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
    CIMOperationDispatcher dequeue function to dequene the
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

/*****************************************************************************
**
**      Request Processing Helper Methods
**
*****************************************************************************/
/*
    Helper struct/methods to issue operations requests to groups of Providers
    defined by a ProviderInfoList. This struct issues requests of the type
    defined by input to the providers defined in the providerInfoList.

    There are separate functions for issuing:
        issueEnumRequests - enumerate operations (enumerate and enumerateNames)
        issueAssocRequests - association operations (includes references
            and associations and their corresponding name operations).

    This struct eliminates the repeated code for issuing requests in the
    handle***Request functions for those input operation requests that issue
    provider operation requests to multiple providers based on a
    ProviderInfoList. It reduces the previously repeated code for issuing
    requests to providers in the operation request processors for these
    functions to a single line.

    This struct not part of CIMOperationRequestDispatcher class because it
    includes template methods.
*/
struct ProviderRequests
{
    // Set specific fields as part of the request type.  These are required
    // because we set different fields in the request message for each
    // operation type with the class for the defined provider.
    // References use the result class and Association requests, the
    // assocClass field.
    static void setSelectedRequestFields(
        CIMAssociatorsRequestMessage* request,
        const ProviderInfo& providerInfo)
    {
        request->assocClass = providerInfo.className;
    }

    static void setSelectedRequestFields(
        CIMAssociatorNamesRequestMessage* request,
        const ProviderInfo& providerInfo)
    {
        request->assocClass = providerInfo.className;
    }

    static void setSelectedRequestFields(
        CIMReferencesRequestMessage* request,
        const ProviderInfo& providerInfo)
    {
        request->resultClass = providerInfo.className;
    }

    static void setSelectedRequestFields(
        CIMReferenceNamesRequestMessage* request,
        const ProviderInfo& providerInfo)
    {
        request->resultClass = providerInfo.className;
    }

    /**************************************************************************
    **
    ** issueAssocRequests - Template method to issue requests for
    **     association/refernece operations.
    **
    **************************************************************************/
    /*  Template method to issue requests for association/reference
        operations. The function issues requests of the type defined for
        the template to the providers in the ProviderInfoList. It also
        execute common code for this set of request types.
        NOTE: The response may already have data in it from a repository
        request.
    */

    template<class REQ, class RSP>
    static void issueAssocRequests(
        CIMOperationRequestDispatcher* dispatcher,
        Uint32 queueID,
        REQ* request,
        AutoPtr<RSP>& response,
        ProviderInfoList providerInfos)
    {
        if (providerInfos.providerCount == 0)
        {
            //
            // If no providers to call and nothing in repository response,
            // return what we have or error  and close operation.
            //
            if (!response.get())
            {
                // No provider is registered and the repository isn't the
                // default.  Return CIM_ERR_NOT_SUPPORTED.

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
                    "CIM_ERR_NOT_SUPPORTED for %s",
                    (const char*)request->className.getString().getCString()));

                response.reset(dynamic_cast<RSP*>(
                    request->buildResponse()));
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_NOT_SUPPORTED,
                    String::EMPTY);
            }
            // Issue response (error or data). This terminates operation.
            dispatcher->_enqueueResponse(request, response.release());
        }
        else
        {
            //
            // Else Providers exist. Issue requests to providers.
            // Set up an aggregate object and save the original request message
            //

            OperationAggregate* poA = new OperationAggregate(new REQ(*request),
                request->getType(),
                request->messageId,
                request->queueIds.top(),
                request->objectName.getClassName(),
                request->nameSpace);

            // Include the repository response in the aggregation, if data
            // in response
            // KS_TODO - I think this is wrong.  It creates a response
            // even if there is no data in the repository return
            if (response.get() != 0)
            {
                poA->setTotalIssued(providerInfos.providerCount + 1);
                // send the repository's results asynchronously to the
                // correct dispatcher callback function.

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
                    "Forwarding to provider Callback result from Repository."
                        " Total Issued: %u. total objects: %u",
                    (providerInfos.providerCount + 1),
                    response->getResponseData().size()));
                dispatcher->_forwardRequestForAggregation(
                    queueID,
                    String(),
                    new REQ(*request),
                    poA,
                    response.release());
            }
            else
            {
                poA->setTotalIssued(providerInfos.providerCount);

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,    // KS_TEMP
                    "Forwarding to provider.. Total Issued: %u ",
                    providerInfos.providerCount));
            }

            // loop for all classes in providerInfos
            for (Uint32 i = 0; i < providerInfos.size(); i++)
            {
                // If this class has a provider
                if (providerInfos[i].hasProvider)
                {
                    // Make copy of request
                    REQ* requestCopy = new REQ(*request);

                    // Set class into new request for each request type
                    setSelectedRequestFields(requestCopy, providerInfos[i]);

                    if (providerInfos[i].providerIdContainer.get() != 0)
                        requestCopy->operationContext.insert(
                            *(providerInfos[i].providerIdContainer.get()));

                    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                        "Forwarding to provider for class %s",
                        CSTRING(providerInfos[i].className.getString())));

                    dispatcher->_forwardRequestForAggregation(
                        providerInfos[i].serviceId,
                        providerInfos[i].controlProviderName, requestCopy, poA);
                    // Note: poA must not be referenced after last
                    // "forwardRequest"
                }
            }
        }
    } // end issueAssocRequests

    /**************************************************************************
    **
    ** IssueEnumerationRequests - Issue enumeration requests to providers
    **     for enumerateInstances and enumerateInstanceNames operations.
    **
    **************************************************************************/
    /*  Common Provider issue code for all
        enumerate functions (enumerateInstance, EnumerateInstanceNames
        openEnumerateInstancesWithPath openEnumerateInstancePaths).
        Issues a request to all providers in the ProviderInfoList provided.
        NOTE: Whereas today the assocRequests function starts with responses
        this one starts after repository responses processed.
    */
    template<class REQ>
    static void issueEnumerationRequests(
        CIMOperationRequestDispatcher* dispatcher,
        Uint32 queueID,
        REQ* request,
        //AutoPtr<RSP>& response,   // Future when we put more in this funct.
        ProviderInfoList providerInfos,
        OperationAggregate* poA)
    {
        // Loop through providerInfos, forwarding requests to providers

        Uint32 numClasses = providerInfos.size();
        for (Uint32 i = 0; i < numClasses; i++)
        {
            ProviderInfo& providerInfo = providerInfos[i];

            if (providerInfo.hasProvider)
            {
                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "Routing Enumeration request for class %s to "
                        "service \"%s\" for control provider \"%s\".  "
                        "Class # %u of %u, aggregation SN %u.",
                    CSTRING(providerInfo.className.getString()),
                    _getServiceName(providerInfo.serviceId),
                    CSTRING(providerInfo.controlProviderName),
                    (unsigned int)(i + 1),
                    (unsigned int)(numClasses),
                    (unsigned int)(poA->getAggregationSN())));

                REQ* requestCopy = new REQ(*request);

                // Test if next in list is valid class.
                // NOTE: We really want to only check for validity except
                //    in the case where PEGASUS_ENABLE_OBJECT_NORMALIZATION
                //    is set.  Then we need the class.  We should be able
                //    to optimize this somehow to eliminate the complete
                //    class construction except when required.
                requestCopy->className = providerInfo.className;

                CIMException checkClassException;
                CIMConstClass cimClass = dispatcher->_getClass(
                        request->nameSpace,
                        providerInfo.className,
                        checkClassException);

                // The following is not correct.
                if (checkClassException.getCode() != CIM_ERR_SUCCESS)
                {
                    CIMResponseMessage* response = request->buildResponse();
                    /// KS_TBD Does not insert exception.

                    // Forward for completion processing.
                    // NOTE: the existence of the response indicates
                    //       that this is a completed response.
                    dispatcher->_forwardRequestForAggregation(
                        queueID,
                        String(),
                        new REQ(*request),
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
                    // Issue the generated request to the provider.
                    dispatcher->_forwardRequestForAggregation(
                        providerInfo.serviceId,
                        providerInfo.controlProviderName,
                        requestCopy,
                        poA);
                }
            }
        }
    }    // end issueEnumerationRequests.

    /**************************************************************************
    **
    ** IssuePullResponses - Handles pullInstancesWithPath and pullInstancePaths
    **
    **************************************************************************/
    /**
        Complete processing for the pull operations, pullInstancesWithPath
       and Pull InstancePaths.  This template replaces all the code in both
       pull functions including the input checking code because both operations
       are exactly the same except for the object type.
       @param this -  Pointer to the CIMOperationRequestDispatcher object.
              Required to execute dispatcher methods
       @param REQ = Operation Request Template. Defines the message type
       @param RSP - Operation Response message (Template) defines the
                    response message for this pull response
       @param name - String with the request name. Used internally for
                    traces, etc.
    */

    template<class REQ, class RSP >
    static void issuePullResponse(
        CIMOperationRequestDispatcher* dispatcher,
        REQ* request,
        AutoPtr<RSP>&  response,
        const char* opSeqName) // PullInstancePaths or PullInstancesWithPath
    {
        PEG_METHOD_ENTER(TRC_DISPATCHER,
            "CIMOperationRequestDispatcher::issuePullResponse");
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "%s request for  "
                "maxObjectCount = \"%s\" .  "
                "enumerationContext = \"%s\" .  ", opSeqName,
            CSTRING(request->maxObjectCount.toString()),
            CSTRING(request->enumerationContext))
            );
        // Find the enumerationContext object from the request parameter
        EnumerationContext* enumerationContext =
             enumerationTable.find(request->enumerationContext);


        // If enumeration Context not found, return invalid exception
        if (dispatcher->_rejectInValidEnumerationContext(request,
            enumerationContext))
        {
            PEG_METHOD_EXIT();
            return;
        }

        enumerationContext->trace();          // KS_TEMP

        // reject if this is a not valid request for the originating Operation
        if (dispatcher->_rejectInvalidPullRequest(request,
            enumerationContext->isValidPullRequestType(request->getType())))
        {
            PEG_METHOD_EXIT();
            return;
        }

        if (dispatcher->_rejectIfContextTimedOut(request,
            enumerationContext->isTimedOut()))
        {
            PEG_METHOD_EXIT();
            return;
        }
        // reject if an operation is already active on this enumeration context
        if (dispatcher->_rejectIfEnumerationContextActive(request,
            enumerationContext->isActive()))
        {
            PEG_METHOD_EXIT();
            return;
        }

        // Set active and stop interOperation timer
        enumerationContext->setActiveState(true);
        response->enumerationContext = request->enumerationContext;

        // We really should do this at the time of the last enqueue
        // KS_PULL_TODO - Confirm that the NULL should cause
        // response with nothing in it.
        // If maxObjectCount = 0, Just respond with empty response
        if (request->maxObjectCount.isNull() ||
            request->maxObjectCount.getValue() == 0)
        {
            // KS_TODO - We set an exception if this returns true indicating
            // that we have exceeded the max allowable number of consecutive
            // zero-length pull operations. At that point we would also
            // terminate the enumeration.

            Boolean pullOverrun =
            enumerationContext->incPullOperationCounter(true);

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "%s Generating 0 object requested response", opSeqName));

            response->endOfSequence = true;
            response->cimException = CIMException();

            dispatcher->_enqueueResponse(request, response.release());

            // set to operation inactive and start interoperation timer

            enumerationContext->setActiveState(false);

            PEG_METHOD_EXIT();
            return;
        }

        /*
            Get Objects From Cache and return with end-of-sequence status
            or if errorstate, return error
        */

        // Ignore return from increment here because this is resetting the
        //  consecutive zero length response counter
        enumerationContext->incPullOperationCounter(false);
        Uint32 localMaxObjectCount = request->maxObjectCount.getValue();

        response->endOfSequence = false;
        response->cimException = CIMException();

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
            "%s getting from cache. isComplete: %s cacheSize: %u error: %s",
            opSeqName,
            _toCharP(enumerationContext->ifProvidersComplete()),
            enumerationContext->responseCacheSize(),
            _toCharP(enumerationContext->isErrorState())  ));

        CIMResponseData & to = response->getResponseData();

        to.setDataType((enumerationContext->getCIMResponseDataType()));

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
          "%s getCacheResponsedata. max objects %u",
          opSeqName,localMaxObjectCount ));

        // Set type per the to datatype
        CIMResponseData from(enumerationContext->getCIMResponseDataType());

        Boolean rtn = enumerationContext->getCacheResponseData(
            localMaxObjectCount, from);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
            "%s AppendResponseData. to type %u from type %u",
            opSeqName,
            to.getResponseDataContent(), from.getResponseDataContent()));

        // If error set, send error response.
        if (enumerationContext->isErrorState())
        {
            response->cimException = enumerationContext->_cimException;
        }
        else
        {
            to.appendResponseData(from);
        }

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
          "%s Send Pull response to type %u from type %u "
              "endOfSequence %s providersComplete %s cacheSize %u",
          opSeqName,
          to.getResponseDataContent(), from.getResponseDataContent(),
          _toCharP(response->endOfSequence),
          _toCharP(enumerationContext->ifProvidersComplete()),
          enumerationContext->responseCacheSize() ));

        if ((response->endOfSequence = enumerationContext->
             ifEnumerationComplete()))
        {
            PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
                "Close Enumeration");
            // close and delete the EnumerationContext object
            enumerationContext->setClosed();
        }
        else
        {
            // set to operation inactive and start interoperation timer
            enumerationContext->setActiveState(false);
        }

        dispatcher->_enqueueResponse(request, response.release());

        if (enumerationContext->isClosed())
        {
            enumerationContext->removeContext();
        }


        } // end issuePullResponse
    };

/****************************************************************************
**
**     Request Parameter Test and reject functions
**     Each function tests a particular possible reason for reject.
**     If th test fails, the reject message is generated and
**     a true response returned.  If the test passes, true returned.
**     Generally named with the prefix reject to indicate that they
**     reject and return true.
**     Each function should terminate the operation processing if true
**     is returned.
**
****************************************************************************/

// Test to determine if Association traversal is enabled.
// FUTURE - This test is an obsolete artifact of the time when we were first
// using association operations and should probably be dropped as an option
// of pegasus and as a test.
// returns true if Not Enabled, false if enabled
Boolean CIMOperationRequestDispatcher::_rejectAssociationTraversalDisabled(
    CIMRequestMessage* request,
    const String& opName)
{
    if (_enableAssociationTraversal)
    {
        // return enabled
        return false;
    }
    else
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, opName);

        _enqueueResponse(request, response);
        return true;
    }
}

/*
    test if this is a valid Pull message corresponding to the type
     of the open. If not valid, put out error message and return false
    @param request
    @param valid Boolean = true if it is valid
    Return true if valid = true, false if valid = false
*/
Boolean CIMOperationRequestDispatcher::_rejectInvalidPullRequest(
    CIMRequestMessage* request, Boolean valid)
{
    if (!valid)
    {
        CIMResponseMessage* response = request->buildResponse();

        CIMException x = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, "Pull and Open Types do not match.");
        response->cimException = x;

        _enqueueResponse(request, response);
    }
    return !valid;
}

/* Test the roleParameter to determine if it exists and is a valid CIMName.
   Generate error if it exists and is not a valid CIMName.
   @return true if invalid and false if valid
*/
Boolean CIMOperationRequestDispatcher::_rejectInvalidRoleParameter(
    CIMRequestMessage* request,
    const String& roleParameter,
    const String& parameterName)
{
    if (roleParameter != String::EMPTY && (!CIMName::legal(roleParameter)))
    {
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_PARAMETER, roleParameter);

        _enqueueResponse(request, response);
        return true;
    }
    return false;
}
/* test to assure that the filter query language amd filter query are as
   required.  At this point, Pegasus refuses any data in these parameters in
   accord with DSP0200 version 1.3.  This will change when the spec and
   Pegasus support these parameters.
*/
Boolean CIMOperationRequestDispatcher::_rejectInvalidFilterParameters(
    CIMRequestMessage* request,
    const String& filterQueryLanguageParam,
    const String& filterQueryParam)
{
    if (filterQueryLanguageParam.size() != 0 ||
            filterQueryParam.size() != 0)
    {
        // KS_TODO - Possible alternate implementation that might
        // reduce code somewhat.
        ////_enqueueExceptionResponse(request,
        ////   PEGASUS_CIM_EXCEPTION(CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED,
        ////       String::EMPTY));
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED,
                "Pegasus does not support Pull Filter parameters");

        _enqueueResponse(request, response);
        return true;
    }
    return false;
}

/* Test to assure that the CIMObjectPath is, in fact, an object path
   and not just a classname.  Return CIM_ERR_INVALID_PARAMETER if not
   valid.  The ObjecPath must be a full instance name to assure that we
   do not try to follow the path for class based operations.
   */
Boolean CIMOperationRequestDispatcher::_rejectInvalidObjectPathParameter(
    CIMRequestMessage* request,
    const CIMObjectPath& path)
{
    if (path.getKeyBindings().size() == 0)
    {
        ////_enqueueExceptionResponse(request,
        ////   PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_PARAMETER,
        ////       "Full Model Path required."));
        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER,
                "Full Model Path required.");

        _enqueueResponse(request, response);
        return true;
    }
    return false;
}

//
// Pegasus does not allow continueOnError parameter. Return Error
// if it exists.
//
Boolean CIMOperationRequestDispatcher::_rejectIfContinueOnError(
    CIMRequestMessage* request,
    Boolean continueOnError)
{
    if (continueOnError)
    {
        CIMResponseMessage* response = request->buildResponse();

        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                                   "ContinueOnError Not supported");

        _enqueueResponse(request, response);
        return true;
    }
    else
        return false;
}

// test and set maxObjectCount for this operation.
// If parameter is required, generate exception if parameter is NULL or
// test for within system max limit if exists.
// MaxObjectCount is optional for all open operations. If not
// supplied, set value = 0. Pegasus allows a systemwide
// maximum to be set.  Since this is only used within
// the operation handler the result is a local function.
// Return an Exception only if the value is outside max limit.
//
// If parameter is optional, set to defaultValue if does not exist or
// test against system max limit if exists.
// @param request The operation request packet.
// @param maxObjectCount - the input parameter from the request packet
// @param requiredParameter Boolean true if parameter required
//     for this operation. This is because it is optional on opens but
//     required on pull operations
// @param value Value to be used if operation returns true;
// @param defaultValue Uint32 value to be used if requiredParameter = false
// and maxObjectCountParam is NULL
// @return - Returns true if parameter OK for this operation or false if
// if fails test.  If true, the value to be used for maxObjectCount for
// this operation is placed in rtnValue. If false return, rtnValue is
// not changed.
// KS_TODO - We duplicate default value and _systemMaxPullOperationObjectCount
Boolean CIMOperationRequestDispatcher::_rejectInvalidMaxObjectCountParam(
    CIMRequestMessage* request,
    const Uint32Arg& maxObjectCountParam,
    Boolean requiredParameter,
    Uint32& rtnValue,
    const Uint32 defaultValue)
{
    if (maxObjectCountParam.isNull())
    {
        if (requiredParameter)
        {
            // required and not exist. Exception exit
            CIMException x = CIMException(CIM_ERR_INVALID_PARAMETER,
                "NULL maxObjectCount parameter is required");
            _enqueueExceptionResponse(request, x);
            return true;
        }
        else   // not required but NULL. return defaultValue
        {
            // not required and not exist, use default.
            // KS-TODO - Why input default here but use
            // _systemMaxPullOperationObjectCount in following test
            rtnValue = defaultValue;
            return false;
        }
    }
    else     // input parameter not null.
    {
        if (maxObjectCountParam.getValue() >
            _systemMaxPullOperationObjectCount)
        {
            CIMException x = CIMException(CIM_ERR_INVALID_PARAMETER,
                "NULL maxObjectCount parameter is outside Pegasus limits");
            _enqueueExceptionResponse(request, x);
            return true;
        }
        else
        {
                rtnValue = maxObjectCountParam.getValue();
        }
    }
    return false;
}

// Test validity of operation timeout parameter.
// If the input value is NULL, we use our own value.
// If the value is gt system parameter we reject.
// If the value is 0 we follow the dictates of a parameter that defines
// this decision. Some systems implementations may allow this value. Others
// may elect to reject 0 (which means no timeout).

Boolean CIMOperationRequestDispatcher::_rejectInvalidOperationTimeout(
    CIMRequestMessage* request,
    const Uint32Arg& operationTimeout)
{
    // NULL is allowed.  The EnumerationContext class will determine
    // the limit during creation based on system default. See
    // Enumeration table
    if (operationTimeout.isNull())
    {
        return false;
    }

    // If system does not allow zero value, send error.
    if (operationTimeout.getValue() == 0)
    {
        if (_rejectZeroOperationTimeoutValue)
        {
            CIMResponseMessage* response = request->buildResponse();
            response->cimException =
                PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_INVALID_OPERATION_TIMEOUT,
                    "Operation Timeout value of 0 not allowed");

            _enqueueResponse(request, response);
            return true;
        }
        else
        {
            return false;
        }
    }

    // If the value is greater than the system allowed max, send error
    // response
    if (operationTimeout.getValue() > _systemMaxOperationTimeout)
    {
       CIMResponseMessage* response = request->buildResponse();
       String strError = "Operation Timeout too large. ";
       // KS_PULL_TBD. Create real value here.
       strError.append(_systemMaxOperationTimeout);
       strError.append(" maximum allowed.");

        response->cimException =
            PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_OPERATION_TIMEOUT,
                strError);

        _enqueueResponse(request, response);
        return true;
    }
    return false;
}
/* Generate error response message if context is invalid.
   Null pointer is current test for validity.
   @param valid Boolean = true if valid
   @return true if valid=true, false if valid=false.
*/
Boolean CIMOperationRequestDispatcher::_rejectInValidEnumerationContext(
    CIMRequestMessage* request,
    void * enumerationContext)
{
    EnumerationContext* en = (EnumerationContext *) enumerationContext;
    if (enumerationContext == 0 || !en->valid())
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "PullInstanceWithPath Invalid Context parameter Received" ));

        CIMResponseMessage* response = request->buildResponse();

        CIMException x = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_ENUMERATION_CONTEXT, "Context undefined");
        response->cimException = x;

        _enqueueResponse(request, response);
        return true;
    }
    return false;
}


/* test if the parameter isTimedOut is true, If true an exception
   return is queued amd true is returned
   @param request - pointer to request
   @isTimedOut - Boolean = true if timeout has occurred
   @return true if timed out.
*/

Boolean CIMOperationRequestDispatcher::_rejectIfContextTimedOut(
    CIMRequestMessage* request,
    Boolean isTimedOut)
{
    if (isTimedOut)
    {
        CIMResponseMessage* response = request->buildResponse();

        CIMException cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_ENUMERATION_CONTEXT,
            "Enumeration Context Timed out");
        response->cimException = cimException;

        _enqueueResponse(request, response);

        return true;
    }
    return false;
}

Boolean CIMOperationRequestDispatcher::_rejectIfEnumerationContextActive(
    CIMRequestMessage* request, Boolean active)
{
    if (active)
    {
        CIMResponseMessage* response = request->buildResponse();

        CIMException x = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_PULL_CANNOT_BE_ABANDONED, String::EMPTY);
        response->cimException = x;

        _enqueueResponse(request, response);
    }
    return active;
}

/**$*******************************************************
    handleGetClassRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleGetClassRequest(
    CIMGetClassRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleGetClassRequest");

    // ATTN: Need code here to expand partial class!

    CIMClass cimClass =
        _repository->getClass(
            request->nameSpace,
            request->className,
            request->localOnly,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList);

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
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

    // ATTN: Need code here to expand partial instance!

    // get the class name
    CIMName className = request->instanceName.getClassName();
    CIMException checkClassException;

    CIMConstClass cimClass =
        _getClass(
            request->nameSpace,
            className,
            checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::handleGetInstanceRequest - "
                "CIM exist exception has occurred.  Namespace: %s  "
                "Class Name: %s  Exception message: \"%s\"",
            CSTRING(request->nameSpace.getString()),
            CSTRING(className.getString()),
            CSTRING(checkClassException.getMessage())));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

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

        _forwardRequestToProviderManager(
            providerInfo.className,
            providerInfo.serviceId,
            providerInfo.controlProviderName,
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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
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
        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::handleDeleteInstanceRequest - "
                "CIM class does not exist exception has occurred.  "
                "Namespace: %s  Class Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(className.getString())));

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_CLASS, className.getString());
    }

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
            providerInfo.serviceId,
            providerInfo.controlProviderName,
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

        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL3,
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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
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
        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::handleCreateInstanceRequest - "
                "CIM class does not exist exception has occurred.  "
                "Namespace: %s  Class Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(className.getString())));

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_CLASS, className.getString());
    }

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
        }

        CIMCreateInstanceRequestMessage* requestCallbackCopy =
            new CIMCreateInstanceRequestMessage(*requestCopy);

        _forwardRequestToProviderManager(
            providerInfo.className,
            providerInfo.serviceId,
            providerInfo.controlProviderName,
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

        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL3,
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
        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::handleModifyInstanceRequest - "
                "CIM class does not exist exception has occurred.  "
                "Namespace: %s  Class Name: %s",
            CSTRING(request->nameSpace.getString()),
            CSTRING(className.getString())));

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_CLASS, className.getString());
    }

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
        }

        CIMModifyInstanceRequestMessage* requestCallbackCopy =
            new CIMModifyInstanceRequestMessage(*requestCopy);

        _forwardRequestToProviderManager(
            providerInfo.className,
            providerInfo.serviceId,
            providerInfo.controlProviderName,
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

        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL3,
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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
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
    // Validate the class name and set the request propertry list
    //

    {
        CIMException checkClassException;

        CIMConstClass cimClass = _getClass(
            request->nameSpace,
            request->className,
            checkClassException);

        if (checkClassException.getCode() != CIM_ERR_SUCCESS)
        {
            CIMResponseMessage* response = request->buildResponse();
            response->cimException = checkClassException;

            _enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return;
        }

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,"PropertyList = %s",
            (const char*)
                _showPropertyList(request->propertyList).getCString()));

        // If DeepInheritance==false and no PropertyList was specified by the
        // client, the provider PropertyList should contain all the properties
        // in the specified class.
        if (!request->deepInheritance && request->propertyList.isNull())
        {
            Uint32 numProperties = cimClass.getPropertyCount();
            Array<String>  propertyListArray;
            for (Uint32 i = 0; i < numProperties; i++)
            {
                propertyListArray.append(
                    cimClass.getProperty(i).getName().getString());
            }
            request->propertyList.append(propertyListArray);
        }

    }

    //
    // Get names of descendent classes and list of providers
    // For Enumerate Operations, the list of providers is used for both
    // repository and provider requests.
    //

    ProviderInfoList providerInfos = _lookupAllInstanceProviders(
        request->nameSpace,
        request->className);

    Uint32 toIssueCount = providerInfos.size();

    _checkEnumerateTooBroad(
        request->nameSpace, request->className, providerInfos.providerCount);

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED

    if ((providerInfos.providerCount == 0) &&
        !(_repository->isDefaultInstanceProvider()))
    {
        PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL1,
            "CIM_ERROR_NOT_SUPPORTED for %s",
            (const char*)request->className.getString().getCString()));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    //
    // Get names of descendent classes:
    // We have instances for Providers and possibly repository.
    // Set up an aggregate object and save a copy of the original request.
    //
    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstancesRequestMessage(*request),
        request->getType(),
        request->messageId,
        request->queueIds.top(),
        request->className);

    // gather up the repository responses and send it to out as one response
    // with many instances
    if (_repository->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        Uint32 numClasses = providerInfos.size();
        for (Uint32 i = 0; i < numClasses; i++)
        {
            ProviderInfo& providerInfo = providerInfos[i];

            // this class is registered to a provider - skip
            if (providerInfo.hasProvider)
                continue;

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Routing EnumerateInstances request for class %s to the "
                    "repository.  Class # %u of %u, aggregation SN %u.",
                CSTRING(providerInfo.className.getString()),
                (unsigned int)(i + 1),
                (unsigned int)numClasses,
                (unsigned int)(poA->getAggregationSN())));

            AutoPtr<CIMEnumerateInstancesResponseMessage> response(
                dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
                    request->buildResponse()));

            CIMException cimException;
            Array<CIMInstance> cimNamedInstances;

            try
            {
                // Enumerate instances only for this class
                cimNamedInstances =
                    _repository->enumerateInstancesForClass(
                        request->nameSpace,
                        providerInfo.className,
                        request->includeQualifiers,
                        request->includeClassOrigin,
                        request->propertyList);

                response->getResponseData().setInstances(cimNamedInstances);
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
        } // for all classes and derived classes

        Uint32 numberResponses = poA->numberResponses();
        Uint32 totalIssued = providerInfos.providerCount
            + (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        if (numberResponses > 0)
        {
            handleEnumerateInstancesResponseAggregation(poA,true);

            CIMResponseMessage* response = poA->removeResponse(0);

            // Forward for completed response processing
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

    ProviderRequests::issueEnumerationRequests(
        this,
        getQueueId(),
        request,
        providerInfos,
        poA);
    /*************************************************
      Old code before creating the template functions.  Keep until
      we are satisifed the new works and is clean
    // Loop through providerInfos, forwarding requests to providers
    for (Uint32 i = 0; i < numClasses; i++)
    {
        ProviderInfo& providerInfo = providerInfos[i];

        // this class is NOT registered to a provider - skip
        if (! providerInfo.hasProvider)
            continue;

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "Routing EnumerateInstances request for class %s to "
                "service \"%s\" for control provider \"%s\".  "
                "Class # %u of %u, aggregation SN %u.",
            CSTRING(providerInfo.className.getString()),
            _getServiceName(providerInfo.serviceId),
            CSTRING(providerInfo.controlProviderName),
            (unsigned int)(i + 1),
            (unsigned int)(numClasses),
            (unsigned int)(poA->_aggregationSN)));

        CIMEnumerateInstancesRequestMessage* requestCopy =
            new CIMEnumerateInstancesRequestMessage(*request);

        requestCopy->className = providerInfo.className;

        CIMException checkClassException;
        CIMConstClass cimClass = _getClass(
            request->nameSpace,
            providerInfo.className,
            checkClassException);

        // ATTN: The following is not correct. Need better way to terminate.
        if (checkClassException.getCode() != CIM_ERR_SUCCESS)
        {
            CIMResponseMessage* response = request->buildResponse();
            // KS_TBD - Why do we not put exception into response here?

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
            _forwardRequestForAggregation(
                providerInfo.serviceId,
                providerInfo.controlProviderName,
                requestCopy,
                poA);
        }
    } // for all classes and dervied classes
    ***********************************************/

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

    {
        CIMException checkClassException;

        CIMConstClass cimClass = _getClass(
            request->nameSpace,
            request->className,
            checkClassException);

        if (checkClassException.getCode() != CIM_ERR_SUCCESS)
        {
            CIMResponseMessage* response = request->buildResponse();
            response->cimException = checkClassException;

            _enqueueResponse(request, response);

            PEG_METHOD_EXIT();
            return;
        }
    }

    //
    // Get names of descendent classes and list of providers
    //

    ProviderInfoList providerInfos = _lookupAllInstanceProviders(
        request->nameSpace,
        request->className);

    _checkEnumerateTooBroad(
        request->nameSpace, request->className, providerInfos.providerCount);

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED
    if ((providerInfos.providerCount == 0)
        && !(_repository->isDefaultInstanceProvider()))
    {
        PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL1,
            "CIM_ERROR_NOT_SUPPORTED for %s",
            (const char*)request->className.getString().getCString()));

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

    Uint32 numClasses = providerInfos.size();

    if (_repository->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        for (Uint32 i = 0; i < numClasses; i++)
        {
            ProviderInfo& providerInfo = providerInfos[i];

            // If class does not have provider call repository to
            // get instances
            if (!providerInfo.hasProvider)
            {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "Routing EnumerateInstanceNames request for class %s to "
                        "the repository.  Class # %u of %u, aggregation SN %u.",
                    CSTRING(providerInfo.className.getString()),
                (unsigned int)(i + 1),
                (unsigned int)numClasses,
                    (unsigned int)(poA->getAggregationSN())));

            AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response(
                dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
                    request->buildResponse()));

            try
            {
                    // Enumerate instance Names only for this class
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
            }
        } // for all classes and derived classes

        Uint32 numberResponses = poA->numberResponses();
        Uint32 totalIssued = providerInfos.providerCount
            + (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        if (numberResponses > 0)
        {
            handleEnumerateInstanceNamesResponseAggregation(poA);

            CIMResponseMessage* response = poA->removeResponse(0);

            // Forward for completed-response processing
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

    ProviderRequests::issueEnumerationRequests(
        this,
        getQueueId(),
        request,
        providerInfos,
        poA);

    /***************************************************************
    // Loop through providerInfos, forwarding requests to providers
    for (Uint32 i = 0; i < numClasses; i++)
    {
        ProviderInfo& providerInfo = providerInfos[i];

        // this class is NOT registered to a provider - skip
        if (! providerInfo.hasProvider)
            continue;

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "Routing EnumerateInstanceNames request for class %s to "
                "service \"%s\" for control provider \"%s\".  "
                "Class # %u of %u, aggregation SN %u.",
            CSTRING(providerInfo.className.getString()),
            _getServiceName(providerInfo.serviceId),
            CSTRING(providerInfo.controlProviderName,
            (unsigned int)(i + 1),
            (unsigned int)(numClasses),
            (unsigned int)(poA->_aggregationSN)));

        CIMEnumerateInstanceNamesRequestMessage* requestCopy =
            new CIMEnumerateInstanceNamesRequestMessage(*request);

        requestCopy->className = providerInfo.className;

        CIMException checkClassException;
        CIMConstClass cimClass = _getClass(
                request->nameSpace,
                providerInfo.className,
                checkClassException);

        // The following is not correct. Need better way to terminate.
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

        if (checkClassException.getCode() == CIM_ERR_SUCCESS)
        {
            _forwardRequestForAggregation(
                providerInfo.serviceId,
                providerInfo.controlProviderName,
                requestCopy,
                poA);
        }
        }
    } // for all classes and derived classes
************************************************/
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
//
//  if (!_enableAssociationTraversal)
//  {
//      CIMResponseMessage* response = request->buildResponse();
//      response->cimException =
//          PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "Associators");
//
//      _enqueueResponse(request, response);
//
//      PEG_METHOD_EXIT();
//      return;
//  }

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

    if (!_checkExistenceOfClass(
            request->nameSpace, request->objectName.getClassName()))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER, request->objectName.toString());
    }

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleAssociators - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0);

    if (isClassRequest)
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
        response->getResponseData().setObjects(cimObjects);

        _enqueueResponse(request, response.release());
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        // Determine list of providers for this request. Note that
        // providerCount may not be the same as the size of the returned
        // array because not all Class will have providers.
        //

        ProviderInfoList providerInfos =
            _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                String::EMPTY);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
            "providerCount = %u.", providerInfos.providerCount));

        //
        // Get the instances from the repository, as necessary
        //

        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        AutoPtr<CIMAssociatorsResponseMessage> response;

        if (_repository->isDefaultInstanceProvider())
        {
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

            response.reset(dynamic_cast<CIMAssociatorsResponseMessage*>(
                request->buildResponse()));
            response->getResponseData().setObjects(cimObjects);

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Associators repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    response->getResponseData().getObjects().size()));
        }

        // Issue Provider requests and process the respository responseData
        ProviderRequests::issueAssocRequests(
            this,
            getQueueId(),
            request,
            response,
            providerInfos);

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

    if (!_checkExistenceOfClass(
            request->nameSpace, request->objectName.getClassName()))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER, request->objectName.toString());
    }

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleAssociatorNames - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0);

    if (isClassRequest)
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
        response->getResponseData().setInstanceNames(objectNames);

        _enqueueResponse(request, response.release());
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        // Determine list of providers for this request
        //

        ProviderInfoList providerInfos =
            _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                String::EMPTY);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "providerCount = %u.", providerInfos.providerCount));

        //
        // Get the instances from the repository, as necessary
        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        //
        AutoPtr<CIMAssociatorNamesResponseMessage> response;

        if (_repository->isDefaultInstanceProvider())
        {
            Array<CIMObjectPath> objectNames = _repository->associatorNames(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                request->resultClass,
                request->role,
                request->resultRole);

            response.reset(dynamic_cast<CIMAssociatorNamesResponseMessage*>(
                request->buildResponse()));
            response->getResponseData().setInstanceNames(objectNames);

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "AssociatorNames repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    objectNames.size()));
        }

        // Issue Provider requests and process the respository responseData
        ProviderRequests::issueAssocRequests(
            this,
            getQueueId(),
            request,
            response,
            providerInfos);

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

    if (!_checkExistenceOfClass(
            request->nameSpace, request->objectName.getClassName()))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER, request->objectName.toString());
    }

    PEG_TRACE(( TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleReferences - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0);

    if (isClassRequest)
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
        response->getResponseData().setObjects(cimObjects);

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

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "providerCount = %u.", providerInfos.providerCount));

        //
        // Get the instances from the repository, as necessary
        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        //
        AutoPtr<CIMReferencesResponseMessage> response;

        if (_repository->isDefaultInstanceProvider())
        {
            Array<CIMObject> cimObjects = _repository->references(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                request->role,
                request->includeQualifiers,
                request->includeClassOrigin,
                request->propertyList);

            response.reset(dynamic_cast<CIMReferencesResponseMessage*>(
                request->buildResponse()));
            response->getResponseData().setObjects(cimObjects);

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "References repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    cimObjects.size()));
        }

        // Issue Provider requests and process the respository responseData
        ProviderRequests::issueAssocRequests(
            this,
            getQueueId(),
            request,
            response,
            providerInfos);

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

    if (!_checkExistenceOfClass(
            request->nameSpace, request->objectName.getClassName()))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER, request->objectName.toString());
    }

    PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleReferenceNames - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0);

    if (isClassRequest)
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

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "providerCount = %u.", providerInfos.providerCount));

        //
        // Get instances from the repository, as necessary
        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        //
        AutoPtr<CIMReferenceNamesResponseMessage> response;

        if (_repository->isDefaultInstanceProvider())
        {
            Array<CIMObjectPath> objectNames = _repository->referenceNames(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                request->role);

            response.reset(dynamic_cast<CIMReferenceNamesResponseMessage*>(
                request->buildResponse()));
            response->getResponseData().setInstanceNames(objectNames);

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "ReferenceNames repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    objectNames.size()));
        }

        // Issue Provider requests and process the respository responseData
        ProviderRequests::issueAssocRequests(
            this,
            getQueueId(),
            request,
            response,
            providerInfos);

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

    ProviderInfo providerInfo =
        _lookupNewInstanceProvider(
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

        _forwardRequestToProviderManager(
            providerInfo.className,
            providerInfo.serviceId,
            providerInfo.controlProviderName,
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

    ProviderInfo providerInfo =
        _lookupNewInstanceProvider(
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

        _forwardRequestToProviderManager(
            providerInfo.className,
            providerInfo.serviceId,
            providerInfo.controlProviderName,
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

        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL3,
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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
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
        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL1,
            "CIMOperationRequestDispatcher::handleInvokeMethodRequest - "
                "CIM class does not exist exception has occurred.  "
                "Namespace: %s  Class Name: %s",
            (const char*) request->nameSpace.getString().getCString(),
            (const char*) className.getString().getCString()));

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_FOUND, className.getString());
    }

    Uint32 serviceId;
    String controlProviderName;

    // Check for class provided by an internal provider
    if (_lookupInternalProvider(
            request->nameSpace, className, serviceId, controlProviderName))
    {
        CIMInvokeMethodRequestMessage* requestCopy =
            new CIMInvokeMethodRequestMessage(*request);

        CIMInvokeMethodRequestMessage* requestCallbackCopy =
            new CIMInvokeMethodRequestMessage(*requestCopy);

        _forwardRequestToProviderManager(
            className,
            serviceId,
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
            _providerManagerServiceId,
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

//KS_PULL_BEGIN
/*************************************************************************
//
//  Pull operations introduced in Pegasus 2.12 per DMTF DSP200 - V 1.4
//
*************************************************************************/

/**$**************************************************************************
**
**    handleOpenEnumerateInstancesRequest
**
*****************************************************************************/

void CIMOperationRequestDispatcher::handleOpenEnumerateInstancesRequest(
    CIMOpenEnumerateInstancesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleOpenEnumerateInstancesRequest");

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "OpenEnumerateInstances request for namespace %s class %s "
            "propertyList=%s "
            "maxObjectCount=%s "
            "operationTimeout=%s",
        (const char*)request->nameSpace.getString().getCString(),
        (const char*)request->className.getString().getCString(),
        (const char*)_toString(request->propertyList).getCString(),
        (const char*)request->maxObjectCount.toString().getCString(),
        (const char*)request->operationTimeout.toString().getCString() ));

    // get the class name or generate error if class not found for target
    // namespace.
    CIMName className = request->className;
    CIMConstClass cimClass;
    if (_rejectInvalidClassParameter(request, className, request->nameSpace,
                                     cimClass))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidFilterParameters(request, request->filterQueryLanguage,
                              request->filterQuery))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectIfContinueOnError(request, request->continueOnError))
    {
        PEG_METHOD_EXIT();
        return;
    }


    Uint32 operationMaxObjectCount;
    if (_rejectInvalidMaxObjectCountParam(request, request->maxObjectCount,
            false, operationMaxObjectCount, Uint32(0)))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Test for valid values in OperationTimeout
    if (_rejectInvalidOperationTimeout(request, request->operationTimeout))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // If DeepInheritance==false and no PropertyList was specified by the
    // client, the provider PropertyList should contain all the properties
    // in the specified class.
    if (!request->deepInheritance && request->propertyList.isNull())
    {
        request->propertyList.set(_buildPropertyList(cimClass));
    }

    //
    // Get names of descendent classes with indication of whether they
    // have providers. providerCount is count of number of entries that
    // actually have providers. Others are subclasses without providers.
    //
    ProviderInfoList providerInfos = _lookupAllInstanceProviders(
        request->nameSpace, request->className);

    Uint32 toIssueCount = providerInfos.size();

    _checkEnumerateTooBroad(
        request->nameSpace, request->className, providerInfos.providerCount);

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED.
    // KS_PULL_QUESTION The following say if no provider and no
    // repository return error. Aggregation(
    //            providerInfo.se
    //Should we not just return nothing.

    if (_checkNoProvidersOrRepository(request, providerInfos.providerCount,
                                      className))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Build a corresponding EnumerateInstancesRequest to send to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers.
    // NOTE: includeQualifiers NOT part of Pull operation

    CIMEnumerateInstancesRequestMessage* enumRequest =
        new CIMEnumerateInstancesRequestMessage(
            request->messageId,
            request->nameSpace,
            request->className,
            request->deepInheritance,
            false,
            request->includeClassOrigin,
            request->propertyList,
            request->queueIds,
            request->authType,
            request->userName);

    // AutoPtr to delete enumRequest at end of handler
    AutoPtr<CIMEnumerateInstancesRequestMessage> dummy(enumRequest);

    enumRequest->operationContext = request->operationContext;

    //
    // Setup the EnumerationContext. Returns pointer to object and context ID
    // string
    //

    String enContextIdStr;
    EnumerationContext* enumerationContext = enumerationTable.createContext(
        request->nameSpace,
        request->operationTimeout,
        request->continueOnError,
        CIM_PULL_INSTANCES_WITH_PATH_REQUEST_MESSAGE,
        CIMResponseData::RESP_INSTANCES,
        enContextIdStr);

    // KS_TEMP debugging code. Delete. OCT2011
    if (enumerationContext->responseCacheSize() != 0)
    {
        cout << "Error in responseCacheSize() " << enContextIdStr
             << endl;
        enumerationContext->valid();
        enumerationContext->trace();
        enumerationTable.trace();
    }
    PEGASUS_ASSERT(enumerationContext->responseCacheSize() == 0);  // KS_TEMP

    // Set up an aggregate object and save a copy of the original request.
    // NOTE: Build the poA for the EnumerateRequest, not the corresponding
    // pull operation.

    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstancesRequestMessage(*enumRequest),
        enumRequest->getType(),
        enumRequest->messageId,
        enumRequest->queueIds.top(),
        enumRequest->className);

    PEGASUS_ASSERT(poA->valid());   // KS_TEMP
    PEGASUS_ASSERT(enumerationContext->valid());  // EXP_PULL_TEMP

    //
    // Set Open... operation parameters into the operationAggregate
    //
    poA->setPullOperation((void *)enumerationContext,
        enContextIdStr, request->nameSpace);

    /// KS_TODO - Why this as a special.  Should be able to do this in more
    // protective way.
    poA->_nameSpace = request->nameSpace;

    Uint32 numClasses = providerInfos.size();
    Uint32 totalIssued = 0;
    //
    // If repository as instance provider is enabled, get instances
    // from the repository
    //
    PEGASUS_ASSERT(poA->valid());   // KS_TEMP
    if (_repository->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        for (Uint32 i = 0; i < numClasses; i++)
        {
            ProviderInfo& providerInfo = providerInfos[i];

            // this class is registered to a provider - skip
            if (providerInfo.hasProvider)
                continue;

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Routing EnumerateInstances request for class %s to the "
                    "repository.  Class # %u of %u, aggregation SN %u.",
                (const char*)providerInfo.className.getString().getCString(),
                (unsigned int)(i + 1),
                (unsigned int)numClasses,
                (unsigned int)(poA->_aggregationSN)));

            // Build enumerateInstances response to send to aggregator.
            AutoPtr<CIMEnumerateInstancesResponseMessage> repositoryResponse(
                dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
                    enumRequest->buildResponse()));

            CIMException cimException;
            Array<CIMInstance> cimNamedInstances;

            try
            {
                // Enumerate instances only for this class
                cimNamedInstances =
                    _repository->enumerateInstancesForClass(
                        request->nameSpace,
                        providerInfo.className,
                        false,  // no includequalifiers
                        request->includeClassOrigin,
                        request->propertyList);

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "RepositoryEnumerateInstances request for class %s to the "
                        "repository.  Class # %u of %u, returned  %u"
                        " cimNamedInstances",
                    (const char*)providerInfo.className.
                        getString().getCString(),
                    (unsigned int)(i + 1),
                    (unsigned int)numClasses,
                    (unsigned int)cimNamedInstances.size()));

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

            repositoryResponse->getResponseData().setInstances(
                cimNamedInstances);

            repositoryResponse->cimException = cimException;

            // append the response message with possible exception to response
            // KS_TODO - this should only occur if there is either
            // data in the response or an error.
            poA->appendResponse(repositoryResponse.release());
        } // for all classes and derived classes

        // KS_TODO - We issue this even if there are no instances in
        // the repository or exceptions from the Repository. Should have
        // short path and not issue this.

        Uint32 numberResponses = poA->numberResponses();
        totalIssued = providerInfos.providerCount +
            (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        PEGASUS_ASSERT(poA->valid());   // KS_TEMP
        // If responses generated, aggregate them, remove remove them from
        // aggregator forward them for Aggregation
        if (numberResponses > 0)
        {
            // Converts all responses to a single response message

            handleEnumerateInstancesResponseAggregation(poA, true);

            CIMResponseMessage* repositoryResponse = poA->removeResponse(0);

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Aggregate Respository Returns. #responses = %u. "
                       "totalIssued = %u",
                (unsigned int)poA->numberResponses(),
                (unsigned int)totalIssued));

            _forwardRequestForAggregation(
                getQueueId(),
                String(),
                new CIMEnumerateInstancesRequestMessage(*enumRequest),
                poA,
                repositoryResponse);
        }
    } // if isDefaultInstanceProvider

    else    // no repository as DefaultInstanceProvider
    {
        // Set the number of expected responses in the OperationAggregate
        totalIssued = providerInfos.providerCount;
        poA->setTotalIssued(totalIssued);
    }

    // Cannot use Operation Aggregate after this point since we are not
    // sure when it might be deleted unless the providerCount is != 0

    // Temp build empty  open response

    AutoPtr<CIMOpenEnumerateInstancesResponseMessage> openResponse(
        dynamic_cast<CIMOpenEnumerateInstancesResponseMessage*>(
            request->buildResponse()));
    //
    // Issue Enumeration requests to all providers.
    //

    // Issue to providers before we send open response to get
    // provider responses before we build response.  This required to allow
    // building initial response of max requested size
    if (providerInfos.providerCount == 0)
    {
        // if there were no repository responses, set complete now.
        if (totalIssued == 0)
        {
            enumerationContext->setProvidersComplete();
        }
    }
    else
    {
        ProviderRequests::issueEnumerationRequests(
            this,
            getQueueId(),
            enumRequest,
            providerInfos,
            poA);
    }

    openResponse->cimException = CIMException();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
               "Cache size after repository put %u maxObjectCount %u",
               enumerationContext->responseCacheSize(),
               operationMaxObjectCount ));

    // Create a temporary response data with correct type.
   CIMResponseData from(CIMResponseData::RESP_INSTANCES);

    // get response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete
    //CIMResponseData &
    Boolean rtn = enumerationContext->getCacheResponseData(
                                operationMaxObjectCount,
                                from);

    // if we had an error, put the exception into the response
    // KS-TODO - Should we return any objects if state is error.
    // Probably not.
    CIMResponseData & to = openResponse->getResponseData();

    if (enumerationContext->isErrorState())
    {
        openResponse->cimException = enumerationContext->_cimException;
    }
    else
    {
        to.appendResponseData(from);
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
      "OpenInstancesWithPath Send Open response to type %u"
          " from type %u eos %s providersComplete %s cacheSize %u",
      to.getResponseDataContent(), from.getResponseDataContent(),
               _toCharP(openResponse->endOfSequence),
               _toCharP(enumerationContext->ifProvidersComplete()),
               enumerationContext->responseCacheSize() ));

    // tests for providers complete and cache empty. and sets response
    // endOfSequence
    if ((openResponse->endOfSequence =
         enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close OpenInstancesWithPath Enumeration");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
    }
    else   // snf enumerationContext only if not endOfSequence
    {
        openResponse->enumerationContext = enContextIdStr;
    }

    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required for pull instances
    // because we are returning named instances.
    // KS_TODO _ This may be done as part of the aggregator also

    to.completeHostNameAndNamespace(cimAggregationLocalHost,
                                    request->nameSpace);

    _enqueueResponse(request, openResponse.release());

    if (enumerationContext->isClosed())
    {
        enumerationContext->removeContext();
    }

    PEG_METHOD_EXIT();
}

/**$**************************************************************************
**
**    handleOpenEnumerateInstancePathsRequest
**
*****************************************************************************/

void CIMOperationRequestDispatcher::handleOpenEnumerateInstancePathsRequest(
    CIMOpenEnumerateInstancePathsRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::"
            "handleOpenEnumerateInstancePathsRequest");

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "OpenEnumerateInstancePaths request for class %s "
            "maxObjectCount = \"%s\" .  "
            "operationTimeout = \"%s\" .  ",
        CSTRING(request->className.getString()),
        CSTRING(request->maxObjectCount.toString()),
        CSTRING(request->operationTimeout.toString())) );

    // get the class name
    CIMName className = request->className;
    CIMConstClass cimClass;
    if (_rejectInvalidClassParameter(request, className, request->nameSpace,
                                     cimClass))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidFilterParameters(request, request->filterQueryLanguage,
                              request->filterQuery))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectIfContinueOnError(request, request->continueOnError))
    {
        PEG_METHOD_EXIT();
        return;
    }

    Uint32 operationMaxObjectCount;
    if (_rejectInvalidMaxObjectCountParam(request, request->maxObjectCount,
            false, operationMaxObjectCount, Uint32(0)))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Test for valid values in OperationTimeout
    if (_rejectInvalidOperationTimeout(request, request->operationTimeout))
    {
        PEG_METHOD_EXIT();
        return;
    }

    //
    // Get names of descendent classes and list of providers
    // For Enumerate Operations, the list of providers is used for both
    // repository and provider requests.
    //

    ProviderInfoList providerInfos = _lookupAllInstanceProviders(
        request->nameSpace, request->className);

    Uint32 toIssueCount = providerInfos.size();

    _checkEnumerateTooBroad(
        request->nameSpace, request->className, providerInfos.providerCount);

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED.
    // KS_PULL_QUESTION The following say if no provider and no
    // repository return error.  Should we not just return
    // nothing.

    if (_checkNoProvidersOrRepository(request, providerInfos.providerCount,
                                      className))
    {
        PEG_METHOD_EXIT();
        return;
    }

   // Build a corresponding EnumerateInstancesNamesRequest to send to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers.
    // NOTE: includeQualifiers NOT part of Pull operation

    CIMEnumerateInstanceNamesRequestMessage* enumRequest =
        new CIMEnumerateInstanceNamesRequestMessage(
            request->messageId,
            request->nameSpace,
            request->className,
            request->queueIds,
            request->authType,
            request->userName);

    // AutoPtr to delete at end of handler
    AutoPtr<CIMEnumerateInstanceNamesRequestMessage> dummy(enumRequest);

    enumRequest->operationContext = request->operationContext;

    //
    // Setup the EnumerationContext. Returns pointer to object and context ID
    // string
    //

    String enContextIdStr;
    EnumerationContext* enumerationContext = enumerationTable.createContext(
        request->nameSpace,
        request->operationTimeout,
        request->continueOnError,
        CIM_PULL_INSTANCE_PATHS_REQUEST_MESSAGE,
        CIMResponseData::RESP_INSTNAMES,
        enContextIdStr);

    //
    // Set up op aggregate object and save a copy of the original request.
    // NOTE: Build the poA for the EnumerateRequest, not the corresponding
    // pull operation.
    //

    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstanceNamesRequestMessage(*enumRequest),
        enumRequest->getType(),
        enumRequest->messageId,
        enumRequest->queueIds.top(),
        enumRequest->className);

    PEGASUS_ASSERT(enumerationContext->valid());  // EXP_PULL_TEMP

    //
    // Set Open... operation parameters into the operationAggregate
    // Includes setting namespace that is used to complete host and naespace
    // in responses
    //
    poA->setPullOperation((void *)enumerationContext,
        enContextIdStr, request->nameSpace);

    Uint32 numClasses = providerInfos.size();
    Uint32 totalIssued = 0;

    PEGASUS_ASSERT(enumerationContext->valid());  // KS_TEMP

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

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Routing OpenEnumerateInstancePaths request for"
                    " class %s to the "
                    "repository.  Class # %u of %u, aggregation SN %u.",
                (const char*)providerInfo.className.getString().getCString(),
                (unsigned int)(i + 1),
                (unsigned int)numClasses,
                (unsigned int)(poA->_aggregationSN)));

            // Build enumerateInstanceNames response to send to aggregator.
            AutoPtr<CIMEnumerateInstanceNamesResponseMessage>
                repositoryResponse(
                    dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
                        enumRequest->buildResponse()));

            try
            {
                // Enumerate instance names only for this class
               const Array<CIMObjectPath>& cops =
                    _repository->enumerateInstanceNamesForClass(
                        request->nameSpace,
                        providerInfo.className);

               repositoryResponse->getResponseData().setInstanceNames(cops);
            }
            catch (const CIMException& exception)
            {
                repositoryResponse->cimException = exception;
            }
            catch (const Exception& exception)
            {
                repositoryResponse->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_FAILED,
                    exception.getMessage());
            }
            catch (...)
            {
                repositoryResponse->cimException =
                     PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
            }

            poA->appendResponse(repositoryResponse.release());
        } // for all classes and derived classes

        // Set the total issued into operation aggregation based on
        // whether we have issued responses for repository.
        Uint32 numberResponses = poA->numberResponses();
        totalIssued = providerInfos.providerCount +
            (numberResponses > 0 ? 1 : 0);
        poA->setTotalIssued(totalIssued);

        if (numberResponses > 0)
        {
            // KS_PULL_TBD Think that this may be worthless since I believe it
            // is duplicated in the forward.
            handleEnumerateInstanceNamesResponseAggregation(poA);

            CIMResponseMessage* localResponse = poA->removeResponse(0);

            _forwardRequestForAggregation(
                getQueueId(),
                String(),
                new CIMEnumerateInstanceNamesRequestMessage(*enumRequest),
                poA,
                localResponse);
        }
    } // if isDefaultInstanceProvider

    else    // repository is not default provider
    {
        // Set the number of expected responses in the OperationAggregate
        totalIssued = providerInfos.providerCount;
        poA->setTotalIssued(totalIssued);
    }

    // Temp build empty response to the open request
    AutoPtr<CIMOpenEnumerateInstancePathsResponseMessage> openResponse(
        dynamic_cast<CIMOpenEnumerateInstancePathsResponseMessage*>(
            request->buildResponse()));

    openResponse->enumerationContext = enContextIdStr;

    // Issue to providers before we send open response to get
    // provider responses before we build response.  This required to allow
    // building initial response of max requested size
    if (providerInfos.providerCount == 0)
    {
        // if there were no repository responses, set complete now.
        if (totalIssued == 0)
        {
                enumerationContext->setProvidersComplete();
        }
    }
    else
    {
        ProviderRequests::issueEnumerationRequests(
            this,
            getQueueId(),
            enumRequest,
            providerInfos,
            poA);
    }

    // Get cache data and issue open response

    openResponse->cimException = CIMException();

    CIMResponseData & to = openResponse->getResponseData();

    // Create a Response data based on what is in the cache now.
    // Create a temporary response data with correct type.
    // KS_TODO - Should be able to allocate this object in the
    // getCacheResponseData function.
    CIMResponseData from(CIMResponseData::RESP_INSTNAMES);

    // get response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object.
    //CIMResponseData &
    Boolean rtn = enumerationContext->getCacheResponseData(
                                operationMaxObjectCount,
                                from);
    if (enumerationContext->isErrorState())
    {
        openResponse->cimException = enumerationContext->_cimException;
    }
    else
    {
        to.appendResponseData(from);
    }

    if ((openResponse->endOfSequence =
         enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close EnumerateNames");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
               "OpenResponse end-of-sequence %s",
               _toCharP(openResponse->endOfSequence)));

    _enqueueResponse(request, openResponse.release());

    if (enumerationContext->isClosed())
    {
        enumerationContext->removeContext();
    }

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleOpenReferenceInstancesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleOpenReferenceInstancesRequest(
    CIMOpenReferenceInstancesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleOpenReferenceInstancesRequest");

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "OpenReferenceInstances request for object %s "
            "maxObjectCount = \"%s\" .  "
            "operationTimeout = \"%s\" .  "
            "resultClass = %s "
            "role = %s "
            "propertyList Size = %u ",
        (const char*)request->objectName.toString().getCString(),
        (const char*)request->maxObjectCount.toString().getCString(),
        (const char*)request->operationTimeout.toString().getCString(),
        (const char*)request->resultClass.getString().getCString(),
        (const char*)request->role.getCString(),
        request->propertyList.size()
        ));

    if (_rejectAssociationTraversalDisabled(request,"OpenReferenceInstances"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidRoleParameter(request, request->role, "role"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidObjectPathParameter(request, request->objectName))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidFilterParameters(request, request->filterQueryLanguage,
                              request->filterQuery))
    {
        PEG_METHOD_EXIT();
        return;
    }
    if (_rejectIfContinueOnError(request, request->continueOnError))
    {
        PEG_METHOD_EXIT();
        return;
    }

    Uint32 operationMaxObjectCount;
    if (_rejectInvalidMaxObjectCountParam(request, request->maxObjectCount,
            false, operationMaxObjectCount, Uint32(0)))
    {
        PEG_METHOD_EXIT();
        return;
    }
    if (_rejectInvalidOperationTimeout(request, request->operationTimeout))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // KS_TODO - Should we use checkClassParameter here in place of
    // _checkExistence...

    if (!_checkExistenceOfClass(
            request->nameSpace, request->objectName.getClassName()))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER,
            request->objectName.getClassName().getString());
    }

    ProviderInfoList providerInfos = _lookupAllAssociationProviders(
        request->nameSpace,
        request->objectName,
        request->resultClass,
        String::EMPTY);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_TEMP
        "OpenReferenceInstances "
        "providerCount = %u.", providerInfos.providerCount));

    //
    // Request the instances from the repository, as necessary.
    // Instances go directly into a response because the are returned as
    // a single array.
    //
    Array<CIMObject> cimObjects;
    if (_repository->isDefaultInstanceProvider())
    {
        cimObjects = _repository->references(
            request->nameSpace,
            request->objectName,
            request->resultClass,
            request->role,
            false,                       // force includeQualifiers to false
            request->includeClassOrigin,
            request->propertyList);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "OpenReferenceInstances repository access: class = %s, count = %u.",
                (const char*)request->objectName.toString().getCString(),
                cimObjects.size()));
        // KS_TODO - Why not just put this directly to the enumcache.
        // If there was an exception is is caught outside of this and
        // not sure any of the other checking on response really gains
        // anything.
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenReferenceInstances 8. ProviderCount = %u",
        providerInfos.providerCount));

    // Build Open Response message.
    AutoPtr<CIMOpenReferenceInstancesResponseMessage> openResponse;
    openResponse.reset(dynamic_cast<CIMOpenReferenceInstancesResponseMessage*>(
        request->buildResponse()));

    // if there are no providers and nothing from repository
    // return not supported exception.
    // KS_TODO We should be able to imply drop through to normal code
    // and drop this completely by cleaning up the exception setting.

    if (providerInfos.providerCount == 0 && cimObjects.size() == 0)
    {
        // We have no providers to call.  Just return what we have.
        //

        openResponse->getResponseData().setObjects(cimObjects);
        // Do not need the following. Means nothing here.
//      if (!response.get())
//      {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "CIM_ERR_NOT_SUPPORTED for %s",
                (const char *)request->className.getString().getCString()));

            openResponse->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                String::EMPTY);
//      }
        openResponse->endOfSequence = true;
        _enqueueResponse(request, openResponse.release());

        PEG_METHOD_EXIT();
        return;
    }
    //
    // Process as multi operation enumeration. 1. Build context, build
    // enumRequest for providers and issue to providers, build open
    // response, get current objects (with wait) and enqueue open
    // response.
    //

    // Create new context object.
    String enContextIdStr;
    EnumerationContext* enumerationContext = enumerationTable.createContext(
        request->nameSpace,
        request->operationTimeout,
        request->continueOnError,
        CIM_PULL_INSTANCES_WITH_PATH_REQUEST_MESSAGE,
        CIMResponseData::RESP_OBJECTS,
        enContextIdStr);

    // Build corresponding CIMReferencesRequestMessage to issue to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers. NOTE: includeQualifiers NOT part of Pull operation

    CIMReferencesRequestMessage* enumRequest = new CIMReferencesRequestMessage(
        request->messageId,
        request->nameSpace,
        request->objectName,
        request->resultClass,
        request->role,
        false,
        request->includeClassOrigin,
        request->propertyList,
        request->queueIds,
        request->authType,
        request->userName);

    // AutoPtr to delete at end of handler
    AutoPtr<CIMReferencesRequestMessage> dummy(enumRequest);

    enumRequest->operationContext = request->operationContext;

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
    "OpenReferenceInstances 10. ProviderCount = %u",
    providerInfos.providerCount));

    //
    // Set up an aggregate object and save the original request message
    //

    OperationAggregate* poA = new OperationAggregate(
        new CIMReferencesRequestMessage(*enumRequest),
        enumRequest->getType(),
        enumRequest->messageId,
        enumRequest->queueIds.top(),
        enumRequest->objectName.getClassName(),
        enumRequest->nameSpace);

    //
    // Set Open... operation parameters into the operationAggregate
    //
    poA->setPullOperation((void *)enumerationContext,
        enContextIdStr, request->nameSpace);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
               "OpenReferenceInstances 10a. ProviderCount = %u "
               "repository count = %u",
               providerInfos.providerCount, cimObjects.size()));

    // Send repository response for aggregation
    // Temp hack because resequencing a single object causes problems
    //if (enumResponse.get() != 0)
    Uint32 totalIssued = 0;
////////  KS_TODO   if (cimObjects.size() != 0 || cimObjects.size() == 0)
    if (cimObjects.size() != 0)
    {
        AutoPtr<CIMReferencesResponseMessage> enumResponse;
        enumResponse.reset(dynamic_cast<CIMReferencesResponseMessage*>(
            enumRequest->buildResponse()));

        enumResponse->getResponseData().setObjects(cimObjects);

        totalIssued = providerInfos.providerCount+1;
        poA->setTotalIssued(totalIssued);
        // send the repository's results for aggregation
        // directly to callback (includes response).
        _forwardRequestForAggregation(
            getQueueId(),
            String(),
            new CIMReferencesRequestMessage(*enumRequest),
            poA,
            enumResponse.release());

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenReferenceInstances 8. ProviderCount = %u",
        providerInfos.providerCount));
    }
    else
    {
        totalIssued = providerInfos.providerCount;
        poA->setTotalIssued(totalIssued);
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
    "OpenReferenceInstances 9. ProviderCount = %u",
    providerInfos.providerCount));

    // Issue requests to all providers defined.
    for (Uint32 i = 0; i < providerInfos.size(); i++)
    {
        if (providerInfos[i].hasProvider)
        {
            CIMReferencesRequestMessage* requestCopy =
                new CIMReferencesRequestMessage(*enumRequest);
            // Insert the association class name to limit the provider
            // to this class.
            requestCopy->resultClass = providerInfos[i].className;

            if (providerInfos[i].providerIdContainer.get() != 0)
                requestCopy->operationContext.insert(
                    *(providerInfos[i].providerIdContainer.get()));

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Forwarding to provider for class %s",
                (const char*)
                    providerInfos[i].className.getString().getCString()));

            _forwardRequestForAggregation(providerInfos[i].serviceId,
                providerInfos[i].controlProviderName, requestCopy, poA);
            // Note: poA must not be referenced after last "forwardRequest"
        }
    }

    //
    // Complete and enqueue open response.
    //

    openResponse->enumerationContext = enContextIdStr;

    // If no providers indicate that providers are complete
    if (providerInfos.providerCount == 0)
    {
        // if there were no repository responses, set complete now.
        if (totalIssued == 0)
        {
                enumerationContext->setProvidersComplete();
        }
    }

    openResponse->cimException = CIMException();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
               "OpenReferenceInstances Cache size after repository put %u"
               " maxObjectCount %u",
               enumerationContext->responseCacheSize(),
               operationMaxObjectCount ));

    // Create a Response data based on what is in the cache now.
    // Create a temporary response data with correct type.

    CIMResponseData from(enumerationContext->getCIMResponseDataType());

    // get response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete
    //CIMResponseData &
    Boolean rtn = enumerationContext->getCacheResponseData(
                                operationMaxObjectCount,
                                from);

    CIMResponseData & to = openResponse->getResponseData();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
      "OpenReferences AppendResponseData. to type %u from type %u",
      to.getResponseDataContent(), from.getResponseDataContent()));

    // if we had an error, put the exception into the response
    // KS-TODO - Should we return any objects if state is error.  Probably
    // not.
    if (enumerationContext->isErrorState())
    {
        openResponse->cimException = enumerationContext->_cimException;
    }
    else
    {
        to.appendResponseData(from);
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
      "OpenReferenceInstancesWithPath Send Pull response to type %u"
          " from type %u eos %s providersComplete %s cacheSize %u",
      to.getResponseDataContent(), from.getResponseDataContent(),
               _toCharP(openResponse->endOfSequence),
               _toCharP(enumerationContext->ifProvidersComplete()),
               enumerationContext->responseCacheSize() ));

    // Do check here after we have processed the results of the get.
    // At this point we are current with the provider response status
    if ((openResponse->endOfSequence =
         enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close References");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
    }

    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required for pull instances
    // because we are returning named instances.
    // KS_TBD _ This may be done as part of the aggregator also

    to.completeHostNameAndNamespace(
        cimAggregationLocalHost, request->nameSpace);

    _enqueueResponse(request, openResponse.release());

    if (enumerationContext->isClosed())
    {
        enumerationContext->removeContext();
    }

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleOpenReferenceInstancePathsRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleOpenReferenceInstancePathsRequest(
    CIMOpenReferenceInstancePathsRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleOpenReferenceInstancePathsRequest");

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "OpenReferenceInstancePaths request for objectName %s "
            "maxObjectCount = \"%s\" .  "
            "operationTimeout = \"%s\" .  "
            "resultClass = %s "
            "role = %s ",
        (const char*)request->objectName.toString().getCString(),
        (const char*)request->maxObjectCount.toString().getCString(),
        (const char*)request->operationTimeout.toString().getCString(),
        (const char*)request->resultClass.getString().getCString(),
        (const char*)request->role.getCString()
        ));

    if (_rejectAssociationTraversalDisabled(request,
        "OpenReferenceInstancePaths"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidRoleParameter(request, request->role, "role"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidObjectPathParameter(request, request->objectName))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidFilterParameters(request, request->filterQueryLanguage,
                              request->filterQuery))
    {
        PEG_METHOD_EXIT();
        return;
    }
    if (_rejectIfContinueOnError(request, request->continueOnError))
    {
        PEG_METHOD_EXIT();
        return;
    }

    Uint32 operationMaxObjectCount;
    if (_rejectInvalidMaxObjectCountParam(request, request->maxObjectCount,
            false, operationMaxObjectCount, Uint32(0)))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidOperationTimeout(request, request->operationTimeout))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (!_checkExistenceOfClass(request->nameSpace,
                                request->objectName.getClassName()))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER,
            request->objectName.getClassName().getString());
    }

    //
    // Get results from providers and the repository
    // Determine list of providers for this request
    //

    ProviderInfoList providerInfos = _lookupAllAssociationProviders(
            request->nameSpace,
            request->objectName,
            request->resultClass,
            String::EMPTY);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_TEMP
        "OpenReferenceInstancePaths "
        "providerCount = %u.", providerInfos.providerCount));
    //
    // Get the instances from the repository, as necessary
    //
    Array<CIMObjectPath> instanceNames;
    if (_repository->isDefaultInstanceProvider())
    {
        instanceNames = _repository->referenceNames(
            request->nameSpace,
            request->objectName,
            request->resultClass,
            request->role);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "OpenReferenceInstamcePaths repository access:"
                   " class = %s, count = %u.",
            (const char*)request->objectName.toString().getCString(),
            instanceNames.size()));
    }

    if (providerInfos.providerCount == 0 && instanceNames.size() == 0)
    {
        //
        // No provider is registered and the repository isn't the
        // default.  Return CIM_ERR_NOT_SUPPORTED.
        AutoPtr<CIMOpenReferenceInstancePathsResponseMessage> response;
        response.reset(
            dynamic_cast<CIMOpenReferenceInstancePathsResponseMessage*>(
                request->buildResponse()));

        response->getResponseData().setInstanceNames(instanceNames);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "CIM_ERR_NOT_SUPPORTED for %s",
            (const char *)request->className.getString().getCString()));

        // KS_PULL_TBD Should be be returning an exception here.
        // Spec issue. Should this be simply empty response
        // KS_PULL_TBD Should not have to do this.  The enum context
        // should be ignored if endOfSequence is true.
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
            "OpenReferenceInstancePaths Rtn Empty."));

        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED,
            String::EMPTY);

        response->endOfSequence = true;
        _enqueueResponse(request, response.release());

        PEG_METHOD_EXIT();
        return;
    }

    //
    // Create new enumerationContext and enumerationContextString.
    //
    String enContextIdStr;
    // Create new context object. Returns pointer to object and context ID
    // string
    EnumerationContext* enumerationContext = enumerationTable.createContext(
        request->nameSpace,
        request->operationTimeout,
        request->continueOnError,
        CIM_PULL_INSTANCE_PATHS_REQUEST_MESSAGE,
        CIMResponseData::RESP_OBJECTPATHS,
        enContextIdStr);

    // Build corresponding EnumerateInstancePathsRequest to send to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers. NOTE: includeQualifiers NOT part of Pull operation

    CIMReferenceNamesRequestMessage* enumRequest =
        new CIMReferenceNamesRequestMessage(
            request->messageId,
            request->nameSpace,
            request->objectName,
            request->resultClass,
            request->role,
            request->queueIds,
            request->authType,
            request->userName);

    // AutoPtr to delete at end of handler
    AutoPtr<CIMReferenceNamesRequestMessage> dummy(enumRequest);

    enumRequest->operationContext = request->operationContext;

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenReferenceInstancePaths 7. ProviderCount = %u",
        providerInfos.providerCount));
    //
    // Set up an aggregate object and save the created enumRequest
    //

    OperationAggregate* poA = new OperationAggregate(
        new CIMReferenceNamesRequestMessage(*enumRequest),
        enumRequest->getType(),
        enumRequest->messageId,
        enumRequest->queueIds.top(),
        enumRequest->objectName.getClassName(),
        enumRequest->nameSpace);

    (enumerationContext->valid());  // EXP_PULL_TEMP

    //
    // Set Open... operation parameters into the operationAggregate
    //
    poA->setPullOperation((void *)enumerationContext,
        enContextIdStr, request->nameSpace);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
    "OpenReferenceInstancePaths 7a. ProviderCount = %u repository count = %u",
    providerInfos.providerCount, instanceNames.size()));

    // If any return from repository, send it to aggregator.
    Uint32 totalIssued = 0;

/////////////////// KS_TODO   if (instanceNames.size() != 0 ||
// instanceNames.size() == 0)
    if (instanceNames.size() != 0)
    {
        AutoPtr<CIMReferenceNamesResponseMessage> enumResponse;
        enumResponse.reset(dynamic_cast<CIMReferenceNamesResponseMessage*>(
            enumRequest->buildResponse()));
        enumResponse->getResponseData().setInstanceNames(instanceNames);

        // KS_TODO - This one worthless if instanceNames.size != 0
        if (enumResponse.get() != 0)
        {
            totalIssued = providerInfos.providerCount+1;
            poA->setTotalIssued(totalIssued);
            // send the repository's results for aggregation
            // directly to callback (includes response).
            _forwardRequestForAggregation(
                getQueueId(),
                String(),
                new CIMReferenceNamesRequestMessage(*enumRequest),
                poA,
                enumResponse.release());

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
            "OpenReferenceInstancePaths 8. ProviderCount = %u",
            providerInfos.providerCount));
        }
        else
        {
            totalIssued = providerInfos.providerCount;
            poA->setTotalIssued(totalIssued);
        }
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
    "OpenReferenceInstancePaths 9. ProviderCount = %u",
    providerInfos.providerCount));

    // Call all providers
    for (Uint32 i = 0; i < providerInfos.size(); i++)
    {
        if (providerInfos[i].hasProvider)
        {
            CIMReferenceNamesRequestMessage* enumRequestCopy =
                new CIMReferenceNamesRequestMessage(*enumRequest);
            // Insert the association class name to limit the provider
            // to this class.
            enumRequestCopy->resultClass = providerInfos[i].className;

            if (providerInfos[i].providerIdContainer.get() != 0)
                enumRequestCopy->operationContext.insert(
                    *(providerInfos[i].providerIdContainer.get()));

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Forwarding to provider for class %s",
                (const char *)providerInfos[i].
                       className.getString().getCString()));

            _forwardRequestForAggregation(providerInfos[i].serviceId,
                providerInfos[i].controlProviderName, enumRequestCopy, poA);
            // Note: poA must not be referenced after last "forwardRequest"
        }
    }

    // Build get from the cache and return any response
    AutoPtr<CIMOpenReferenceInstancePathsResponseMessage> openResponse(
        dynamic_cast<CIMOpenReferenceInstancePathsResponseMessage*>(
            request->buildResponse()));

    openResponse->enumerationContext = enContextIdStr;

    if (providerInfos.providerCount == 0)
    {
        // if there were no repository responses, set complete now.
        if (totalIssued == 0)
        {
                enumerationContext->setProvidersComplete();
        }
    }

    openResponse->cimException = CIMException();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
               "Cache size after repository put %u maxObjectCount %u",
               enumerationContext->responseCacheSize(),
               operationMaxObjectCount ));

    // Create a Response data based on what is in the cache now.
    // Create a temporary response data with correct type.

    CIMResponseData from(CIMResponseData::RESP_OBJECTPATHS);

    // get response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete
    Boolean rtn = enumerationContext->getCacheResponseData(
                                operationMaxObjectCount,
                                from);

    CIMResponseData & to = openResponse->getResponseData();

    // if we had an error, put the exception into the response
    // KS-TODO - Should we return any objects if state is error.  Probably
    // not.
    if (enumerationContext->isErrorState())
    {
        openResponse->cimException = enumerationContext->_cimException;
    }
    else
    {
        to.appendResponseData(from);
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
      "PullInstancePaths Send Pull response to type %u"
          " from type %u eos %s providersComplete %s cacheSize %u",
      to.getResponseDataContent(), from.getResponseDataContent(),
               _toCharP(openResponse->endOfSequence),
               _toCharP(enumerationContext->ifProvidersComplete()),
               enumerationContext->responseCacheSize() ));

   if ((openResponse->endOfSequence =
        enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close ReferencePaths");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
    }

    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required for pull instances
    // because we are returning named instances.
    // KS_TBD _ This may be done as part of the aggregator also

    to.completeHostNameAndNamespace(
        cimAggregationLocalHost, request->nameSpace);

    _enqueueResponse(request, openResponse.release());

    if (enumerationContext->isClosed())
    {
        enumerationContext->removeContext();
    }

    PEG_METHOD_EXIT();
}

/**$********************************************************
    handlePullInstancesWithPath
************************************************************/

void CIMOperationRequestDispatcher::handlePullInstancesWithPath(
    CIMPullInstancesWithPathRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handlePullInstancesWithPath");

    // Both pull operations execute off of a single templated
    // function.
    // Build and send response.  getCache used to wait for objects
    AutoPtr<CIMPullInstancesWithPathResponseMessage> response(
        dynamic_cast<CIMPullInstancesWithPathResponseMessage*>(
            request->buildResponse()));

    ProviderRequests::issuePullResponse(this, request, response,
                                        "PullInstancesWithPath");
    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleOpenAssociatorInstancesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleOpenAssociatorInstancesRequest(
    CIMOpenAssociatorInstancesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleOpenAssociatorInstancesRequest");

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "OpenAssociatorInstances request for object %s "
            "maxObjectCount = \"%s\".  "
            "operationTimeout = \"%s\".  "
            "assocClass = \"%s\".  "
            "resultClass = \"%s\".  "
            "role = \"%s\".  "
            "resultRole = \"%s\".  "
            "propertyList Size = %u",
        (const char*)request->objectName.toString().getCString(),
        (const char*)request->maxObjectCount.toString().getCString(),
        (const char*)request->operationTimeout.toString().getCString(),
        (const char*)request->assocClass.getString().getCString(),
        (const char*)request->resultClass.getString().getCString(),
        (const char*)request->role.getCString(),
        (const char*)request->resultRole.getCString(),
        request->propertyList.size()
        ));

    if (_rejectAssociationTraversalDisabled(request, "ReferenceNames"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidRoleParameter(request, request->role, "role"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidRoleParameter(request, request->resultRole, "resultRole"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidObjectPathParameter(request, request->objectName))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidFilterParameters(request, request->filterQueryLanguage,
                              request->filterQuery))
    {
        PEG_METHOD_EXIT();
        return;
    }
    if (_rejectIfContinueOnError(request, request->continueOnError))
    {
        PEG_METHOD_EXIT();
        return;
    }

    Uint32 operationMaxObjectCount;
    if (_rejectInvalidMaxObjectCountParam(request, request->maxObjectCount,
            false, operationMaxObjectCount, Uint32(0)))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidOperationTimeout(request, request->operationTimeout))
    {
        PEG_METHOD_EXIT();
        return;
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenAssociatorInstances 3. ProviderCount"));

    if (!_checkExistenceOfClass(
            request->nameSpace, request->objectName.getClassName()))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER, request->objectName.toString());
    }

    ProviderInfoList providerInfos = _lookupAllAssociationProviders(
            request->nameSpace,
            request->objectName,
            request->assocClass,
            String::EMPTY);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenAssociatorInstances 5. ProviderCount. %u",
         providerInfos.providerCount ));
    //
    // Get the instances from the repository, as necessary
    //
    Array<CIMObject> cimObjects;
    if (_repository->isDefaultInstanceProvider())
    {
        cimObjects = _repository->associators(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                request->resultClass,
                request->role,
                request->resultRole,
                false,                      // force includeQualifiers to false
                request->includeClassOrigin,
                request->propertyList);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "OpenAssociator repository access: class = %s, count = %u.",
                (const char*)request->objectName.toString().getCString(),
                cimObjects.size()));
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenAssociatorInstances 8. ProviderCount = %u",
        providerInfos.providerCount));

    // Build OpenResponse Message
    AutoPtr<CIMOpenAssociatorInstancesResponseMessage> openResponse;
    openResponse.reset(dynamic_cast<CIMOpenAssociatorInstancesResponseMessage*>(
        request->buildResponse()));

    // if there are no providers and nothing from repository
    // return not supported exception with endOfSequence true

    if (providerInfos.providerCount == 0 && cimObjects.size() == 0)
    {
        //
        // We have no providers to call.  Just return what we have.
        //
        openResponse->getResponseData().setObjects(cimObjects);
//      if (!response.get())
//      {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "CIM_ERR_NOT_SUPPORTED for %s",
                (const char *)request->className.getString().getCString()));

            openResponse->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                String::EMPTY);
//      }
        openResponse->endOfSequence = true;
        _enqueueResponse(request, openResponse.release());

        PEG_METHOD_EXIT();
        return;
    }

    //
    // Process as multi operation enumeration. 1. Build context, build
    // enumRequest for providers and issue to providers, build open
    // response, get current objects (with wait) and enqueue open
    // response.
    //

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenAssociatorInstances 7. ProviderCount = %u",
        providerInfos.providerCount));

    // Create a new enumeration context
    String enContextIdStr;
    EnumerationContext* enumerationContext = enumerationTable.createContext(
        request->nameSpace,
        request->operationTimeout,
        request->continueOnError,
        CIM_PULL_INSTANCES_WITH_PATH_REQUEST_MESSAGE,
        CIMResponseData::RESP_OBJECTS,
        enContextIdStr);

    // Build corresponding CIMReferencesRequestMessage to issue to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers. NOTE: includeQualifiers NOT part of Pull operation

    CIMAssociatorsRequestMessage* enumRequest =
        new CIMAssociatorsRequestMessage(
            request->messageId,
            request->nameSpace,
            request->objectName,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole,
            false,
            request->includeClassOrigin,
            request->propertyList,
            request->queueIds,
            request->authType,
            request->userName);

    // AutoPtr to delete at end of handler
    AutoPtr<CIMAssociatorsRequestMessage> dummy(enumRequest);

    enumRequest->operationContext = request->operationContext;
    //
    // Set up an aggregate object and save the original request message
    //

    OperationAggregate* poA = new OperationAggregate(
        new CIMAssociatorsRequestMessage(*enumRequest),
        enumRequest->getType(),
        enumRequest->messageId,
        enumRequest->queueIds.top(),
        enumRequest->objectName.getClassName(),
        enumRequest->nameSpace);

    //
    // Set Open... operation parameters into the operationAggregate
    //
    poA->setPullOperation((void *)enumerationContext,
        enContextIdStr, request->nameSpace);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
               "OpenAssociatorInstances 10a. ProviderCount = %u "
               "repository count = %u",
               providerInfos.providerCount, cimObjects.size()));

    // Send repository response for aggregation
    // Temp hack because resequencing a single object causes problems
    //if (enumResponse.get() != 0)

///// KS_TODO Why    if (cimObjects.size() != 0 || cimObjects.size() == 0)
    if (cimObjects.size() != 0)
    {
        AutoPtr<CIMAssociatorsResponseMessage> enumResponse;
        enumResponse.reset(dynamic_cast<CIMAssociatorsResponseMessage*>(
            enumRequest->buildResponse()));

        enumResponse->getResponseData().setObjects(cimObjects);

        poA->setTotalIssued(providerInfos.providerCount+1);
        // send the repository's results for aggregation
        // directly to callback (includes response).
        _forwardRequestForAggregation(
            getQueueId(),
            String(),
            new CIMAssociatorsRequestMessage(*enumRequest),
            poA,
            enumResponse.release());

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenAssociatorInstances reposiotry _forwardForAggregation."
        " ProviderCount= %u objectCount= %u",
        providerInfos.providerCount, cimObjects.size() ));
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
                new CIMAssociatorsRequestMessage(*enumRequest);
            // Insert the association class name to limit the provider
            // to this class.
            requestCopy->assocClass = providerInfos[i].className;

            if (providerInfos[i].providerIdContainer.get() != 0)
                requestCopy->operationContext.insert(
                    *(providerInfos[i].providerIdContainer.get()));

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Forwarding to provider for class %s",
                (const char *)
                       providerInfos[i].className.getString().getCString()));

            _forwardRequestForAggregation(providerInfos[i].serviceId,
                providerInfos[i].controlProviderName, requestCopy, poA);
            // Note: poA must not be referenced after last "forwardRequest"
        }
    }

    //
    // Complete and enqueue open response.
    //

    openResponse->enumerationContext = enContextIdStr;

    if (providerInfos.providerCount == 0)
    {
        enumerationContext->setProvidersComplete();
    }

    openResponse->cimException = CIMException();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
               "OpenAssociatorInstances Cache size after repository put %u"
               " maxObjectCount %u",
               enumerationContext->responseCacheSize(),
               operationMaxObjectCount ));

    // Create a Response data based on what is in the cache now.
    // Create a temporary response data with correct type.

    CIMResponseData from(enumerationContext->getCIMResponseDataType());

    // get response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete

    Boolean rtn = enumerationContext->getCacheResponseData(
                                operationMaxObjectCount, from);

    CIMResponseData & to = openResponse->getResponseData();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
      "OpenAssociatorInstances AppendResponseData. to type %u from type %u",
      to.getResponseDataContent(), from.getResponseDataContent()));

    // if we had an error, put the exception into the response
    // KS-TODO - Should we return any objects if state is error.  Probably
    // not.
    if (enumerationContext->isErrorState())
    {
        openResponse->cimException = enumerationContext->_cimException;
    }
    else
    {
        to.appendResponseData(from);
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
      "OpenAssociatorInstances Send Pull response to type %u"
          " from type %u eos %s providersComplete %s cacheSize %u",
      to.getResponseDataContent(), from.getResponseDataContent(),
               _toCharP(openResponse->endOfSequence),
               _toCharP(enumerationContext->ifProvidersComplete()),
               enumerationContext->responseCacheSize() ));

    // If the providers are complete close the enumeration. Else
    // prepare for the next operation by setting the inactive state
    // and starting the timer.
    // KS_TODO - Would like to move the close and remove to the
    // function also.
    if ((openResponse->endOfSequence =
         enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close AssociatorInstances");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
    }

    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required for pull instances
    // because we are returning named instances.
    // KS_TBD _ This may be done as part of the aggregator also

    to.completeHostNameAndNamespace(
        cimAggregationLocalHost, request->nameSpace);

    _enqueueResponse(request, openResponse.release());

    if (enumerationContext->isClosed())
    {
        enumerationContext->removeContext();
    }

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleOpenAssociatorInstancePathsRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleOpenAssociatorInstancePathsRequest(
    CIMOpenAssociatorInstancePathsRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleOpenAssociatorInstancesRequest");

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "OpenAssociatorInstancesPaths request for object %s "
            "maxObjectCount = \"%s\" .  "
            "operationTimeout = \"%s\" .  "
            "assocClass = %s "
            "resultClass = %s "
            "role = %s "
            "resultRole = %s ",
        (const char*)request->objectName.toString().getCString(),
        (const char*)request->maxObjectCount.toString().getCString(),
        (const char*)request->operationTimeout.toString().getCString(),
        (const char*)request->assocClass.getString().getCString(),
        (const char*)request->resultClass.getString().getCString(),
        (const char*)request->role.getCString(),
        (const char*)request->resultRole.getCString()
        ));

    if (_rejectAssociationTraversalDisabled(request,
        "OpenAssociatorInstancePaths"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidRoleParameter(request, request->role, "role"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidRoleParameter(request, request->resultRole, "roleRole"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidObjectPathParameter(request, request->objectName))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidFilterParameters(request, request->filterQueryLanguage,
                              request->filterQuery))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectIfContinueOnError(request, request->continueOnError))
    {
        PEG_METHOD_EXIT();
        return;
    }

    Uint32 operationMaxObjectCount;
    if (_rejectInvalidMaxObjectCountParam(request, request->maxObjectCount,
            false, operationMaxObjectCount, Uint32(0)))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectInvalidOperationTimeout(request, request->operationTimeout))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (!_checkExistenceOfClass(request->nameSpace,
                                request->objectName.getClassName()))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_PARAMETER,
            request->objectName.getClassName().getString());
    }

    //
    // Get results from providers and the repository
    // Determine list of providers for this request
    //

    ProviderInfoList providerInfos = _lookupAllAssociationProviders(
            request->nameSpace,
            request->objectName,
            request->resultClass,
            String::EMPTY);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_TEMP
        "OpenAssociatorInstancePaths "
        "providerCount = %u.", providerInfos.providerCount));
    //
    // Get the instances from the repository, as necessary
    //
    Array<CIMObjectPath> objectNames;
    if (_repository->isDefaultInstanceProvider())
    {
        objectNames = _repository->associatorNames(
            request->nameSpace,
            request->objectName,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "OpenAssociatorInstancePaths repository access: class = %s,"
            " count = %u.",
            (const char*)request->objectName.toString().getCString(),
            objectNames.size()));
    }

    if (providerInfos.providerCount == 0 && objectNames.size() == 0)
    {
        //
        // No provider is registered and the repository isn't the
        // default.  Return CIM_ERR_NOT_SUPPORTED.
        //
        AutoPtr<CIMOpenAssociatorInstancePathsResponseMessage> response;
        response.reset(
            dynamic_cast<CIMOpenAssociatorInstancePathsResponseMessage*>(
            request->buildResponse()));
        response->getResponseData().setInstanceNames(objectNames);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "CIM_ERR_NOT_SUPPORTED for %s",
            (const char *)request->className.getString().getCString()));

        // KS_PULL_TBD Should be be returning an exception here.
        // Spec issue. Should this be simply empty response
        // KS_PULL_TBD Should not have to do this.  The enum context
        // should be ignored if endOfSequence is true.
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
            "OpenAssociatorInstancePaths Rtn Empty."));

        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED,
            String::EMPTY);

        response->endOfSequence = true;
        _enqueueResponse(request, response.release());

        PEG_METHOD_EXIT();
        return;
    }

    //
    // Create new enumerationContext and enumerationContextString.
    //
    String enContextIdStr;
    EnumerationContext* enumerationContext = enumerationTable.createContext(
        request->nameSpace,
        request->operationTimeout,
        request->continueOnError,
        CIM_PULL_INSTANCE_PATHS_REQUEST_MESSAGE,
        CIMResponseData::RESP_OBJECTPATHS,
        enContextIdStr);

    // Build corresponding EnumerateInstancePathsRequest to send to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers. NOTE: includeQualifiers NOT part of Pull operation

    CIMAssociatorNamesRequestMessage* enumRequest =
        new CIMAssociatorNamesRequestMessage(
            request->messageId,
            request->nameSpace,
            request->objectName,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole,
            request->queueIds,
            request->authType,
            request->userName);

    AutoPtr<CIMAssociatorNamesRequestMessage> dummy(enumRequest);

    enumRequest->operationContext = request->operationContext;

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenAssociatorInstancePaths 7. ProviderCount = %u",
        providerInfos.providerCount));
    //
    // Set up an aggregate object and save the created enumRequest
    //

    OperationAggregate* poA = new OperationAggregate(
        new CIMAssociatorNamesRequestMessage(*enumRequest),
        enumRequest->getType(),
        enumRequest->messageId,
        enumRequest->queueIds.top(),
        enumRequest->objectName.getClassName(),
        enumRequest->nameSpace);

    (enumerationContext->valid());  // EXP_PULL_TEMP

    //
    // Set Open... operation parameters into the operationAggregate
    //
    poA->setPullOperation((void *)enumerationContext,
        enContextIdStr, request->nameSpace);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
    "OpenAssociatorInstancePaths 7a. ProviderCount = %u repository count = %u",
    providerInfos.providerCount, objectNames.size()));

    // If any return from repository, send it to aggregator.
/////// KS_TODO     if (objectNames.size() != 0 || objectNames.size() == 0)
    if (objectNames.size() != 0)
    {
        AutoPtr<CIMAssociatorNamesResponseMessage> enumResponse;
        enumResponse.reset(dynamic_cast<CIMAssociatorNamesResponseMessage*>(
            enumRequest->buildResponse()));
        enumResponse->getResponseData().setInstanceNames(objectNames);

        // KS_TODO - This one worthless if instanceNames.size != 0
        if (enumResponse.get() != 0)
        {
            poA->setTotalIssued(providerInfos.providerCount+1);
            // send the repository's results for aggregation
            // directly to callback (includes response).
            _forwardRequestForAggregation(
                getQueueId(),
                String(),
                new CIMAssociatorNamesRequestMessage(*enumRequest),
                poA,
                enumResponse.release());

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
            "OpenAssociatorInstancePaths 8. ProviderCount = %u",
            providerInfos.providerCount));
        }
        else
        {
            poA->setTotalIssued(providerInfos.providerCount);
        }
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
    "OpenAssociatorInstancePaths 9. ProviderCount = %u",
    providerInfos.providerCount));

    // Call all providers
    for (Uint32 i = 0; i < providerInfos.size(); i++)
    {
        if (providerInfos[i].hasProvider)
        {
            CIMAssociatorNamesRequestMessage* enumRequestCopy =
                new CIMAssociatorNamesRequestMessage(*enumRequest);
            // Insert the association class name to limit the provider
            // to this class.
            enumRequestCopy->assocClass = providerInfos[i].className;

            if (providerInfos[i].providerIdContainer.get() != 0)
                enumRequestCopy->operationContext.insert(
                    *(providerInfos[i].providerIdContainer.get()));

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Forwarding to provider for class %s",
                (const char *)providerInfos[i].
                       className.getString().getCString()));

            _forwardRequestForAggregation(providerInfos[i].serviceId,
                providerInfos[i].controlProviderName, enumRequestCopy, poA);
            // Note: poA must not be referenced after last "forwardRequest"
        }
    }

    // Build get from the cache and return any response
    AutoPtr<CIMOpenAssociatorInstancePathsResponseMessage> openResponse(
        dynamic_cast<CIMOpenAssociatorInstancePathsResponseMessage*>(
            request->buildResponse()));

    openResponse->enumerationContext = enContextIdStr;

    if (providerInfos.providerCount == 0)
    {
        enumerationContext->setProvidersComplete();
        openResponse->endOfSequence = true;
    }
    else
    {
        openResponse->endOfSequence = false;
    }

    openResponse->cimException = CIMException();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
               "Cache size after repository put %u maxObjectCount %u",
               enumerationContext->responseCacheSize(),
               operationMaxObjectCount ));

    // Create a Response data based on what is in the cache now.
    // Create a temporary response data with correct type.

    CIMResponseData from(CIMResponseData::RESP_OBJECTPATHS);

    // get response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete
    Boolean rtn = enumerationContext->getCacheResponseData(
                                operationMaxObjectCount,
                                from);

    CIMResponseData & to = openResponse->getResponseData();

    // if we had an error, put the exception into the response
    // KS-TODO - Should we return any objects if state is error.  Probably
    // not.
    if (enumerationContext->isErrorState())
    {
        openResponse->cimException = enumerationContext->_cimException;
    }
    else
    {
        to.appendResponseData(from);

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
          "OpenAssociatorInstances Send Pull response to type %u"
              " from type %u eos %s providersComplete %s cacheSize %u",
          to.getResponseDataContent(), from.getResponseDataContent(),
                   _toCharP(openResponse->endOfSequence),
                   _toCharP(enumerationContext->ifProvidersComplete()),
                   enumerationContext->responseCacheSize() ));
    }

   if ((openResponse->endOfSequence =
        enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close AssociatorInstancePaths");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
    }

    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required for pull instances
    // because we are returning named instances.
    // KS_TBD _ This may be done as part of the aggregator also

    to.completeHostNameAndNamespace(cimAggregationLocalHost,
        request->nameSpace);

    _enqueueResponse(request, openResponse.release());

    if (enumerationContext->isClosed())
    {
        enumerationContext->removeContext();
    }

    PEG_METHOD_EXIT();
}


void CIMOperationRequestDispatcher::handleEnumerationCount(
        CIMEnumerationCountRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnumerationCount");

    PEGASUS_STD(cout) << "CIMOpDispatcher::handleEnumerationCount class = "
        << request->className.getString()
         << " enumContext= " << request->enumerationContext
         << PEGASUS_STD(endl);

    // Determine if the enumerationContext exists

    EnumerationContext* en =
        enumerationTable.find(request->enumerationContext);

    // test for invalid context and if found, error out.
    if (en == 0)
    {
        CIMResponseMessage* response = request->buildResponse();
        CIMException x = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_ENUMERATION_CONTEXT, String::EMPTY);
        response->cimException = x;

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    // KS_TODO code to do not supported error
    CIMResponseMessage* response = request->buildResponse();
    response->cimException =
        PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

    _enqueueResponse(request, response);

    PEG_METHOD_EXIT();
    return;
}


/**$********************************************************
    handlePullInstancesPaths
************************************************************/

void CIMOperationRequestDispatcher::handlePullInstancePaths(
    CIMPullInstancePathsRequestMessage* request)

{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handlePullInstancePaths");

    // Both pull operations execute off of a single templated
    // function.
    // Build and send response.  getCache used to wait for objects
    AutoPtr<CIMPullInstancePathsResponseMessage> response(
        dynamic_cast<CIMPullInstancePathsResponseMessage*>(
            request->buildResponse()));

    ProviderRequests::issuePullResponse(this, request, response,
                                        "PullInstancePaths");

    PEG_METHOD_EXIT();
    return;
}

/**$********************************************************
    handleCloseEnumeration
************************************************************/

void CIMOperationRequestDispatcher::handleCloseEnumeration(
        CIMCloseEnumerationRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleCloseEnumeration");

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "CloseEnumeration request for  "
            "enumerationContext = \"%s\" .  ",
        (const char*)request->enumerationContext.getCString())
        );

    EnumerationContext* en = enumerationTable.find(request->enumerationContext);

    if (_rejectInValidEnumerationContext(request, en))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (_rejectIfContextTimedOut(request, en->isTimedOut()))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // If another operation is active for this context, reject this operation.
    // The specification allows as an option concurrent close (close
    // while pull request active) but we do not for now.  Complicates the
    // code to much for right now.
    if (_rejectIfEnumerationContextActive(request, en->isActive()))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Set the Enumeration Closed.
    en->setClosed();

    // need to confirm that the providers are complete and if not
    // to force process when they are complete.
//  if (en->ifProvidersComplete())
//  {
//      PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
//         "Close Operation. Providers complete, Close enumeration"));
//
//      enumerationTable.remove(request->enumerationContext);
//  }
//  else
//  {
//      PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
//         "Close Operation. Providers not complete, Close enumeration"));
//  }

    AutoPtr<CIMCloseEnumerationResponseMessage> response(
        dynamic_cast<CIMCloseEnumerationResponseMessage*>(
            request->buildResponse()));

    response->cimException = CIMException();

    _enqueueResponse(request, response.release());

    PEG_METHOD_EXIT();
    return;
}

//KS_PULL_END

/**************************************************************************/
// End of the Pull operation functions
/**************************************************************************/

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
    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::AssociatorNames Response - "
            "Namespace: %s  Class name: %s Response Count: %u",
        CSTRING(poA->_nameSpace.getString()),
        CSTRING(poA->_className.getString()),
        poA->numberResponses()));

    // Work backward and delete each response off the end of the array
    CIMResponseData & to = toResponse->getResponseData();
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMAssociatorNamesResponseMessage* fromResponse =
            (CIMAssociatorNamesResponseMessage*)poA->getResponse(i);
        CIMResponseData & from = fromResponse->getResponseData();
        to.appendResponseData(from);
        poA->deleteResponse(i);
    }
    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required because XML reader
    // will fail without them populated
    to.completeHostNameAndNamespace(cimAggregationLocalHost,poA->_nameSpace);

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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::Associators Response - "
            "Namespace: %s  Class name: %s Response Count: %u",
        CSTRING(poA->_nameSpace.getString()),
        CSTRING(poA->_className.getString()),
        poA->numberResponses()));

    // Work backward and delete each response off the end of the array
    CIMResponseData & to = toResponse->getResponseData();
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMAssociatorsResponseMessage* fromResponse =
            (CIMAssociatorsResponseMessage*)poA->getResponse(i);
        CIMResponseData & from = fromResponse->getResponseData();
        to.appendResponseData(from);
        poA->deleteResponse(i);
    }
    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required because XML reader
    // will fail without them populated
    to.completeHostNameAndNamespace(cimAggregationLocalHost,poA->_nameSpace);

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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::References Response - "
            "Namespace: %s  Class name: %s Response Count: %u",
        CSTRING(poA->_nameSpace.getString()),
        CSTRING(poA->_className.getString()),
        poA->numberResponses()));

    // Work backward and delete each response off the end of the array
    CIMResponseData & to = toResponse->getResponseData();
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMReferencesResponseMessage* fromResponse =
            (CIMReferencesResponseMessage*)poA->getResponse(i);
        CIMResponseData & from = fromResponse->getResponseData();
        to.appendResponseData(from);
        poA->deleteResponse(i);
    }
    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required because XML reader
    // will fail without them populated
    to.completeHostNameAndNamespace(cimAggregationLocalHost,poA->_nameSpace);

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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::ReferenceNames Response - "
            "Namespace: %s  Class name: %s Response Count: %u",
        CSTRING(poA->_nameSpace.getString()),
        CSTRING(poA->_className.getString()),
        poA->numberResponses()));

    // Work backward and delete each response off the end of the array
    CIMResponseData & to = toResponse->getResponseData();
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMReferenceNamesResponseMessage* fromResponse =
            (CIMReferenceNamesResponseMessage*)poA->getResponse(i);
        CIMResponseData & from = fromResponse->getResponseData();
        to.appendResponseData(from);
        poA->deleteResponse(i);
    }
    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required because XML reader
    // will fail without them populated
    to.completeHostNameAndNamespace(cimAggregationLocalHost,poA->_nameSpace);

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

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::EnumerateInstanceNames Response - "
            "Namespace: %s  Class name: %s Response Count: %u",
        CSTRING(poA->_nameSpace.getString()),
        CSTRING(poA->_className.getString()),
        poA->numberResponses()));

    // Work backward and delete each response off the end of the array
    CIMResponseData & to = toResponse->getResponseData();
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMEnumerateInstanceNamesResponseMessage* fromResponse =
            (CIMEnumerateInstanceNamesResponseMessage*)poA->getResponse(i);

        CIMResponseData & from = fromResponse->getResponseData();
        to.appendResponseData(from);

        poA->deleteResponse(i);
    }

    // If pull operation complete host/namespace information. These
    // operations return instances with path including host, etc.
    if (poA->_pullOperation)
    {
        to.completeHostNameAndNamespace(cimAggregationLocalHost,
            poA->_nameSpace);
    }
    PEG_METHOD_EXIT();
}

/* The function aggregates individual EnumerateInstance Responses into a
   single response
   for return to the client. It aggregates the responses into the
   first response (0).
   ATTN: KS 28 May 2002 - At this time we do not do the following:
   1. eliminate duplicates.
   2. prune the properties if deepInheritance is set.
   This function does not send any responses.
*/
void CIMOperationRequestDispatcher::
    handleEnumerateInstancesResponseAggregation(
        OperationAggregate* poA,
        bool hasPropList)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnumerateInstancesResponse");

    CIMEnumerateInstancesResponseMessage* toResponse =
        (CIMEnumerateInstancesResponseMessage*)poA->getResponse(0);

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::EnumerateInstancesResponseAggregation"
            "- Namespace: %s  Class name: %s Response Count: %u",
        CSTRING(poA->_nameSpace.getString()),
        CSTRING(poA->_className.getString()),
        poA->numberResponses()));

    CIMEnumerateInstancesRequestMessage* request =
        (CIMEnumerateInstancesRequestMessage*)poA->getRequest();

    CIMResponseData & to = toResponse->getResponseData();
    // Re-add the property list as stored from request after deepInheritance fix
    // since on OOP on the response message the property list gets lost
    if (hasPropList)
    {
        to.setPropertyList(request->propertyList);
    }

    // Work backward and delete each response off the end of the array
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMEnumerateInstancesResponseMessage* fromResponse =
            (CIMEnumerateInstancesResponseMessage*)poA->getResponse(i);

        CIMResponseData & from = fromResponse->getResponseData();
        to.appendResponseData(from);
        poA->deleteResponse(i);
    }
    // If pull operation complete host/namespace information. These
    // operations return instances with path including host, etc.
    if (poA->_pullOperation)
    {
        to.completeHostNameAndNamespace(cimAggregationLocalHost,
            poA->_nameSpace);
    }
//  PEG_TRACE((
//      TRC_DISPATCHER,
//      Tracer::LEVEL4,
//      "CIMOperationRequestDispatcher::"
//      "EnumerateInstancesResponseAggregation - "
//      "Include Qualifiers: %s Include Class Origin: %s",
//      (request->includeQualifiers == true ? "true" : "false"),
//      (request->includeClassOrigin == true ? "true" : "false")));

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
                CIMClass cimClass =
                    _repository->getClass(
                        request->nameSpace,
                        request->instanceName.getClassName(),
                        false, //localOnly,
                        false, //includeQualifiers,
                        false, //includeClassOrigin,
                        CIMPropertyList());

                PEG_TRACE((
                    TRC_DISPATCHER,
                    Tracer::LEVEL4,
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

        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL4,
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
// KS_TODO - We have two apparently parallel class parameter checks
// One generates response, etc (_checkClassParameter) the other
// returns and has the user generate the error response.  Mostly these
// are generated with a throw.  Should reduce this to one function that
// is clean and uses minimum space. Also they generate different set
// of traces and we need common base for traces.
/*
    Test the validity of the class name parameter and get the corresponding
    class. Returns the class if found.  If not found, returns with return
    code false.
    @param request
    @param className target class for operation
    @param namespace CIMNamespaceName for this operation
    @param cimClass CIMConstClass containing the requested class if the
    class exists.
    @return false if class found or true if class not found
*/

Boolean CIMOperationRequestDispatcher::_rejectInvalidClassParameter(
    CIMRequestMessage* request,
    const CIMName& className,
    const CIMNamespaceName& nameSpace,
    CIMConstClass& cimClass)
{
    CIMException checkClassException;

    cimClass = _getClass(
        nameSpace,
        className,
        checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        ///KS_PULL_TBD - Add this exception function.
        //_enqueueExceptionResponse(request,checkClassException);

        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;
        _enqueueResponse(request, response);
        return true;
    }
    return false;
}

/*
    Check the existence of a class matchin a classname.  Note that
    this code checks for the special classname PEGASUS_CLASSNAME___NAMESPACE
    and returns true.
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
        CIMClass cimClass =
            _repository->getClass(
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

    PEG_TRACE((
        TRC_DISPATCHER,
        Tracer::LEVEL4,
        "CIMOperationRequestDispatcher::_checkExistenceOfClass - "
            "Namespace: %s  Class Name: %s found.",
        (const char*) nameSpace.getString().getCString(),
        (const char*) className.getString().getCString()));

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

        PEG_TRACE((
            TRC_DISPATCHER,
            Tracer::LEVEL3,
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

void CIMOperationRequestDispatcher::_checkEnumerateTooBroad(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Uint32 providerCount)
{
    if (providerCount > _maximumEnumerateBreadth)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "ERROR: Enumerate operation too broad for class %s.  "
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

// KS_TODO - Should have className text on the response message.
Boolean CIMOperationRequestDispatcher::_checkNoProvidersOrRepository(
    CIMRequestMessage* request, Uint32 providerCount, const CIMName& className)
{
    if ((providerCount == 0) &&
        !(_repository->isDefaultInstanceProvider()))
    {
        PEG_TRACE(( TRC_DISPATCHER,Tracer::LEVEL4,
            "CIM_ERROR_NOT_SUPPORTED for %s",
            CSTRING(className.getString()) ));

        CIMResponseMessage* response = request->buildResponse();
        response->cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

        _enqueueResponse(request, response);
        return true;
    }
    else   // We have either providers or a repository
    {
        return false;
    }
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
