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
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WsmResponse_h
#define Pegasus_WsmResponse_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/WsmServer/WsmRequest.h>
#include <Pegasus/WsmServer/WsmFault.h>
#include <Pegasus/WsmServer/WsmInstance.h>
#include <Pegasus/WsmServer/WsmUtils.h>

PEGASUS_NAMESPACE_BEGIN

class WsmResponse
{
protected:

    WsmResponse(
        WsmOperationType type,
        const WsmRequest* request,
        const ContentLanguageList& contentLanguages)
        : _type(type),
          _messageId(WsmUtils::getMessageId()),
          _relatesTo(request->messageId),
          _queueId(request->queueId),
          _httpMethod(request->httpMethod),
          _httpCloseConnect(request->httpCloseConnect),
          _maxEnvelopeSize(request->maxEnvelopeSize),
          _contentLanguages(contentLanguages)
    {
    }

    WsmResponse(
        WsmOperationType type,
        const String& relatesTo,
        Uint32 queueId,
        HttpMethod httpMethod,
        Boolean httpCloseConnect,
        const ContentLanguageList& contentLanguages)
        : _type(type),
          _messageId(WsmUtils::getMessageId()),
          _relatesTo(relatesTo),
          _queueId(queueId),
          _httpMethod(httpMethod),
          _httpCloseConnect(httpCloseConnect),
          _maxEnvelopeSize(0),
          _contentLanguages(contentLanguages)
    {
    }

public:

    virtual ~WsmResponse()
    {
    }

    WsmOperationType getType() const
    {
        return _type;
    }

    String& getMessageId()
    {
        return _messageId;
    }

    String& getRelatesTo()
    {
        return _relatesTo;
    }

    Uint32 getQueueId() const
    {
        return _queueId;
    }

    HttpMethod getHttpMethod() const
    {
        return _httpMethod;
    }

    Boolean getHttpCloseConnect() const
    {
        return _httpCloseConnect;
    }

    Uint32 getMaxEnvelopeSize() const
    {
        return _maxEnvelopeSize;
    }

    ContentLanguageList& getContentLanguages()
    {
        return _contentLanguages;
    }

private:

    WsmResponse(const WsmResponse&);
    WsmResponse& operator=(const WsmResponse&);

    WsmOperationType _type;
    String _messageId;
    String _relatesTo;
    Uint32 _queueId;
    HttpMethod _httpMethod;
    Boolean _httpCloseConnect;
    Uint32 _maxEnvelopeSize;
    ContentLanguageList _contentLanguages;
};

class WsmFaultResponse : public WsmResponse
{
public:

    WsmFaultResponse(
        const String& relatesTo,
        Uint32 queueId,
        HttpMethod httpMethod,
        Boolean httpCloseConnect,
        const WsmFault& fault)
        : WsmResponse(
              WSM_FAULT,
              relatesTo,
              queueId,
              httpMethod,
              httpCloseConnect,
              fault.getReasonLanguage()),
          _fault(fault)
    {
    }

    WsmFaultResponse(
        const WsmRequest* request,
        const WsmFault& fault)
        : WsmResponse(
              WSM_FAULT,
              request,
              fault.getReasonLanguage()),
          _fault(fault)
    {
    }

    ~WsmFaultResponse()
    {
    }

    WsmFault& getFault()
    {
        return _fault;
    }

private:

    WsmFault _fault;
};

class SoapFaultResponse : public WsmResponse
{
public:

    SoapFaultResponse(
        const String& relatesTo,
        Uint32 queueId,
        HttpMethod httpMethod,
        Boolean httpCloseConnect,
        const SoapNotUnderstoodFault& fault)
        : WsmResponse(
              SOAP_FAULT,
              relatesTo,
              queueId,
              httpMethod,
              httpCloseConnect,
              fault.getMessageLanguage()),
          _fault(fault)
    {
    }

    ~SoapFaultResponse()
    {
    }

    SoapNotUnderstoodFault& getFault()
    {
        return _fault;
    }

private:

    SoapNotUnderstoodFault _fault;
};

class WxfGetResponse : public WsmResponse
{
public:

    WxfGetResponse(
        const WsmInstance& inst,
        const WxfGetRequest* request,
        const ContentLanguageList& contentLanguages)
        : WsmResponse(
              WS_TRANSFER_GET,
              request,
              contentLanguages),
          _instance(inst)
    {
    }

    ~WxfGetResponse()
    {
    }

    WsmInstance& getInstance()
    {
        return _instance;
    }

private:

    WsmInstance _instance;
};

class WxfPutResponse : public WsmResponse
{
public:

