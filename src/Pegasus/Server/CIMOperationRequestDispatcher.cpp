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
#include <Pegasus/Server/EnumerationContextTable.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

#define CSTRING(ARG) (const char*) ARG.getCString()

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
#ifdef PEGASUS_PULL_OPERATION_MAX_TIMEOUT
    Uint32 _pullOperationMaxTimeout =
        PEGASUS_PULL_OPERATION_MAX_TIMEOUT;
#else
    Uint32 _pullOperationMaxTimeout = 15;
#endif

// Sets the maximum size for the response cache in each
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
// This variable is not externalized because we are not yet sure
// if that is logical.
Uint32 responseCacheDefaultMaximumSize = 1000;
//
// Define the table that will contain enumeration contexts for Open, Pull,
// Close, and countEnumeration operations.  The default interoperation
// timeout and max cache size are set as part of creating the table.
//
static EnumerationContextTable enumerationContextTable(
    _pullOperationMaxTimeout,
    responseCacheDefaultMaximumSize);

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
    CIMName className,
    CIMNamespaceName nameSpace,
    Uint32 totalIssued,
    Boolean requiresHostnameCompletion,
    Boolean hasPropList,
    QueryExpressionRep* query,
    String queryLanguage)
    : _messageId(request->messageId),
      _msgRequestType(request->getType()),
      _dest(request->queueIds.top()),
      _className(className),
      _nameSpace(nameSpace),
      _requiresHostnameCompletion(requiresHostnameCompletion),
      _hasPropList(hasPropList),
      _query(query),
      _queryLanguage(queryLanguage),
      _pullOperation(false),
      _enumerationFinished(false),
      _closeReceived(false),
      _request(request),
      _totalIssued(totalIssued),
      _totalReceived(0), _totalReceivedComplete(0),
      _totalReceivedExpected(0), _totalReceivedErrors(0),
      _totalReceivedNotSupported(0)
{
}

OperationAggregate::~OperationAggregate()
{
    delete _request;
    delete _query;
}

Boolean OperationAggregate::valid() const
{
    // test if valid OperationAggregate object.
    return _magic;
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
    PEGASUS_ASSERT(response != 0);       /// KS_TEMP
    _responseList.append(response);

    Boolean returnValue = (_totalIssued == numberResponses());

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
        "numberResponses = %u append. Rtns %s size before = %u",
                (Uint32)_responseList.size(),
                boolToString(returnValue), _responseList.size() ));
    return returnValue;
}


/* Returns the count of the current number of responses in the
   responseList.  Note:this is not the Total number of responses,
   just the number currently available for aggregation.
*/

Uint32 OperationAggregate::numberResponses() const
{
    PEGASUS_ASSERT(valid());   // KS_TEMP TODO
//// KS_TODO This was temporary code during testing.
////  AutoMutex autoMut(_appendResponseMutex);
////  Uint32 size = _responseList.size();
////  PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
////      "numberResponses = %u read", size ));
////  return size;

    //// TODO AutoMutex autoMut(_appendResponseMutex);
    return _responseList.size();
}

CIMResponseMessage* OperationAggregate::getResponse(const Uint32& pos)
{
    PEGASUS_ASSERT(valid());   // KS_TEMP;
    AutoMutex autoMut(_appendResponseMutex);
    CIMResponseMessage* tmp = _responseList[pos];
    //// TODO remove this diagnostic trace.
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
        "numberResponses = %u get # %u", _responseList.size(),
                pos ));

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
        "numberResponses = %u remove # %u size before %u",
        _responseList.size(),
        pos, tmpsize));
    return tmp;
}

void OperationAggregate::setRequest(CIMOperationRequestMessage* request)
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
        "numberResponses == %u delete # %u", _responseList.size(), pos ));
}

// There are many response pieces (chunks) from potentially many
// threads funneling through this function in random order. This isolates a
// single response (by locking) from a given thread and "resequences" the
// response as part of one large response. It is crucial that the first
// response to come through here be sequenced (or indexed) as 0 and the last
// response from the last thread be marked as "isComplete"

// NOTE: Assumes no chunks can come AFTER a "isComplete" message
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
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "%s: Response has error.  Namespace: %s, Class name: %s, "
                "Response Sequence: %s",
            "OperationAggregate::resequenceResponse",
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
        boolToString(isComplete),
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
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // TODO KS_TEMP
        "%s: return status.  isComplete: %s Total responses: %u, "
            "total chunks: %u, total errors: %u totalIssued: %u",
        func, boolToString(isComplete),
        _totalReceivedComplete,
        _totalReceived,
        _totalReceivedErrors,
        _totalIssued));
}
//EXP_PULL_BEGIN

/* setPullOperation sets variables in the Operation Aggregate
    specific to the pull operations upon the Open... operations for use by
    subsequent pull operations. This includes:
    _pullOperation - Flag to indicate that this Operation Aggregate is part
    of a pull operation.
    _enumerationContext - Pointer to the Enumeration Context
    _EnumerationContextName - Name property for this context
*/
void OperationAggregate::setPullOperation(const void* enContext)
{
    _pullOperation = true;
    _enumerationContext = (void *) enContext;
}

/*
*
*   Common functions use by the request Handlers
*/
/*
    build an array of CIMNames with all of the properties in the
    class.
    @param class CIMClass with the properties
    @return Array<String> containing the names of all the properties
    in the class.
*/
void _buildPropertyListWithTags(CIMConstClass& thisClass,
    CIMPropertyList& propertyList)
{
    Array<String> propertyNameArray;
    Uint32 numProperties = thisClass.getPropertyCount();

    for (Uint32 i = 0; i < numProperties; i++)
    {
        propertyNameArray.append(
            thisClass.getProperty(i).getName().getString());
    }
    propertyList.append(propertyNameArray);
}
//EXP_PULL_END

/*************************************************************************
**
** Implementation of ProviderInfoList Class.
** NOTE: Much of this class implemented in the header file
**
***************************************************************************/
Boolean ProviderInfoList::hasMore(Boolean isProvider)
    {
        ConstArrayIterator<ProviderInfo> iterator(_array);
        while (_index < iterator.size())
        {
            if (iterator[_index].hasProvider == isProvider)
            {
                return true;
            }
            else
            {
                _index++;
            }
        }
        // Reset index when complete. This is so the hasMore can be used
        // multiple times to cycle through the list.
        _index = 0;
        return false;
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

    // Define the variable that controls whether we allow 0 as a pull
    // interoperation timeout value.  Since the behavior for a zero value is
    // that the server maintains no timer for the context, it may be the
    // decision of some implementors to not allow this value.
#ifdef PEGASUS_PULL_OPERATION_REJECT_ZERO_TIMEOUT_VALUE
    _rejectZeroOperationTimeoutValue = true
#else
    // Default setting if nothing supplied externally
    _rejectZeroOperationTimeoutValue = false;
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

/*  Send the given response synchronously using the given aggregation object.
    return whether the sent message was complete or not. The parameters are
    pointer references because they can be come invalid from external deletes
    if the message is complete after queueing. They can be zeroed in this
    function preventing the caller from referencing deleted pointers.
    If pull operation, send to EnumerationContext queue.  If not pull sent
    directly to output queue.
    Operations for internal clients are gathered completely since there is
    no chunking.
*/
Boolean CIMOperationRequestDispatcher::_enqueueResponse(
    OperationAggregate*& poA,
    CIMResponseMessage*& response)
{
    // Obtain the _enqueueResponseMutex mutex for this chunked request.
    // This mutex serializes chunked responses from all incoming
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

        // Operations which run through here include:
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
            handleOperationResponseAggregation(poA);
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

        response->dest = poA->_dest;
        poA->resequenceResponse(*response);

        // now get the completion status of the response after it has been
        // resequenced. This will reset the completion status of the entire
        // message, not just this one thread coming through here.

        isComplete = response->isComplete();

        // can the destination service queue handle async responses ?
        // (i.e multiple responses from one request). Certain known ones
        // cannot handle it. Most notably, the internal client.

        MessageQueue* q = lookup(poA->_dest);
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

        // Log only if this is the last of the aggregated response
        if (isComplete)
        {
            _logOperation(poA->getRequest(), response);
        }

        // Send it syncronously so that multiple responses will show up in the
        // receiving queue according to the order that we have set the response
        // index. If this was a single complete response, we could in theory
        // send it async (i.e SendForget), however, there is no need to make a
        // condition point based off this.

        // If it is a pull operation response, send to the output caching queue
        // in the enumeration context.  If not a pull operation,
        // directly queue.
        if (poA->_pullOperation)
        {
            // pull operation. Put CIMResponseData into Enum Context unless
            // enum context closed.

            EnumerationContext* en =
                (EnumerationContext*)poA->_enumerationContext;

            // The following is test and validation code
            // KS_TODO remove all of this before release
            PEGASUS_ASSERT(en);          // KS_TEMP
            PEGASUS_ASSERT(en->valid()); // KS_TEMP
            enumerationContextTable.valid();    // KS_TEMP
            enumerationContextTable.tableValidate();    // KS_TEMP

            //// KS_TODO DELETE the following is test code just to be sure
            //// the context is still in the table
            EnumerationContext* enTest = enumerationContextTable.find(
                en->getName());

            if (enTest == 0)
            {
                cout << "Error, EnumContext not found "
                    << en->getName() << endl;
                en->trace();
                PEGASUS_ASSERT(false);
            }

            // If this is an exception set the error in EnumerationContext
            if (response->cimException.getCode())
            {
                en->setErrorState(response->cimException);
            }

            // Send to the EnumerationContext cache along with the
            // isComplete indicator. Return indicates cache is closed
            // and providers complete so clean up.
            // The enumerationContext could be cleaned up by the
            // timer thread once the providers are marked complete in
            // the enumerationContext (if the client had timed out )

            if (!en->putCache(response, isComplete))
            {
                // If providers complete and refused by putCache (i.e.
                // sequence closed, we can now get rid of the context.
                if (isComplete)
                {
                    //// KS_TODO Should we remove here? Confirm there is
                    //// something in _waiting at this point?
                    enumerationContextTable.removeCxt(
                       en->getName(), true);
                }
            }

            delete response;
        }
        // Otherwise not a pull operation; queue response
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
   @exception - Throws exception if the className is in error or the request
   is to broad.  The className exception should NEVER occur since
   the className should have already been validated in the operation
   validation code.
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
    ConstArrayIterator<CIMName> iterator(classNames);
    for (Uint32 i = 0, n = iterator.size(); i < n; i++)
    {
        // Lookup any instance providers and add to send list
        ProviderInfo providerInfo =
            _lookupInstanceProvider(nameSpace, iterator[i]);

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
        providerList.appendClass(providerInfo);
   }

    // Reject if the request is too broad
    _rejectEnumerateTooBroad(
        nameSpace, className, providerList.providerCount);

   // Diagnostic to display list of classes returned. Enable the following
   // line to see what this function returns.
   // providerList.trace(nameSpace, className);


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
    // option, go back and change the call to avoid the double
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
        "Association providerCount = %u.", providerInfoList.providerCount));

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
    PEGASUS_ASSERT(poA->valid());           // KS_TEMP TODO
    if (poA->_pullOperation)                // KS_TEMP
    {
        PEGASUS_ASSERT(poA->_enumerationContext); //KS_TEMP
    }

    Boolean entireResponseIsComplete = service->_enqueueResponse(poA, response);

    if (entireResponseIsComplete)
    {
        // also deletes the copied request attached to it //// TODO temp
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "Provider response complete. isPull %s",
            boolToString(poA->_pullOperation)));
        // delete OperationAggregation and attached request.
        delete poA;
        poA = 0;
    }
    else
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,   //// TODO temp
        "Provider response to a request not complete.");
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
    if (controlProviderName.size() == 0)
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

