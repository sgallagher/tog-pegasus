//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusDispatcher_Dispatcher_h
#define PegasusDispatcher_Dispatcher_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Server/ProviderTable.h>

PEGASUS_NAMESPACE_BEGIN

class CIMRepository;
class ProviderTable;

class PEGASUS_SERVER_LINKAGE CIMOperationRequestDispatcher : public MessageQueue
{
public:

    CIMOperationRequestDispatcher(CIMRepository* repository);

    virtual ~CIMOperationRequestDispatcher();

    virtual void handleEnqueue();

    virtual const char* getQueueName() const;

    void handleGetClassRequest(
	CIMGetClassRequestMessage* request);

    void handleGetInstanceRequest(
	CIMGetInstanceRequestMessage* request);

    void handleDeleteClassRequest(
	CIMDeleteClassRequestMessage* request);

    void handleDeleteInstanceRequest(
	CIMDeleteInstanceRequestMessage* request);

    void handleCreateClassRequest(
	CIMCreateClassRequestMessage* request);

    void handleCreateInstanceRequest(
	CIMCreateInstanceRequestMessage* request);

    void handleModifyClassRequest(
	CIMModifyClassRequestMessage* request);

    void handleModifyInstanceRequest(
	CIMModifyInstanceRequestMessage* request);

    void handleEnumerateClassesRequest(
	CIMEnumerateClassesRequestMessage* request);

    void handleEnumerateClassNamesRequest(
	CIMEnumerateClassNamesRequestMessage* request);

    void handleEnumerateInstancesRequest(
	CIMEnumerateInstancesRequestMessage* request);

    void handleEnumerateInstanceNamesRequest(
	CIMEnumerateInstanceNamesRequestMessage* request);

    void handleAssociatorsRequest(
	CIMAssociatorsRequestMessage* request);

    void handleAssociatorNamesRequest(
	CIMAssociatorNamesRequestMessage* request);

    void handleReferencesRequest(
	CIMReferencesRequestMessage* request);

    void handleReferenceNamesRequest(
	CIMReferenceNamesRequestMessage* request);

    void handleGetQualifierRequest(
	CIMGetQualifierRequestMessage* request);

    void handleSetQualifierRequest(
	CIMSetQualifierRequestMessage* request);

    void handleDeleteQualifierRequest(
	CIMDeleteQualifierRequestMessage* request);

    void handleEnumerateQualifiersRequest(
	CIMEnumerateQualifiersRequestMessage* request);

protected:

    void _enqueueResponse(
	CIMRequestMessage* request,
	CIMResponseMessage* response);

    CIMProvider* _lookupProviderForClass(
	const String& nameSpace,
	const String& className);

    CIMRepository* _repository;
    ProviderTable _providerTable;
};

PEGASUS_NAMESPACE_END

#endif /* PegasusDispatcher_Dispatcher_h */
