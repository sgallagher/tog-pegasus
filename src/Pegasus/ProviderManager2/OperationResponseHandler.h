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
//      Carol Ann Krug Graves, Hewlett-Packard Company (carolann_graves@hp.com)
//      Dave Rosckes (rosckes@us.ibm.com)
//      Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//      Adrian Schuur (schuur@de.ibm.com)
//      Seema Gupta (gseema@in.ibm.com) for PEP135
//      Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperationResponseHandler_h
#define Pegasus_OperationResponseHandler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/ContentLanguages.h>  // l10n

#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMIndication.h>
#include <Pegasus/Common/CIMValue.h>

#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/OperationContextInternal.h>

#include <Pegasus/Common/ObjectNormalizer.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/XmlWriter.h>

#include <Pegasus/ProviderManager2/SimpleResponseHandler.h>
#include <Pegasus/ProviderManager2/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PPM_LINKAGE OperationResponseHandler
{
    friend class SimpleResponseHandler;

public:
    OperationResponseHandler(
        CIMRequestMessage * request,
        CIMResponseMessage * response);

    virtual ~OperationResponseHandler(void);

    CIMRequestMessage * getRequest(void) const
    {
        return(_request);
    }

    CIMResponseMessage * getResponse(void) const
    {
        return(_response);
    }

    virtual void setStatus(
        const Uint32 code,
        const String & message = String::EMPTY)
    {
        _response->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
    }

    virtual void setStatus(
        const Uint32 code,
        const ContentLanguages & langs,
        const String & message = String::EMPTY)
    {
        _response->cimException =
            PEGASUS_CIM_EXCEPTION_LANG(
            langs,
            CIMStatusCode(code),
            message);
    }

protected:
    // the default for all derived handlers. Some handlers may not apply
    // async behavior because their callers cannot handle partial responses.
    virtual Boolean isAsync(void) const
    {
        return(true);
    }

    // send (deliver) asynchronously
    virtual void send(Boolean isComplete);

    // transfer any objects from handler to response. this does not clear()
    virtual void transfer(void)
    {
    }

    // validate whatever is necessary before the transfer
    virtual void validate(void)
    {
    }

    virtual String getClass(void) const
    {
        return(String("OperationResponseHandler"));
    }

    Uint32 getResponseObjectTotal(void) const
    {
        return(_responseObjectTotal);
    }

    // there can be many objects per message (or none at all - i.e complete())
    Uint32 getResponseMessageTotal(void) const
    {
        return(_responseMessageTotal);
    }

    Uint32 getResponseObjectThreshold(void) const
    {
        return(_responseObjectThreshold);
    }

    CIMRequestMessage * _request;
    CIMResponseMessage * _response;

private:
    Uint32 _responseObjectTotal;
    Uint32 _responseMessageTotal;
    Uint32 _responseObjectThreshold;

};