/** _forwardRequestToSingleProvider
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
void CIMOperationRequestDispatcher::_forwardRequestToSingleProvider(
    const ProviderInfo& providerInfo,
    CIMOperationRequestMessage* request,
    CIMOperationRequestMessage* requestCopy)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToSingleProvider");

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
    Helper functions that create a response message with the defined
    exception and queues it.
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
   Enqueue the response provided with the call to destination defined
   by request.
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

    // Internal client does not have async capabilities, call enqueue()
    // for handling legacy messages directly.
    MessageQueue* queue = MessageQueue::lookup(request->queueIds.top());
    PEGASUS_ASSERT(queue != 0);

    queue->enqueue(response);

    PEG_METHOD_EXIT();
}

/*
    handleEnqueue is the Request input processor to the Dispatcher.
    It processes all incoming request messages and distributes them to
    the appropriate request handler functions based on the operation type.
    This function includes
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

        case CIM_OPEN_QUERY_INSTANCES_REQUEST_MESSAGE:
            handleOpenQueryInstancesRequest(
                (CIMOpenQueryInstancesRequestMessage*)opRequest);
            break;
//KS_PULL_END

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
    {
        handleEnqueue(request);
    }

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
        // return when AssociationTraversal is enabled
        return false;
    }
    else
    {
        //// KS_TODO add explicit international message
        _enqueueExceptionResponse(request,CIM_ERR_NOT_SUPPORTED, opName);
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

Boolean CIMOperationRequestDispatcher::_rejectNoProvidersOrRepository(
    CIMOperationRequestMessage* request,
    const ProviderInfoList& providerInfos)
{
    if ((providerInfos.providerCount == 0) &&
        !(_repository->isDefaultInstanceProvider()))
    {
        CIMException cimException = PEGASUS_CIM_EXCEPTION_L(
            CIM_ERR_NOT_SUPPORTED, MessageLoaderParms(
                "Server.CIMOperationRequestDispatcher."
                    "REQUEST_CLASS_NOT_SUPPORTED",
                "No provider or repository defined for class $0.",
                request->className.getString()));

        _enqueueExceptionResponse(request, cimException);
        return true;
    }
    else   // We have either providers or a repository
    {
        return false;
    }
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
    @param request. NOTE: This depends on the namespace and class in
        the CIMOperationRequestMessage request.
    @param cimClass CIMConstClass containing the requested class if the
    class exists.
    @return false if class found or true if class not found
*/
Boolean CIMOperationRequestDispatcher::_rejectInvalidClassParameter(
    CIMOperationRequestMessage* request,
    CIMConstClass& targetClass)
{
    CIMException checkClassException;

    targetClass = _getClass(
        request->nameSpace,
        request->className,
        checkClassException);

    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "CIMOperationRequestDispatcher - "
                "CIM class exist exception has occurred.  Namespace: %s  "
                "Class Name: %s  Exception message: \"%s\"",
            CSTRING(request->nameSpace.getString()),
            CSTRING(request->className.getString()),
            CSTRING(checkClassException.getMessage())));

        _enqueueExceptionResponse(request,checkClassException);
        return true;
    }
    return false;
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

//EXP_PULL_BEGIN
/*
    Test if this is a valid Pull message corresponding to the type
     of the open. If not valid, put out error message and return false
    @param request
    @param valid Boolean = true if it is valid
    Return true if valid = true, false if valid = false
*/
Boolean CIMOperationRequestDispatcher::_rejectInvalidPullRequest(
    CIMOperationRequestMessage* request, Boolean valid)
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

/* Test to assure that the filter query language amd filter query are as
   required.  At this point, Pegasus refuses any data in these parameters in
   accord with DSP0200 version 1.3.  This will change when the spec and
   Pegasus support these parameters.
*/
Boolean CIMOperationRequestDispatcher::_rejectInvalidFilterParameters(
    CIMOperationRequestMessage* request,
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
    CIMOperationRequestMessage* request,
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
    CIMOperationRequestMessage* request,
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
    CIMOperationRequestMessage* request,
    Uint32 maxObjectCountParam,
    Boolean requiredParameter,
    Uint32& rtnValue,
    const Uint32 defaultValue)
{
    {
        if (maxObjectCountParam > _systemMaxPullOperationObjectCount)
        {
            // KS_TODO - Would be more helpful if it supplied numbers
            CIMException x = CIMException(CIM_ERR_INVALID_PARAMETER,
                "maxObjectCount parameter is outside OpenPegasus size limit");
            _enqueueExceptionResponse(request, x);
            return true;
        }
        else
        {
                rtnValue = maxObjectCountParam;
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
    CIMOperationRequestMessage* request,
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
   @param valid Boolean = true if valid
   @return true if valid=true, false if valid=false.
*/
Boolean CIMOperationRequestDispatcher::_rejectInValidEnumerationContext(
    CIMOperationRequestMessage* request,
    void * enumerationContext)
{
    EnumerationContext* en = (EnumerationContext *) enumerationContext;
    if (en == 0)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "%s Invalid Context parameter Received",
             MessageTypeToString(request->getType()) ));

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
    CIMOperationRequestMessage* request,
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

Boolean CIMOperationRequestDispatcher::_rejectIfEnumerationContextProcessing(
    CIMOperationRequestMessage* request, Boolean processing)
{
    if (processing)
    {
        CIMResponseMessage* response = request->buildResponse();
//// KS_TODO think this may be wrong error message.
        CIMException x = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_PULL_CANNOT_BE_ABANDONED, String::EMPTY);
        response->cimException = x;

        _enqueueResponse(request, response);
    }
    return processing;
}
// EXP_PULL_END

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
*/

/*  This struct not part of CIMOperationRequestDispatcher class because it
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

    // Set the appropriate data into the CIMResponseData container of the
    // response message.  This is used with the open handlers
    // to set the response data into the repository response messages
    // because some messages have paths and others objects.

    static void setCIMResponseData(
        CIMAssociatorsResponseMessage* response,
        Array<CIMObject>& repositoryData)
    {
        response->getResponseData().setObjects(repositoryData);
    }

    static void setCIMResponseData(
        CIMAssociatorNamesResponseMessage* response,
        Array<CIMObjectPath>& repositoryData)
    {
        response->getResponseData().setInstanceNames(repositoryData);;
    }

    static void setCIMResponseData(
        CIMReferencesResponseMessage* response,
        Array<CIMObject>& repositoryData)
    {
        response->getResponseData().setObjects(repositoryData);;
    }

    static void setCIMResponseData(
        CIMReferenceNamesResponseMessage* response,
        Array<CIMObjectPath>& repositoryData)
    {
        response->getResponseData().setInstanceNames(repositoryData);;
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
        REQ* request,
        AutoPtr<RSP>& response,
        ProviderInfoList& providerInfos,
        const char * reqMsgName)
    {
        // No providers and nothing from repository. Return empty
        if ((providerInfos.providerCount == 0) && (response.get() == 0))
        {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
                "%s Request, Returns nothing for %s", reqMsgName,
                CSTRING(request->className.getString()) ));

            response.reset(dynamic_cast<RSP*>(request->buildResponse()));

            dispatcher->_enqueueResponse(request, response.release());
        }
        else
        {
            OperationAggregate *poA = new OperationAggregate(
                new REQ(*request),
                request->objectName.getClassName(),
                request->nameSpace,
                providerInfos.providerCount,
                true);   // requiresHostNameCompletion = true

            // Include the repository response in the aggregation, if it
            // exists
            if (response.get() != 0)
            {
                poA->incTotalIssued();
                dispatcher->_forwardResponseForAggregation(
                    new REQ(*request),
                    poA,
                    response.release());
            }

            while (providerInfos.hasMore(true))
            {
                ProviderInfo& providerInfo = providerInfos.getNext();

                // Make copy of request
                REQ* requestCopy = new REQ(*request);

                // Insert the association class name to limit the provider
                // to this class. This is a template function that sets the
                // class fields as required for each msg type.
                setSelectedRequestFields(requestCopy, providerInfo);

                if (providerInfo.providerIdContainer.get() != 0)
                {
                    requestCopy->operationContext.insert(
                        *(providerInfo.providerIdContainer.get()));
                }

                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "%s Forwarding to provider for class %s", reqMsgName,
                    CSTRING(providerInfo.className.getString()) ));

                dispatcher->_forwardAggregatingRequestToProvider(
                    providerInfo,
                    requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest
            }
        }
    } // end issueAssocRequest

    /**************************************************************************
    **
    ** issueAssocRequests - Template method to issue requests for
    **     OpenAssociator and OpenReference operations.
    **
    **************************************************************************/
    /*  Template method issues the provider requests, repository
        responses and provider requests for:
        OpenAssociators
        OpenAssociatorNames
        OpenReferences
        OpenReferenceNames
        It differs from the issueAssocRequests in that this builds the
        enumerationContext for the operations.
    */
    //template<class REQ, class IREQ, class RSP, class IRSP, class OBJTYPE>

    template<class IREQ>
    static void issueOpenAssocRequestMsgsToProviders(
       CIMOperationRequestDispatcher* dispatcher,
       IREQ* internalRequest,
       ProviderInfoList& providerInfos,
       EnumerationContext * enumerationContext,
       OperationAggregate* poA,
       const char * reqMsgName)
    {
          ///  KS_TODOMarking comment for now.  Have not figured out how to
          /// get the MsgType to this function for the internalResponse.
////      // If response from repository, forward for aggregation.
////
////      // The following is call dependent (instancePaths or cimObjects
////      // Need an array of something parameter
////      if (repositoryData.size() != 0)
////      {
////          // This one is a problem unless we define a new parameter
////          // for internalResponse. Maybe need the same thing as the
////          // setSelectedRequestFields. But since this is a conditional
////          // not sure how to provide the parameter.
////          AutoPtr<IRSP> internalResponse;
////          internalResponse.reset(dynamic_cast<IRSP*>(
////              internalRequest->buildResponse()));
////          // Sets the data into the appropriate type in CIMResponseData
////          // There are functions for CIMObjects and CIMObjectPaths
////          internalResponse->getResponseData().setArrayData(repositoryData);
////
////          poA->incTotalIssued();
////
////          // send the repository's results for aggregation
////          // directly to callback (includes response).
////          //
////          dispatcher->_forwardResponseForAggregation(
////              new REQ(*internalRequest),
////              poA,
////              internalResponse.release());
////
////          PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
////          "%s repository _forwardForAggregation."
////          " ProviderCount= %u total issued = %u", reqMsgName,
////          providerInfos.providerCount, poA->getTotalIssued()));
////      }

        // Issue requests to all providers defined.
        while (providerInfos.hasMore(true))
        {
            ProviderInfo& providerInfo = providerInfos.getNext();

            IREQ* requestCopy = new IREQ(*internalRequest);
            // Insert the association class name to limit the provider
            // to this class.
            setSelectedRequestFields(requestCopy, providerInfo);

            if (providerInfo.providerIdContainer.get() != 0)
            {
                requestCopy->operationContext.insert(
                    *(providerInfo.providerIdContainer.get()));
            }

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "%s Forwarding to provider for class %s",reqMsgName,
                CSTRING(providerInfo.className.getString()) ));

            dispatcher->_forwardAggregatingRequestToProvider(
               providerInfo, requestCopy, poA);
            // Note: poA must not be referenced after last "forwardRequest"
        }
    }    // end of issueOpenAssocRequestMessagesToProviders

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
        REQ* request,
        //AutoPtr<RSP>& response,   // Future when we put more in this funct.
        ProviderInfoList providerInfos,
        OperationAggregate* poA)
    {
        // Loop through providerInfos, forwarding requests to providers
        while (providerInfos.hasMore(true))
        {
            ProviderInfo& providerInfo = providerInfos.getNext();

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Routing request for class %s to "
                    "service \"%s\" for control provider \"%s\".  "
                    "Class # %u of %u",
                CSTRING(providerInfo.className.getString()),
                _getServiceName(providerInfo.serviceId),
                CSTRING(providerInfo.controlProviderName),
                providerInfos.getIndex() + 1,
                providerInfos.size() ));

            // set this className into the new request
            REQ* requestCopy = new REQ(*request);

            // set this className into the new request
            requestCopy->className = providerInfo.className;

            // Forward to provider. If fails return empty response
            // KS_TODO Confirm that this short bypass is needed and works.
            // The _forwardEnumerationToProvider handles any class
            // checking and special tasks for NORMALIZATION
            if (!dispatcher->_forwardEnumerationToProvider(
               providerInfo, poA, requestCopy))
            {
                CIMResponseMessage* response = requestCopy->buildResponse();
                dispatcher->_forwardResponseForAggregation(
                   requestCopy,
                   poA,
                   response);
            }
        }
    }
