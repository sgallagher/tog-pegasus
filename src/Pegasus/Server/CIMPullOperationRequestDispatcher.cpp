/****************************************************************************
**
**          Temporary code that is really part of CIMOperationRequestDispatcher
**          Maintained as seperate file included in dispatcher.cpp
**          to keep editing simple during development.
**          KS_TODO - Move this code into CIMOperationRequestDispatcher.cpp
**
*****************************************************************************/

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
            "propertyList.size = \"%u\" . "
            "maxObjectCount = \"%s\" .  "
            "operationTimeout = \"%s\" .  ",
        (const char*)request->nameSpace.getString().getCString(),
        (const char*)request->className.getString().getCString(),
        request->propertyList.size(),
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

    // AutoPtr to delete at end of handler
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

    PEGASUS_ASSERT(enumerationContext->cacheSize() == 0);  // KS_TEMP
    //
    // Set up an aggregate object and save a copy of the original request.
    // NOTE: Build the poA for the EnumerateRequest, not the corresponding
    // pull operation.
    //
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

    // KS_TBD - Not sure we need this any more.
    //poA->setCountToDeliver(request->maxObjectCount);

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

            handleEnumerateInstancesResponseAggregation(poA);

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
               enumerationContext->cacheSize(),
               operationMaxObjectCount ));

    // Create a temporary response data with correct type.
    // KS_TODO - Should be able to allocate this object in the
    // getCacheResponseData function.
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
               enumerationContext->cacheSize() ));

    // tests for providers complete and cache empty. and sets response
    // endOfSequence
    if ((openResponse->endOfSequence =
         enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close Enumeration");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
        enumerationTable.remove(enumerationContext);
        enumerationTable.trace();
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

    // KS_PULL_TODO Think this can go away in future
    //poA->setCountToDeliver(request->maxObjectCount);

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
        enumerationTable.remove(enumerationContext);
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL3,
               "OpenResponse end-of-sequence %s",
               _toCharP(openResponse->endOfSequence)));

    _enqueueResponse(request, openResponse.release());

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
    // KS_TODO We should be able tos imply drop through to normal code
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
    if (cimObjects.size() != 0 || cimObjects.size() == 0)
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
               enumerationContext->cacheSize(),
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
               enumerationContext->cacheSize() ));

    // Do check here after we have processed the results of the get.
    // At this point we are current with the provider response status
    if ((openResponse->endOfSequence =
         enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close References");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
        enumerationTable.remove(enumerationContext);
    }

    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required for pull instances
    // because we are returning named instances.
    // KS_TBD _ This may be done as part of the aggregator also

    to.completeHostNameAndNamespace(
        cimAggregationLocalHost, request->nameSpace);

    _enqueueResponse(request, openResponse.release());

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

    // KS_TODO Think this is all obsolete.
    //poA->setCountToDeliver(request->maxObjectCount);

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4, // KS_PULL_TEMP
    "OpenReferenceInstancePaths 7a. ProviderCount = %u repository count = %u",
    providerInfos.providerCount, instanceNames.size()));

    // If any return from repository, send it to aggregator.
    Uint32 totalIssued = 0;
    if (instanceNames.size() != 0 || instanceNames.size() == 0)
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
               enumerationContext->cacheSize(),
               operationMaxObjectCount ));

    // Create a Response data based on what is in the cache now.
    // Create a temporary response data with correct type.
    // KS_TODO - Should be able to allocate this object in the
    // getCacheResponseData function.

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
               enumerationContext->cacheSize() ));

   if ((openResponse->endOfSequence =
        enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close ReferencePaths");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
        enumerationTable.remove(enumerationContext);
    }

    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required for pull instances
    // because we are returning named instances.
    // KS_TBD _ This may be done as part of the aggregator also

    to.completeHostNameAndNamespace(
        cimAggregationLocalHost, request->nameSpace);

    _enqueueResponse(request, openResponse.release());

    PEG_METHOD_EXIT();
}

/**$********************************************************
    handlePullInstancesWithPath
************************************************************/

// KS_TODO - Testsingle template function that will process both
// pullInstancePaths and PullInstancesWithPath. This is now built and
// usage is enabled with the PULL_TEMPLATE_TEST flag below.
// Right now the template code causes errors.

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

    if (cimObjects.size() != 0 || cimObjects.size() == 0)
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
        "OpenRAssociatorInstances 8. ProviderCount = %u",
        providerInfos.providerCount));
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
               enumerationContext->cacheSize(),
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
               enumerationContext->cacheSize() ));

    // If the providers are complete close the enumeration. Else
    // prepare for the next operation by setting the inactive state
    // and starting the timer.
    // KS_TODO - WOuld like to move the close and remove to the
    // function also.
    if ((openResponse->endOfSequence =
         enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close AssociatorInstances");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
        enumerationTable.remove(enumerationContext);
    }

    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required for pull instances
    // because we are returning named instances.
    // KS_TBD _ This may be done as part of the aggregator also

    to.completeHostNameAndNamespace(
        cimAggregationLocalHost, request->nameSpace);

    _enqueueResponse(request, openResponse.release());

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
    if (objectNames.size() != 0 || objectNames.size() == 0)
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
               enumerationContext->cacheSize(),
               operationMaxObjectCount ));

    // Create a Response data based on what is in the cache now.
    // Create a temporary response data with correct type.
    // KS_TODO - Should be able to allocate this object in the
    // getCacheResponseData function.

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
                   enumerationContext->cacheSize() ));
    }

   if ((openResponse->endOfSequence =
        enumerationContext->ifEnumerationComplete()))
    {
        PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Close AssociatorInstancePaths");
        // delete the EnumerationContext object
        enumerationContext->setClosed();
        enumerationTable.remove(enumerationContext);
    }

    // fill in host, namespace on all instances on all elements of array
    // if they have been left out. This is required for pull instances
    // because we are returning named instances.
    // KS_TBD _ This may be done as part of the aggregator also

    to.completeHostNameAndNamespace(cimAggregationLocalHost,
        request->nameSpace);

    _enqueueResponse(request, openResponse.release());

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

    en->setClosed();

    // need to confirm that the providers are complete and if not
    // to force process when they are complete.
    if (en->ifProvidersComplete())
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
           "Close Operation. Providers complete, Close enumeration"));

        enumerationTable.remove(request->enumerationContext);
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

    PEG_METHOD_EXIT();
    return;
}

//KS_PULL_END



