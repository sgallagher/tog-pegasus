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

#ifndef Pegasus_WsmToCimRequestMapper_h
#define Pegasus_WsmToCimRequestMapper_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/WsmServer/WsmRequest.h>

PEGASUS_NAMESPACE_BEGIN

/**
    Converts WsmRequest messages to CIMOperationRequestMessages.
*/
class PEGASUS_WSMSERVER_LINKAGE WsmToCimRequestMapper
{
public:

    WsmToCimRequestMapper(CIMRepository* repository);
    ~WsmToCimRequestMapper();

    CIMOperationRequestMessage* mapToCimRequest(WsmRequest* request);

    CIMGetInstanceRequestMessage* mapToCimGetInstanceRequest(
        WxfGetRequest* request);
    CIMModifyInstanceRequestMessage* mapToCimModifyInstanceRequest(
        WxfPutRequest* request);
    CIMCreateInstanceRequestMessage* mapToCimCreateInstanceRequest(
        WxfCreateRequest* request);
    CIMDeleteInstanceRequestMessage* mapToCimDeleteInstanceRequest(
        WxfDeleteRequest* request);
    CIMEnumerateInstancesRequestMessage* mapToCimEnumerateInstancesRequest(
        WsenEnumerateRequest* request);

    CIMName convertResourceUriToClassName(const String& resourceUri);
    String convertEPRAddressToHostname(const String& addr);

    static Boolean stringToReal64(const char* stringValue, Real64& x);

    void convertWsmToCimValue(
        WsmValue& wsmValue,
        const CIMNamespaceName& nameSpace,
        CIMValue& cimValue);

    static void convertStringToCimValue(
        const String& str,
        CIMType cimType,
        CIMValue& cimValue);

    void convertStringArrayToCimValue(
        const Array<String>& strs,
        CIMType cimType,
        CIMValue& cimValue);

    void convertWsmToCimInstance(
        WsmInstance& wsmInstance,
        const CIMNamespaceName& nameSpace,
        CIMInstance& cimInstance);

    void convertEPRToObjectPath(
        const WsmEndpointReference& epr,
        CIMObjectPath& objectPath);

    static void convertWsmToCimDatetime(
        const String& wsmDT, CIMDateTime& cimDT);

private:

    CIMRepository* _repository;

    void _disallowAllClassesResourceUri(const String& resourceUri);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WsmToCimRequestMapper_h */