// EXP_PULL_BEGIN
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
    static void processPullRequest(
        CIMOperationRequestDispatcher* dispatcher,
        REQ* request,
        AutoPtr<RSP>&  response,
        const char* opSeqName) // PullInstancePaths or PullInstancesWithPath
    {
        PEG_METHOD_ENTER(TRC_DISPATCHER,
            "CIMOperationRequestDispatcher::processPullRequest");

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "%s request for  "
                "maxObjectCount = \"%u\" .  "
                "enumerationContext = \"%s\" .  ", opSeqName,
            request->maxObjectCount,
            CSTRING(request->enumerationContext) ));

        // Find the enumerationContext object from the request parameter
        EnumerationContext* enumContext =
             enumerationContextTable.find(request->enumerationContext);

        // If enumeration Context not found or value is zero,
        // return invalid exception
        if (dispatcher->_rejectInValidEnumerationContext(request, enumContext))
        {
            PEG_METHOD_EXIT();
            return;
        }
        // lock the context until we have set processing state to avoid
        // conflict with timer thread.
        {
            AutoMutex contextLock(enumContext->_contextLock);

            // reject if this is a not valid request for the originating
            // operation
            if (dispatcher->_rejectInvalidPullRequest(request,
                enumContext->isValidPullRequestType(request->getType())))
            {
                PEG_METHOD_EXIT();
                return;
            }
            // reject if an operation is already active on this enumeration
            // context
            if (dispatcher->_rejectIfEnumerationContextProcessing(request,
                enumContext->isProcessing()))
            {
                PEG_METHOD_EXIT();
                return;
            }

            if (dispatcher->_rejectIfContextTimedOut(request,
                enumContext->isTimedOut()))
            {
                PEG_METHOD_EXIT();
                return;
            }

            // Set active and stop interOperation timer
            enumContext->setProcessingState(true);
        }

        // If maxObjectCount = 0, Respond with empty response unless
        // consecutieve requests with maxObjectCount = 0 exceeds limit.
        if (request->maxObjectCount == 0)
        {
            // test limit of the maxObjectCount consecutive zero counter
            // The parameter indicates that this operation has maxObjectCount
            // zero. Note that this function also increments the count
            // of pull operations total for the enumeration sequence
            if (enumContext->incAndTestPullCounters(true))
            {
                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "%s Generating 0 object requested response", opSeqName));

                response->endOfSequence = false;
                response->cimException = CIMException();

                // set operation inactive and start interoperation timer

                enumContext->setProcessingState(false);
            }
            else  // Limit reached, kill the whole pull sequence
            {
                // TODO - Confirm that this is the correct exception here
                // could also be the ABANDONED one
                //
                PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                    "%s Exceeded maxObjectCount consecutive zero limit",
                    opSeqName));
                response->endOfSequence = true;
                response->cimException = CIMException(
                    CIM_ERR_SERVER_LIMITS_EXCEEDED,
                    "Maximum consecutive zero maxObjectCount pull requests"
                        " exceeded");

                enumContext->setClientClosed();
            }

            // Send a zero object or exception response
            dispatcher->_enqueueResponse(request, response.release());
            PEG_METHOD_EXIT();
            return;
        }
        else
        {
            // Ignore return from increment here because this is just
            // resetting the consecutive zero length response counter
            enumContext->incAndTestPullCounters(false);
        }

        /*
            Get Objects From Cache and return with end-of-sequence status
            or if errorstate, return error
        */

        Uint32 localMaxObjectCount = request->maxObjectCount;

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
            "%s getting from cache. isComplete: %s cacheSize: %u error: %s",
            opSeqName,
            boolToString(enumContext->ifProvidersComplete()),
            enumContext->responseCacheSize(),
            boolToString(enumContext->isErrorState())  ));

        // If error set, send error response, else send the next group of
        // response objects which is in the from CIMResponseData object.
        // This implies error response before data response.
        // Do local variable to avoid issue of error being inserted between
        // isErrorState and setNextEnumerationState
        Boolean errorFound = false;
        if ((errorFound = enumContext->isErrorState()))
        {
            response->cimException = enumContext->_cimException;
        }
        else
        {
////          PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
////            "%s getCacheResponsedata. max objects %u",
////            opSeqName,localMaxObjectCount ));

            CIMResponseData fromCache(enumContext->getCIMResponseDataType());

            enumContext->getCache(localMaxObjectCount,fromCache);

            CIMResponseData & to = response->getResponseData();

            to.setDataType((enumContext->getCIMResponseDataType()));
            to.appendResponseData(fromCache);
        }

        // Check here after we have processed the results of the get.
        // This function sets the operation closed if providers are complete
        // and nothing left in the cache, thus closing the operation.
        // Otherwise it sets the processing state = false so the next
        // operation can be processed.
        // At this point we are current with the provider response status
        // If return true, enumeration sequence complete.
        // Set endOfSequence if true. Otherwise sets enumerationContext
        // name.
        if (enumContext->setNextEnumerationState(errorFound))
        {
            response->endOfSequence = true;
        }
        // only return enum name if not endOfSequence
        else
        {
            response->enumerationContext = enumContext->getName();
        }

        dispatcher->_enqueueResponse(request, response.release());

        if (enumContext->isClosed())
        {
            enumerationContextTable.removeContext(enumContext);
        }

    } // end issuePullResponse

    /**************************************************************************
    **
    ** issue OpenRequest Response message - Template method to issue the open
    **     response message for all of the OpenRequests.
    **
    **************************************************************************/
    /*  Template method to issue the open response message itself. This method
        inserts the required field into the open response, communicates with
        the enumeration context, enqueues the response and if the enumerate
        sequence is complete, closes the enumerationContext.
    */
    template<class REQ, class RSP >
    static void issueOpenResponseMessage(
        CIMOperationRequestDispatcher* dispatcher,
        REQ* openRequest,
        AutoPtr<RSP>& openResponse,
        EnumerationContext* enumContext,
        const char* reqMsgName,
        Uint32 operationMaxObjectCount)
    {
        PEG_METHOD_ENTER(TRC_DISPATCHER,
            "CIMOperationRequestDispatcher::issueOpenResponseMessage");

        Boolean errorFound = false;
        if ((errorFound = enumContext->isErrorState()))
        {
            openResponse->cimException = enumContext->_cimException;
        }
        else
        {
            // Create a Response data based on what is in the cache now.

            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "%s Cache size after repository put %u"
                " maxObjectCount %u", reqMsgName,
                enumContext->responseCacheSize(),
                operationMaxObjectCount ));

            CIMResponseData fromCache(enumContext->getCIMResponseDataType());

            // get response data from the cache up to maxObjectCount and return
            // it in a new CIMResponseData object. This function waits for
            // sufficient objects in cache or providers complete
            enumContext->getCache(operationMaxObjectCount,fromCache);

            CIMResponseData & to = openResponse->getResponseData();
            to.appendResponseData(fromCache);
