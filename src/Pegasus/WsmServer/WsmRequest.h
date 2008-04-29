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

#ifndef Pegasus_WsmRequest_h
#define Pegasus_WsmRequest_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/WsmServer/WsmSelectorSet.h>
#include <Pegasus/WsmServer/WsmInstance.h>

PEGASUS_NAMESPACE_BEGIN

enum WsmOperationType
{
    WSM_FAULT,
    SOAP_FAULT,

    WSM_IDENTITY_IDENTIFY,

    WS_TRANSFER_GET,
    WS_TRANSFER_PUT,
    WS_TRANSFER_CREATE,
    WS_TRANSFER_DELETE,

    WS_ENUMERATION_ENUMERATE,
    WS_ENUMERATION_PULL,
    WS_ENUMERATION_RELEASE
    // etc.
};

class WsmRequest
{
public:

    WsmRequest(
        WsmOperationType type,
        const String& messageId_)
        : messageId(messageId_),
          _type(type)
    {
    }

    virtual ~WsmRequest()
    {
    }

    WsmOperationType getType() const
    {
        return _type;
    }

    String messageId;
    String authType;
    String userName;
    String ipAddress;
    HttpMethod httpMethod;
    AcceptLanguageList acceptLanguages;
    ContentLanguageList contentLanguages;
    Boolean httpCloseConnect;
    Uint32 queueId;
    Boolean requestEpr;
    Uint32 maxEnvelopeSize;

private:

    WsmOperationType _type;
};

class WsmGetRequest : public WsmRequest
{
public:

    WsmGetRequest(
        const String& messageId,
        const WsmEndpointReference& epr_)
        : WsmRequest(WS_TRANSFER_GET, messageId),
          epr(epr_)
    {
    }

    WsmEndpointReference epr;
};

class WsmPutRequest : public WsmRequest
{
public:

    WsmPutRequest(
        const String& messageId,
        const WsmEndpointReference& epr_,
        const WsmInstance& instance_)
        : WsmRequest(WS_TRANSFER_PUT, messageId),
          epr(epr_),
          instance(instance_)
    {
    }

    WsmEndpointReference epr;
    WsmInstance instance;
};

class WsmCreateRequest : public WsmRequest
{
public:

    WsmCreateRequest(
        const String& messageId,
        const WsmEndpointReference& epr_,
        const WsmInstance& instance_)
        : WsmRequest(WS_TRANSFER_CREATE, messageId),
          epr(epr_),
          instance(instance_)
    {
    }

    WsmEndpointReference epr;
    WsmInstance instance;
};

class WsmDeleteRequest : public WsmRequest
{
public:

    WsmDeleteRequest(
        const String& messageId,
        const WsmEndpointReference& epr_)
        : WsmRequest(WS_TRANSFER_DELETE, messageId),
          epr(epr_)
    {
    }

    WsmEndpointReference epr;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WsmRequest_h */
