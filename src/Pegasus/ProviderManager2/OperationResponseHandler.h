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

#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMIndication.h>
#include <Pegasus/Common/CIMValue.h>

#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/OperationContextInternal.h>

#include <Pegasus/Common/ObjectNormalizer.h>

#include <Pegasus/Common/ResponseHandler.h>
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

    CIMRequestMessage * getRequest(void) const;

    CIMResponseMessage * getResponse(void) const;

    virtual void setStatus(
        const Uint32 code,
        const String & message = String::EMPTY);

    virtual void setStatus(
        const Uint32 code,
        const ContentLanguages & langs,
        const String & message = String::EMPTY);

protected:
    // the default for all derived handlers. Some handlers may not apply
    // async behavior because their callers cannot handle partial responses.
    virtual Boolean isAsync(void) const;

    // send (deliver) asynchronously
    virtual void send(Boolean isComplete);

    // transfer any objects from handler to response. this does not clear()
    virtual void transfer(void);

    // validate whatever is necessary before the transfer
    virtual void validate(void);

    virtual String getClass(void) const;

    Uint32 getResponseObjectTotal(void) const;

    // there can be many objects per message (or none at all - i.e complete())
    Uint32 getResponseMessageTotal(void) const;

    Uint32 getResponseObjectThreshold(void) const;

    CIMRequestMessage * _request;
    CIMResponseMessage * _response;

private:
    Uint32 _responseObjectTotal;
    Uint32 _responseMessageTotal;
    Uint32 _responseObjectThreshold;

};

class PEGASUS_PPM_LINKAGE GetInstanceResponseHandler : public OperationResponseHandler, public SimpleInstanceResponseHandler
{
public:
    GetInstanceResponseHandler(
        CIMGetInstanceRequestMessage * request,
        CIMGetInstanceResponseMessage * response);

    virtual void deliver(const CIMInstance & cimInstance);
    virtual void deliver(const Array<CIMInstance> & cimInstanceArray)
    {
        SimpleInstanceResponseHandler::deliver(cimInstanceArray);
    }

    virtual void complete(void);

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

    virtual void validate(void);

private:
    ObjectNormalizer _normalizer;

};

class PEGASUS_PPM_LINKAGE EnumerateInstancesResponseHandler : public OperationResponseHandler, public SimpleInstanceResponseHandler
{
public:
    EnumerateInstancesResponseHandler(
        CIMEnumerateInstancesRequestMessage * request,
        CIMEnumerateInstancesResponseMessage * response);

    virtual void deliver(const CIMInstance & cimInstance);
    virtual void deliver(const Array<CIMInstance> & cimInstanceArray)
    {
        SimpleInstanceResponseHandler::deliver(cimInstanceArray);
    }

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

private:
    ObjectNormalizer _normalizer;

};

class PEGASUS_PPM_LINKAGE EnumerateInstanceNamesResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    EnumerateInstanceNamesResponseHandler(
        CIMEnumerateInstanceNamesRequestMessage * request,
        CIMEnumerateInstanceNamesResponseMessage * response);

    virtual void deliver(const CIMObjectPath & cimObjectPath);
    virtual void deliver(const Array<CIMObjectPath> & cimObjectPathArray)
    {
        SimpleObjectPathResponseHandler::deliver(cimObjectPathArray);
    }

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

private:
    ObjectNormalizer _normalizer;

};

class PEGASUS_PPM_LINKAGE CreateInstanceResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    CreateInstanceResponseHandler(
        CIMCreateInstanceRequestMessage * request,
        CIMCreateInstanceResponseMessage * response);

    virtual void deliver(const CIMObjectPath & cimObjectPath);
    virtual void deliver(const Array<CIMObjectPath> & cimObjectPathArray)
    {
        SimpleObjectPathResponseHandler::deliver(cimObjectPathArray);
    }

    virtual void complete(void);

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

};

class PEGASUS_PPM_LINKAGE ModifyInstanceResponseHandler : public OperationResponseHandler, public SimpleResponseHandler
{
public:
    ModifyInstanceResponseHandler(
        CIMModifyInstanceRequestMessage * request,
        CIMModifyInstanceResponseMessage * response);

protected:
    virtual String getClass(void) const;

};

class PEGASUS_PPM_LINKAGE DeleteInstanceResponseHandler : public OperationResponseHandler, public SimpleResponseHandler
{
public:
    DeleteInstanceResponseHandler(
        CIMDeleteInstanceRequestMessage * request,
        CIMDeleteInstanceResponseMessage * response);

protected:
    virtual String getClass(void) const;

};

class PEGASUS_PPM_LINKAGE GetPropertyResponseHandler : public OperationResponseHandler, public SimpleValueResponseHandler
{
public:
    GetPropertyResponseHandler(
        CIMGetPropertyRequestMessage * request,
        CIMGetPropertyResponseMessage * response);