////        // KS_TODO Delete this code when we are really confident
////          PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
////            "%s AppendedResponseData. to type %u from "
////                "type %u toSize %u fromSize %u", reqMsgName,
////            to.getResponseDataContent(),
////            fromCache.getResponseDataContent(),
////            to.size(), fromCache.size()));
        }
////      // KS_TODO Delete this code when we are really confident of pull
////      CIMResponseData & tempto = openResponse->getResponseData();
////      PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
////        "%s Send response to type %u"
////            "  providersComplete %s remaining cacheSize %u", reqMsgName,
////        tempto.getResponseDataContent(),
////        boolToString(enumContext->ifProvidersComplete()),
////        enumContext->responseCacheSize() ));

        // Do check here after processing the results of the get.
        // The function either closes the operation of providers are complete
        // and the response cache is empty or sets the processing state =
        // false to allow the next operation.
        // At this point context is current with provider response status
        // If return = true, enumerateion sequence complete.
        if (enumContext->setNextEnumerationState(errorFound))
        {
            openResponse->endOfSequence = true;
        }
        else
        {
            openResponse->enumerationContext = enumContext->getName();
        }

        dispatcher->_enqueueResponse(openRequest, openResponse.release());

        if (enumContext->isClosed())
        {
            enumerationContextTable.removeContext(enumContext);
        }
        PEG_METHOD_EXIT();
    }  // issueOpenResponseMessages Template
};
// EXP_PULL_END

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
    if (_rejectInvalidClassParameter(request, cimClass))
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

        _forwardRequestToSingleProvider(
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

        _forwardRequestToSingleProvider(
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

        _forwardRequestToSingleProvider(
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

        _forwardRequestToSingleProvider(
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
    if (_rejectInvalidClassParameter(request,cimClass))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Set the propertylist to be forwarded in accord with the propertyList
    // parameter. The property list forwarded to providers
    // is constructed from properties in the top level class  when
    // !deepInheritance && propertyList.isNull() to assure
    // that providers will return only the properties of the requested class.
    // NOTE - The request class pl should really be null.  Subclasses should
    // get the propertyList. But we do not have a record today of the
    // original class in the providerInfo list so everybody gets the
    // list.

    if (!request->deepInheritance && request->propertyList.isNull())
    {
        _buildPropertyListFromClass(cimClass, request->propertyList);
    }

    //
    // Get names of descendent classes and list of providers
    // For Enumerate Operations; the list of providers is used for both
    // repository and provider requests.
    //
    ProviderInfoList providerInfos = _lookupAllInstanceProviders(
        request->nameSpace,
        request->className);

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED

    if (_rejectNoProvidersOrRepository(request, providerInfos))
    {
        PEG_METHOD_EXIT();
        return;
    }

    //
    // Set up an aggregate object with a copy of the original request.
    // NOTE: OperationAggregate released only when operation complete.
    // requiresHostnameCompletion false, hasPropList true
    //
    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstancesRequestMessage(*request),
        request->className,
        request->nameSpace,
        providerInfos.providerCount,
        false,          // completeHostAndNamespace = false
        true);          // has propertyList = true

    // Gather the repository responses and send as one response
    // with many instances
    //
    if (_enumerateFromRepository(request, poA, providerInfos))
    {
        CIMResponseMessage* response = poA->removeResponse(0);
        _forwardResponseForAggregation(
            new CIMEnumerateInstancesRequestMessage(*request),
            poA, response);
    }
    ProviderRequests::issueEnumerationRequests(
        this,
        request,
        providerInfos,
        poA);

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
    // Validate the class name in the request
    //
    CIMConstClass cimClass;
    if (_rejectInvalidClassParameter(request, cimClass))
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

    if (_rejectNoProvidersOrRepository(request, providerInfos))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // We have instances for Providers and possibly repository.
    // Set up an aggregate object and save a copy of the original request.
    // NOTE: OperationAggregate released only when operation complete
    //
    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstanceNamesRequestMessage(*request),
        request->className,
        request->nameSpace,
        providerInfos.providerCount,
        false);   // completeHostAndNamespace = false, and no property list

    if (_enumerateFromRepository(request, poA, providerInfos))
    {
        CIMResponseMessage* response = poA->removeResponse(0);

        _forwardResponseForAggregation(
            new CIMEnumerateInstanceNamesRequestMessage(*request),
            poA,
            response);
    }

    // Template to issue EnumerationRequest messages to Providers.
    ProviderRequests::issueEnumerationRequests(
        this,
        request,
        providerInfos,
        poA);

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

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleAssociators - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    //// KS_TODO Remove once validated.
    PEGASUS_ASSERT(request->className == request->objectName.getClassName());

    if (_rejectAssociationTraversalDisabled(request, "Associators"))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Validate role and resultrole parameters
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

    //// TODO THIS is ClassParameter, open is ObjectPathParameter
    if (_rejectInvalidClassParameter(request, request->nameSpace,
                                request->objectName))
    {
        PEG_METHOD_EXIT();
        return;
    }

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

    // This is an instance request
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        // Determine list of providers for this request. Note that
        // providerCount may not be the same as the size of the returned
        // array because not all Class will have providers.
        //
        ProviderInfoList providerInfos = _lookupAllAssociationProviders(
            request->nameSpace,
            request->objectName,
            request->assocClass,
            String::EMPTY);

        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if (_rejectNoProvidersOrRepository(request, providerInfos))
        {
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
        // Issue Provider requests and process the respository responseData
        ProviderRequests::issueAssocRequests(
            this,
            request,
            response,
            providerInfos,
            "Associators");
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

    PEG_TRACE(( TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleAssociatorNames - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    //// KS_TODO Remove once validated.
    PEGASUS_ASSERT(request->className == request->objectName.getClassName());

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

    // If Class request, get class data from repository
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
    // Else this is an Instance request
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

        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if (_rejectNoProvidersOrRepository(request, providerInfos))
        {
            PEG_METHOD_EXIT();
            return;
        }

        // Get from repository if it exists.
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
        // Issue Provider requests and process the respository responseData
        ProviderRequests::issueAssocRequests(
            this,
            request,
            response,
            providerInfos,
            "AssociatorNames");

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

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleReferences - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    //// KS_TODO Remove once validated.
    PEGASUS_ASSERT(request->className == request->objectName.getClassName());

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


    // If true, this is a class Request
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

    // Else Instance Request
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        // Determine list of Classes and providers for this request
        //
        ProviderInfoList providerInfos = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                String::EMPTY);

        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if (_rejectNoProvidersOrRepository(request, providerInfos))
        {
            PEG_METHOD_EXIT();
            return;
        }
        //
        // Get the instances from the repository, as necessary.
        // Hold the repository results in a response message.
        // If not using the repository, this pointer is null.
        //
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

        // Issue Provider requests and process the respository responseData
        ProviderRequests::issueAssocRequests(
            this,
            request,
            response,
            providerInfos,
            "References");

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

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher::handleReferenceNames - "
            "Namespace: %s  Class name: %s",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString())));

    //// KS_TODO Remove once validated.
    PEGASUS_ASSERT(request->className == request->objectName.getClassName());

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

    //  else Instance operation
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

        // If no provider is registered and the repository isn't the default,
        // return CIM_ERR_NOT_SUPPORTED

        if (_rejectNoProvidersOrRepository(request, providerInfos))
        {
            PEG_METHOD_EXIT();
            return;
        }

        // Get  objectNames from repository if repository
        // is default provider.
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

        // Issue Provider requests and process the respository responseData
        // NOTE: This is a template
        ProviderRequests::issueAssocRequests(
            this,
            request,
            response,
            providerInfos,
            "ReferenceNames");

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

        _forwardRequestToSingleProvider(
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

        _forwardRequestToSingleProvider(
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

        _forwardRequestToSingleProvider(
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

//KS_PULL_BEGIN
/*************************************************************************
//
//  Pull operations introduced in Pegasus 2.14 per DMTF DSP200 - V 1.4
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
        "OpenEnumerateInstances request namespace=%s class=%s "
            "deepInheritance=%s "
            "includeClassOrigin=%s "
            "propertyList=%s "
            "filterQueryLanguage=%s "
            "filterQuery=%s "
            "operationTimeout=%s "
            "continueOnError=%s "
            "maxObjectCount=%u ",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->className.getString()),
        boolToString(request->deepInheritance),
        boolToString(request->includeClassOrigin),
        CSTRING(request->propertyList.toString()),
        CSTRING(request->filterQueryLanguage),
        CSTRING(request->filterQuery),
        CSTRING(request->operationTimeout.toString()),
        boolToString(request->continueOnError),
        request->maxObjectCount ));

    // get the class name or generate error if class not found for target
    // namespace.
    CIMName className = request->className;
    CIMConstClass cimClass;
    if (_rejectInvalidClassParameter(request, cimClass))
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
    // in the specified class. This forces subclasses to handle defined
    // list rather than their full property list.
    if (!request->deepInheritance && request->propertyList.isNull())
    {
        _buildPropertyListWithTags(cimClass, request->propertyList);
    }

    //
    // Get names of descendent classes with indication of whether they
    // have providers. providerCount is count of number of entries that
    // actually have providers. Others are subclasses without providers.
    //
    ProviderInfoList providerInfos = _lookupAllInstanceProviders(
        request->nameSpace, request->className);

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED.
    // KS_PULL_QUESTION The following say if no provider and no
    // repository return error. Aggregation(
    //            providerInfo.se
    //Should we not just return nothing.

    if (_rejectNoProvidersOrRepository(request, providerInfos))
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Build a corresponding EnumerateInstancesRequest to send to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers.
    // NOTE: includeQualifiers NOT part of Pull operation

    CIMEnumerateInstancesRequestMessage* internalRequest =
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

    internalRequest->operationContext = request->operationContext;

    internalRequest->internalOperation = true;

    // AutoPtr to delete internalRequest at end of handler
    AutoPtr<CIMEnumerateInstancesRequestMessage> dummy(internalRequest);

    //
    // Setup the EnumerationContext. Returns pointer to object and context ID
    // string
    //

    EnumerationContext* enumerationContext =
        enumerationContextTable.createContext(
            request->nameSpace,
            request->operationTimeout,
            request->continueOnError,
            CIM_PULL_INSTANCES_WITH_PATH_REQUEST_MESSAGE,
            CIMResponseData::RESP_INSTANCES);

    enumerationContext->setRequestProperties(
        request->includeClassOrigin, request->propertyList);

    //// KS_TEMP KS_TODO REMOVE debugging code. Delete.
    if (enumerationContext->responseCacheSize() != 0)
    {
        enumerationContext->valid();
        enumerationContext->trace();
        enumerationContextTable.trace();
    }

    PEGASUS_ASSERT(enumerationContext->responseCacheSize() == 0);  // KS_TEMP

    // Set up an aggregate object and save a copy of the original request.
    // NOTE: Build the poA for the EnumerateRequest, not the corresponding
    // pull operation.

    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstancesRequestMessage(*internalRequest),
        internalRequest->className,
        internalRequest->nameSpace,
        providerInfos.providerCount,
        true,       // completeHostAndNamespace = false
        true);      // _hasPropList = true

    PEGASUS_ASSERT(poA->valid());   // KS_TEMP
    PEGASUS_ASSERT(enumerationContext->valid());  // EXP_PULL_TEMP

    //
    // Set Open... operation parameters into the operationAggregate
    //
    poA->setPullOperation((void *)enumerationContext);

    //
    // If repository as instance provider is enabled, get instances
    // from the repository
    //
    PEGASUS_ASSERT(poA->valid());   // KS_TEMP

    if (_enumerateFromRepository(internalRequest, poA, providerInfos))
    {
        CIMResponseMessage* response = poA->removeResponse(0);
        // Forward the Enum response.  It will be mapped into
        // the EnumerateionContext cache.
        _forwardResponseForAggregation(
            new CIMEnumerateInstancesRequestMessage(*internalRequest),
            poA, response);
    }

    // Cannot use Operation Aggregate after this point since we are not
    // sure when it might be deleted unless the providerCount is != 0

    // Build empty open response

    AutoPtr<CIMOpenEnumerateInstancesResponseMessage> openResponse(
        dynamic_cast<CIMOpenEnumerateInstancesResponseMessage*>(
            request->buildResponse()));
    //
    // Issue Enumeration requests to all providers.
    //

    // Issue requests to providers before we send open response to get
    // provider responses before we build response.  This required to allow
    // building initial response of max requested size
    ProviderRequests::issueEnumerationRequests(
        this,
        internalRequest,
        providerInfos,
        poA);

    // Issue the Response to the Open Request. This may cause
    // wait for the enumeration cache before the response is issued.
    // Gets response data from the cache up to maxObjectCount and returns
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete.
    // If there was an error in the provider responses, puts the exception
    // into the response
    ProviderRequests::issueOpenResponseMessage(
       this,
       request,
       openResponse,
       enumerationContext,
       "OpenEnumerateInstances",
       operationMaxObjectCount);

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
        "OpenEnumerateInstancePaths request namespace=%s class %s "
            "filterQueryLanguage=%s "
            "filterQuery=%s "
            "operationTimeout=%s "
            "continueOnError=%s "
            "maxObjectCount=%u",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->className.getString()),
        CSTRING(request->filterQueryLanguage),
        CSTRING(request->filterQuery),
        CSTRING(request->operationTimeout.toString()),
        boolToString(request->continueOnError),
        request->maxObjectCount ));

    // get the class name
    // KS_TODO can we eliminate this completely??
    CIMName className = request->className;
    CIMConstClass cimClass;
    if (_rejectInvalidClassParameter(request, cimClass))
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

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED.
    // KS_PULL_QUESTION The following says if no provider and no
    // repository return error.  Should we not just return
    // nothing.

    if (_rejectNoProvidersOrRepository(request, providerInfos))
    {
        PEG_METHOD_EXIT();
        return;
    }

   // Build a corresponding EnumerateInstancesNamesRequest to send to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers.
    // NOTE: includeQualifiers NOT part of Pull operation

    CIMEnumerateInstanceNamesRequestMessage* internalRequest =
        new CIMEnumerateInstanceNamesRequestMessage(
            request->messageId,
            request->nameSpace,
            request->className,
            request->queueIds,
            request->authType,
            request->userName);

    internalRequest->operationContext = request->operationContext;

    internalRequest->internalOperation = true;

    // AutoPtr to delete at end of handler
    AutoPtr<CIMEnumerateInstanceNamesRequestMessage> dummy(internalRequest);

    internalRequest->operationContext = request->operationContext;

    //
    // Setup the EnumerationContext. Returns pointer to object and context ID
    // string
    //

    EnumerationContext* enumerationContext =
        enumerationContextTable.createContext(
            request->nameSpace,
            request->operationTimeout,
            request->continueOnError,
            CIM_PULL_INSTANCE_PATHS_REQUEST_MESSAGE,
            CIMResponseData::RESP_INSTNAMES);

    //
    // Set up op aggregate object and save a copy of the original request.
    // NOTE: Build the poA for the EnumerateRequest, not the corresponding
    // pull operation.
    //

    OperationAggregate* poA= new OperationAggregate(
        new CIMEnumerateInstanceNamesRequestMessage(*internalRequest),
        internalRequest->className,
        internalRequest->nameSpace,
        providerInfos.providerCount,
        true,           // completeHostAndNamespace = false
        false);         // No property List for this operation

    PEGASUS_ASSERT(enumerationContext->valid());  // EXP_PULL_TEMP

    //
    // Set Open... operation parameters into the operationAggregate
    // Includes setting namespace that is used to complete host and naespace
    // in responses
    //
    poA->setPullOperation((void *)enumerationContext);

    PEGASUS_ASSERT(enumerationContext->valid());  // KS_TEMP

    // gather up the repository responses and send it to out as one response
    // with many instances
    //
    if (_enumerateFromRepository(internalRequest, poA, providerInfos))
    {
        CIMResponseMessage* response = poA->removeResponse(0);
        _forwardResponseForAggregation(
            new CIMEnumerateInstanceNamesRequestMessage(*internalRequest),
            poA, response);
    }

    // Temp build empty response to the open request
    AutoPtr<CIMOpenEnumerateInstancePathsResponseMessage> openResponse(
        dynamic_cast<CIMOpenEnumerateInstancePathsResponseMessage*>(
            request->buildResponse()));

    ProviderRequests::issueEnumerationRequests(
        this,
        internalRequest,
        providerInfos,
        poA);

    // Issue the Response to the Open Request. Note that this may cause
    // wait for the enumeration cache before the response is issued.
    // gets response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete.
    // If there was an error in the provider responses, puts the exception
    // into the response
    ProviderRequests::issueOpenResponseMessage(
       this,
       request,
       openResponse,
       enumerationContext,
       "OpenEnumerateInstancePaths",
       operationMaxObjectCount);

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
        "OpenReferenceInstances request namespace=%s object=%s "
            "resultClass=%s "
            "role=%s "
            "includeClassOrigin=%s "
            "propertyList=%s "
            "filterQueryLanguage=%s "
            "filterQuery=%s "
            "operationTimeout=%s "
            "continueOnError=%s "
            "maxObjectCount=%u",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString()),
        CSTRING(request->resultClass.getString()),
        CSTRING(request->role),
        boolToString(request->includeClassOrigin),
        CSTRING(request->propertyList.toString()),
        CSTRING(request->filterQueryLanguage),
        CSTRING(request->filterQuery),
        CSTRING(request->operationTimeout.toString()),
        boolToString(request->continueOnError),
        request->maxObjectCount ));

    //// KS_TODO Remove once validated.
    PEGASUS_ASSERT(request->className == request->objectName.getClassName());

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

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED

    if (_rejectNoProvidersOrRepository(request, providerInfos))
    {
        PEG_METHOD_EXIT();
        return;
    }

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

    // Build Open Response message. We are going to issue some sort of
    // response whether there are providers, repository returns or not.
    AutoPtr<CIMOpenReferenceInstancesResponseMessage> openResponse;
    openResponse.reset(dynamic_cast<CIMOpenReferenceInstancesResponseMessage*>(
        request->buildResponse()));

    // if there are no providers and nothing from repository
    // return empty and exit
    // KS_TODO We should be able to drop through to normal code
    // and drop this completely.

    if ((providerInfos.providerCount == 0) && (cimObjects.size() == 0))
    {
        // We have no providers to call and nothing from repository.
        // Return empty

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "Repository Empty, NO providers for %s",
            (const char *)request->className.getString().getCString()));

        openResponse->endOfSequence = true;
        _enqueueResponse(request, openResponse.release());

        PEG_METHOD_EXIT();
        return;
    }

    //
    // Process as multi operation enumeration. 1. Build context, build
    // internalRequest for providers and issue to providers, build open
    // response, get current objects (with wait) and enqueue open
    // response.
    //

    // Build corresponding CIMReferencesRequestMessage to issue to
    // providers. We do not pass the Open operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers. NOTE: includeQualifiers NOT part of Pull operation

    CIMReferencesRequestMessage* internalRequest =
        new CIMReferencesRequestMessage(
            request->messageId,
            request->nameSpace,
            request->objectName,
            request->resultClass,
            request->role,
            false,              // force includeQualifiers to false
            request->includeClassOrigin,
            request->propertyList,
            request->queueIds,
            false,              // Not a class request
            request->authType,
            request->userName);

    internalRequest->operationContext = request->operationContext;
    internalRequest->internalOperation = true;

    // AutoPtr to delete at end of handler
    AutoPtr<CIMReferencesRequestMessage> dummy(internalRequest);

    // Create new context object.
    EnumerationContext* enumerationContext =
        enumerationContextTable.createContext(
            request->nameSpace,
            request->operationTimeout,
            request->continueOnError,
            CIM_PULL_INSTANCES_WITH_PATH_REQUEST_MESSAGE,
            CIMResponseData::RESP_OBJECTS);

    enumerationContext->setRequestProperties(
        request->includeClassOrigin, request->propertyList);

    //
    // Set up an aggregate object and save the original request message
    //
    OperationAggregate* poA = new OperationAggregate(
        new CIMReferencesRequestMessage(*internalRequest),
        internalRequest->objectName.getClassName(),
        internalRequest->nameSpace,
        providerInfos.providerCount,
        true,   // requiresHostnameCompletion
        true);  // hasPropList

    //
    // Set Open... operation parameters into the operationAggregate
    // since operationAggregate is what is  returned from providers.
    //
    poA->setPullOperation((void *)enumerationContext);

    // If response from repository not empty, forward for aggregation.
    if (cimObjects.size() != 0)
    {
        AutoPtr<CIMReferencesResponseMessage> internalResponse;
        internalResponse.reset(dynamic_cast<CIMReferencesResponseMessage*>(
            internalRequest->buildResponse()));
        internalResponse->getResponseData().setObjects(cimObjects);

        poA->incTotalIssued();

        // Send the repository results for aggregation
        // directly to callback (includes response).
        //
        _forwardResponseForAggregation(
            new CIMReferencesRequestMessage(*internalRequest),
            poA,
            internalResponse.release());

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenReferenceInstances repository _forwardForAggregation."
        " ProviderCount= %u total issued = %u",
        providerInfos.providerCount, poA->getTotalIssued()));
    }

    ProviderRequests::issueOpenAssocRequestMsgsToProviders(
        this,
        internalRequest,
        providerInfos,
        enumerationContext,
        poA,
        "OpenReferenceInstances"
        );

    // Issue the Response to the Open Request. Note that this may cause
    // wait for the enumeration cache before the response is issued.
    // gets response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete.
    // If there was an error in the provider responses, puts the exception
    // into the response
    ProviderRequests::issueOpenResponseMessage(
       this,
       request,
       openResponse,
       enumerationContext,
       "OpenReferenceInstances",
       operationMaxObjectCount);

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
        "OpenReferenceInstancePaths request for Namespace=%s objectName %s "
            "resultClass=%s "
            "role=%s "
            "filterQueryLanguage=%s "
            "filterQuery=%s "
            "operationTimeout=%s "
            "continueOnError=%s "
            "maxObjectCount=%u",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString()),
        CSTRING(request->resultClass.getString()),
        CSTRING(request->role),
        CSTRING(request->filterQueryLanguage),
        CSTRING(request->filterQuery),
        CSTRING(request->operationTimeout.toString()),
        boolToString(request->continueOnError),
        request->maxObjectCount ));

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
    // Build Open Response message.
    AutoPtr<CIMOpenReferenceInstancePathsResponseMessage> openResponse;
    openResponse.reset(
        dynamic_cast<CIMOpenReferenceInstancePathsResponseMessage*>(
            request->buildResponse()));

    if (providerInfos.providerCount == 0 && instanceNames.size() == 0)
    {
        //
        // No provider is registered and the repository isn't the
        // default.  Return CIM_ERR_NOT_SUPPORTED.

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
            "OpenReferenceInstancePaths Rtn Empty."));

        openResponse->endOfSequence = true;
        _enqueueResponse(request, openResponse.release());

        PEG_METHOD_EXIT();
        return;
    }

    // Build corresponding EnumerateInstancePathsRequest to send to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers. NOTE: includeQualifiers NOT part of Pull operation

    CIMReferenceNamesRequestMessage* internalRequest =
        new CIMReferenceNamesRequestMessage(
            request->messageId,
            request->nameSpace,
            request->objectName,
            request->resultClass,
            request->role,
            request->queueIds,
            false,
            request->authType,
            request->userName);

    internalRequest->operationContext = request->operationContext;

    internalRequest->internalOperation = true;

    // AutoPtr to delete at end of handler
    AutoPtr<CIMReferenceNamesRequestMessage> dummy(internalRequest);

    //
    // Create new enumerationContext and enumerationContextString.
    //
    EnumerationContext* enumerationContext =
        enumerationContextTable.createContext(
            request->nameSpace,
            request->operationTimeout,
            request->continueOnError,
            CIM_PULL_INSTANCE_PATHS_REQUEST_MESSAGE,
            CIMResponseData::RESP_OBJECTPATHS);

    /// KS_TODO should we have to do this here or is this default?
    enumerationContext->setRequestProperties(
        false, CIMPropertyList());
    //
    // Set up an aggregate object and save the created internalRequest
    //
    OperationAggregate* poA = new OperationAggregate(
        new CIMReferenceNamesRequestMessage(*internalRequest),
        internalRequest->objectName.getClassName(),
        internalRequest->nameSpace,
        providerInfos.providerCount,
        true,          // completeHostAndNamespace = false
        false);        // No property list for this operation

    (enumerationContext->valid());  // EXP_PULL_TEMP KS_TODO

    //
    // Set Open... operation parameters into the operationAggregate
    //
    poA->setPullOperation((void *)enumerationContext);

    // If any return from repository, send it to aggregator.
    // Send repository response for aggregation
    if (instanceNames.size() != 0)
    {
        AutoPtr<CIMReferenceNamesResponseMessage> internalResponse;
        internalResponse.reset(dynamic_cast<CIMReferenceNamesResponseMessage*>(
            internalRequest->buildResponse()));

        internalResponse->getResponseData().setInstanceNames(instanceNames);

        poA->incTotalIssued();
        // send the repository's results for aggregation
        // directly to callback (includes response).
        _forwardResponseForAggregation(
            new CIMReferenceNamesRequestMessage(*internalRequest),
            poA,
            internalResponse.release());
    }

     ProviderRequests::issueOpenAssocRequestMsgsToProviders(
        this,
        internalRequest,
        providerInfos,
        enumerationContext,
        poA,
        "OpenReferenceInstancePaths"
        );

    // Issue the Response to the Open Request. Note that this may cause
    // wait for the enumeration cache before the response is issued.
    // gets response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete.
    // If there was an error in the provider responses, puts the exception
    // into the response
    ProviderRequests::issueOpenResponseMessage(
       this,
       request,
       openResponse,
       enumerationContext,
       "OpenReferenceInstancePaths",
       operationMaxObjectCount);
    PEG_METHOD_EXIT();
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
        "OpenAssociatorInstancesRequest request namespace=%s object=%s "
            "assocClass = \"%s\"  "
            "resultClass=%s "
            "role=%s "
            "resultRole = \"%s\"  "
            "includeClassOrigin=%s "
            "propertyList=%s "
            "filterQueryLanguage=%s "
            "filterQuery=%s "
            "operationTimeout=%s "
            "continueOnError=%s "
            "maxObjectCount=%u",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString()),
        CSTRING(request->assocClass.getString()),
        CSTRING(request->resultClass.getString()),
        CSTRING(request->role),
        CSTRING(request->resultClass.getString()),
        boolToString(request->includeClassOrigin),
        CSTRING(request->propertyList.toString()),
        CSTRING(request->filterQueryLanguage),
        CSTRING(request->filterQuery),
        CSTRING(request->operationTimeout.toString()),
        boolToString(request->continueOnError),
        request->maxObjectCount ));

    if (_rejectAssociationTraversalDisabled(request, "OpenAssociatorInstances"))
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

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED

    if (_rejectNoProvidersOrRepository(request, providerInfos))
    {
        PEG_METHOD_EXIT();
        return;
    }

    //
    // Get the instances from the repository, as necessary.
    // Hold the repository results in a response message.
    // If not using the repository, this pointer is null.
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
            "OpenAssociatorInstances repository access: class = %s,"
                " count = %u.",
                CSTRING(request->objectName.toString()),
                cimObjects.size() ));
    }

    // Build OpenResponse Message
    AutoPtr<CIMOpenAssociatorInstancesResponseMessage> openResponse;
    openResponse.reset(dynamic_cast<CIMOpenAssociatorInstancesResponseMessage*>(
        request->buildResponse()));

    // if there are no providers and nothing from repository
    // return with endOfSequence true. Do direct enqueue response.
    //
    // // KS TODO can we completely bypass this code and just use
    // the main delivery path even if the repository returns nothing?

    if ((providerInfos.providerCount == 0) && (cimObjects.size() == 0))
    {
        //
        // We have no providers to call.  Just return what we have.
        //

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "OpenAssociatorInstances. No Objects found for %s",
            (const char *)request->className.getString().getCString()));

        openResponse->endOfSequence = true;
        _enqueueResponse(request, openResponse.release());

        PEG_METHOD_EXIT();
        return;
    }

    //
    // Process as multi operation enumeration. 1. Build context, build
    // internalRequest for providers and issue to providers, build open
    // response, get current objects (with wait) and enqueue open
    // response.
    //

    // Build corresponding CIMReferencesRequestMessage to issue to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers. NOTE: includeQualifiers NOT part of Pull operation

    CIMAssociatorsRequestMessage* internalRequest =
        new CIMAssociatorsRequestMessage(
            request->messageId,
            request->nameSpace,
            request->objectName,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole,
            false,          // force includeQualifiers to false
            request->includeClassOrigin,
            request->propertyList,
            request->queueIds,
            false,          // By definition not a class request
            request->authType,
            request->userName);

    internalRequest->operationContext = request->operationContext;

    internalRequest->internalOperation = true;

    // AutoPtr to delete at end of handler
    // KS_TODO Clean this one up
    AutoPtr<CIMAssociatorsRequestMessage> dummy(internalRequest);


    // Create a new enumeration context
    EnumerationContext* enumerationContext =
        enumerationContextTable.createContext(
            request->nameSpace,
            request->operationTimeout,
            request->continueOnError,
            CIM_PULL_INSTANCES_WITH_PATH_REQUEST_MESSAGE,
            CIMResponseData::RESP_OBJECTS);

    enumerationContext->setRequestProperties(
        request->includeClassOrigin, request->propertyList);

    //
    // Set up an aggregate object and save the original request message
    //

    OperationAggregate* poA = new OperationAggregate(
        new CIMAssociatorsRequestMessage(*internalRequest),
        internalRequest->objectName.getClassName(),
        internalRequest->nameSpace,
        providerInfos.providerCount,
        true,             // completeHostAndNamespace = false
        true);            // Operation has property list

    //
    // Set Open... operation parameters into the operationAggregate
    //
    poA->setPullOperation((void *)enumerationContext);

    // Send repository response for aggregation

    if (cimObjects.size() != 0)
    {
        AutoPtr<CIMAssociatorsResponseMessage> internalResponse;
        internalResponse.reset(dynamic_cast<CIMAssociatorsResponseMessage*>(
            internalRequest->buildResponse()));

        internalResponse->getResponseData().setObjects(cimObjects);

        poA->incTotalIssued();

        // send the repository's results for aggregation
        // directly to callback (i.e. includes response).

        _forwardResponseForAggregation(
            new CIMAssociatorsRequestMessage(*internalRequest),
            poA,
            internalResponse.release());

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenAssociatorInstances repository _forwardForAggregation."
        " ProviderCount= %u total issued = %u",
        providerInfos.providerCount,  poA->getTotalIssued()));
    }

    ProviderRequests::issueOpenAssocRequestMsgsToProviders(
        this,
        internalRequest,
        providerInfos,
        enumerationContext,
        poA,
        "OpenAssociatorInstances"
        );

    // Issue the Response to the Open Request. Note that this may cause
    // wait for the enumeration cache before the response is issued.
    // gets response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete.
    // If there was an error in the provider responses, puts the exception
    // into the response
    ProviderRequests::issueOpenResponseMessage(
       this,
       request,
       openResponse,
       enumerationContext,
       "OpenAssociatorInstances",
       operationMaxObjectCount);

    PEG_METHOD_EXIT();
}

/**$*******************************************************
    handleOpenAssociatorInstancePathsRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleOpenAssociatorInstancePathsRequest(
    CIMOpenAssociatorInstancePathsRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::"
        "handleOpenAssociatorInstancePathssRequest");


    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "OpenAssociatorInstancePaths request namespace=%s object=%s "
            "assocClass = %s "
            "resultClass=%s "
            "role=%s "
            "resultRole = %s "
            "filterQueryLanguage=%s "
            "filterQuery=%s "
            "operationTimeout=%s "
            "continueOnError=%s "
            "maxObjectCount=%u",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->objectName.toString()),
        CSTRING(request->assocClass.getString()),
        CSTRING(request->resultClass.getString()),
        CSTRING(request->role),
        CSTRING(request->resultRole),
        CSTRING(request->filterQueryLanguage),
        CSTRING(request->filterQuery),
        CSTRING(request->operationTimeout.toString()),
        boolToString(request->continueOnError),
        request->maxObjectCount ));

    //// KS_TODO Remove once validated.
    PEGASUS_ASSERT(request->className == request->objectName.getClassName());

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

    // Build OpenResponse Message
    AutoPtr<CIMOpenAssociatorInstancePathsResponseMessage> openResponse;
    openResponse.reset(
        dynamic_cast<CIMOpenAssociatorInstancePathsResponseMessage*>(
            request->buildResponse()));


    if (providerInfos.providerCount == 0 && objectNames.size() == 0)
    {
        //
        // No provider is registered and the repository isn't the
        // default.  Return CIM_ERR_NOT_SUPPORTED.
        //

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
            "OpenAssociatorInstancePaths. No Objects found for %s",
            CSTRING(request->className.getString()) ));

        openResponse->endOfSequence = true;
        _enqueueResponse(request, openResponse.release());

        PEG_METHOD_EXIT();
        return;
    }
    //
    // Process as multi operation enumeration. 1. Build context, build
    // internalRequest for providers and issue to providers, build open
    // response, get current objects (with wait) and enqueue open
    // response.
    //
    // Build corresponding EnumerateInstancePathsRequest to send to
    // providers. We do not pass the Pull operations request
    // on to Providers but use the EnumerateInstancesRequest message to
    // activate providers. NOTE: includeQualifiers NOT part of Pull operation

    CIMAssociatorNamesRequestMessage* internalRequest =
        new CIMAssociatorNamesRequestMessage(
            request->messageId,
            request->nameSpace,
            request->objectName,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole,
            request->queueIds,
            false,
            request->authType,
            request->userName);

    internalRequest->operationContext = request->operationContext;
    internalRequest->internalOperation = true;

    AutoPtr<CIMAssociatorNamesRequestMessage> dummy(internalRequest);

    //
    // Create new enumerationContext and enumerationContextString.
    //
    EnumerationContext* enumerationContext =
        enumerationContextTable.createContext(
            request->nameSpace,
            request->operationTimeout,
            request->continueOnError,
            CIM_PULL_INSTANCE_PATHS_REQUEST_MESSAGE,
            CIMResponseData::RESP_OBJECTPATHS);

    //
    // Set up an aggregate object and save the created internalRequest
    //

    OperationAggregate* poA = new OperationAggregate(
        new CIMAssociatorNamesRequestMessage(*internalRequest),
        internalRequest->objectName.getClassName(),
        internalRequest->nameSpace,
        providerInfos.providerCount,
        true,                  // completeHostAndNamespace = false
        false);                // No property list for this operation

    //
    // Set Open... operation parameters into the operationAggregate
    //
    poA->setPullOperation((void *)enumerationContext);

    // If any return from repository, send it to aggregator.
    if (objectNames.size() != 0)
    {
        AutoPtr<CIMAssociatorNamesResponseMessage> internalResponse;
        internalResponse.reset(dynamic_cast<CIMAssociatorNamesResponseMessage*>(
            internalRequest->buildResponse()));
        internalResponse->getResponseData().setInstanceNames(objectNames);

        poA->incTotalIssued();
        // send the repository's results for aggregation
        // directly to callback (includes response).
        _forwardResponseForAggregation(
            new CIMAssociatorNamesRequestMessage(*internalRequest),
            poA,
            internalResponse.release());

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
        "OpenAssociatorInstancePaths 8. ProviderCount = %u",
        providerInfos.providerCount));
    }

    ProviderRequests::issueOpenAssocRequestMsgsToProviders(
        this,
        internalRequest,
        providerInfos,
        enumerationContext,
        poA,
        "OpenAssociatorInstancePaths"
        );

    // Issue the Response to the Open Request. Note that this may cause
    // wait for the enumeration cache before the response is issued.
    // gets response data from the cache up to maxObjectCount and return
    // it in a new CIMResponseData object. This function waits for
    // sufficient objects in cache or providers complete.
    // If there was an error in the provider responses, puts the exception
    // into the response
    ProviderRequests::issueOpenResponseMessage(
       this,
       request,
       openResponse,
       enumerationContext,
       "OpenAssociatorInstancesPaths",
       operationMaxObjectCount);

    PEG_METHOD_EXIT();
}

/**$**************************************************************************
**
**    handleOpenQueryInstancesRequest
**
*****************************************************************************/

void CIMOperationRequestDispatcher::handleOpenQueryInstancesRequest(
    CIMOpenQueryInstancesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleOpenQueryInstancesRequest");

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "OpenQueryInstances request namespace=%s "
            "queryLanguage=%s "
            "query=%s "
            "returnQueryResultClass=%s "
            "operationTimeout=%s "
            "continueOnError=%s "
            "maxObjectCount=%u ",
        CSTRING(request->nameSpace.getString()),
        CSTRING(request->queryLanguage),
               CSTRING(request->query),
        boolToString(request->returnQueryResultClass),
        CSTRING(request->operationTimeout.toString()),
        boolToString(request->continueOnError),
        request->maxObjectCount ));

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

    AutoPtr<CIMExecQueryResponseMessage> response(
        dynamic_cast<CIMExecQueryResponseMessage*>(
            request->buildResponse()));

    Boolean exception = false;

#ifdef PEGASUS_DISABLE_EXECQUERY
    response->cimException =
        PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
    exception=true;
#endif
    // KS_TODO this operation currently not supported. It requires extending
    // The WQL and CQL dispatch support functions to interface with the
    // pull operations.
////  response->cimException =
////      PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);
////  exception=true;

    if (exception)
    {
        _enqueueResponse(request, response.release());
        PEG_METHOD_EXIT();
        return;
    }
    // Create the corresponding CIMExecQueryRequestMessage
    CIMExecQueryRequestMessage* internalRequest =
        new CIMExecQueryRequestMessage(
            request->messageId,
            request->nameSpace,
            request->queryLanguage,
            request->query,
            request->queueIds,
            request->authType,
            request->userName);

    internalRequest->operationContext = request->operationContext;

    // AutoPtr to delete enumRequest at end of handler
    AutoPtr<CIMExecQueryRequestMessage> dummy(internalRequest);

    //
    // Setup the EnumerationContext. Returns pointer to object and context ID
    // string
    //

    EnumerationContext* enumerationContext =
        enumerationContextTable.createContext(
            request->nameSpace,
            request->operationTimeout,
            request->continueOnError,
            CIM_PULL_INSTANCES_WITH_PATH_REQUEST_MESSAGE,
            CIMResponseData::RESP_INSTANCES);

////  enumerationContext->setRequestProperties(
////      request->includeClassOrigin, request->propertyList);
      //// This moved to query processor
////  poA->setPullOperation((void *)enumerationContext);

    //// KS_TEMP KS_TODO REMOVE debugging code. Delete. OCT2011
    if (enumerationContext->responseCacheSize() != 0)
    {
        enumerationContext->valid();
        enumerationContext->trace();
        enumerationContextTable.trace();
    }

    PEGASUS_ASSERT(enumerationContext->responseCacheSize() == 0);  // KS_TEMP

    if (QuerySupportRouter::routeHandleExecQueryRequest(
       this, internalRequest, (void*)enumerationContext) == false)
    {
        if (internalRequest->operationContext.contains(
                SubscriptionFilterConditionContainer::NAME))
        {
            SubscriptionFilterConditionContainer sub_cntr =
                internalRequest->operationContext.get(
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

        // Build empty open response

    AutoPtr<CIMOpenQueryInstancesRequestMessage> openResponse(
        dynamic_cast<CIMOpenQueryInstancesRequestMessage*>(
            request->buildResponse()));
////  Generates error for missing cimException, getResponseData endOfSequence
//// enumerationContext not partof Pegasus::CIMOpenQueryInstancesRequestMessage
////  ProviderRequests::issueOpenResponseMessage(
////     this,
////     request,
////     openResponse,
////     enumerationContext,
////     "OpenExecQuery",
////     operationMaxObjectCount);

    PEG_METHOD_EXIT();
    return;
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

    EnumerationContext* en = enumerationContextTable.find(
       request->enumerationContext);

    // test for invalid context and if found, error out.
    if (_rejectInValidEnumerationContext(request, en))
    {
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
    handlePullInstancesWithPath
************************************************************/

void CIMOperationRequestDispatcher::handlePullInstancesWithPath(
    CIMPullInstancesWithPathRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handlePullInstancesWithPath");

    // Both pull operations execute off of a single templated function.
    // Build and send response.  getCache used to wait for objects
    AutoPtr<CIMPullInstancesWithPathResponseMessage> response(
        dynamic_cast<CIMPullInstancesWithPathResponseMessage*>(
            request->buildResponse()));
        enumerationContextTable.trace();
    ProviderRequests::processPullRequest(this, request, response,
        "PullInstancesWithPath");
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
        enumerationContextTable.trace();
    ProviderRequests::processPullRequest(this, request, response,
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
        (const char*)request->enumerationContext.getCString() ));

    EnumerationContext* en = enumerationContextTable.find(
       request->enumerationContext);

    if (_rejectInValidEnumerationContext(request, en))
    {
        PEG_METHOD_EXIT();
        return;
    }
    // lock the context until we have set processing state to avoid
    // conflict with timer thread.
    {
        AutoMutex contextLock(en->_contextLock);

        if (_rejectIfContextTimedOut(request, en->isTimedOut()))
        {
            PEG_METHOD_EXIT();
            return;
        }

        // If another operation is active for this context, reject this
        // operation.
        // The specification allows as an option concurrent close (close
        // while pull request active) but we do not for now.  Complicates the
        // code to much for right now.
        if (_rejectIfEnumerationContextProcessing(request, en->isProcessing()))
        {
            PEG_METHOD_EXIT();
            return;
        }
        if (_rejectIfContextTimedOut(request, en->isTimedOut()))
        {
            PEG_METHOD_EXIT();
            return;
        }
        // Stop interoperation timer. Turns off the
        // interoperation timeout thread for this context.
        en->stopTimer();
    }

    // Set the Enumeration Closed. No more requests will be accepted
    // for this enumerationContext
    en->setClientClosed();

    // KS_TODO At this point, provider complete operation could sneak in
    // and remove context since client closed. We need to protect against
    // that. Probably by not setting client closed yet. or by getting the
    // context to local variable before we close.

    // Confirm that the providers are complete and if not
    // to force process when they are complete.
    if (en->ifProvidersComplete())
    {
        //// KS_TODO get rid of display
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
           "Close Operation. Providers complete, Close enumeration"));
        // KS_TODO remove this directly, not with find.
        enumerationContextTable.removeCxt(request->enumerationContext, true);
    }
    else
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
           "Close Operation. Providers not complete, Close enumeration"));
    }

    AutoPtr<CIMCloseEnumerationResponseMessage> response(
        dynamic_cast<CIMCloseEnumerationResponseMessage*>(
            request->buildResponse()));

    response->cimException = CIMException();

    _enqueueResponse(request, response.release());

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
       "Close Operation. Send Response"));
    PEG_METHOD_EXIT();
    return;
}

