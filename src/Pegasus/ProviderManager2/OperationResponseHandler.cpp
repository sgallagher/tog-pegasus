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
// Modified By:
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//
//%/////////////////////////////////////////////////////////////////////////////

#include "OperationResponseHandler.h"

#include <Pegasus/Common/Logger.h>

#include <Pegasus/ProviderManager2/ProviderManagerService.h>

PEGASUS_NAMESPACE_BEGIN

//
// OperationResponseHandler
//

OperationResponseHandler::OperationResponseHandler(
    CIMRequestMessage *request,
    CIMResponseMessage *response)
    : _request(request),
    _response(response),
    _responseObjectTotal(0),
    _responseMessageTotal(0),
	_responseObjectThreshold(0)
{
#ifndef PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD
 #define PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD 100
#elif PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD  == 0
 #undef PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD
 #define PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD  ~0
#endif

    if (!request || (request->requestIsOOP == true))
    {
        _responseObjectThreshold = ~0;
    }
	else
    {
        _responseObjectThreshold = PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD;

#ifdef PEGASUS_DEBUG
	    static const char *responseObjectThreshold =
		    getenv("PEGASUS_RESPONSE_OBJECT_COUNT_THRESHOLD");

        if (responseObjectThreshold)
	    {
		    Uint32 i = (Uint32)atoi(responseObjectThreshold);

            if (i > 0)
            {
                _responseObjectThreshold = i;
            }
	    }
#endif
    }
}

OperationResponseHandler::~OperationResponseHandler()
{
	_request = 0;
	_response = 0;
}

CIMRequestMessage * OperationResponseHandler::getRequest(void) const
{
    return(_request);
}

CIMResponseMessage * OperationResponseHandler::getResponse(void) const
{
    return(_response);
}

void OperationResponseHandler::setStatus(
    const Uint32 code,
    const String & message)
{
    _response->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
}

void OperationResponseHandler::setStatus(
    const Uint32 code,
    const ContentLanguages & langs,
    const String & message)
{
    _response->cimException =
        PEGASUS_CIM_EXCEPTION_LANG(
        langs,
        CIMStatusCode(code),
        message);
}

Boolean OperationResponseHandler::isAsync(void) const
{
    return(true);
}

// This is only called from SimpleResponseHandler.deliver() but lives in this
// class because all asyncronous response must have a "response" pointer
// to go through. Only operation classes have a response pointer
void OperationResponseHandler::send(Boolean isComplete)
{
	// some handlers do not send async because their callers cannot handle
	// partial responses. If this is the case, stop here.

	if (isAsync() == false)
	{
		// preserve tradional behavior
		if (isComplete == true)
        {
            transfer();
        }

        return;
	}

	SimpleResponseHandler *simpleP = dynamic_cast<SimpleResponseHandler*>(this);

	// It is possible to instantiate this class directly (not derived)
	// The caller would do this only if the operation does not have any data to
	// be returned

	if (! simpleP)
	{
		// if there is no data to be returned, then the message should NEVER be
		// incomplete (even on an error)
		if (isComplete == false)
        {
            PEGASUS_ASSERT(false);
        }

        return;
	}

	SimpleResponseHandler &simple = *simpleP;
	PEGASUS_ASSERT(_response);
	Uint32 objectCount = simple.size();

	// have not reached threshold yet
	if ((isComplete == false) && (objectCount < _responseObjectThreshold))
    {
        return;
    }

	CIMResponseMessage *response = _response;

	// for complete responses, just use the one handed down from caller
	// otherwise, create our own that the caller never sees but is
	// utilized for async responses underneath

	if (isComplete == false)
    {
        _response = _request->buildResponse();
    }

	_response->setComplete(isComplete);
	_responseObjectTotal += objectCount;

	// since we are reusing response for every chunk,keep track of original count
	_response->setIndex(_responseMessageTotal++);

	// set the originally allocated response to one more than the current.
	// The reason for doing this is proactive in case of an exception. This
	// allows the last response to be set as it may not re-enter this code.

	if (isComplete == false)
    {
        response->setIndex(_responseMessageTotal);
    }

	validate();

	if (_response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
        simple.clear();
    }

	String function = getClass() + "::" + "transfer";
	Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        function);

	transfer();
	simple.clear();

	// l10n
	_response->operationContext.set(ContentLanguageListContainer(simple.getLanguages()));

	// call thru ProviderManager to get externally declared entry point

	if (isComplete == false)
	{
		ProviderManagerService::handleCimResponse(*_request, *_response);
	}

	// put caller's allocated response back in place. Note that _response
	// is INVALID after sending because it has been deleted externally

	_response = response;
}