    virtual void deliver(const CIMValue & cimValue);
    virtual void deliver(const Array<CIMValue> & cimValueArray)
    {
        SimpleValueResponseHandler::deliver(cimValueArray);
    }

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

    virtual void validate(void);

};

class PEGASUS_PPM_LINKAGE SetPropertyResponseHandler : public OperationResponseHandler, public SimpleResponseHandler
{
public:
    SetPropertyResponseHandler(
        CIMSetPropertyRequestMessage * request,
        CIMSetPropertyResponseMessage * response);

protected:
    virtual String getClass(void) const;

};

class PEGASUS_PPM_LINKAGE ExecQueryResponseHandler : public OperationResponseHandler, public SimpleInstance2ObjectResponseHandler
{
public:
    ExecQueryResponseHandler(
        CIMExecQueryRequestMessage * request,
        CIMExecQueryResponseMessage * response);

    virtual void deliver(const CIMInstance & cimInstance);
    virtual void deliver(const Array<CIMInstance> & cimInstanceArray)
    {
        SimpleInstance2ObjectResponseHandler::deliver(cimInstanceArray);
    }

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

    virtual Boolean isAsync(void) const;

};

class PEGASUS_PPM_LINKAGE AssociatorsResponseHandler : public OperationResponseHandler, public SimpleObjectResponseHandler
{
public:
    AssociatorsResponseHandler(
        CIMAssociatorsRequestMessage * request,
        CIMAssociatorsResponseMessage * response);

    virtual void deliver(const CIMObject & cimObject);
    virtual void deliver(const Array<CIMObject> & cimObjectArray)
    {
        SimpleObjectResponseHandler::deliver(cimObjectArray);
    }

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

};

class PEGASUS_PPM_LINKAGE AssociatorNamesResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    AssociatorNamesResponseHandler(
        CIMAssociatorNamesRequestMessage * request,
        CIMAssociatorNamesResponseMessage * response);

    virtual void deliver(const CIMObjectPath & cimObjectPath);
    virtual void deliver(const Array<CIMObjectPath> & cimObjectPathArray)
    {
        SimpleObjectPathResponseHandler::deliver(cimObjectPathArray);
    }

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

};

class PEGASUS_PPM_LINKAGE ReferencesResponseHandler : public OperationResponseHandler, public SimpleObjectResponseHandler
{
public:
    ReferencesResponseHandler(
        CIMReferencesRequestMessage * request,
        CIMReferencesResponseMessage * response);

    virtual void deliver(const CIMObject & cimObject);
    virtual void deliver(const Array<CIMObject> & cimObjectArray)
    {
        SimpleObjectResponseHandler::deliver(cimObjectArray);
    }

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

};

class PEGASUS_PPM_LINKAGE ReferenceNamesResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    ReferenceNamesResponseHandler(
        CIMReferenceNamesRequestMessage * request,
        CIMReferenceNamesResponseMessage * response);

    virtual void deliver(const CIMObjectPath & cimObjectPath);
    virtual void deliver(const Array<CIMObjectPath> & cimObjectPathArray)
    {
        SimpleObjectPathResponseHandler::deliver(cimObjectPathArray);
    }

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

};

class PEGASUS_PPM_LINKAGE InvokeMethodResponseHandler : public OperationResponseHandler, public SimpleMethodResultResponseHandler
{
public:
    InvokeMethodResponseHandler(
        CIMInvokeMethodRequestMessage * request,
        CIMInvokeMethodResponseMessage * response);

    virtual void deliverParamValue(const CIMParamValue & cimParamValue);
    virtual void deliverParamValue(
        const Array<CIMParamValue> & cimParamValueArray)
    {
        SimpleMethodResultResponseHandler::deliverParamValue(
            cimParamValueArray);
    }

    virtual void deliver(const CIMValue & cimValue);

protected:
    virtual String getClass(void) const;

    virtual void transfer(void);

};

typedef void (*PEGASUS_INDICATION_CALLBACK)(CIMProcessIndicationRequestMessage*);

class PEGASUS_PPM_LINKAGE EnableIndicationsResponseHandler : public OperationResponseHandler, public SimpleIndicationResponseHandler
{
public:
    EnableIndicationsResponseHandler(
        CIMRequestMessage * request,
        CIMResponseMessage * response,
        CIMInstance & provider,
        PEGASUS_INDICATION_CALLBACK indicationCallback);

    virtual void deliver(const CIMIndication & cimIndication);

    virtual void deliver(const OperationContext & context, const CIMIndication & cimIndication);

    virtual void deliver(const Array<CIMIndication> & cimIndications);

    virtual void deliver(const OperationContext & context, const Array<CIMIndication> & cimIndications);

protected:
    virtual String getClass(void) const;

    virtual Boolean isAsync(void) const;

private:
    PEGASUS_INDICATION_CALLBACK _indicationCallback;

};

PEGASUS_NAMESPACE_END

#endif