//KS_PULL_END

/**************************************************************************/
// End of the Pull operation functions
/**************************************************************************/

//
//   Return Aggregated responses back to the Correct Aggregator
//
//   The aggregator includes an aggregation object that is used to
//   accumulate responses.  It is attached to each request sent and
//   received back as part of the response call back in the "parm"
//   Responses are aggregated until the count reaches the sent count and
//   then the aggregation code is called to create a single response from
//   the accumulated responses.
//   It uses information from the OperationAggregate to execute
//   special functions such as fixing the property list and
//   settiing hostName, etc.
//
/*********************************************************************/


void CIMOperationRequestDispatcher::handleOperationResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::"
            "handleOperationResponseAggregation");

    CIMResponseDataMessage* toResponse =
        (CIMResponseDataMessage*) poA->getResponse(0);
    PEG_TRACE(( TRC_DISPATCHER, Tracer::LEVEL3,
        "CIMOperationRequestDispatcher - "
            "Request Type %s Response Type %s"
            "Namespace: %s  Class name: %s Response Count: %u",
        MessageTypeToString(poA->_msgRequestType),
        MessageTypeToString(toResponse->getType()),
        CSTRING(poA->_nameSpace.getString()),
        CSTRING(poA->_className.getString()),
        poA->numberResponses()));

