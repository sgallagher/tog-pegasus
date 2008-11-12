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

#ifndef Pegasus_CimToWsmResponseMapper_h
#define Pegasus_CimToWsmResponseMapper_h

#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/WsmServer/WsmResponse.h>
#include <Pegasus/WsmServer/WsmRequest.h>

PEGASUS_NAMESPACE_BEGIN

/**
    Converts CIMResponseMessage messages to WsmResponses.
*/
class PEGASUS_WSMSERVER_LINKAGE CimToWsmResponseMapper
{
public:

    CimToWsmResponseMapper();
    ~CimToWsmResponseMapper();

    WsmResponse* mapToWsmResponse(
        const WsmRequest* wsmRequest,
        const CIMResponseMessage* message);
    WsmFault mapCimExceptionToWsmFault(const CIMException& cimException);

    void convertCimToWsmInstance(
        const CIMConstInstance& cimInstance,
        WsmInstance& wsmInstance,
        const String& nameSpace);
    void convertCimToWsmValue(
        const CIMValue& cimValue,
        WsmValue& wsmValue,
        const String& nameSpace);
    void convertObjPathToEPR(
        const CIMObjectPath& objPath,
        WsmEndpointReference& epr,
        const String& nameSpace);
    void convertCimToWsmDatetime(const CIMDateTime& cimDT, String& wsmDT);

private:

    WxfGetResponse* _mapToWxfGetResponse(
        const WxfGetRequest* wsmRequest,
        const CIMGetInstanceResponseMessage* response);
    WxfPutResponse* _mapToWxfPutResponse(
        const WxfPutRequest* wsmRequest,
        const CIMModifyInstanceResponseMessage* response);
    WxfCreateResponse* _mapToWxfCreateResponse(
        const WxfCreateRequest* wsmRequest,
        const CIMCreateInstanceResponseMessage* response);
    WxfDeleteResponse* _mapToWxfDeleteResponse(
        const WxfDeleteRequest* wsmRequest,
        const CIMDeleteInstanceResponseMessage* response);
    WsenEnumerateResponse* _mapToWsenEnumerateResponseObject(
        const WsenEnumerateRequest* wsmRequest,
        const CIMEnumerateInstancesResponseMessage* response);
    WsenEnumerateResponse* _mapToWsenEnumerateResponseObjectAndEPR(
        const WsenEnumerateRequest* wsmRequest,
        const CIMEnumerateInstancesResponseMessage* response);
    WsenEnumerateResponse* _mapToWsenEnumerateResponseEPR(
        const WsenEnumerateRequest* wsmRequest,
        const CIMEnumerateInstanceNamesResponseMessage* response);
    WsmFaultResponse* _mapToWsmFaultResponse(
        const WsmRequest* wsmRequest,
        const CIMResponseMessage* message);

    ContentLanguageList _getContentLanguages(const OperationContext& context)
    {
        if (context.contains(ContentLanguageListContainer::NAME))
        {
            return ((ContentLanguageListContainer)
                context.get(ContentLanguageListContainer::NAME)).getLanguages();
        }

        return ContentLanguageList();
    }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CimToWsmResponseMapper_h */