void OperationResponseHandler::transfer(void)
{
}

void OperationResponseHandler::validate(void)
{
}

String OperationResponseHandler::getClass(void) const
{
    return(String("OperationResponseHandler"));
}

Uint32 OperationResponseHandler::getResponseObjectTotal(void) const
{
    return(_responseObjectTotal);
}

Uint32 OperationResponseHandler::getResponseMessageTotal(void) const
{
    return(_responseMessageTotal);
}

Uint32 OperationResponseHandler::getResponseObjectThreshold(void) const
{
    return(_responseObjectThreshold);
}

//
// GetInstanceResponseHandler
//

GetInstanceResponseHandler::GetInstanceResponseHandler(
    CIMGetInstanceRequestMessage * request,
    CIMGetInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
{
    #ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    // Attempt to get the cached class definition used to validate results of this
    // operation. If it does not exist, then this feature is disabled for this
    // operation.
    CIMClass cimClass;

    try
    {
        CachedClassDefinitionContainer container =
            request->operationContext.get(CachedClassDefinitionContainer::NAME);

        cimClass = container.getClass();

    }
    catch(Exception &)
    {
        // Do nothing. Container is missing, which implies normalization is disabled
        // for this operation.
    }

    _normalizer =
        ObjectNormalizer(
            cimClass,
            request->includeQualifiers,
            request->includeClassOrigin);
    #endif
}

void GetInstanceResponseHandler::deliver(const CIMInstance & cimInstance)
{
    if(cimInstance.isUninitialized())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    if(SimpleInstanceResponseHandler::size() != 0)
    {
        MessageLoaderParms message(
            "Server.OperationResponseHandler.TOO_MANY_OBJECTS_DELIVERED",
            "Too many objects delivered.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    #ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    // The normalizer expects an object path embedded in instances even
    // though it is not required by this operation. Use the requested
    // object path is missing from the instance.
    CIMInstance localInstance(cimInstance);

    if(localInstance.getPath().getKeyBindings().size() == 0)
    {
        // ATTN: should clone before modification
        localInstance.setPath(static_cast<CIMGetInstanceRequestMessage *>(getRequest())->instanceName);
    }

    SimpleInstanceResponseHandler::deliver(_normalizer.processInstance(localInstance));
    #else
    SimpleInstanceResponseHandler::deliver(cimInstance);
    #endif
}

void GetInstanceResponseHandler::complete(void)
{
    if(SimpleInstanceResponseHandler::size() == 0)
    {
        MessageLoaderParms message(
            "Server.OperationResponseHandler.TOO_FEW_OBJECTS_DELIVERED",
            "Too few objects delivered.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleInstanceResponseHandler::complete();
}

String GetInstanceResponseHandler::getClass(void) const
{
    return(String("GetInstanceResponseHandler"));
}

void GetInstanceResponseHandler::transfer(void)
{
    if(size() > 0)
    {
        CIMGetInstanceResponseMessage & msg =
            *static_cast<CIMGetInstanceResponseMessage *>(getResponse());

        msg.cimInstance = getObjects()[0];
    }
}

void GetInstanceResponseHandler::validate(void)
{
    if(getResponseObjectTotal() == 0)
    {
        // error? provider claims success,
        // but did not deliver an instance.
        setStatus(CIM_ERR_NOT_FOUND);
    }
}

//
// EnumerateInstancesResponseHandler
//

EnumerateInstancesResponseHandler::EnumerateInstancesResponseHandler(
    CIMEnumerateInstancesRequestMessage * request,
    CIMEnumerateInstancesResponseMessage * response)
    : OperationResponseHandler(request, response)
{
    #ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    // Attempt to get the cached class definition used to validate results of this
    // operation. If it does not exist, then this feature is disabled for this
    // operation.
    CIMClass cimClass;

    try
    {
        CachedClassDefinitionContainer container =
            request->operationContext.get(CachedClassDefinitionContainer::NAME);

        cimClass = container.getClass();
    }
    catch(Exception &)
    {
        // Do nothing. Container is missing, which implies normalization is disabled
        // for this operation.
    }

    _normalizer =
        ObjectNormalizer(
            cimClass,
            request->includeQualifiers,
            request->includeClassOrigin);
    #endif
}

void EnumerateInstancesResponseHandler::deliver(const CIMInstance & cimInstance)
{
    if(cimInstance.isUninitialized())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    #ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    SimpleInstanceResponseHandler::deliver(_normalizer.processInstance(cimInstance));
    #else
    SimpleInstanceResponseHandler::deliver(cimInstance);
    #endif
}

String EnumerateInstancesResponseHandler::getClass(void) const
{
    return(String("EnumerateInstancesResponseHandler"));
}

void EnumerateInstancesResponseHandler::transfer(void)
{
    CIMEnumerateInstancesResponseMessage & msg =
        *static_cast<CIMEnumerateInstancesResponseMessage *>(getResponse());

    msg.cimNamedInstances = getObjects();
}

//
// EnumerateInstanceNamesResponseHandler
//

EnumerateInstanceNamesResponseHandler::EnumerateInstanceNamesResponseHandler(
    CIMEnumerateInstanceNamesRequestMessage * request,
    CIMEnumerateInstanceNamesResponseMessage * response)
    : OperationResponseHandler(request, response)
{
    #ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    // Attempt to get the cached class definition used to validate results of this
    // operation. If it does not exist, then this feature is disabled for this
    // operation.
    CIMClass cimClass;

    try
    {
        CachedClassDefinitionContainer container =
            request->operationContext.get(CachedClassDefinitionContainer::NAME);

        cimClass = container.getClass();
    }
    catch(Exception &)
    {
        // Do nothing. Container is missing, which implies normalization is disabled
        // for this operation.
    }

    _normalizer =
        ObjectNormalizer(
            cimClass,
            false,
            false);
    #endif
}

void EnumerateInstanceNamesResponseHandler::deliver(const CIMObjectPath & cimObjectPath)
{
    if(cimObjectPath.getClassName().isNull())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    #ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    SimpleObjectPathResponseHandler::deliver(_normalizer.processInstanceObjectPath(cimObjectPath));
    #else
    SimpleObjectPathResponseHandler::deliver(cimObjectPath);
    #endif
}

String EnumerateInstanceNamesResponseHandler::getClass(void) const
{
    return(String("EnumerateInstanceNamesResponseHandler"));
}

void EnumerateInstanceNamesResponseHandler::transfer(void)
{
    CIMEnumerateInstanceNamesResponseMessage & msg =
        *static_cast<CIMEnumerateInstanceNamesResponseMessage *>(getResponse());

    msg.instanceNames = getObjects();
}

//
// CreateInstanceResponseHandler
//

CreateInstanceResponseHandler::CreateInstanceResponseHandler(
    CIMCreateInstanceRequestMessage * request,
    CIMCreateInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

void CreateInstanceResponseHandler::deliver(const CIMObjectPath & cimObjectPath)
{
    if(cimObjectPath.getClassName().isNull())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    if(SimpleObjectPathResponseHandler::size() != 0)
    {
        MessageLoaderParms message(
            "Server.OperationResponseHandler.TOO_MANY_OBJECTS_DELIVERED",
            "Too many objects delivered.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleObjectPathResponseHandler::deliver(cimObjectPath);
}

void CreateInstanceResponseHandler::complete(void)
{
    if(SimpleObjectPathResponseHandler::size() == 0)
    {
        MessageLoaderParms message(
            "Server.OperationResponseHandler.TOO_FEW_OBJECTS_DELIVERED",
            "Too few objects delivered.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleObjectPathResponseHandler::complete();
}

String CreateInstanceResponseHandler::getClass(void) const
{
    return(String("CreateInstanceResponseHandler"));
}

#if 0
// ATTN: is it an error to not return instance name?
void CreateInstanceResponseHandler::validate(void)
{
    if(getResponseObjectTotal() == 0)
    {
        setStatus(CIM_ERR_NOT_FOUND);
    }
}
#endif

void CreateInstanceResponseHandler::transfer(void)
{
    if(size() > 0)
    {
        CIMCreateInstanceResponseMessage & msg =
            *static_cast<CIMCreateInstanceResponseMessage *>(getResponse());

        msg.instanceName = getObjects()[0];
    }
}

//
// ModifyInstanceResponseHandler
//

ModifyInstanceResponseHandler::ModifyInstanceResponseHandler(
    CIMModifyInstanceRequestMessage * request,
    CIMModifyInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

String ModifyInstanceResponseHandler::getClass(void) const
{
    return(String("ModifyInstanceResponseHandler"));
}

//
// DeleteInstanceResponseHandler
//

DeleteInstanceResponseHandler::DeleteInstanceResponseHandler(
    CIMDeleteInstanceRequestMessage * request,
    CIMDeleteInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

String DeleteInstanceResponseHandler::getClass(void) const
{
    return(String("DeleteInstanceResponseHandler"));
}

//
// GetPropertyResponseHandler
//

GetPropertyResponseHandler::GetPropertyResponseHandler(
    CIMGetPropertyRequestMessage * request,
    CIMGetPropertyResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

void GetPropertyResponseHandler::deliver(const CIMValue & cimValue)
{
    if(cimValue.isNull())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleValueResponseHandler::deliver(cimValue);
}

String GetPropertyResponseHandler::getClass(void) const
{
    return(String("GetPropertyResponseHandler"));
}

void GetPropertyResponseHandler::transfer(void)
{
    if(size() > 0)
    {
        CIMGetPropertyResponseMessage & msg =
            *static_cast<CIMGetPropertyResponseMessage *>(getResponse());

        msg.value = getObjects()[0];
    }
}

void GetPropertyResponseHandler::validate(void)
{
    // error? provider claims success,
    // but did not deliver an instance.
    if(getResponseObjectTotal() == 0)
    {
        setStatus(CIM_ERR_NOT_FOUND);
    }
}

//
// SetPropertyResponseHandler
//

SetPropertyResponseHandler::SetPropertyResponseHandler(
    CIMSetPropertyRequestMessage * request,
    CIMSetPropertyResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

String SetPropertyResponseHandler::getClass(void) const
{
    return(String("SetPropertyResponseHandler"));
}

//
// ExecQueryResponseHandler
//

ExecQueryResponseHandler::ExecQueryResponseHandler(
    CIMExecQueryRequestMessage * request,
    CIMExecQueryResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

void ExecQueryResponseHandler::deliver(const CIMInstance & cimInstance)
{
    if(cimInstance.isUninitialized())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleInstance2ObjectResponseHandler::deliver(cimInstance);
}

String ExecQueryResponseHandler::getClass(void) const
{
    return(String("ExecQueryResponseHandler"));
}

void ExecQueryResponseHandler::transfer(void)
{
    CIMExecQueryResponseMessage & msg =
        *static_cast<CIMExecQueryResponseMessage *>(getResponse());

    msg.cimObjects = getObjects();
}

Boolean ExecQueryResponseHandler::isAsync(void) const
{
    return(false);
}

//
// AssociatorsResponseHandler
//

AssociatorsResponseHandler::AssociatorsResponseHandler(
    CIMAssociatorsRequestMessage * request,
    CIMAssociatorsResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

void AssociatorsResponseHandler::deliver(const CIMObject & cimObject)
{
    if(cimObject.isUninitialized())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleObjectResponseHandler::deliver(cimObject);
}

String AssociatorsResponseHandler::getClass(void) const
{
    return(String("AssociatorsResponseHandler"));
}

void AssociatorsResponseHandler::transfer(void)
{
    CIMAssociatorsResponseMessage & msg =
        *static_cast<CIMAssociatorsResponseMessage *>(getResponse());

    msg.cimObjects = getObjects();
}

//
// AssociatorNamesResponseHandler
//

AssociatorNamesResponseHandler::AssociatorNamesResponseHandler(
    CIMAssociatorNamesRequestMessage * request,
    CIMAssociatorNamesResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

void AssociatorNamesResponseHandler::deliver(const CIMObjectPath & cimObjectPath)
{
    if(cimObjectPath.getClassName().isNull())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleObjectPathResponseHandler::deliver(cimObjectPath);
}

String AssociatorNamesResponseHandler::getClass(void) const
{
    return(String("AssociatorNamesResponseHandler"));
}

void AssociatorNamesResponseHandler::transfer(void)
{
    CIMAssociatorNamesResponseMessage & msg =
        *static_cast<CIMAssociatorNamesResponseMessage *>(getResponse());

    msg.objectNames = getObjects();
}

//
// ReferencesResponseHandler
//

ReferencesResponseHandler::ReferencesResponseHandler(
    CIMReferencesRequestMessage * request,
    CIMReferencesResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

void ReferencesResponseHandler::deliver(const CIMObject & cimObject)
{
    if(cimObject.isUninitialized())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleObjectResponseHandler::deliver(cimObject);
}

String ReferencesResponseHandler::getClass(void) const
{
    return(String("ReferencesResponseHandler"));
}

void ReferencesResponseHandler::transfer(void)
{
    CIMReferencesResponseMessage & msg =
        *static_cast<CIMReferencesResponseMessage *>(getResponse());

    msg.cimObjects = getObjects();
}

//
// ReferenceNamesResponseHandler
//

ReferenceNamesResponseHandler::ReferenceNamesResponseHandler(
    CIMReferenceNamesRequestMessage * request,
    CIMReferenceNamesResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

void ReferenceNamesResponseHandler::deliver(const CIMObjectPath & cimObjectPath)
{
    if(cimObjectPath.getClassName().isNull())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleObjectPathResponseHandler::deliver(cimObjectPath);
}

String ReferenceNamesResponseHandler::getClass(void) const
{
    return(String("ReferenceNamesResponseHandler"));
}

void ReferenceNamesResponseHandler::transfer(void)
{
    CIMReferenceNamesResponseMessage & msg =
        *static_cast<CIMReferenceNamesResponseMessage *>(getResponse());

    msg.objectNames = getObjects();
}

//
// InvokeMethodResponseHandler
//

InvokeMethodResponseHandler::InvokeMethodResponseHandler(
    CIMInvokeMethodRequestMessage * request,
    CIMInvokeMethodResponseMessage * response)
    : OperationResponseHandler(request, response)
{
}

void InvokeMethodResponseHandler::deliverParamValue(const CIMParamValue & cimParamValue)
{
    if(cimParamValue.isUninitialized())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleMethodResultResponseHandler::deliverParamValue(cimParamValue);
}

void InvokeMethodResponseHandler::deliver(const CIMValue & cimValue)
{
    if(cimValue.isNull())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    SimpleMethodResultResponseHandler::deliver(cimValue);
}

String InvokeMethodResponseHandler::getClass(void) const
{
    return(String("InvokeMethodResponseHandler"));
}

void InvokeMethodResponseHandler::transfer(void)
{
    CIMInvokeMethodResponseMessage & msg =
        *static_cast<CIMInvokeMethodResponseMessage *>(getResponse());

    msg.outParameters = getParamValues();

    // ATTN-RK-20020903: Is it legal for the return value to be null?
    // if not, then the check must be done here since deliver() works off the
    // virtual size, which refers to out parameters!
    msg.retValue = getReturnValue();
}

//
// EnableIndicationsResponseHandler
//

typedef void (*PEGASUS_INDICATION_CALLBACK)(CIMProcessIndicationRequestMessage*);

EnableIndicationsResponseHandler::EnableIndicationsResponseHandler(
    CIMRequestMessage * request,
    CIMResponseMessage * response,
    CIMInstance & provider,
    PEGASUS_INDICATION_CALLBACK indicationCallback)
    : OperationResponseHandler(request, response),
    _indicationCallback(indicationCallback)
{
    _provider = provider;
}

void EnableIndicationsResponseHandler::deliver(const CIMIndication & cimIndication)
{
    OperationContext context;

    Array<CIMObjectPath> subscriptionInstanceNames;

    context.insert(SubscriptionInstanceNamesContainer(subscriptionInstanceNames));

    deliver(context, cimIndication);
}

void EnableIndicationsResponseHandler::deliver(const OperationContext & context, const CIMIndication & cimIndication)
{
    if(cimIndication.isUninitialized())
    {
        MessageLoaderParms message(
            "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
            "The object is not initialized.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    // ATTN: temporarily convert indication to instance
    CIMInstance cimInstance(cimIndication);

    //  Get list of subscription instance names from context
    Array<CIMObjectPath> subscriptionInstanceNames;

    try
    {
        SubscriptionInstanceNamesContainer container =
            context.get(SubscriptionInstanceNamesContainer::NAME);

        subscriptionInstanceNames = container.getInstanceNames();
    }
    catch(Exception &)
    {
        subscriptionInstanceNames.clear();
    }

    // l10n
    ContentLanguages contentLangs;

    try
    {
        // Get the Content-Language for this indication.  The provider
        // does not have to add specify a language for the indication.
        ContentLanguageListContainer langContainer =
            context.get(ContentLanguageListContainer::NAME);

        contentLangs = langContainer.getLanguages();
    }
    catch(Exception &)
    {
        // The provider did not explicitly set a Content-Language for
        // the indication.  Fall back to the lang set in this object.
        contentLangs = getLanguages();
    }
    // l10n -end

    // create message
    // l10n
    CIMProcessIndicationRequestMessage * request =
        new CIMProcessIndicationRequestMessage(
        XmlWriter::getNextMessageId(),
        cimInstance.getPath().getNameSpace(),
        cimInstance,
        subscriptionInstanceNames,
        _provider,
        QueueIdStack());  // Must be filled in by the callback function

    request->operationContext = context;

    try
    {
        request->operationContext.set(ContentLanguageListContainer(contentLangs));
    }
    catch(Exception &)
    {
        request->operationContext.insert(ContentLanguageListContainer(contentLangs));
    }

    _indicationCallback(request);
}

void EnableIndicationsResponseHandler::deliver(const Array<CIMIndication> & cimIndications)
{
    OperationContext context;

    deliver(context, cimIndications);
}

void EnableIndicationsResponseHandler::deliver(const OperationContext & context, const Array<CIMIndication> & cimIndications)
{
    for(Uint32 i = 0, n = cimIndications.size(); i < n; i++)
    {
        deliver(context, cimIndications[i]);
    }
}

String EnableIndicationsResponseHandler::getClass(void) const
{
    return(String("EnableIndicationsResponseHandler"));
}

Boolean EnableIndicationsResponseHandler::isAsync(void) const
{
    return(false);
}

PEGASUS_NAMESPACE_END