//// KS_TODO temporary while we finish pull testing
    PEG_TRACE(( TRC_DISPATCHER, Tracer::LEVEL4,
        "CIMOperationRequestDispatcher::handleOperationResponseAggregation"
        " -  Type %s requiresHostnameCompletion : %s _hasPropList: %s",
        MessageTypeToString(poA->_msgRequestType),
        (poA->_requiresHostnameCompletion == true ? "true" : "false"),
        (poA->_hasPropList == true ? "true" : "false")));

    CIMResponseData & to = toResponse->getResponseData();

    // Re-add the property list as stored from request after deepInheritance fix
    // since on OOP on the response message the property list gets lost
    // This is only done for EnumerateInstances type requests
    if (poA->_hasPropList)
    {
        switch (poA->_msgRequestType)
        {
            case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
            {
                CIMEnumerateInstancesRequestMessage* request =
                    (CIMEnumerateInstancesRequestMessage*)poA->getRequest();
                to.setPropertyList(request->propertyList);
                break;
            }
            case CIM_ASSOCIATORS_REQUEST_MESSAGE:
            {
                CIMAssociatorsRequestMessage* request =
                    (CIMAssociatorsRequestMessage*)poA->getRequest();
                to.setPropertyList(request->propertyList);
                break;
            }
            case CIM_REFERENCES_REQUEST_MESSAGE:
            {
                CIMReferencesRequestMessage* request =
                    (CIMReferencesRequestMessage*)poA->getRequest();
                to.setPropertyList(request->propertyList);
                break;
            }
            default:
            {
                PEG_TRACE(( TRC_DISPATCHER, Tracer::LEVEL4,
                    "CIMOperationRequestDispatcher::"
                    "handleOperationResponseAggregation "
                    "ERRORNOTHANDLINGPROPERTYLIST -  Type %s"
                    "requiresHostnameCompletion : %s _hasPropList: %s",
                    MessageTypeToString(poA->_msgRequestType),
                    (poA->_requiresHostnameCompletion ==
                      true ? "true" : "false"),
                    (poA->_hasPropList == true ? "true" : "false")));
            }
        }
    }

    // Work backward and delete each response off the end of the array
    // and append it to the new responseData
    for (Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
        CIMResponseDataMessage* fromResponse =
            (CIMResponseDataMessage*)poA->getResponse(i);
        CIMResponseData & from = fromResponse->getResponseData();
        to.appendResponseData(from);
        poA->deleteResponse(i);
    }

    if (poA->_requiresHostnameCompletion)
    {
        // fill in host, namespace on all instances on all elements of array
        // if they have been left out. This is required because XML reader
        // will fail without them populated
        to.completeHostNameAndNamespace(System::getHostName(),
            poA->_nameSpace, poA->_pullOperation);
    }
    to.traceResponseData();
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
                //
                // NULLness check is done as getFunctPtr may reurn NULL in case
                // of when querry language is not CQL, WQL
                if ( applyQueryToEnumeration)
                {
                    applyQueryToEnumeration(response, poA->_query);
                }

                CIMEnumerateInstancesResponseMessage* fromResponse =
                    (CIMEnumerateInstancesResponseMessage*) response;
                CIMClass cimClass;

                Boolean clsRead = false;
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

