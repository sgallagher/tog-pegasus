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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "OperationResponseHandler.h"

PEGASUS_NAMESPACE_BEGIN

/*
GetInstanceResponseHandler::GetInstanceResponseHandler(
    CIMGetInstanceRequestMessage * request,
    CIMGetInstanceResponseMessage * response)
    : OperationResponseHandler<CIMInstance>(request, response)
{
}

void GetInstanceResponseHandler::complete(OperationContext & context)
{
    if(getObjects().size() > 0)
    {
	((CIMGetInstanceResponseMessage *)getResponse())->cimInstance = getObjects()[0];
    }
    else
    {
	// error? provider claims success, but did not deliver an instance.
	setStatus(CIM_ERR_NOT_FOUND);
    }
}

EnumerateInstancesResponseHandler::EnumerateInstancesResponseHandler(
    CIMEnumerateInstancesRequestMessage * request,
    CIMEnumerateInstancesResponseMessage * response)
    : OperationResponseHandler<CIMInstance>(request, response)
{
}

void EnumerateInstancesResponseHandler::complete(OperationContext & context)
{
    Array<CIMNamedInstance> cimInstances;

    // ATTN: can be removed once CIMNamedInstance is removed
    for(Uint32 i = 0, n = getObjects().size(); i < n; i++)
    {
	CIMInstance cimInstance(getObjects()[i]);

	cimInstances.append(CIMNamedInstance(cimInstance.getPath(), cimInstance));
    }

    ((CIMEnumerateInstancesResponseMessage *)getResponse())->cimNamedInstances = cimInstances;
}

EnumerateInstanceNamesResponseHandler::EnumerateInstanceNamesResponseHandler(
    CIMEnumerateInstanceNamesRequestMessage * request,
    CIMEnumerateInstanceNamesResponseMessage * response)
    : OperationResponseHandler<CIMReference>(request, response)
{
}

void EnumerateInstanceNamesResponseHandler::complete(OperationContext & context)
{
    ((CIMEnumerateInstanceNamesResponseMessage *)getResponse())->instanceNames = getObjects();
}

CreateInstanceResponseHandler::CreateInstanceResponseHandler(
    CIMCreateInstanceRequestMessage * request,
    CIMCreateInstanceResponseMessage * response)
    : OperationResponseHandler<CIMReference>(request, response)
{
}

void CreateInstanceResponseHandler::complete(OperationContext & context)
{
    if(getObjects().size() > 0)
    {
	((CIMCreateInstanceResponseMessage *)getResponse())->instanceName = getObjects()[0];
    }

    // ATTN: is it an error to not return instance name?
}

ModifyInstanceResponseHandler::ModifyInstanceResponseHandler(
    CIMModifyInstanceRequestMessage * request,
    CIMModifyInstanceResponseMessage * response)
    : OperationResponseHandler<CIMInstance>(request, response)
{
}

DeleteInstanceResponseHandler::DeleteInstanceResponseHandler(
    CIMDeleteInstanceRequestMessage * request,
    CIMDeleteInstanceResponseMessage * response)
    : OperationResponseHandler<CIMInstance>(request, response)
{
}
*/

EnableIndicationsResponseHandler::EnableIndicationsResponseHandler(void)
    : ResponseHandler<CIMIndication>()
{
}

void EnableIndicationsResponseHandler::deliver(const CIMIndication & cimIndication)
{
}

void EnableIndicationsResponseHandler::deliver(OperationContext & context, const CIMIndication & cimIndication)
{
}

void EnableIndicationsResponseHandler::deliver(const Array<CIMIndication> & cimIndications)
{
}

void EnableIndicationsResponseHandler::deliver(OperationContext & context, const Array<CIMIndication> & cimIndications)
{
}

PEGASUS_NAMESPACE_END