    WxfPutResponse(
        const WxfPutRequest* request,
        const ContentLanguageList& contentLanguages)
        : WsmResponse(
              WS_TRANSFER_PUT,
              request,
              contentLanguages),
          _reference(request->epr),
          _requestedEPR(request->requestEpr)
    {
    }

    ~WxfPutResponse()
    {
    }

    WsmEndpointReference& getEPR()
    {
        return _reference;
    }

    Boolean getRequestedEPR()
    {
        return _requestedEPR;
    }

private:

    // The client can request the potentially updated EPR by specifying the
    // wsman:RequestEPR header.  CIM does not allow a ModifyInstance operation
    // to change key values, though, so this will always be the same as the
    // EPR in the request.
    WsmEndpointReference _reference;
    Boolean _requestedEPR;
};

class WxfCreateResponse : public WsmResponse
{
public:

    WxfCreateResponse(
        const WsmEndpointReference& ref,
        const WxfCreateRequest* request,
        const ContentLanguageList& contentLanguages)
        : WsmResponse(
              WS_TRANSFER_CREATE,
              request,
              contentLanguages),
          _reference(ref)
    {
    }

    ~WxfCreateResponse()
    {
    }

    WsmEndpointReference& getEPR()
    {
        return _reference;
    }

private:

    WsmEndpointReference _reference;
};

class WxfDeleteResponse : public WsmResponse
{
public:

    WxfDeleteResponse(
        const WxfDeleteRequest* request,
        const ContentLanguageList& contentLanguages)
        : WsmResponse(
              WS_TRANSFER_DELETE,
              request,
              contentLanguages)
    {
    }

    ~WxfDeleteResponse()
    {
    }
};

class WsenEnumerateResponse : public WsmResponse
{
public:

    WsenEnumerateResponse(
        const Array<WsmInstance>& inst,
        Uint32 itemCount,
        const WsenEnumerateRequest* request,
        const ContentLanguageList& contentLanguages)
        : WsmResponse(
              WS_ENUMERATION_ENUMERATE,
              request,
              contentLanguages),
          _enumerationContext((Uint64) -1),
          _instances(inst),
          _isComplete(false),
          _requestItemCount(request->requestItemCount),
          _itemCount(itemCount),
          _enumerationMode(request->enumerationMode)
    {
    }
    ~WsenEnumerateResponse()
    {
    }
    Array<WsmInstance>& getInstances()
    {
        return _instances;
    }
    void setComplete()
    {
        _isComplete = true;
    }
    Boolean isComplete()
    {
        return _isComplete;
    }
    Boolean requestedItemCount()
    {
        return _requestItemCount;
    }
    void setItemCount(Uint32 count)
    {
        _itemCount = count;
    }
    Uint32 getItemCount()
    {
        return _itemCount;
    }
    WsenEnumerationMode getEnumerationMode()
    {
        return _enumerationMode;
    }
    Uint64 getEnumerationContext()
    {
        return _enumerationContext;
    }
    void setEnumerationContext(Uint64 context)
    {
        _enumerationContext = context;
    }

private:

    Uint64 _enumerationContext;
    Array<WsmInstance> _instances;
    Boolean _isComplete;
    Boolean _requestItemCount;
    Uint32 _itemCount;
    WsenEnumerationMode _enumerationMode;
};

class WsenPullResponse : public WsmResponse
{
public:

    WsenPullResponse(
        const Array<WsmInstance>& inst,
        WsenEnumerationMode enumerationMode,
        const WsenPullRequest* request,
        const ContentLanguageList& contentLanguages)
        : WsmResponse(
              WS_ENUMERATION_PULL,
              request,
              contentLanguages),
          _enumerationContext((Uint64) -1),
          _instances(inst),
          _isComplete(false),
          _enumerationMode(enumerationMode)
    {
    }
    ~WsenPullResponse()
    {
    }
    Array<WsmInstance>& getInstances()
    {
        return _instances;
    }
    void setComplete()
    {
        _isComplete = true;
    }
    Boolean isComplete()
    {
        return _isComplete;
    }
    WsenEnumerationMode getEnumerationMode()
    {
        return _enumerationMode;
    }
    Uint64 getEnumerationContext()
    {
        return _enumerationContext;
    }
    void setEnumerationContext(Uint64 context)
    {
        _enumerationContext = context;
    }

private:

    Uint64 _enumerationContext;
    Array<WsmInstance> _instances;
    Boolean _isComplete;
    WsenEnumerationMode _enumerationMode;
};

class WsenReleaseResponse : public WsmResponse
{
public:

    WsenReleaseResponse(
        const WsenReleaseRequest* request,
        const ContentLanguageList& contentLanguages)
        : WsmResponse(
              WS_ENUMERATION_RELEASE,
              request,
              contentLanguages)
    {
    }

    ~WsenReleaseResponse()
    {
    }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WsmResponse_h */