class GetInstanceResponseHandler : public OperationResponseHandler, public SimpleInstanceResponseHandler
{
public:
    GetInstanceResponseHandler(
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

    virtual void deliver(const CIMInstance & cimInstance)
    {
        if(cimInstance.isUninitialized())
        {
            MessageLoaderParms message(
                "Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
                "The object is not initialized.");

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

protected:
    virtual String getClass(void) const
    {
        return(String("GetInstanceResponseHandler"));
    }

    virtual void transfer(void)
    {
        if(size() > 0)
        {
            CIMGetInstanceResponseMessage & msg =
                *static_cast<CIMGetInstanceResponseMessage *>(getResponse());

            msg.cimInstance = getObjects()[0];
        }
    }

    virtual void validate(void)
    {
        if(getResponseObjectTotal() == 0)
        {
            // error? provider claims success,
            // but did not deliver an instance.
            setStatus(CIM_ERR_NOT_FOUND);
        }
    }

private:
    ObjectNormalizer _normalizer;

};

class EnumerateInstancesResponseHandler : public OperationResponseHandler, public SimpleInstanceResponseHandler
{
public:
    EnumerateInstancesResponseHandler(
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

    virtual void deliver(const CIMInstance & cimInstance)
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

protected:
    virtual String getClass(void) const
    {
        return(String("EnumerateInstancesResponseHandler"));
    }

    virtual void transfer(void)
    {
        CIMEnumerateInstancesResponseMessage & msg =
            *static_cast<CIMEnumerateInstancesResponseMessage *>(getResponse());

        msg.cimNamedInstances = getObjects();
    }

private:
    ObjectNormalizer _normalizer;

};

class EnumerateInstanceNamesResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    EnumerateInstanceNamesResponseHandler(
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

    virtual void deliver(const CIMObjectPath & cimObjectPath)
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

protected:
    virtual String getClass(void) const
    {
        return(String("EnumerateInstanceNamesResponseHandler"));
    }

    virtual void transfer(void)
    {
        CIMEnumerateInstanceNamesResponseMessage & msg =
            *static_cast<CIMEnumerateInstanceNamesResponseMessage *>(getResponse());

        msg.instanceNames = getObjects();
    }

private:
    ObjectNormalizer _normalizer;

};

class CreateInstanceResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    CreateInstanceResponseHandler(
        CIMCreateInstanceRequestMessage * request,
        CIMCreateInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void deliver(const CIMObjectPath & cimObjectPath)
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

protected:
    virtual String getClass(void) const
    {
        return(String("CreateInstanceResponseHandler"));
    }

    #if 0
    // ATTN: is it an error to not return instance name?
    virtual void validate(void)
    {
        if(getResponseObjectTotal() == 0)
        {
            setStatus(CIM_ERR_NOT_FOUND);
        }
    }
    #endif

    virtual void transfer(void)
    {
        if(size() > 0)
        {
            CIMCreateInstanceResponseMessage & msg =
                *static_cast<CIMCreateInstanceResponseMessage *>(getResponse());

            msg.instanceName = getObjects()[0];
        }
    }
};

class ModifyInstanceResponseHandler : public OperationResponseHandler, public SimpleResponseHandler
{
public:
    ModifyInstanceResponseHandler(
        CIMModifyInstanceRequestMessage * request,
        CIMModifyInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

protected:
    virtual String getClass(void) const
    {
        return(String("ModifyInstanceResponseHandler"));
    }
};

class DeleteInstanceResponseHandler : public OperationResponseHandler, public SimpleResponseHandler
{
public:
    DeleteInstanceResponseHandler(
        CIMDeleteInstanceRequestMessage * request,
        CIMDeleteInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

protected:
    virtual String getClass(void) const
    {
        return(String("DeleteInstanceResponseHandler"));
    }

};

class GetPropertyResponseHandler : public OperationResponseHandler, public SimpleValueResponseHandler
{
public:
    GetPropertyResponseHandler(
        CIMGetPropertyRequestMessage * request,
        CIMGetPropertyResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void deliver(const CIMValue & cimValue)
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

protected:
    virtual String getClass(void) const
    {
        return(String("GetPropertyResponseHandler"));
    }

    virtual void transfer(void)
    {
        if(size() > 0)
        {
            CIMGetPropertyResponseMessage & msg =
                *static_cast<CIMGetPropertyResponseMessage *>(getResponse());

            msg.value = getObjects()[0];
        }
    }

    virtual void validate(void)
    {
        // error? provider claims success,
        // but did not deliver an instance.
        if(getResponseObjectTotal() == 0)
        {
            setStatus(CIM_ERR_NOT_FOUND);
        }
    }
};

class SetPropertyResponseHandler : public OperationResponseHandler, public SimpleResponseHandler
{
public:
    SetPropertyResponseHandler(
        CIMSetPropertyRequestMessage * request,
        CIMSetPropertyResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

protected:
    virtual String getClass(void) const
    {
        return(String("SetPropertyResponseHandler"));
    }

};

class ExecQueryResponseHandler : public OperationResponseHandler, public SimpleInstance2ObjectResponseHandler
{
public:
    ExecQueryResponseHandler(
        CIMExecQueryRequestMessage * request,
        CIMExecQueryResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void deliver(const CIMInstance & cimInstance)
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

protected:
    virtual String getClass(void) const
    {
        return(String("ExecQueryResponseHandler"));
    }

    virtual void transfer(void)
    {
        CIMExecQueryResponseMessage & msg =
            *static_cast<CIMExecQueryResponseMessage *>(getResponse());

        msg.cimObjects = getObjects();
    }

    virtual Boolean isAsync(void) const
    {
        return(false);
    }
};

class AssociatorsResponseHandler : public OperationResponseHandler, public SimpleObjectResponseHandler
{
public:
    AssociatorsResponseHandler(
        CIMAssociatorsRequestMessage * request,
        CIMAssociatorsResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void deliver(const CIMObject & cimObject)
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

protected:
    virtual String getClass(void) const
    {
        return(String("AssociatorsResponseHandler"));
    }

    virtual void transfer(void)
    {
        CIMAssociatorsResponseMessage & msg =
            *static_cast<CIMAssociatorsResponseMessage *>(getResponse());

        msg.cimObjects = getObjects();
    }
};

class AssociatorNamesResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    AssociatorNamesResponseHandler(
        CIMAssociatorNamesRequestMessage * request,
        CIMAssociatorNamesResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void deliver(const CIMObjectPath & cimObjectPath)
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

protected:
    virtual String getClass(void) const
    {
        return(String("AssociatorNamesResponseHandler"));
    }

    virtual void transfer(void)
    {
        CIMAssociatorNamesResponseMessage & msg =
            *static_cast<CIMAssociatorNamesResponseMessage *>(getResponse());

        msg.objectNames = getObjects();
    }
};

class ReferencesResponseHandler : public OperationResponseHandler, public SimpleObjectResponseHandler
{
public:
    ReferencesResponseHandler(
        CIMReferencesRequestMessage * request,
        CIMReferencesResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void deliver(const CIMObject & cimObject)
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

protected:
    virtual String getClass(void) const
    {
        return(String("ReferencesResponseHandler"));
    }

    virtual void transfer(void)
    {
        CIMReferencesResponseMessage & msg =
            *static_cast<CIMReferencesResponseMessage *>(getResponse());

        msg.cimObjects = getObjects();
    }
};

class ReferenceNamesResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    ReferenceNamesResponseHandler(
        CIMReferenceNamesRequestMessage * request,
        CIMReferenceNamesResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void deliver(const CIMObjectPath & cimObjectPath)
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

protected:
    virtual String getClass(void) const
    {
        return(String("ReferenceNamesResponseHandler"));
    }

    virtual void transfer(void)
    {
        CIMReferenceNamesResponseMessage & msg =
            *static_cast<CIMReferenceNamesResponseMessage *>(getResponse());

        msg.objectNames = getObjects();
    }
};

class InvokeMethodResponseHandler : public OperationResponseHandler, public SimpleMethodResultResponseHandler
{
public:
    InvokeMethodResponseHandler(
        CIMInvokeMethodRequestMessage * request,
        CIMInvokeMethodResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void deliverParamValue(const CIMParamValue & cimParamValue)
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

    virtual void deliver(const CIMValue & cimValue)
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

protected:
    virtual String getClass(void) const
    {
        return(String("InvokeMethodResponseHandler"));
    }

    virtual void transfer(void)
    {
        CIMInvokeMethodResponseMessage & msg =
            *static_cast<CIMInvokeMethodResponseMessage *>(getResponse());

        msg.outParameters = getParamValues();

        // ATTN-RK-20020903: Is it legal for the return value to be null?
        // if not, then the check must be done here since deliver() works off the
        // virtual size, which refers to out parameters!
        msg.retValue = getReturnValue();
    }
};

typedef void (*PEGASUS_INDICATION_CALLBACK)(CIMProcessIndicationRequestMessage*);

class EnableIndicationsResponseHandler : public OperationResponseHandler, public SimpleIndicationResponseHandler
{
public:
    EnableIndicationsResponseHandler(
        CIMRequestMessage * request,
        CIMResponseMessage * response,
        CIMInstance & provider,
        PEGASUS_INDICATION_CALLBACK indicationCallback)
    : OperationResponseHandler(request, response),
        _indicationCallback(indicationCallback)
    {
        _provider = provider;
    }

    virtual void deliver(const CIMIndication & cimIndication)
    {
        OperationContext context;

        Array<CIMObjectPath> subscriptionInstanceNames;

        context.insert(SubscriptionInstanceNamesContainer(subscriptionInstanceNames));

        deliver(context, cimIndication);
    }

    virtual void deliver(const OperationContext & context, const CIMIndication & cimIndication)
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

    virtual void deliver(const Array<CIMIndication> & cimIndications)
    {
        OperationContext context;

        deliver(context, cimIndications);
    }

    virtual void deliver(const OperationContext & context, const Array<CIMIndication> & cimIndications)
    {
        for(Uint32 i = 0, n = cimIndications.size(); i < n; i++)
        {
            deliver(context, cimIndications[i]);
        }
    }

protected:
    virtual String getClass(void) const
    {
        return(String("EnableIndicationsResponseHandler"));
    }

    virtual Boolean isAsync(void) const
    {
        return(false);
    }

private:
    PEGASUS_INDICATION_CALLBACK _indicationCallback;

};

PEGASUS_NAMESPACE_END

#endif