//Issue  enumerateInstance...ForClass calls to the repository for each class
// that doesn ot have a provider and put the returned responses into the
// ResponseData.
// Handles exceptions from the repository inserting them as CIMException
// into the response.
// This is common code for all Enumerates to the repository because they
// all use the same pattern, doing a enumerate...ForClass loop to get
// the instances/paths for each class that does not have a provider.
//
// TODO: Do we really need the exceptions here?  Actually, review to see
// if we really need the enumerate for classes itself and could not just
// use enumerateInstances Since there should NEVER be anything for both
// provider and repository.
//
// Returns a single response in the OperationAggregate object with the
// counters properly set up.
// This function throws an exception of there is an exception from the
// repository.
//

Boolean CIMOperationRequestDispatcher::_enumerateFromRepository(
    CIMOperationRequestMessage* request,
    OperationAggregate* poA,
    ProviderInfoList& providerInfos)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_enumerateFromRepository");
    Boolean responsesExist = false;
    // Create AutoPtr to force delete if exception
    AutoPtr<OperationAggregate> lPoA(poA);

    if (_repository->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        while (providerInfos.hasMore(false))
        {
            ProviderInfo& providerInfo = providerInfos.getNext();
            // If this class is not registered for provider, get instances
            // from repository
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "Routing  request for class %s to the "
                    "repository.  Class # %u of %u",
               CSTRING(providerInfo.className.getString()),
               (providerInfos.getIndex()), providerInfos.size() ));

            switch (request->getType())
            {
                case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
                {
                    const CIMEnumerateInstancesRequestMessage* req =
                        reinterpret_cast<const
                            CIMEnumerateInstancesRequestMessage*>(request);
                    // create response from request
                    AutoPtr<CIMEnumerateInstancesResponseMessage> response(
                        dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
                            req->buildResponse()));

                    // Enumerate instances only for this class
                    response->getResponseData().setInstances(
                        _repository->enumerateInstancesForClass(
                            req->nameSpace,
                            providerInfo.className,
                            req->includeQualifiers,
                            req->includeClassOrigin,
                            req->propertyList));

                    poA->appendResponse(response.release());
                    break;
                }
                case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
                {
                    const CIMEnumerateInstanceNamesRequestMessage* req =
                        reinterpret_cast<const
                            CIMEnumerateInstanceNamesRequestMessage*>(request);
                    // create response from request
                    AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response(
                        dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
                            req->buildResponse()));

                    // Enumerate instances only for this class
                    response->getResponseData().setInstanceNames(
                        _repository->enumerateInstanceNamesForClass(
                            req->nameSpace,
                            providerInfo.className));

                    poA->appendResponse(response.release());
                    break;
                }

                default:
                {
                    PEGASUS_UNREACHABLE(PEGASUS_ASSERT(0));
                }
            } // switch statement
        }  // while loop

        // Set the local number of expected responses counter
        responsesExist = (poA->numberResponses() != 0);

        if (responsesExist)
        {
            // Aggregate the responses into a single response object
            handleOperationResponseAggregation(poA);
            poA->incTotalIssued();
        }
    }

    // release poA and exit with flag indicating responses exist or not
    lPoA.release();
    PEG_METHOD_EXIT();
    return responsesExist;
}

// Forward request to provider unless class does not exist
// If class does not exist, return false
// KS_TODO Possibly change this to always send.  Sends exception message if
// class get error. No need for Boolean Return
Boolean CIMOperationRequestDispatcher::_forwardEnumerationToProvider(
    ProviderInfo &providerInfo,
    OperationAggregate* poA,
    CIMOperationRequestMessage* request)
{
    CIMException checkClassException;

    CIMConstClass cimClass = _getClass(
        poA->_nameSpace,
        providerInfo.className,
        checkClassException);

    // The following is not clean. Need better way to terminate.
    // This builds an error response message in the middle of the
    // normal response but lets rest of operation continue.
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        //// TODO we are building from a generic, not a specific type.
        cout << "Exception from _forwardEnumerationToProvider" << endl;
        CIMResponseMessage* response = request->buildResponse();
        response->cimException = checkClassException;
        _forwardResponseForAggregation(request,  poA, response);
    }

    else
    {
        if (providerInfo.providerIdContainer.get() != 0)
        {
            request->operationContext.insert(
                *(providerInfo.providerIdContainer.get()));
        }

#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
        if (providerInfo.hasProviderNormalization)
        {
            request->operationContext.insert(
                CachedClassDefinitionContainer(cimClass));
        }
#endif
        _forwardAggregatingRequestToProvider(
           providerInfo, request, poA);
    }
    return true;
}

PEGASUS_NAMESPACE_END
